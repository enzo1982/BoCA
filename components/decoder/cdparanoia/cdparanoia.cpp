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

extern "C" {
#	include <cdda_interface.h>
#	include <cdda_paranoia.h>
}

#include <glob.h>

#include "cdparanoia.h"
#include "config.h"

using namespace smooth::IO;

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
	info.disc	= 1;
	info.numDiscs	= 1;

	track.SetInfo(info);

	/* Delete DeviceInfo component.
	 */
	boca.DeleteComponent(component);

	return Success();
}

BoCA::DecoderCDParanoia::DecoderCDParanoia()
{
	configLayer = NIL;

	packageSize = 0;

	drive	    = NIL;
	paranoia    = NIL;
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
		drive = cdda_identify(info->GetNthDeviceInfo(track.drive).path, CDDA_MESSAGE_FORGETIT, NIL);

		boca.DeleteComponent(info);
	}

	if (drive == NIL) return False;

	cdda_open(drive);

	Int	 startSector = 0;
	Int	 endSector   = 0;

	if (!GetTrackSectors(startSector, endSector)) return False;

	nextSector  = startSector;
	sectorsLeft = endSector - startSector;

	/* Set ripping spped.
	 */
	Config	*config = Config::Get();

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

		paranoia_seek(paranoia, startSector, SEEK_SET);
		paranoia_modeset(paranoia, paranoiaMode);
	}

	return True;
}

Bool BoCA::DecoderCDParanoia::Deactivate()
{
	if (drive == NIL) return False;

	if (paranoia != NIL) paranoia_free(paranoia);

	cdda_close(drive);

	return True;
}

Bool BoCA::DecoderCDParanoia::Seek(Int64 samplePosition)
{
	Int	 startSector = 0;
	Int	 endSector   = 0;

	if (!GetTrackSectors(startSector, endSector)) return False;

	startSector += samplePosition / 588;

	nextSector  = startSector;
	sectorsLeft = endSector - startSector;

	if (paranoia != NIL) paranoia_seek(paranoia, startSector, SEEK_SET);

	return True;
}

Int BoCA::DecoderCDParanoia::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (inBytes >= track.fileSize) return -1;

	Int	 sectors = Math::Min(sectorsLeft, paranoia == NIL ? 26 : 1);

	data.Resize(sectors * 2352);

	if (paranoia != NIL)
	{
		int16_t	*audio = paranoia_read(paranoia, NIL);

		memcpy(data, audio, data.Size());
	}
	else
	{
		cdda_read(drive, data, nextSector, sectors);
	}

	nextSector  += sectors;
	sectorsLeft -= sectors;

	inBytes += data.Size();

	return data.Size();
}

Bool BoCA::DecoderCDParanoia::GetTrackSectors(Int &startSector, Int &endSector)
{
	Int	 numTocEntries = drive->tracks;

	if (track.cdTrack == 0)
	{
		if (!(drive->disc_toc[0].bFlags & 0x04))
		{
			startSector = 0;
			endSector   = drive->disc_toc[0].dwStartSector;

			return True;
		}
	}
	else
	{
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
