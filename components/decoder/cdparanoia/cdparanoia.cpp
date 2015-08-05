 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

extern "C" {
#	include <cdda_interface.h>
#	include <cdda_paranoia.h>
}

#include "cdparanoia.h"
#include "config.h"

#ifndef PARANOIA_CB_CACHEERR
#	define PARANOIA_CB_CACHEERR 13
#endif

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

const String &BoCA::DecoderCDParanoia::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>cdparanoia Ripper Component</name>	\
	    <version>1.0</version>			\
	    <id>cdparanoia-dec</id>			\
	    <type>decoder</type>			\
	    <require>cdparanoia-info</require>		\
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
	void paranoiaCallback(long offset, int state)
	{
		if (state == PARANOIA_CB_CACHEERR) DecoderCDParanoia::readDecoder->numCacheErrors++;
	}
};

Array<UnsignedInt64>	 BoCA::DecoderCDParanoia::lastRead;

Threads::Mutex		 BoCA::DecoderCDParanoia::readMutex;
BoCA::DecoderCDParanoia	*BoCA::DecoderCDParanoia::readDecoder = NIL;

Bool BoCA::DecoderCDParanoia::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.StartsWith("device://cdda:") ||
	       lcURI.EndsWith(".cda");
}

Error BoCA::DecoderCDParanoia::GetStreamInfo(const String &streamURI, Track &track)
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdparanoia-info");

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

	track.SetInfo(info);

	/* Delete DeviceInfo component.
	 */
	boca.DeleteComponent(component);

	return Success();
}

BoCA::DecoderCDParanoia::DecoderCDParanoia()
{
	configLayer	= NIL;

	packageSize	= 0;

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

BoCA::DecoderCDParanoia::~DecoderCDParanoia()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderCDParanoia::Activate()
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*info = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdparanoia-info");

	if (info != NIL)
	{
		if (lastRead.Length() == 0) for (Int i = 0; i < info->GetNumberOfDevices(); i++) lastRead.Add(0);

		drive = cdda_identify(info->GetNthDeviceInfo(track.drive).path, CDDA_MESSAGE_FORGETIT, NIL);

		boca.DeleteComponent(info);
	}

	if (drive == NIL) return False;

	cdda_open(drive);

	/* Set ripping spped.
	 */
	const Config	*config = GetConfiguration();

	Int	 speed = config->GetIntValue("Ripper", String("RippingSpeedDrive").Append(String::FromInt(track.drive)), 0);

	if (speed > 0)	cdda_speed_set(drive, speed);
	else		cdda_speed_set(drive, -1);

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

		paranoia = paranoia_init(drive);

		paranoia_modeset(paranoia, paranoiaMode);
	}

	/* Call seek to initialize ripper to start of track.
	 */
	Seek(0);

	return True;
}

Bool BoCA::DecoderCDParanoia::Deactivate()
{
	if (drive == NIL) return False;

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

	if (paranoia != NIL) paranoia_free(paranoia);

	cdda_close(drive);

	return True;
}

Bool BoCA::DecoderCDParanoia::Seek(Int64 samplePosition)
{
	const Config	*config = GetConfiguration();

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
	sectorsLeft = endSector - startSector;

	if (paranoia != NIL) paranoia_seek(paranoia, startSector, SEEK_SET);

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

			paranoia_read(paranoia, &paranoiaCallback);

			readMutex.Release();

			paranoia_seek(paranoia, startSector, SEEK_SET);
		}
		else
		{
			Buffer<UnsignedByte>	 buffer(2352);

			cdda_read(drive, buffer, nextSector, 1);
		}
	}

	return True;
}

Int BoCA::DecoderCDParanoia::ReadData(Buffer<UnsignedByte> &data)
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

		int16_t	*audio = paranoia_read(paranoia, &paranoiaCallback);

		readMutex.Release();

		memcpy(data + prependBytes, audio, data.Size());
	}
	else
	{
		cdda_read(drive, data + prependBytes, nextSector, sectors);
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

Bool BoCA::DecoderCDParanoia::GetTrackSectors(Int &startSector, Int &endSector)
{
	Int	 numTocEntries = drive->tracks;

	if (track.cdTrack == 0)
	{
		/* Special handling for hidden track one audio.
		 */
		if (!(drive->disc_toc[0].bFlags & 0x04))
		{
			startSector = 0;
			endSector   = drive->disc_toc[0].dwStartSector;

			return True;
		}
	}
	else
	{
		/* Handle regular tracks.
		 */
		for (Int i = 0; i < numTocEntries; i++)
		{
			if (!(drive->disc_toc[i].bFlags & 0x04) && (drive->disc_toc[i].bTrack == track.cdTrack))
			{
				startSector = drive->disc_toc[i].dwStartSector;
				endSector   = drive->disc_toc[i + 1].dwStartSector;

				return True;
			}
		}
	}

	return False;
}

ConfigLayer *BoCA::DecoderCDParanoia::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCDParanoia();

	return configLayer;
}
