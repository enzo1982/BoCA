 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2016 Robert Kausch <robert.kausch@freac.org>
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

#include <cdio/cdio.h>

#if LIBCDIO_VERSION_NUM < 93
#	define cdio_free(p) free(p)
#endif

#if HAVE_OLD_PARANOIA_INCLUDE
#	include <cdio/paranoia.h>
#else
#	include <cdio/paranoia/paranoia.h>
#endif

#include "cdio.h"
#include "config.h"

#ifndef PARANOIA_CB_CACHEERR
#	define PARANOIA_CB_CACHEERR 13
#endif

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

const String &BoCA::DecoderCDIO::GetComponentSpecs()
{
	static String	 componentSpecs = "			\
								\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
	  <component>						\
	    <name>CDIO Ripper Component %VERSION%</name>	\
	    <version>1.0</version>				\
	    <id>cdio-dec</id>					\
	    <type>decoder</type>				\
	    <require>cdio-info</require>			\
	    <replace>cdparanoia-dec</replace>			\
	    <format>						\
	      <name>Windows CD Audio Track</name>		\
	      <lossless>true</lossless>				\
	      <extension>cda</extension>			\
	    </format>						\
	  </component>						\
								\
	";

	componentSpecs.Replace("%VERSION%", String("v").Append(String(cdio_version_string).Head(String(cdio_version_string).Find(" "))));

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
}

Void smooth::DetachDLL()
{
}

namespace BoCA
{
	void paranoiaCallback(long int offset, paranoia_cb_mode_t state)
	{
		if (state == PARANOIA_CB_CACHEERR) DecoderCDIO::readDecoder->numCacheErrors++;
	}
};

Array<UnsignedInt64>	 BoCA::DecoderCDIO::lastRead;

Threads::Mutex		 BoCA::DecoderCDIO::readMutex;
BoCA::DecoderCDIO	*BoCA::DecoderCDIO::readDecoder = NIL;


Bool BoCA::DecoderCDIO::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.StartsWith("device://cdda:") ||
	       lcURI.EndsWith(".cda");
}

Error BoCA::DecoderCDIO::GetStreamInfo(const String &streamURI, Track &track)
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdio-info");

	if (component == NIL) return Error();

	track.isCDTrack	= True;

	Format	 format;

	format.channels	= 2;
	format.rate	= 44100;
	format.bits	= 16;
	format.order	= BYTE_INTEL;

	track.SetFormat(format);

	Int	 trackNumber = -1;
	Int	 trackLength = 0;
	Int	 audiodrive  = 0;

	if (streamURI.StartsWith("device://cdda:"))
	{
		audiodrive = streamURI.SubString(14, 1).ToInt();
		trackNumber = streamURI.SubString(16, streamURI.Length() - 16).ToInt();
	}
	else if (streamURI.EndsWith(".cda"))
	{
		/* Find track number and length.
		 */
		{
			InStream	*in = new InStream(STREAM_FILE, streamURI, IS_READ);

			in->Seek(22);

			trackNumber = in->InputNumber(2);

			in->Seek(32);

			trackLength = in->InputNumber(4);

			delete in;
		}

		for (audiodrive = 0; audiodrive < component->GetNumberOfDevices(); audiodrive++)
		{
			Bool		 done = False;
			const MCDI	&mcdi = component->GetNthDeviceMCDI(audiodrive);

			for (Int i = 0; i < mcdi.GetNumberOfEntries(); i++)
			{
				if (mcdi.GetNthEntryType(i) != ENTRY_AUDIO) continue;

				Int	 length = mcdi.GetNthEntryOffset(i + 1) - mcdi.GetNthEntryOffset(i);

				if (length == trackLength && mcdi.GetNthEntryTrackNumber(i) == trackNumber)
				{
					done = True;

					break;
				}
			}

			if (done) break;
		}
	}

	if (trackNumber == -1) return Error();

	/* Fill MCDI data.
	 */
	Info	 info;

	info.mcdi = component->GetNthDeviceMCDI(audiodrive);

	Int	 entryNumber = -1;

	if (trackNumber == 0)
	{
		if (info.mcdi.GetNthEntryType(0) == ENTRY_AUDIO)
		{
			entryNumber = 0;
			trackLength = info.mcdi.GetNthEntryOffset(0);
		}
	}
	else
	{
		for (Int i = 0; i < info.mcdi.GetNumberOfEntries(); i++)
		{
			if (info.mcdi.GetNthEntryType(i) == ENTRY_AUDIO && info.mcdi.GetNthEntryTrackNumber(i) == trackNumber)
			{
				entryNumber = i;
				trackLength = info.mcdi.GetNthEntryTrackLength(i);

				break;
			}
		}
	}

	if (entryNumber == -1) return Error();

	track.length	= trackLength * samplesPerSector;
	track.fileSize	= trackLength * bytesPerSector;

	track.cdTrack	= trackNumber;
	track.drive	= audiodrive;
	track.outfile	= NIL;

	info.track	= trackNumber;
	info.numTracks	= info.mcdi.GetNumberOfAudioTracks();

	/* Read ISRC if requested.
	 */
	const Config	*config = GetConfiguration();

	if (config->GetIntValue("Ripper", "ReadISRC", 0))
	{
		CdIo_t	*cd = cdio_open(component->GetNthDeviceInfo(track.drive).path, DRIVER_UNKNOWN);

		if (cd != NIL)
		{
			char	*isrc = cdio_get_track_isrc(cd, track.cdTrack);

			if (isrc != NIL)
			{
				/* Check if the ISRC is valid.
				 */
				if (Info::IsISRC(isrc)) info.isrc = isrc;

				cdio_free(isrc);
			}

			cdio_destroy(cd);
		}
	}

	track.SetInfo(info);

	/* Delete DeviceInfo component.
	 */
	boca.DeleteComponent(component);

	return Success();
}

BoCA::DecoderCDIO::DecoderCDIO()
{
	configLayer	= NIL;

	packageSize	= 0;

	cd		= NIL;
	drive		= NIL;
	paranoia	= NIL;

	nextSector	= 0;
	sectorsLeft	= 0;

	readOffset	= 0;

	skipSamples	= 0;
	prependSamples	= 0;
	appendSamples	= 0;

	numCacheErrors	= 0;

	lastRead.EnableLocking();
}

BoCA::DecoderCDIO::~DecoderCDIO()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderCDIO::Activate()
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*info = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdio-info");

	if (info != NIL)
	{
		if (lastRead.Length() == 0) for (Int i = 0; i < info->GetNumberOfDevices(); i++) lastRead.Add(0);

		cd = cdio_open(info->GetNthDeviceInfo(track.drive).path, DRIVER_UNKNOWN);

		boca.DeleteComponent(info);
	}

	if (cd == NIL) return False;

	/* Set ripping speed.
	 */
	const Config	*config = GetConfiguration();

	Int	 speed = config->GetIntValue("Ripper", String("RippingSpeedDrive").Append(String::FromInt(track.drive)), 0);

	if (speed > 0)	cdio_set_speed(cd, speed);
	else		cdio_set_speed(cd, -1);

	/* Enable paranoia mode.
	 */
	paranoia = NIL;

	if (config->GetIntValue("Ripper", "CDParanoia", False))
	{
		int	 paranoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

		switch (config->GetIntValue("Ripper", "CDParanoiaMode", 3))
		{
			case 0:
				paranoiaMode = PARANOIA_MODE_OVERLAP;
				break;
			case 1:
				paranoiaMode &= ~PARANOIA_MODE_VERIFY;
				break;
			case 2:
				paranoiaMode &= ~(PARANOIA_MODE_SCRATCH | PARANOIA_MODE_REPAIR);
				break;
		}

		drive = cdio_cddap_identify_cdio(cd, CDDA_MESSAGE_FORGETIT, NIL);

		cdio_cddap_open(drive);

		paranoia = cdio_paranoia_init(drive);

		cdio_paranoia_modeset(paranoia, paranoiaMode);
	}

	/* Call seek to initialize ripper to start of track.
	 */
	Seek(0);

	return True;
}

Bool BoCA::DecoderCDIO::Deactivate()
{
	if (cd == NIL) return False;

	/* Check for drive cache errors.
	 */
	if (numCacheErrors > 0)
	{
		Config	*config		= Config::Get();
		Bool	 noCacheWarning = config->GetIntValue("Ripper", "NoCacheWarning", False);

		if (!noCacheWarning)
		{
			MessageDlg	*msgBox = new MessageDlg("The CD-ROM drive appears to be seeking impossibly quickly.\n"
								 "This could be due to timer bugs, a drive that really is improbably fast,\n"
								 "or, most likely, a bug in cdparanoia's cache modelling.\n\n"
								 "Please consider using another drive for ripping audio CDs and send a bug\n"
								 "report to support@freac.org to assist developers in correcting the problem.", "Warning", Message::Buttons::Ok, Message::Icon::Warning, "Do not display this warning again", &noCacheWarning);

			msgBox->ShowDialog();

			config->SetIntValue("Ripper", "NoCacheWarning", noCacheWarning);
			config->SaveSettings();

			Object::DeleteObject(msgBox);
		}
	}

	if (paranoia != NIL)
	{
		cdio_paranoia_free(paranoia);
		cdio_cddap_close_no_free_cdio(drive);
	}

	cdio_destroy(cd);

	return True;
}

Bool BoCA::DecoderCDIO::Seek(Int64 samplePosition)
{
	const Config	*config = GetConfiguration();

	Int	 startSector = 0;
	Int	 endSector   = 0;
	Bool	 lastTrack   = False;

	if (!GetTrackSectors(startSector, endSector, lastTrack)) return False;

	startSector += samplePosition / samplesPerSector;

	/* Calculate offset values.
	 */
	readOffset = config->GetIntValue("Ripper", String("UseOffsetDrive").Append(String::FromInt(track.drive)), 0) ? config->GetIntValue("Ripper", String("ReadOffsetDrive").Append(String::FromInt(track.drive)), 0) : 0;

	startSector += readOffset / samplesPerSector;
	endSector   += readOffset / samplesPerSector;

	if	(readOffset % samplesPerSector < 0) {		      startSector--; skipSamples = samplesPerSector + readOffset % samplesPerSector; }
	else if (readOffset % samplesPerSector > 0) { if (!lastTrack) endSector++;   skipSamples = 		      readOffset % samplesPerSector; }

	if (startSector < 0) { prependSamples = -startSector * samplesPerSector; startSector = 0; }

	if (lastTrack && readOffset > 0) { appendSamples = readOffset; endSector -= readOffset / samplesPerSector; }

	skipSamples += samplePosition % samplesPerSector;

	/* Set nextSector and sectors left to be read.
	 */
	nextSector  = startSector;
	sectorsLeft = endSector - startSector + 1;

	/* Open ripper.
	 */
	if (paranoia != NIL) cdio_paranoia_seek(paranoia, startSector, SEEK_SET);

	inBytes = 4 * samplePosition;

	/* Wait for drive to spin up if requested.
	 */
	Int		 spinUpTime = config->GetIntValue("Ripper", String("SpinUpTimeDrive").Append(String::FromInt(track.drive)), 0);
	UnsignedInt64	 startTime  = S::System::System::Clock();

	while (spinUpTime > 0 && startTime - lastRead.GetNth(track.drive) > 2500 && S::System::System::Clock() - startTime < (UnsignedInt64) Math::Abs(spinUpTime * 1000))
	{
		if (paranoia != NIL)
		{
			readMutex.Lock();
			readDecoder = this;

			cdio_paranoia_read(paranoia, &paranoiaCallback);

			readMutex.Release();

			cdio_paranoia_seek(paranoia, startSector, SEEK_SET);
		}
		else
		{
			Buffer<UnsignedByte>	 buffer(bytesPerSector);

			cdio_read_audio_sectors(cd, buffer, nextSector, 1);
		}
	}

	return True;
}

Int BoCA::DecoderCDIO::ReadData(Buffer<UnsignedByte> &data)
{
	if (inBytes >= track.fileSize) return -1;

	/* Set up buffer respecting empty samples to prepend.
	 */
	Int	 sectors = Math::Min(sectorsLeft, paranoia == NIL ? 26 : 1);

	Int	 prependBytes = 4 * prependSamples;
	Int	 appendBytes  = 4 * appendSamples;
	Int	 skipBytes    = 4 * skipSamples;

	prependSamples = 0;
	skipSamples    = 0;

	data.Resize(sectors * bytesPerSector + prependBytes);

	if (prependBytes > 0) data.Zero();

	/* Append offset bytes to last track.
	 */
	if (sectors == 0)
	{
		data.Resize(prependBytes + appendBytes - skipBytes);
		data.Zero();

		inBytes += data.Size();

		return data.Size();
	}

	/* Rip chunk.
	 */
	if (paranoia != NIL)
	{
		readMutex.Lock();
		readDecoder = this;

		int16_t	*audio = cdio_paranoia_read(paranoia, &paranoiaCallback);

		readMutex.Release();

		memcpy(data + prependBytes, audio, sectors * bytesPerSector);
	}
	else
	{
		cdio_read_audio_sectors(cd, data + prependBytes, nextSector, sectors);
	}

	nextSector  += sectors;
	sectorsLeft -= sectors;

	lastRead.SetNth(track.drive, S::System::System::Clock());

	/* Strip samples to skip from the beginning.
	 */
	Int	 dataBytes = Math::Min(track.fileSize - inBytes, (Int64) sectors * bytesPerSector + prependBytes - skipBytes);

	if (skipBytes > 0) memmove(data, data + skipBytes, dataBytes);

	inBytes += dataBytes;

	return dataBytes;
}

Bool BoCA::DecoderCDIO::GetTrackSectors(Int &startSector, Int &endSector, Bool &lastTrack)
{
	AS::Registry		&boca	   = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdio-info");

	if (component == NIL) return False;

	MCDI	 mcdi = component->GetNthDeviceMCDI(track.drive);

	boca.DeleteComponent(component);

	if (track.cdTrack == 0)
	{
		/* Special handling for hidden track one audio.
		 */
		if (mcdi.GetNthEntryType(0) == ENTRY_AUDIO)
		{
			startSector = 0;
			endSector   = mcdi.GetNthEntryOffset(0) - 1;
			lastTrack   = False;

			return True;
		}
	}
	else
	{
		/* Handle regular tracks.
		 */
		for (Int i = 0; i < mcdi.GetNumberOfEntries(); i++)
		{
			if (mcdi.GetNthEntryType(i) == ENTRY_AUDIO && mcdi.GetNthEntryTrackNumber(i) == track.cdTrack)
			{
				startSector = mcdi.GetNthEntryOffset(i);
				endSector   = startSector + mcdi.GetNthEntryTrackLength(i) - 1;

				if (mcdi.GetNthEntryType(i + 1) == ENTRY_AUDIO) lastTrack  = False;
				else						lastTrack  = True;

				return True;
			}
		}
	}

	return False;
}

ConfigLayer *BoCA::DecoderCDIO::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCDIO();

	return configLayer;
}
