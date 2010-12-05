 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
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

#include "cdparanoia_in.h"
#include "config.h"

using namespace smooth::IO;

static Int	 numDrives = 0;

const String &BoCA::CDParanoiaIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (numDrives >= 1)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>cdparanoia Ripper Component</name>	\
		    <version>1.0</version>			\
		    <id>cdparanoia-in</id>			\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Windows CD Audio Track</name>	\
		      <extension>cda</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	BoCA::Config	*config = BoCA::Config::Get();

	for (Int i = 0; i < 16; i++)
	{
		cdrom_drive	*cd = cdda_identify(String("/dev/cdrom").Append(i > 0 ? String::FromInt(i) : String(NIL)), CDDA_MESSAGE_FORGETIT, NIL);

		if (cd == NIL) break;

		numDrives++;
	}

	/* ToDo: Remove next line once config->cdrip_numdrives becomes unnecessary.
	 */
	config->cdrip_numdrives = numDrives;

	if (numDrives <= config->GetIntValue("Ripper", "ActiveDrive", 0)) config->SetIntValue("Ripper", "ActiveDrive", 0);
}

Void smooth::DetachDLL()
{
}

Bool BoCA::CDParanoiaIn::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.StartsWith("cdda://") ||
	       lcURI.EndsWith(".cda");
}

Error BoCA::CDParanoiaIn::GetStreamInfo(const String &streamURI, Track &track)
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

	if (component == NIL) return Error();

	track.isCDTrack	= True;

	Format	 format;

	format.channels		= 2;
	format.rate		= 44100;
	format.bits		= 16;
	format.order		= BYTE_INTEL;

	track.SetFormat(format);

	Int	 trackNumber = 0;
	Int	 trackLength = 0;
	Int	 audiodrive = 0;

	if (streamURI.StartsWith("cdda://"))
	{
		audiodrive = streamURI.SubString(7, 1).ToInt();
		trackNumber = streamURI.SubString(9, streamURI.Length() - 9).ToInt();
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

	if (trackNumber == 0) return Error();

	/* Fill MCDI data.
	 */
	Info	 info;

	info.mcdi = component->GetNthDeviceMCDI(audiodrive);

	Int	 entryNumber = -1;

	for (Int i = 0; i < info.mcdi.GetNumberOfEntries(); i++)
	{
		trackLength = info.mcdi.GetNthEntryOffset(i + 1) - info.mcdi.GetNthEntryOffset(i);

		/* Strip 11250 sectors off of the track length if
		 * we are the last audio track before a new session.
		 */
		if ((i > 0 && info.mcdi.GetNthEntryType(i) != info.mcdi.GetNthEntryType(i + 1) && info.mcdi.GetNthEntryTrackNumber(i + 1) != 0xAA) ||
		    (i < info.mcdi.GetNumberOfEntries() - 1 && info.mcdi.GetNthEntryOffset(i + 2) - info.mcdi.GetNthEntryType(i + 1) <= 0))
		{
			trackLength -= 11250;
		}

		if (info.mcdi.GetNthEntryType(i) == ENTRY_AUDIO && info.mcdi.GetNthEntryTrackNumber(i) == trackNumber)
		{
			entryNumber = i;

			break;
		}
	}

	if (entryNumber == -1) return Error();

	track.length	= (trackLength * 2352) / (format.bits / 8);
	track.fileSize	= trackLength * 2352;

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

BoCA::CDParanoiaIn::CDParanoiaIn()
{
	configLayer	= NIL;

	packageSize	= 0;
}

BoCA::CDParanoiaIn::~CDParanoiaIn()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::CDParanoiaIn::Activate()
{
	Int	 startSector = 0;
	Int	 endSector = 0;

	drive = cdda_identify(String("/dev/cdrom").Append(track.drive > 0 ? String::FromInt(track.drive) : String(NIL)), CDDA_MESSAGE_FORGETIT, NIL);

	if (drive == NIL) return False;

	cdda_open(drive);

	Int	 numTocEntries = drive->tracks;
	Int	 entryNumber = -1;

	for (Int i = 0; i < numTocEntries; i++)
	{
		startSector = drive->disc_toc[i].dwStartSector;
		endSector = drive->disc_toc[i + 1].dwStartSector;

		if (!(drive->disc_toc[i].bFlags & 0x04) && (drive->disc_toc[i].bTrack == track.cdTrack))
		{
			nextSector  = startSector;
			sectorsLeft = endSector - startSector;

			entryNumber = i;

			break;
		}
	}

	if (entryNumber == -1) return False;

	Config	*config = Config::Get();

	Int	 speed = config->GetIntValue("Ripper", "RippingSpeed", 0);

	if (speed > 0)	cdda_speed_set(drive, speed);
	else		cdda_speed_set(drive, -1);

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

Bool BoCA::CDParanoiaIn::Deactivate()
{
	if (drive == NIL) return False;

	if (paranoia != NIL) paranoia_free(paranoia);

	cdda_close(drive);

	return True;
}

Int BoCA::CDParanoiaIn::ReadData(Buffer<UnsignedByte> &data, Int size)
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

ConfigLayer *BoCA::CDParanoiaIn::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCDParanoia();

	return configLayer;
}
