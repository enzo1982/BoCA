 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>CDRip Ripper Component %VERSION%</name>	\
		    <version>1.0</version>				\
		    <id>cdrip-dec</id>					\
		    <type>decoder</type>				\
		    <require>cdrip-info</require>			\
		    <format>						\
		      <name>Windows CD Audio Track</name>		\
		      <lossless>true</lossless>				\
		      <extension>cda</extension>			\
		    </format>						\
		  </component>						\
									\
		";

		LONG	 cdripVersion = ex_CR_GetCDRipVersion();

		componentSpecs.Replace("%VERSION%", String("v").Append(String::FromInt(cdripVersion / 100)).Append(".").Append(String::FromInt(cdripVersion % 100)));
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

	const Config	*config = GetConfiguration();

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
		audiodrive  = streamURI.SubString(14, 1).ToInt();
		trackNumber = streamURI.SubString(16, streamURI.Length() - 16).ToInt();
	}
	else if (streamURI.EndsWith(".cda"))
	{
		/* Find track number and length.
		 */
		{
			InStream	 in(STREAM_FILE, streamURI, IS_READ);

			in.Seek(22); trackNumber = in.InputNumber(2);
			in.Seek(32); trackLength = in.InputNumber(4);
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

	track.length	 = trackLength * samplesPerSector;
	track.fileSize	 = trackLength * bytesPerSector;

	track.cdTrack	 = trackNumber;
	track.drive	 = audiodrive;
	track.outputFile = NIL;

	info.track	 = trackNumber;
	info.numTracks	 = info.mcdi.GetNumberOfAudioTracks();

	/* Read CD-Text and cdplayer.ini.
	 */
	Int	 discid = ComputeDiscID(info.mcdi);

	if (config->GetIntValue(ConfigureCDRip::ConfigID, "ReadCDText", True)	   && cdTextDiscID   != discid) { cdText.ReadCDText(track.drive);   cdTextDiscID   = discid; }
	if (config->GetIntValue(ConfigureCDRip::ConfigID, "ReadCDPlayerIni", True) && cdPlayerDiscID != discid) { cdPlayer.ReadCDInfo(track.drive); cdPlayerDiscID = discid; }

	if (config->GetIntValue(ConfigureCDRip::ConfigID, "ReadCDText", True) && cdText.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
	{
		const CDInfo	&cdInfo = cdText.GetCDInfo();

		if (cdInfo.GetTrackArtist(trackNumber) != NIL)	info.artist = cdInfo.GetTrackArtist(trackNumber);
		else						info.artist = cdInfo.GetArtist();

		info.title  = cdInfo.GetTrackTitle(trackNumber);
		info.album  = cdInfo.GetTitle();
	}
	else if (config->GetIntValue(ConfigureCDRip::ConfigID, "ReadCDPlayerIni", True) && cdPlayer.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
	{
		const CDInfo	&cdInfo = cdPlayer.GetCDInfo();

		info.artist = cdInfo.GetArtist();
		info.title  = cdInfo.GetTrackTitle(trackNumber);
		info.album  = cdInfo.GetTitle();
	}

	/* Read ISRC if requested.
	 */
	if (config->GetIntValue(ConfigureCDRip::ConfigID, "ReadISRC", 0))
	{
		CDROMDRIVE	*cd = ex_CR_OpenCDROM(track.drive);

		if (cd != NIL)
		{
			ISRC	 data;

			ex_CR_ReadAndGetISRC(cd, &data, track.cdTrack);

			/* Check if the ISRC is valid.
			 */
			if (Info::IsISRC(data.isrc)) info.isrc = data.isrc;

			ex_CR_CloseCDROM(cd);
		}
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

	cd		= NIL;

	readOffset	= 0;

	skipSamples	= 0;
	prependSamples	= 0;
	appendSamples	= 0;

	dataBufferSize	= 0;

	lastRead.EnableLocking();
}

BoCA::DecoderCDRip::~DecoderCDRip()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderCDRip::Activate()
{
	const Config	*config = GetConfiguration();

	if (lastRead.Length() == 0) for (Int i = 0; i < ex_CR_GetNumCDROM(); i++) lastRead.Add(0);

	/* Get paranoia mode.
	 */
	int	 nParanoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

	switch (config->GetIntValue(ConfigureCDRip::ConfigID, "CDParanoiaMode", 3))
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
	cd = ex_CR_OpenCDROM(track.drive);

	if (cd == NIL) return False;

	CDROMPARAMS	 params;

	ex_CR_GetCDROMParameters(cd, &params);

	params.nRippingMode		= config->GetIntValue(ConfigureCDRip::ConfigID, "CDParanoia", False);
	params.nParanoiaMode		= nParanoiaMode;
	params.bSwapLefRightChannel	= config->GetIntValue(ConfigureCDRip::ConfigID, "SwapChannels", False);
	params.bJitterCorrection	= config->GetIntValue(ConfigureCDRip::ConfigID, "JitterCorrection", False);
//	params.bDetectJitterErrors	= config->GetIntValue(ConfigureCDRip::ConfigID, "DetectJitterErrors", True);
//	params.bDetectC2Errors		= config->GetIntValue(ConfigureCDRip::ConfigID, "DetectC2Errors", True);
	params.nSpeed			= config->GetIntValue(ConfigureCDRip::ConfigID, String("RippingSpeedDrive").Append(String::FromInt(track.drive)), 0);
	params.bEnableMultiRead		= False;
	params.nMultiReadCount		= 0;

	/* Set maximum speed if no limit is requested.
	 */
	if (params.nSpeed == 0) params.nSpeed = 64;

	ex_CR_SetCDROMParameters(cd, &params);

	/* Lock tray if requested.
	 */
	if (config->GetIntValue(ConfigureCDRip::ConfigID, "LockTray", True)) ex_CR_LockCD(cd, True);

	/* Call seek to initialize ripper to start of track.
	 */
	if (!Seek(0)) return False;

	return True;
}

Bool BoCA::DecoderCDRip::Deactivate()
{
	const Config	*config = GetConfiguration();

	/* Check for drive cache errors.
	 */
	if (ex_CR_GetNumberOfCacheErrors(cd) > 0)
	{
		Config	*config		= Config::Get();
		Bool	 noCacheWarning = config->GetIntValue(ConfigureCDRip::ConfigID, "NoCacheWarning", False);

		if (!noCacheWarning)
		{
			MessageDlg	*msgBox = new MessageDlg("The CD-ROM drive appears to be seeking impossibly quickly.\n"
								 "This could be due to timer bugs, a drive that really is improbably fast,\n"
								 "or, most likely, a bug in cdparanoia's cache modelling.\n\n"
								 "Please consider using another drive for ripping audio CDs and send a bug\n"
								 "report to support@freac.org to assist developers in correcting the problem.", "Warning", Message::Buttons::Ok, Message::Icon::Warning, "Do not display this warning again", &noCacheWarning);

			msgBox->ShowDialog();

			config->SetIntValue(ConfigureCDRip::ConfigID, "NoCacheWarning", noCacheWarning);
			config->SaveSettings();

			Object::DeleteObject(msgBox);
		}
	}

	ex_CR_CloseRipper(cd);

	/* Unlock tray if previously locked.
	 */
	if (config->GetIntValue(ConfigureCDRip::ConfigID, "LockTray", True)) ex_CR_LockCD(cd, False);

	ex_CR_CloseCDROM(cd);

	return True;
}

Bool BoCA::DecoderCDRip::Seek(Int64 samplePosition)
{
	const Config	*config = GetConfiguration();

	ex_CR_CloseRipper(cd);

	Int	 startSector = 0;
	Int	 endSector   = 0;
	Bool	 lastTrack   = False;

	if (!GetTrackSectors(startSector, endSector, lastTrack)) return False;

	startSector += samplePosition / samplesPerSector;

	/* Calculate offset values.
	 */
	readOffset = config->GetIntValue(ConfigureCDRip::ConfigID, String("UseOffsetDrive").Append(String::FromInt(track.drive)), 0) ? config->GetIntValue(ConfigureCDRip::ConfigID, String("ReadOffsetDrive").Append(String::FromInt(track.drive)), 0) : 0;

	startSector += readOffset / samplesPerSector;
	endSector   += readOffset / samplesPerSector;

	if	(readOffset % Int(samplesPerSector) < 0) {		   startSector--; skipSamples = samplesPerSector + readOffset % Int(samplesPerSector); }
	else if (readOffset % Int(samplesPerSector) > 0) { if (!lastTrack) endSector++;   skipSamples = 		   readOffset % Int(samplesPerSector); }

	if (startSector < 0) { prependSamples = -startSector * samplesPerSector; startSector = 0; }

	if (lastTrack && readOffset > 0) { appendSamples = readOffset; endSector -= readOffset / samplesPerSector; }

	skipSamples += samplePosition % samplesPerSector;

	/* Open ripper.
	 */
	LONG	 bufferSize = 0;

	ex_CR_OpenRipper(cd, &bufferSize, startSector, endSector);

	dataBufferSize = bufferSize;

	inBytes = 4 * samplePosition;

	/* Wait for drive to spin up if requested.
	 */
	Int		 spinUpTime = config->GetIntValue(ConfigureCDRip::ConfigID, String("SpinUpTimeDrive").Append(String::FromInt(track.drive)), 0);
	UnsignedInt64	 startTime  = S::System::System::Clock();

	while (spinUpTime > 0 && startTime - lastRead.GetNth(track.drive) > 2500 && S::System::System::Clock() - startTime < (UnsignedInt64) Math::Abs(spinUpTime * 1000))
	{
		Buffer<UnsignedByte>	 buffer(dataBufferSize);

		BOOL	 abort = false;
		LONG	 bytes = dataBufferSize;

		ex_CR_RipChunk(cd, buffer, &bytes, abort);

		ex_CR_CloseRipper(cd);
		ex_CR_OpenRipper(cd, &bufferSize, startSector, endSector);
	}

	return True;
}

Int BoCA::DecoderCDRip::ReadData(Buffer<UnsignedByte> &data)
{
	if (inBytes >= track.fileSize) return -1;

	/* Set up buffer respecting empty samples to prepend.
	 */
	Int	 prependBytes = 4 * prependSamples;
	Int	 appendBytes  = 4 * appendSamples;
	Int	 skipBytes    = 4 * skipSamples;

	prependSamples = 0;
	skipSamples    = 0;

	data.Resize(dataBufferSize + prependBytes);

	if (prependBytes > 0) data.Zero();

	/* Rip chunk.
	 */
	BOOL		 abort = false;
	LONG		 bytes = dataBufferSize;
	CDEX_ERR	 error = ex_CR_RipChunk(cd, data + prependBytes, &bytes, abort);

	if (error != CDEX_OK) return -1;

	lastRead.SetNth(track.drive, S::System::System::Clock());

	/* Append offset bytes to last track.
	 */
	if (bytes == 0)
	{
		data.Resize(prependBytes + appendBytes - skipBytes);
		data.Zero();

		inBytes += data.Size();

		return data.Size();
	}

	/* Strip samples to skip from the beginning.
	 */
	Int	 dataBytes = Math::Min(track.fileSize - inBytes, (Int64) bytes + prependBytes - skipBytes);

	if (skipBytes > 0) memmove(data, data + skipBytes, dataBytes);

	inBytes += dataBytes;

	return dataBytes;
}

Bool BoCA::DecoderCDRip::GetTrackSectors(Int &startSector, Int &endSector, Bool &lastTrack)
{
	AS::Registry		&boca	   = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

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

Int BoCA::DecoderCDRip::ComputeDiscID(const MCDI &mcdi)
{
	Int	 numTocEntries = mcdi.GetNumberOfEntries();
	Int	 n = 0;

	for (Int i = 0; i < numTocEntries; i++)
	{
		Int	 offset = mcdi.GetNthEntryOffset(i) + 150;

		n += cddb_sum(offset / 75);
	}

	Int	 t = mcdi.GetNthEntryOffset(numTocEntries) / 75 - mcdi.GetNthEntryOffset(0) / 75;

	return ((n % 0xff) << 24 | t << 8 | numTocEntries);
}
