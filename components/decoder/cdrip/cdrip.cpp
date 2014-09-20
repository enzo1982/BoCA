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

#include "cdrip.h"
#include "paranoia/cdda_paranoia.h"
#include "config.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

const String &BoCA::DecoderCDRip::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (cdripdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>CDRip Ripper Component</name>		\
		    <version>1.0</version>			\
		    <id>cdrip-dec</id>				\
		    <type>decoder</type>	\
		    <require>cdrip-info</require>		\
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
}

Void smooth::DetachDLL()
{
	FreeCDRipDLL();
}

BoCA::CDPlayerIni	 BoCA::DecoderCDRip::cdPlayer;
Int			 BoCA::DecoderCDRip::cdPlayerDiscID = -1;

BoCA::CDText		 BoCA::DecoderCDRip::cdText;
Int			 BoCA::DecoderCDRip::cdTextDiscID   = -1;

Array<UnsignedInt64>	 BoCA::DecoderCDRip::lastRead;

Bool BoCA::DecoderCDRip::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.StartsWith("device://cdda:") ||
	       lcURI.EndsWith(".cda");
}

Error BoCA::DecoderCDRip::GetStreamInfo(const String &streamURI, Track &track)
{
	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

	if (component == NIL) return Error();

	Config	*config = Config::Get();

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

	/* Read CDText and cdplayer.ini
	 */
	{
		Int	 discid = ComputeDiscID();

		if (config->GetIntValue("Ripper", "ReadCDText", True)	  && cdTextDiscID   != discid) { cdText.ReadCDText();   cdTextDiscID   = discid; }
		if (config->GetIntValue("Ripper", "ReadCDPlayerIni", True) && cdPlayerDiscID != discid) { cdPlayer.ReadCDInfo(); cdPlayerDiscID = discid; }

		if (config->GetIntValue("Ripper", "ReadCDText", True) && cdText.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
		{
			const CDInfo	&cdInfo = cdText.GetCDInfo();

			if (cdInfo.GetTrackArtist(trackNumber) != NIL)	info.artist = cdInfo.GetTrackArtist(trackNumber);
			else						info.artist = cdInfo.GetArtist();

			info.title  = cdInfo.GetTrackTitle(trackNumber);
			info.album  = cdInfo.GetTitle();
		}
		else if (config->GetIntValue("Ripper", "ReadCDPlayerIni", True) && cdPlayer.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
		{
			const CDInfo	&cdInfo = cdPlayer.GetCDInfo();

			info.artist = cdInfo.GetArtist();
			info.title  = cdInfo.GetTrackTitle(trackNumber);
			info.album  = cdInfo.GetTitle();
		}
	}

	/* Read ISRC if requested.
	 */
	if (config->GetIntValue("Ripper", "ReadISRC", 0))
	{
		ISRC	 data;

		ex_CR_ReadAndGetISRC(&data, track.cdTrack);

		/* Check if the ISRC is valid.
		 */
		if (Info::IsISRC(data.isrc)) info.isrc = data.isrc;
	}

	track.SetInfo(info);

	/* Delete DeviceInfo component.
	 */
	boca.DeleteComponent(component);

	return Success();
}

BoCA::DecoderCDRip::DecoderCDRip()
{
	configLayer	= NIL;

	packageSize	= 0;

	readOffset	= 0;

	skipSamples	= 0;
	prependSamples	= 0;

	dataBufferSize	= 0;

	lastRead.EnableLocking();
}

BoCA::DecoderCDRip::~DecoderCDRip()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderCDRip::Activate()
{
	Config	*config = Config::Get();

	if (lastRead.Length() == 0) for (Int i = 0; i < ex_CR_GetNumCDROM(); i++) lastRead.Add(0);

	/* Get paranoia mode.
	 */
	int	 nParanoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

	switch (config->GetIntValue("Ripper", "CDParanoiaMode", 3))
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

	/* Change drive parameters.
	 */
	CDROMPARAMS	 params;

	ex_CR_SetActiveCDROM(track.drive);
	ex_CR_GetCDROMParameters(&params);

	params.nRippingMode		= config->GetIntValue("Ripper", "CDParanoia", False);
	params.nParanoiaMode		= nParanoiaMode;
	params.bSwapLefRightChannel	= config->GetIntValue("Ripper", "SwapChannels", False);
	params.bJitterCorrection	= config->GetIntValue("Ripper", "JitterCorrection", False);
//	params.bDetectJitterErrors	= config->GetIntValue("Ripper", "DetectJitterErrors", True);
//	params.bDetectC2Errors		= config->GetIntValue("Ripper", "DetectC2Errors", True);
	params.nSpeed			= config->GetIntValue("Ripper", String("RippingSpeedDrive").Append(String::FromInt(track.drive)), 0);
	params.bEnableMultiRead		= False;
	params.nMultiReadCount		= 0;

	/* Set maximum speed if no limit is requested.
	 */
	if (params.nSpeed == 0) params.nSpeed = 64;

	ex_CR_SetCDROMParameters(&params);

	/* Lock tray if requested.
	 */
	if (config->GetIntValue("Ripper", "LockTray", True)) ex_CR_LockCD(True);

	/* Call seek to initialize ripper to start of track.
	 */
	Seek(0);

	return True;
}

Bool BoCA::DecoderCDRip::Deactivate()
{
	Config	*config = Config::Get();

	/* Check for drive cache errors.
	 */
	if (ex_CR_GetNumberOfCacheErrors() > 0)
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

	ex_CR_CloseRipper();

	/* Unlock tray if previously locked.
	 */
	if (config->GetIntValue("Ripper", "LockTray", True)) ex_CR_LockCD(False);

	return True;
}

Bool BoCA::DecoderCDRip::Seek(Int64 samplePosition)
{
	Config	*config = Config::Get();

	ex_CR_CloseRipper();

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

	/* Open ripper.
	 */
	LONG	 lDataBufferSize = 0;

	ex_CR_OpenRipper(&lDataBufferSize, startSector, endSector);

	dataBufferSize = lDataBufferSize;

	/* Wait for drive to spin up if requested.
	 */
	Int		 spinUpTime = config->GetIntValue("Ripper", String("SpinUpTimeDrive").Append(String::FromInt(track.drive)), 0);
	UnsignedInt64	 startTime  = S::System::System::Clock();

	while (spinUpTime > 0 && startTime - lastRead.GetNth(track.drive) > 2500 && S::System::System::Clock() - startTime < (UnsignedInt64) Math::Abs(spinUpTime * 1000))
	{
		Buffer<UnsignedByte>	 buffer(dataBufferSize);

		BOOL	 abort = false;
		LONG	 lSize = dataBufferSize;

		ex_CR_RipChunk(buffer, &lSize, abort);

		ex_CR_CloseRipper();
		ex_CR_OpenRipper(&lDataBufferSize, startSector, endSector);
	}

	return True;
}

Int BoCA::DecoderCDRip::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (inBytes >= track.fileSize) return -1;

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

	lastRead.SetNth(track.drive, S::System::System::Clock());

	/* Strip samples to skip from the beginning.
	 */
	Int	 dataBytes = lSize + prependBytes - skipBytes;

	if (skipBytes > 0) memmove(data, data + skipBytes, dataBytes);

	inBytes += dataBytes;

	return dataBytes;
}

Bool BoCA::DecoderCDRip::GetTrackSectors(Int &startSector, Int &endSector)
{
	ex_CR_ReadToc();

	Int	 numTocEntries = ex_CR_GetNumTocEntries();

	if (track.cdTrack == 0)
	{
		/* Special handling for hidden track one audio.
		 */
		TOCENTRY	 entry = ex_CR_GetTocEntry(0);

		if (!(entry.btFlag & CDROMDATAFLAG))
		{
			startSector = 0;
			endSector   = entry.dwStartSector;

			return True;
		}
	}
	else
	{
		/* Handle regular tracks.
		 */
		for (Int i = 0; i < numTocEntries; i++)
		{
			TOCENTRY	 entry	   = ex_CR_GetTocEntry(i);
			TOCENTRY	 nextentry = ex_CR_GetTocEntry(i + 1);

			if (!(entry.btFlag & CDROMDATAFLAG) && (entry.btTrackNumber == track.cdTrack))
			{
				startSector = entry.dwStartSector;
				endSector   = nextentry.dwStartSector;

				return True;
			}
		}
	}

	return False;
}

ConfigLayer *BoCA::DecoderCDRip::GetConfigurationLayer()
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

Int BoCA::DecoderCDRip::ComputeDiscID()
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
