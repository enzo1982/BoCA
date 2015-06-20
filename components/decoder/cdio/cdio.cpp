 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <cdio/cdio.h>

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
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>CDIO Ripper Component</name>		\
	    <version>1.0</version>			\
	    <id>cdio-dec</id>				\
	    <type>decoder</type>			\
	    <require>cdio-info</require>		\
	    <replace>cdparanoia-dec</replace>		\
	    <format>					\
	      <name>Windows CD Audio Track</name>	\
	      <lossless>true</lossless>			\
	      <extension>cda</extension>		\
	    </format>					\
	  </component>					\
							\
	";

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
		trackLength = info.mcdi.GetNthEntryOffset(0);

		if (info.mcdi.GetNthEntryType(0) == ENTRY_AUDIO) entryNumber = 0;
	}
	else
	{
		for (Int i = 0; i < info.mcdi.GetNumberOfEntries(); i++)
		{
			trackLength = info.mcdi.GetNthEntryOffset(i + 1) - info.mcdi.GetNthEntryOffset(i);

			/* Strip 11400 sectors off of the track length if
			 * we are the last audio track before a new session.
			 */
			if ((i > 0 && info.mcdi.GetNthEntryType(i) != info.mcdi.GetNthEntryType(i + 1) && info.mcdi.GetNthEntryTrackNumber(i + 1) != 0xAA) ||
			    (i < info.mcdi.GetNumberOfEntries() - 1 && info.mcdi.GetNthEntryOffset(i + 2) - info.mcdi.GetNthEntryType(i + 1) <= 0))
			{
				trackLength -= 11400;
			}

			if (info.mcdi.GetNthEntryType(i) == ENTRY_AUDIO && info.mcdi.GetNthEntryTrackNumber(i) == trackNumber)
			{
				entryNumber = i;

				break;
			}
		}
	}

	if (entryNumber == -1) return Error();

	track.length	= (trackLength * 1176) / (format.bits / 8);
	track.fileSize	= trackLength * 1176 * format.channels;

	track.cdTrack	= trackNumber;
	track.drive	= audiodrive;
	track.outfile	= NIL;

	info.track	= trackNumber;
	info.numTracks	= info.mcdi.GetNumberOfAudioTracks();

	/* Read ISRC if requested.
	 */
	Config	*config = Config::Get();

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
	Config	*config = Config::Get();

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

	Config	*config = Config::Get();

	/* Check for drive cache errors.
	 */
	if (numCacheErrors > 0)
	{
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
	Config	*config = Config::Get();

	Int	 startSector = 0;
	Int	 endSector   = 0;

	if (!GetTrackSectors(startSector, endSector)) return False;

	startSector += samplePosition / 588;

	/* Calculate offset values.
	 */
	readOffset = config->GetIntValue("Ripper", String("UseOffsetDrive").Append(String::FromInt(track.drive)), 0) ? config->GetIntValue("Ripper", String("ReadOffsetDrive").Append(String::FromInt(track.drive)), 0) : 0;

	startSector += readOffset / 588;
	endSector   += readOffset / 588;

	if	(readOffset % 588 < 0) { startSector--; skipSamples = 588 + readOffset % 588; }
	else if (readOffset % 588 > 0) { endSector++;	skipSamples = 	    readOffset % 588; }

	if (startSector < 0) { prependSamples = -startSector * 588; startSector = 0; }

	skipSamples += samplePosition % 588;

	/* Set nextSector and sectors left to be read.
	 */
	nextSector  = startSector;
	sectorsLeft = endSector - startSector + 1;

	if (paranoia != NIL) cdio_paranoia_seek(paranoia, startSector, SEEK_SET);

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
			Buffer<UnsignedByte>	 buffer(2352);

			cdio_read_audio_sectors(cd, buffer, nextSector, 1);
		}
	}

	return True;
}

Int BoCA::DecoderCDIO::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (inBytes >= track.fileSize) return -1;

	/* Set up buffer respecting empty samples to prepend.
	 */
	Int	 sectors = Math::Min(sectorsLeft, paranoia == NIL ? 26 : 1);

	Int	 prependBytes = 4 * prependSamples;
	Int	 skipBytes    = 4 * skipSamples;

	prependSamples = 0;
	skipSamples    = 0;

	data.Resize(sectors * 2352 + prependBytes);

	if (prependBytes > 0) data.Zero();

	/* Rip chunk.
	 */
	if (paranoia != NIL)
	{
		readMutex.Lock();
		readDecoder = this;

		int16_t	*audio = cdio_paranoia_read(paranoia, &paranoiaCallback);

		readMutex.Release();

		memcpy(data + prependBytes, audio, data.Size());
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
	Int	 dataBytes = sectors * 2352 + prependBytes - skipBytes;

	if (skipBytes > 0) memmove(data, data + skipBytes, dataBytes);

	inBytes += dataBytes;

	return dataBytes;
}

Bool BoCA::DecoderCDIO::GetTrackSectors(Int &startSector, Int &endSector)
{
	Int	 firstTrack = cdio_get_first_track_num(cd);
	Int	 lastTrack  = cdio_get_last_track_num(cd);

	if (track.cdTrack == 0)
	{
		/* Special handling for hidden track one audio.
		 */
		if (cdio_get_track_format(cd, firstTrack) == TRACK_FORMAT_AUDIO)
		{
			startSector = 0;
			endSector   = cdio_get_track_lsn(cd, firstTrack) - 1;

			return True;
		}
	}
	else
	{
		/* Handle regular tracks.
		 */
		for (Int i = firstTrack; i <= lastTrack; i++)
		{
			if (cdio_get_track_format(cd, i) == TRACK_FORMAT_AUDIO && i == track.cdTrack)
			{
				startSector = cdio_get_track_lsn(cd, i);
				endSector   = cdio_get_track_last_lsn(cd, i);

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
