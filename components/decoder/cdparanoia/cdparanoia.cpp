 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

extern "C" {
#	include <cdda_interface.h>
#	include <cdda_paranoia.h>
}

#include "cdparanoia.h"
#include "config.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

const String &BoCA::DecoderCDParanoia::GetComponentSpecs()
{
	static String	 componentSpecs = "			\
								\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
	  <component>						\
	    <name>cdparanoia Ripper Component %VERSION%</name>	\
	    <version>1.0</version>				\
	    <id>cdparanoia-dec</id>				\
	    <type>decoder</type>				\
	    <require>cdparanoia-info</require>			\
	    <format>						\
	      <name>Windows CD Audio Track</name>		\
	      <lossless>true</lossless>				\
	      <extension>cda</extension>			\
	    </format>						\
	  </component>						\
								\
	";

#ifdef PARANOIA_CB_CACHEERR
	componentSpecs.Replace("%VERSION%", String("v").Append(cdda_version()));
#else
	componentSpecs.Replace("%VERSION%", NIL);
#endif
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
#ifdef PARANOIA_CB_CACHEERR
		if (state == PARANOIA_CB_CACHEERR) DecoderCDParanoia::readDecoder->numCacheErrors++;
#endif
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
	appendSamples	= 0;

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

	Int	 speed = config->GetIntValue(ConfigureCDParanoia::ConfigID, String("RippingSpeedDrive").Append(String::FromInt(track.drive)), 0);

	if (speed > 0)	cdda_speed_set(drive, speed);
	else		cdda_speed_set(drive, -1);

	/* Enable paranoia mode.
	 */
	paranoia = NIL;

	if (config->GetIntValue(ConfigureCDParanoia::ConfigID, "CDParanoia", False))
	{
		int	 paranoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

		switch (config->GetIntValue(ConfigureCDParanoia::ConfigID, "CDParanoiaMode", 3))
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
		Bool	 noCacheWarning = config->GetIntValue(ConfigureCDParanoia::ConfigID, "NoCacheWarning", False);

		if (!noCacheWarning)
		{
			MessageDlg	*msgBox = new MessageDlg("The CD-ROM drive appears to be seeking impossibly quickly.\n"
								 "This could be due to timer bugs, a drive that really is improbably fast,\n"
								 "or, most likely, a bug in cdparanoia's cache modelling.\n\n"
								 "Please consider using another drive for ripping audio CDs and send a bug\n"
								 "report to support@freac.org to assist developers in correcting the problem.", "Warning", Message::Buttons::Ok, Message::Icon::Warning, "Do not display this warning again", &noCacheWarning);

			msgBox->ShowDialog();

			config->SetIntValue(ConfigureCDParanoia::ConfigID, "NoCacheWarning", noCacheWarning);
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
	Bool	 lastTrack   = False;

	if (!GetTrackSectors(startSector, endSector, lastTrack)) return False;

	startSector += samplePosition / samplesPerSector;

	/* Calculate offset values.
	 */
	readOffset = config->GetIntValue(ConfigureCDParanoia::ConfigID, String("UseOffsetDrive").Append(String::FromInt(track.drive)), 0) ? config->GetIntValue(ConfigureCDParanoia::ConfigID, String("ReadOffsetDrive").Append(String::FromInt(track.drive)), 0) : 0;

	startSector += readOffset / samplesPerSector;
	endSector   += readOffset / samplesPerSector;

	if	(readOffset % Int(samplesPerSector) < 0) {		   startSector--; skipSamples = samplesPerSector + readOffset % Int(samplesPerSector); }
	else if (readOffset % Int(samplesPerSector) > 0) { if (!lastTrack) endSector++;   skipSamples = 		   readOffset % Int(samplesPerSector); }

	if (startSector < 0) { prependSamples = -startSector * samplesPerSector; startSector = 0; }

	if (lastTrack && readOffset > 0) { appendSamples = readOffset; endSector -= readOffset / samplesPerSector; }

	skipSamples += samplePosition % samplesPerSector;

	/* Set nextSector and sectors left to be read.
	 */
	nextSector  = startSector;
	sectorsLeft = endSector - startSector + 1;

	/* Open ripper.
	 */
	if (paranoia != NIL) paranoia_seek(paranoia, startSector, SEEK_SET);

	inBytes = 4 * samplePosition;

	/* Wait for drive to spin up if requested.
	 */
	Int		 spinUpTime = config->GetIntValue(ConfigureCDParanoia::ConfigID, String("SpinUpTimeDrive").Append(String::FromInt(track.drive)), 0);
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
			Buffer<UnsignedByte>	 buffer(bytesPerSector);

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

		int16_t	*audio = paranoia_read(paranoia, &paranoiaCallback);

		readMutex.Release();

		memcpy(data + prependBytes, audio, sectors * bytesPerSector);
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
	Int	 dataBytes = Math::Min(track.fileSize - inBytes, (Int64) sectors * bytesPerSector + prependBytes - skipBytes);

	if (skipBytes > 0) memmove(data, data + skipBytes, dataBytes);

	inBytes += dataBytes;

	return dataBytes;
}

Bool BoCA::DecoderCDParanoia::GetTrackSectors(Int &startSector, Int &endSector, Bool &lastTrack)
{
	AS::Registry		&boca	   = AS::Registry::Get();
	AS::DeviceInfoComponent	*component = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdparanoia-info");

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

ConfigLayer *BoCA::DecoderCDParanoia::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCDParanoia();

	return configLayer;
}
