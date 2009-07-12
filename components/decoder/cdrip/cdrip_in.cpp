 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::CDRipIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (cdripdll != NIL && Config::Get()->cdrip_numdrives >= 1)
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

Void smooth::AttachDLL(Void *instance)
{
	LoadCDRipDLL();

	if (cdripdll == NIL) return;

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

	if (error == CDEX_OK)
	{
		config->cdrip_numdrives = ex_CR_GetNumCDROM();

		for (int i = 0; i < config->cdrip_numdrives; i++)
		{
			ex_CR_SetActiveCDROM(i);

			CDROMPARAMS	 params;

			ex_CR_GetCDROMParameters(&params);

			config->cdrip_drives.Add(params.lpszCDROMID);
		}

		if (config->cdrip_numdrives <= config->cdrip_activedrive) config->cdrip_activedrive = 0;
	}
}

Void smooth::DetachDLL()
{
	if (cdripdll != NIL) ex_CR_DeInit();

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
	Config	*config = Config::Get();

	Format	&format = track.GetFormat();
	Info	&info = track.GetInfo();

	track.isCDTrack	= True;

	format.channels		= 2;
	format.rate		= 44100;
	format.bits		= 16;
	format.order		= BYTE_INTEL;

	Int	 trackNumber = 0;
	Int	 trackLength = 0;
	Int	 numAudioTracks = 0;
	Int	 audiodrive = 0;

	if (streamURI.StartsWith("cdda://"))
	{
		audiodrive = streamURI.SubString(7, 1).ToInt();
		trackNumber = streamURI.SubString(9, streamURI.Length() - 9).ToInt();
	}
	else if (streamURI.EndsWith(".cda"))
	{
		InStream	*in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

		in->Seek(22);

		trackNumber = in->InputNumber(2);

		in->Seek(32);

		trackLength = in->InputNumber(4);

		delete in;

		for (audiodrive = 0; audiodrive < config->cdrip_numdrives; audiodrive++)
		{
			Bool	 done = False;

			ex_CR_SetActiveCDROM(audiodrive);
			ex_CR_ReadToc();

			Int	 numTocEntries = ex_CR_GetNumTocEntries();

			for (Int j = 0; j < numTocEntries; j++)
			{
				TOCENTRY	 entry = ex_CR_GetTocEntry(j);
				TOCENTRY	 nextentry = ex_CR_GetTocEntry(j + 1);
				Int		 length = nextentry.dwStartSector - entry.dwStartSector;

				if (!(entry.btFlag & CDROMDATAFLAG) && entry.btTrackNumber == trackNumber && length == trackLength)
				{
					done = True;

					break;
				}
			}

			if (done) break;
		}
	}

	if (trackNumber == 0) return Error();

	ex_CR_SetActiveCDROM(audiodrive);
	ex_CR_ReadToc();

	Int	 numTocEntries = ex_CR_GetNumTocEntries();
	Int	 entryNumber = -1;

	for (Int i = 0; i < numTocEntries; i++)
	{
		TOCENTRY	 entry = ex_CR_GetTocEntry(i);
		TOCENTRY	 nextentry = ex_CR_GetTocEntry(i + 1);

		trackLength = nextentry.dwStartSector - entry.dwStartSector;

		if ((i > 0) && (entry.btFlag != nextentry.btFlag) && (nextentry.btTrackNumber != 0xAA))
		{
			trackLength -= 11250;
		}

		if (!(entry.btFlag & CDROMDATAFLAG) && (entry.btTrackNumber == trackNumber))
		{
			entryNumber = i;

			break;
		}
	}

	if (entryNumber == -1) return Error();

	for (Int i = 0; i < numTocEntries; i++)
	{
		TOCENTRY	 entry = ex_CR_GetTocEntry(i);

		if (!(entry.btFlag & CDROMDATAFLAG)) numAudioTracks++;
	}

	track.length	= (trackLength * 2352) / (format.bits / 8);
	track.fileSize	= trackLength * 2352;

	track.cdTrack	= trackNumber;
	track.drive	= audiodrive;
	track.outfile	= NIL;

	info.track	= trackNumber;
	info.numTracks	= numAudioTracks;
	info.disc	= 1;
	info.numDiscs	= 1;

	/* Read CDText and cdplayer.ini
	 */
	{
		Int	 discid = ComputeDiscID();

		if (config->GetIntValue("CDRip", "ReadCDText", True)	  && cdTextDiscID   != discid) { cdText.ReadCDText();   cdTextDiscID   = discid; }
		if (config->GetIntValue("CDRip", "ReadCDPlayerIni", True) && cdPlayerDiscID != discid) { cdPlayer.ReadCDInfo(); cdPlayerDiscID = discid; }

		if (config->GetIntValue("CDRip", "ReadCDText", True) && cdText.GetCDText().Get(trackNumber) != NIL)
		{
			info.artist = cdText.GetCDText().Get(0);
			info.title  = cdText.GetCDText().Get(trackNumber);
			info.album  = cdText.GetCDText().Get(100);
		}
		else if (config->GetIntValue("CDRip", "ReadCDPlayerIni", True) && cdPlayer.GetCDInfo().Get(trackNumber) != NIL)
		{
			info.artist = cdPlayer.GetCDInfo().Get(0);
			info.title  = cdPlayer.GetCDInfo().Get(trackNumber);
			info.album  = cdPlayer.GetCDInfo().Get(100);
		}
	}

	/* Fill MCDI data.
	 */
	{
		info.mcdi.Resize(4 + 8 * numTocEntries + 8);

		((short *) (UnsignedByte *) info.mcdi)[0] = htons(info.mcdi.Size() - 2);

		info.mcdi[2] = 1;
		info.mcdi[3] = numTocEntries;

		for (Int i = 0; i <= numTocEntries; i++)
		{
			TOCENTRY	 entry = ex_CR_GetTocEntry(i);

			info.mcdi[4 + 8 * i + 0] = 0;
			info.mcdi[4 + 8 * i + 1] = entry.btFlag;
			info.mcdi[4 + 8 * i + 2] = entry.btTrackNumber;
			info.mcdi[4 + 8 * i + 3] = 0;

			((unsigned long *) (UnsignedByte *) info.mcdi)[1 + 2 * i + 1] = htonl(entry.dwStartSector);
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

	return Success();
}

BoCA::CDRipIn::CDRipIn()
{
	packageSize	= 0;

	ripperOpen	= False;
}

BoCA::CDRipIn::~CDRipIn()
{
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

	BOOL	 abort = false;

	data.Resize(dataBufferSize);

	ex_CR_RipChunk(data, &size, abort);

	inBytes += size;

	return size;
}

Bool BoCA::CDRipIn::OpenRipper(Int startSector, Int endSector)
{
	if (!ripperOpen)
	{
		Config		*config = Config::Get();

		CDROMPARAMS	 params;
		int		 nParanoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

		switch (config->cdrip_paranoia_mode)
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

		params.nRippingMode		= config->cdrip_paranoia;
		params.nParanoiaMode		= nParanoiaMode;
		params.bSwapLefRightChannel	= config->cdrip_swapchannels;
		params.bJitterCorrection	= config->cdrip_jitter;
		params.bDetectJitterErrors	= config->cdrip_detectJitterErrors;
		params.bDetectC2Errors		= config->cdrip_detectC2Errors;
		params.nSpeed			= config->GetIntValue("CDRip", "RippingSpeed", 0);
		params.bEnableMultiRead		= True;
		params.nMultiReadCount		= 2;

		ex_CR_SetCDROMParameters(&params);

		if (config->GetIntValue("CDRip", "LockTray", True)) ex_CR_LockCD(True);

		ex_CR_OpenRipper(&dataBufferSize, startSector, endSector);

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

int cddb_sum(int n)
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
