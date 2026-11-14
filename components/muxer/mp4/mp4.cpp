 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "mp4.h"

using namespace smooth::IO;

const String &BoCA::MuxerMP4::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (mp4v2dll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>MP4 Muxer</name>			\
		    <version>1.0</version>			\
		    <id>mp4-mux</id>				\
		    <type>muxer</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeMP4v2DLL();
}

namespace BoCA
{
	int64_t	 MP4IO_size(void *);
	int	 MP4IO_seek(void *, int64_t);
	int	 MP4IO_read(void *, void *, int64_t, int64_t *);
	int	 MP4IO_write(void *, const void *, int64_t, int64_t *);
	int	 MP4IO_truncate(void *, int64_t);

	static MP4IOCallbacks	 mp4ReadCallbacks   = { MP4IO_size, MP4IO_seek, MP4IO_read, NIL, NIL };
	static MP4IOCallbacks	 mp4WriteCallbacks  = { MP4IO_size, MP4IO_seek, NIL, MP4IO_write, NIL };
	static MP4IOCallbacks	 mp4ModifyCallbacks = { MP4IO_size, MP4IO_seek, MP4IO_read, MP4IO_write, MP4IO_truncate };
};

BoCA::MuxerMP4::MuxerMP4()
{
	mode		= MUXER_MODE_READ;

	mp4File		= NIL;
	mp4Track	= MP4_INVALID_TRACK_ID;

	sampleId	= 1;
	numberOfSamples	= 0;

	position	= 0;
}

BoCA::MuxerMP4::~MuxerMP4()
{
	if (mp4File != NIL) Close();
}

Bool BoCA::MuxerMP4::Open(MuxerMode nMode)
{
	if (mp4File != NIL || driver == NIL) return False;

	mode	 = nMode;

	sampleId = 1;
	position = 0;

	if (mode == MUXER_MODE_READ || mode == MUXER_MODE_MODIFY)
	{
		/* Open MP4 file and find audio track.
		 */
		if (mode == MUXER_MODE_READ) mp4File = ex_MP4ReadCallbacks(&mp4ReadCallbacks, driver);
		else			     mp4File = ex_MP4ModifyCallbacks(&mp4ModifyCallbacks, driver, 0);

		if (mp4File == NIL)
		{
			errorState  = True;
			errorString = "Invalid file format";

			return False;
		}

		mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

		if (mp4Track == MP4_INVALID_TRACK_ID || ex_MP4GetSampleSize(mp4File, mp4Track, 1) == 0)
		{
			ex_MP4Close(mp4File, 0);

			mp4File	 = NIL;
			mp4Track = MP4_INVALID_TRACK_ID;

			errorState  = True;
			errorString = "No audio track found";

			return False;
		}

		/* Get track properties.
		 */
		numberOfSamples	= ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track);
		timeScale	= ex_MP4GetTrackTimeScale(mp4File, mp4Track);

		format.channels	= ex_MP4GetTrackAudioChannels(mp4File, mp4Track);

		/* Get codec and codec configuration.
		 */
		unsigned char	*configBuffer	  = NIL;
		unsigned int	 configBufferSize = 0;
		unsigned int	 configOffset	  = 0;

		codec = MUXER_CODEC_UNKNOWN;

		if (ex_MP4HaveTrackAtom(mp4File, mp4Track, "mdia.minf.stbl.stsd.alac"))
		{
			codec = MUXER_CODEC_ALAC;

			ex_MP4GetTrackBytesProperty(mp4File, mp4Track, "mdia.minf.stbl.stsd.alac.alac.decoderConfig", (uint8_t **) &configBuffer, (uint32_t *) &configBufferSize);

			/* Work around a bug in MP4v2 that ignores the version
			 * and flags bytes at the start of the alac atom.
			 */
			if (configBufferSize == 28 || configBufferSize == 52) configOffset = 4;
		}
		else if (ex_MP4GetTrackAudioMpeg4Type(mp4File, mp4Track) != MP4_MPEG4_INVALID_AUDIO_TYPE)
		{
			codec = MUXER_CODEC_AAC;

			ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &configBuffer, (uint32_t *) &configBufferSize);
		}

		codecConfig.Resize(0);

		if (configBuffer != NIL)
		{
			codecConfig.Resize(configBufferSize - configOffset);

			memcpy(codecConfig, configBuffer + configOffset, configBufferSize - configOffset);

			ex_MP4Free(configBuffer);
		}

		/* Read gapless information.
		 */
		gaplessInfo = ReadGaplessInfo();
	}
	else if (mode == MUXER_MODE_WRITE)
	{
		if (codec != MUXER_CODEC_AAC)
		{
			errorState  = True;
			errorString = "Unsupported codec";

			return False;
		}

		const Format	&trackFormat = track.GetFormat();

		if (timeScale <= 0) timeScale = trackFormat.rate;

		/* Create MP4 file and audio track.
		 */
		uint32_t	 flags = (track.length	     >= 0xFFFF0000 ||
					  track.approxLength >= 0xFFFF0000) ? MP4_CREATE_64BIT_DATA | MP4_CREATE_64BIT_TIME : 0;

		mp4File	 = ex_MP4CreateCallbacks(&mp4WriteCallbacks, driver, flags);

		if (mp4File == NIL)
		{
			errorState  = True;
			errorString = "Could not create MP4 file";

			return False;
		}

		mp4Track = ex_MP4AddAudioTrack(mp4File, timeScale, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);
		ex_MP4SetTrackIntegerProperty(mp4File, mp4Track, "mdia.minf.stbl.stsd.mp4a.channels", trackFormat.channels);

		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, codecConfig, codecConfig.Size());

		format.channels	= trackFormat.channels;

		numberOfSamples	= 0;
	}
	else
	{
		errorState  = True;
		errorString = "Unsupported muxer mode";

		return False;
	}

	return True;
}

Bool BoCA::MuxerMP4::Close()
{
	if (mp4File == NIL) return False;

	/* Write iTunes metadata with gapless information.
	 */
	if (mode == MUXER_MODE_WRITE && gaplessInfo) WriteGaplessInfo();

	/* Close MP4 file.
	 */
	ex_MP4Close(mp4File, 0);

	mp4File	 = NIL;
	mp4Track = MP4_INVALID_TRACK_ID;

	return True;
}

Int64 BoCA::MuxerMP4::GetNumberOfPackets()
{
	if (mp4File == NIL) return -1;

	return numberOfSamples;
}

Int64 BoCA::MuxerMP4::GetDuration()
{
	if (mp4File == NIL) return -1;

	if (mode == MUXER_MODE_WRITE) return position;

	return ex_MP4GetTrackDuration(mp4File, mp4Track);
}

Int64 BoCA::MuxerMP4::SeekPacket(Int64 seekPosition)
{
	if (mp4File == NIL || mode != MUXER_MODE_READ) return -1;

	/* Find sample containing the requested position.
	 */
	MP4SampleId	 seekSampleId = ex_MP4GetSampleIdFromTime(mp4File, mp4Track, seekPosition, true);

	if (seekSampleId == MP4_INVALID_SAMPLE_ID) return -1;

	sampleId = seekSampleId;
	position = ex_MP4GetSampleTime(mp4File, mp4Track, sampleId);

	return position;
}

Int BoCA::MuxerMP4::ReadPacket(Buffer<UnsignedByte> &buffer, Int64 &packetPosition, Int &flags)
{
	if (mp4File == NIL || mode != MUXER_MODE_READ) return -1;

	if (sampleId > numberOfSamples) return -1;

	/* Read MP4 sample.
	 */
	unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, sampleId);

	buffer.Resize(bufferSize);

	unsigned char	*data	    = buffer;

	MP4Timestamp	 startTime  = 0;
	MP4Duration	 duration   = 0;

	if (!ex_MP4ReadSample(mp4File, mp4Track, sampleId, (uint8_t **) &data, (uint32_t *) &bufferSize, &startTime, &duration, NIL, NIL)) return -1;

	buffer.Resize(bufferSize);

	/* Set packet information.
	 */
	position       = startTime + duration;

	packetPosition = position;
	flags	       = PACKET_FLAG_NONE;

	if (sampleId == 1)		 flags |= PACKET_FLAG_BOS;
	if (sampleId == numberOfSamples) flags |= PACKET_FLAG_EOS;

	sampleId++;

	return bufferSize;
}

Int BoCA::MuxerMP4::WritePacket(const UnsignedByte *data, Int size, Int64 packetPosition, Int flags)
{
	if (mp4File == NIL || mode != MUXER_MODE_WRITE) return -1;

	/* Write MP4 sample.
	 */
	MP4Duration	 duration = Math::Max(Int64(0), packetPosition - position);

	if (!ex_MP4WriteSample(mp4File, mp4Track, (const uint8_t *) data, size, duration, 0, true)) return -1;

	position = packetPosition;

	numberOfSamples++;

	return size;
}

Bool BoCA::MuxerMP4::UpdateStreamInfo()
{
	if (mp4File == NIL || mode != MUXER_MODE_MODIFY) return False;

	/* Update number of channels in sound atom.
	 *
	 * Note: Always set the value, as the channel count reported by MP4v2
	 *	 is taken from the codec configuration if available.
	 */
	const Format	&trackFormat = track.GetFormat();

	if (trackFormat.channels > 0)
	{
		if (!ex_MP4SetTrackIntegerProperty(mp4File, mp4Track, "mdia.minf.stbl.stsd.*[0].channels", trackFormat.channels)) return False;

		format.channels = trackFormat.channels;
	}

	return True;
}

Bool BoCA::MuxerMP4::OptimizeFile(const String &streamURI)
{
	/* Optimize MP4 structure.
	 */
	String	 tempFile = String(streamURI).Append(".temp");

	if (!ex_MP4Optimize(streamURI.ConvertTo("UTF-8"), tempFile.ConvertTo("UTF-8")))
	{
		File(tempFile).Delete();

		return False;
	}

	File(streamURI).Delete();
	File(tempFile).Move(streamURI);

	return True;
}

Bool BoCA::MuxerMP4::ReadGaplessInfo()
{
	Bool	 result = False;

	/* Look for iTunes metadata with gapless information.
	 */
	MP4ItmfItemList	*items = ex_MP4ItmfGetItemsByMeaning(mp4File, "com.apple.iTunes", "iTunSMPB");

	if (items != NIL)
	{
		if (items->size == 1)
		{
			/* Read value as string.
			 */
			Buffer<char>	 value(items->elements[0].dataList.elements[0].valueSize + 1);

			memset(value, 0, items->elements[0].dataList.elements[0].valueSize + 1);
			memcpy(value, items->elements[0].dataList.elements[0].value, items->elements[0].dataList.elements[0].valueSize);

			/* Parse value string.
			 */
			const Array<String>	&values = String(value).Trim().Explode(" ");

			gaplessDelay   = (Int64) Number::FromHexString(values.GetNth(1));
			gaplessPadding = (Int64) Number::FromHexString(values.GetNth(2));
			gaplessLength  = (Int64) Number::FromHexString(values.GetNth(3));

			result = True;
		}

		ex_MP4ItmfItemListFree(items);
	}

	return result;
}

Bool BoCA::MuxerMP4::WriteGaplessInfo()
{
	MP4ItmfItem	*item  = ex_MP4ItmfItemAlloc("----", 1);
	String		 value = String().Append(" 00000000")
					 .Append(" ").Append(Number((Int64) gaplessDelay).ToHexString(8).ToUpper())
					 .Append(" ").Append(Number((Int64) gaplessPadding).ToHexString(8).ToUpper())
					 .Append(" ").Append(Number(gaplessLength).ToHexString(16).ToUpper())
					 .Append(" 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000");

	item->mean = (char *) "com.apple.iTunes";
	item->name = (char *) "iTunSMPB";

	item->dataList.elements[0].typeCode  = MP4_ITMF_BT_UTF8;
	item->dataList.elements[0].value     = (uint8_t *) value.ConvertTo("UTF-8");
	item->dataList.elements[0].valueSize = value.Length();

	Bool	 result = ex_MP4ItmfAddItem(mp4File, item);

	item->mean = NIL;
	item->name = NIL;

	item->dataList.elements[0].typeCode  = MP4_ITMF_BT_IMPLICIT;
	item->dataList.elements[0].value     = NIL;
	item->dataList.elements[0].valueSize = 0;

	ex_MP4ItmfItemFree(item);

	return result;
}

int64_t BoCA::MP4IO_size(void *handle)
{
	Driver	*driver = (Driver *) handle;

	return driver->GetSize();
}

int BoCA::MP4IO_seek(void *handle, int64_t pos)
{
	Driver	*driver = (Driver *) handle;

	if (driver->Seek(pos) == -1) return 1;

	return 0;
}

int BoCA::MP4IO_read(void *handle, void *buffer, int64_t size, int64_t *nout)
{
	Driver	*driver = (Driver *) handle;

	*nout = driver->ReadData((UnsignedByte *) buffer, size);

	if (*nout == 0) return 1;

	return 0;
}

int BoCA::MP4IO_write(void *handle, const void *buffer, int64_t size, int64_t *nout)
{
	Driver	*driver = (Driver *) handle;

	*nout = driver->WriteData((const UnsignedByte *) buffer, size);

	if (*nout == 0) return 1;

	return 0;
}

int BoCA::MP4IO_truncate(void *handle, int64_t size)
{
	Driver	*driver = (Driver *) handle;

	if (!driver->Truncate(size)) return 1;

	return 0;
}
