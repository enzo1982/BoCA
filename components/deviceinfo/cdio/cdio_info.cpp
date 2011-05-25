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

extern "C" {
#	define DO_NOT_WANT_PARANOIA_COMPATIBILITY
#	include <cdio/cdda.h>
}

#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>

#ifndef PATH_MAX
#	define PATH_MAX 32768
#endif

#include "cdio_info.h"

static Int	 numDrives = 0;

const String &BoCA::CDIOInfo::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (numDrives >= 1)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>CDIO Device Info Component</name>	\
		    <version>1.0</version>			\
		    <id>cdrip-info</id>				\
		    <type>deviceinfo</type>			\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

const Array<String> &BoCA::CDIOInfo::FindDrives()
{
	static Array<String>	 driveNames;
	static Bool		 initialized = False;

	if (initialized) return driveNames;

#ifndef __NetBSD__
	char	**deviceNames = cdio_get_devices(DRIVER_DEVICE);
#else
	char	*deviceNames[3] = { "/dev/cd0d", "/dev/cd1d", NIL };
#endif

	for (Int i = 0; deviceNames[i] != NIL; i++)
	{
		String		 deviceName = deviceNames[i];

		/* Resolve link if it is one.
		 */
		Buffer<char>	 buffer(PATH_MAX + 1);

		buffer.Zero();

		if (readlink(deviceName, buffer, buffer.Size() - 1) >= 0)
		{
			if (buffer[0] == '/') deviceName = buffer;
			else		      deviceName = String(File(deviceName).GetFilePath()).Append("/").Append(buffer);
		}

		/* Check if we aleady know this device.
		 */
		foreach (const String &driveName, driveNames)
		{
			if (driveName == deviceName)
			{
				deviceName = NIL;

				break;
			}
		}

		if (deviceName == NIL) continue;

		/* Try to open device and add it to the list.
		 */
		cdrom_drive_t	*cd = cdio_cddap_identify(deviceName, CDDA_MESSAGE_FORGETIT, NIL);

		if (cd != NIL)
		{
			driveNames.Add(deviceName);

			cdio_cddap_close(cd);
		}
	}

#ifndef __NetBSD__
	cdio_free_device_list(deviceNames);
#endif

	initialized = True;

	return driveNames;
}

Void smooth::AttachDLL(Void *instance)
{
	BoCA::Config		*config	    = BoCA::Config::Get();
	const Array<String>	&driveNames = BoCA::CDIOInfo::FindDrives();

	numDrives = driveNames.Length();

	/* ToDo: Remove next line once config->cdrip_numdrives becomes unnecessary.
	 */
	config->cdrip_numdrives = numDrives;

	if (numDrives <= config->GetIntValue("Ripper", "ActiveDrive", 0)) config->SetIntValue("Ripper", "ActiveDrive", 0);
}

Void smooth::DetachDLL()
{
}

Bool	 BoCA::CDIOInfo::initialized = False;

BoCA::CDIOInfo::CDIOInfo()
{
	if (!initialized)
	{
		CollectDriveInfo();

		initialized = True;
	}
}

BoCA::CDIOInfo::~CDIOInfo()
{
}

Bool BoCA::CDIOInfo::OpenNthDeviceTray(Int n)
{
	const Array<String>	&driveNames = FindDrives();
	cdrom_drive_t		*cd	    = cdio_cddap_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

	if (cd != NIL)
	{
		int	 device_name_length = strlen(cd->cdda_device_name);
		char	 device_name[device_name_length + 1];

		strncpy(device_name, cd->cdda_device_name, device_name_length + 1);

		cdio_cddap_close(cd);

		/* Eject.
		 */
		cdio_eject_media_drive(device_name);

		return True;
	}

	return False;
}

Bool BoCA::CDIOInfo::CloseNthDeviceTray(Int n)
{
	const Array<String>	&driveNames = FindDrives();
	cdrom_drive_t		*cd	    = cdio_cddap_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

	if (cd != NIL)
	{
		int	 device_name_length = strlen(cd->cdda_device_name);
		char	 device_name[device_name_length + 1];

		strncpy(device_name, cd->cdda_device_name, device_name_length + 1);

		cdio_cddap_close(cd);

		/* Close tray.
		 */
		cdio_close_tray(device_name, NIL);

		return True;
	}

	return False;
}

const Array<String> &BoCA::CDIOInfo::GetNthDeviceTrackList(Int n)
{
	static Array<String>	 trackList;

	trackList.RemoveAll();

	const MCDI	&mcdi = GetNthDeviceMCDI(n);

	for (Int i = 0; i < mcdi.GetNumberOfEntries(); i++)
	{
		/* Find all valid audio tracks (tracks with a positive length).
		 */
		if (mcdi.GetNthEntryType(i) == ENTRY_AUDIO && mcdi.GetNthEntryOffset(i + 1) - mcdi.GetNthEntryOffset(i) > 0)
		{
			/* Add CD track to joblist using a cdda:// URI
			 */
			trackList.Add(String("cdda://")
				     .Append(String::FromInt(n))
				     .Append("/")
				     .Append(String::FromInt(mcdi.GetNthEntryTrackNumber(i))));
		}
	}

	return trackList;
}

const BoCA::MCDI &BoCA::CDIOInfo::GetNthDeviceMCDI(Int n)
{
	static MCDI	 mcdi = MCDI(Buffer<UnsignedByte>());

	/* Do not read the TOC again if the last request
	 * was less than a quarter of a second ago.
	 */
	static Int		 lastDrive  = -1;
	static UnsignedInt64	 lastAccess = 0;

	{
		UnsignedInt64	 clockValue = S::System::System::Clock();

		if (lastDrive == n && clockValue - lastAccess < 250)
		{
			lastAccess = clockValue;

			return mcdi;
		}
	}

	mcdi.SetData(Buffer<UnsignedByte>());

	const Array<String>	&driveNames = FindDrives();
	cdrom_drive_t		*cd	    = cdio_cddap_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

	if (cd != NIL)
	{
		typedef struct
		{
			unsigned char	 rsvd;
			unsigned char	 ADR;
			unsigned char	 trackNumber;
			unsigned char	 rsvd2;
			int32_t		 addr;
		} __attribute__((__packed__)) TOCTRACK;

		typedef struct
		{
			unsigned short	 tocLen;
			unsigned char	 firstTrack;
			unsigned char	 lastTrack;
			TOCTRACK	 tracks[100];
		} __attribute__((__packed__)) TOC;

		if (cdio_cddap_open(cd) == 0)
		{
			TOC	 toc;

			toc.tocLen	= htons(2 + cd->tracks * 8 + 8);
			toc.firstTrack	= 1;
			toc.lastTrack	= cd->tracks;

			for (Int i = 0; i < cd->tracks; i++)
			{
				toc.tracks[i].rsvd	  = 0;
				toc.tracks[i].ADR	  = (0x01 << 4) | (!cdio_cddap_track_audiop(cd, cd->disc_toc[i].bTrack) << 2) | (cdio_cddap_track_copyp(cd, cd->disc_toc[i].bTrack) << 1) | (cdio_cddap_track_preemp(cd, cd->disc_toc[i].bTrack));
				toc.tracks[i].trackNumber = cd->disc_toc[i].bTrack;
				toc.tracks[i].rsvd2	  = 0;
				toc.tracks[i].addr	  = htonl(cd->disc_toc[i].dwStartSector);
			}

			toc.tracks[cd->tracks].rsvd	   = 0;
			toc.tracks[cd->tracks].ADR	   = (0x01 << 4) | (!cdio_cddap_track_audiop(cd, 0xAA) << 2) | (cdio_cddap_track_copyp(cd, 0xAA) << 1) | (cdio_cddap_track_preemp(cd, 0xAA));
			toc.tracks[cd->tracks].trackNumber = 0xAA;
			toc.tracks[cd->tracks].rsvd2	   = 0;
			toc.tracks[cd->tracks].addr	   = htonl(cd->disc_toc[cd->tracks].dwStartSector);

			Buffer<UnsignedByte>	 buffer(ntohs(toc.tocLen) + 2);

			memcpy(buffer, &toc, ntohs(toc.tocLen) + 2);

			mcdi.SetData(buffer);
		}

		cdio_cddap_close(cd);
	}

	lastDrive  = n;
	lastAccess = S::System::System::Clock();

	return mcdi;
}

Void BoCA::CDIOInfo::CollectDriveInfo()
{
	const Array<String>	&driveNames = FindDrives();

	foreach (const String &driveName, driveNames)
	{ 
		cdrom_drive_t	*cd = cdio_cddap_identify(driveName, CDDA_MESSAGE_FORGETIT, NIL);

		if (cd == NIL) continue;

		Device	 drive;

		drive.type = DEVICE_CDROM;
		drive.name = cd->drive_model;

		drive.canOpenTray = True;

		devices.Add(drive);

		cdio_cddap_close(cd);
	}
}
