 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "cdrip_in.h"
#include "paranoia/cdda_paranoia.h"
#include "config.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::CDRipIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (cdripdll != NIL && ex_CR_GetNumCDROM() >= 1)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>CDRip Ripper Component</name>		\
		    <version>1.0</version>			\
		    <id>cdrip-in</id>				\
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

static Bool	 initializedCDRip = False;

Void smooth::AttachDLL(Void *instance)
{
	LoadCDRipDLL();

	if (cdripdll == NIL) return;

	if (!ex_CR_IsInitialized())
	{
		BoCA::Config	*config = BoCA::Config::Get();

		Long		 error = CDEX_OK;
		OSVERSIONINFOA	 vInfo;

		vInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		GetVersionExA(&vInfo);

		if (vInfo.dwPlatformId != VER_PLATFORM_WIN32_NT) config->SetIntValue("CDRip", "UseNTSCSI", False);

		error = ex_CR_Init(config->GetIntValue("CDRip", "UseNTSCSI", True));

		if (error != CDEX_OK		 && 
		    error != CDEX_ACCESSDENIED	 &&
		    vInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			config->SetIntValue("CDRip", "UseNTSCSI", !config->GetIntValue("CDRip", "UseNTSCSI", True));

			error = ex_CR_Init(config->GetIntValue("CDRip", "UseNTSCSI", True));
		}

		if	(error == CDEX_ACCESSDENIED)	BoCA::Utilities::ErrorMessage("Access to CD-ROM drives was denied by Windows.\n\nPlease contact your system administrator in order\nto be granted the right to access the CD-ROM drive.");
		else if (error != CDEX_OK &&
			 error != CDEX_NOCDROMDEVICES)	BoCA::Utilities::ErrorMessage("Unable to load ASPI drivers! CD ripping disabled!");

		/* ToDo: Remove next line once config->cdrip_numdrives becomes unnecessary.
		 */
		config->cdrip_numdrives = ex_CR_GetNumCDROM();

		if (ex_CR_GetNumCDROM() <= config->GetIntValue("Ripper", "ActiveDrive", 0)) config->SetIntValue("Ripper", "ActiveDrive", 0);

		initializedCDRip = True;
	}
}

Void smooth::DetachDLL()
{
	if (cdripdll == NIL) return;

	if (initializedCDRip) ex_CR_DeInit();

	FreeCDRipDLL();
}

BoCA::CDPlayerIni	 BoCA::CDRipIn::cdPlayer;
Int			 BoCA::CDRipIn::cdPlayerDiscID	= -1;

BoCA::CDText		 BoCA::CDRipIn::cdText;
Int			 BoCA::CDRipIn::cdTextDiscID	= -1;

Bool BoCA::CDRipIn::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.StartsWith("cdda://") ||
	       lcURI.EndsWith(".cda");
}

Error BoCA::CDRipIn::GetStreamInfo(const String &streamURI, Track &track)
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

	if (component == NIL) return Error();

	Config	*config = Config::Get();

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

	track.length	= (trackLength * 1176) / (format.bits / 8);
	track.fileSize	= trackLength * 1176 * format.channels;

	track.cdTrack	= trackNumber;
	track.drive	= audiodrive;
	track.outfile	= NIL;

	info.track	= trackNumber;
	info.numTracks	= info.mcdi.GetNumberOfAudioTracks();
	info.disc	= 1;
	info.numDiscs	= 1;

	/* Delete DeviceInfo component.
	 */
	boca.DeleteComponent(component);

	/* Read CDText and cdplayer.ini
	 */
	{
		Int	 discid = ComputeDiscID();

		if (config->GetIntValue("CDRip", "ReadCDText", True)	  && cdTextDiscID   != discid) { cdText.ReadCDText();   cdTextDiscID   = discid; }
		if (config->GetIntValue("CDRip", "ReadCDPlayerIni", True) && cdPlayerDiscID != discid) { cdPlayer.ReadCDInfo(); cdPlayerDiscID = discid; }

		if (config->GetIntValue("CDRip", "ReadCDText", True) && cdText.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
		{
			const CDInfo	&cdInfo = cdText.GetCDInfo();

			if (cdInfo.GetTrackArtist(trackNumber) != NIL)	info.artist = cdInfo.GetTrackArtist(trackNumber);
			else						info.artist = cdInfo.GetArtist();

			info.title  = cdInfo.GetTrackTitle(trackNumber);
			info.album  = cdInfo.GetTitle();
		}
		else if (config->GetIntValue("CDRip", "ReadCDPlayerIni", True) && cdPlayer.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
		{
			const CDInfo	&cdInfo = cdPlayer.GetCDInfo();

			info.artist = cdInfo.GetArtist();
			info.title  = cdInfo.GetTrackTitle(trackNumber);
			info.album  = cdInfo.GetTitle();
		}
	}

	/* Read ISRC if requested.
	 */
	if (config->GetIntValue("CDRip", "ReadISRC", 0))
	{
		ISRC	 data;

		ex_CR_ReadAndGetISRC(&data, track.cdTrack);

		/* Check if the ISRC is valid.
		 */
		if (data.isrc[0] >= 'A' && data.isrc[0] <= 'Z' &&
		    data.isrc[1] >= 'A' && data.isrc[1] <= 'Z')
		{
			info.isrc = data.isrc;
		}
	}

	track.SetInfo(info);

	return Success();
}

BoCA::CDRipIn::CDRipIn()
{
	configLayer	= NIL;

	packageSize	= 0;
	ripperOpen	= False;

	readOffset	= 0;

	skipSamples	= 0;
	prependSamples	= 0;
}

BoCA::CDRipIn::~CDRipIn()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::CDRipIn::Activate()
{
	Int	 startSector = 0;
	Int	 endSector = 0;

	ex_CR_SetActiveCDROM(track.drive);
	ex_CR_ReadToc();

	Int	 numTocEntries = ex_CR_GetNumTocEntries();
	Int	 entryNumber = -1;

	for (Int i = 0; i < numTocEntries; i++)
	{
		TOCENTRY	 entry = ex_CR_GetTocEntry(i);
		TOCENTRY	 nextentry = ex_CR_GetTocEntry(i + 1);

		startSector = entry.dwStartSector;
		endSector = nextentry.dwStartSector;

		if (!(entry.btFlag & CDROMDATAFLAG) && (entry.btTrackNumber == track.cdTrack))
		{
			entryNumber = i;

			break;
		}
	}

	if (entryNumber == -1) return False;

	OpenRipper(startSector, endSector);

	return True;
}

Bool BoCA::CDRipIn::Deactivate()
{
	CloseRipper();

	return True;
}

Int BoCA::CDRipIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (inBytes >= track.fileSize)
	{
		CloseRipper();

		return -1;
	}

	/* Set up buffer respecting empty samples to prepend.
	 */
	Int	 prependBytes = 4 * prependSamples;
	Int	 skipBytes    = 4 * skipSamples;

	prependSamples = 0;
	skipSamples    = 0;

	data.Resize(dataBufferSize + prependBytes);

	if (prependBytes > 0) data.Zero();

	/* Rip chunk.
	 */
	BOOL	 abort = false;
	LONG	 lSize = size;

	ex_CR_RipChunk(data + prependBytes, &lSize, abort);

	/* Strip samples to skip from the beginning.
	 */
	Int	 dataBytes = lSize + prependBytes - skipBytes;

	if (skipBytes > 0) memmove(data, data + skipBytes, dataBytes);

	inBytes += dataBytes;

	return dataBytes;
}

Bool BoCA::CDRipIn::OpenRipper(Int startSector, Int endSector)
{
	if (!ripperOpen)
	{
		Config		*config = Config::Get();

		CDROMPARAMS	 params;
		int		 nParanoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

		switch (config->GetIntValue("CDRip", "CDParanoiaMode", 3))
		{
			case 0:
				nParanoiaMode = PARANOIA_MODE_OVERLAP;
				break;
			case 1:
				nParanoiaMode &= ~PARANOIA_MODE_VERIFY;
				break;
			case 2:
				nParanoiaMode &= ~(PARANOIA_MODE_SCRATCH | PARANOIA_MODE_REPAIR);
				break;
		}
 
		ex_CR_GetCDROMParameters(&params);

		params.nRippingMode		= config->GetIntValue("CDRip", "CDParanoia", False);
		params.nParanoiaMode		= nParanoiaMode;
		params.bSwapLefRightChannel	= config->GetIntValue("CDRip", "SwapChannels", False);
		params.bJitterCorrection	= config->GetIntValue("CDRip", "JitterCorrection", False);
		params.bDetectJitterErrors	= config->GetIntValue("CDRip", "DetectJitterErrors", True);
		params.bDetectC2Errors		= config->GetIntValue("CDRip", "DetectC2Errors", True);
		params.nSpeed			= config->GetIntValue("Ripper", String("RippingSpeedDrive").Append(String::FromInt(track.drive)), 0);
		params.bEnableMultiRead		= False;
		params.nMultiReadCount		= 0;

		/* Set maximum speed if no limit is requested.
		 */
		if (params.nSpeed == 0) params.nSpeed = 64;

		ex_CR_SetCDROMParameters(&params);

		/* Lock tray if requested.
		 */
		if (config->GetIntValue("CDRip", "LockTray", True)) ex_CR_LockCD(True);

		/* Calculate offset values.
		 */
		readOffset = config->GetIntValue("Ripper", String("ReadOffsetDrive").Append(String::FromInt(track.drive)), 0);

		startSector += readOffset / 588;
		endSector   += readOffset / 588;

		if	(readOffset % 588 < 0) { startSector--; skipSamples = 588 + readOffset % 588; }
		else if (readOffset % 588 > 0) { endSector++;	skipSamples = 	    readOffset % 588; }

		if (startSector < 0) { prependSamples = -startSector * 588; startSector = 0; }

		/* Open ripper.
		 */
		LONG	 lDataBufferSize = dataBufferSize;

		ex_CR_OpenRipper(&lDataBufferSize, startSector, endSector);

		ripperOpen = True;
	}

	return True;
}

Bool BoCA::CDRipIn::CloseRipper()
{
	if (ripperOpen)
	{
		ex_CR_CloseRipper();

		if (Config::Get()->GetIntValue("CDRip", "LockTray", True)) ex_CR_LockCD(False);

		ripperOpen = False;
	}

	return True;
}

ConfigLayer *BoCA::CDRipIn::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCDRip();

	return configLayer;
}

static int cddb_sum(int n)
{
	int	 ret = 0;

	while (n > 0)
	{
		ret = ret + (n % 10);
		n = n / 10;
	}

	return ret;
}

Int BoCA::CDRipIn::ComputeDiscID()
{
	Int	 numTocEntries = ex_CR_GetNumTocEntries();
	Int	 n = 0;

	for (Int i = 0; i < numTocEntries; i++)
	{
		Int	 offset = ex_CR_GetTocEntry(i).dwStartSector + 150;

		n += cddb_sum(offset / 75);
	}

	Int	 t = ex_CR_GetTocEntry(numTocEntries).dwStartSector / 75 - ex_CR_GetTocEntry(0).dwStartSector / 75;

	return ((n % 0xff) << 24 | t << 8 | numTocEntries);
}
