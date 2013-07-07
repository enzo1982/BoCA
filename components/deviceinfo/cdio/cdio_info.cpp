 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
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

	for (Int i = 0; deviceNames != NIL && deviceNames[i] != NIL; i++)
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
		CdIo_t	*cd = cdio_open(deviceName, DRIVER_UNKNOWN);

		if (cd != NIL)
		{
			driveNames.Add(deviceName);

			cdio_destroy(cd);
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

	/* Eject.
	 */
	if (cdio_eject_media_drive(driveNames.GetNth(n)) == DRIVER_OP_SUCCESS) return True;
	else								       return False;
}

Bool BoCA::CDIOInfo::CloseNthDeviceTray(Int n)
{
	const Array<String>	&driveNames = FindDrives();

	/* Close tray.
	 */
	if (cdio_close_tray(driveNames.GetNth(n), NIL) == DRIVER_OP_SUCCESS) return True;
	else								     return False;
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
			/* Add CD track to joblist using a device:// URI
			 */
			trackList.Add(String("device://cdda:")
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
	CdIo_t			*cd	    = cdio_open(driveNames.GetNth(n), DRIVER_UNKNOWN);

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

		TOC	 toc;

		toc.tocLen	= htons(2 + cdio_get_num_tracks(cd) * 8 + 8);
		toc.firstTrack	= cdio_get_first_track_num(cd);
		toc.lastTrack	= cdio_get_last_track_num(cd);

		for (Int i = 0; i <= toc.lastTrack - toc.firstTrack; i++)
		{
			toc.tracks[i].rsvd	  = 0;
			toc.tracks[i].ADR	  = (0x01 << 4) | ((cdio_get_track_format(cd, toc.firstTrack + i) != TRACK_FORMAT_AUDIO) << 2) | (cdio_get_track_copy_permit(cd, toc.firstTrack + i) << 1) | (cdio_get_track_preemphasis(cd, toc.firstTrack + i));
			toc.tracks[i].trackNumber = toc.firstTrack + i;
			toc.tracks[i].rsvd2	  = 0;
			toc.tracks[i].addr	  = htonl(cdio_get_track_lsn(cd, toc.firstTrack + i));
		}

		toc.tracks[toc.lastTrack - toc.firstTrack + 1].rsvd	   = 0;
		toc.tracks[toc.lastTrack - toc.firstTrack + 1].ADR	   = (0x01 << 4) | ((cdio_get_track_format(cd, CDIO_CDROM_LEADOUT_TRACK) != TRACK_FORMAT_AUDIO) << 2) | (cdio_get_track_copy_permit(cd, CDIO_CDROM_LEADOUT_TRACK) << 1) | (cdio_get_track_preemphasis(cd, CDIO_CDROM_LEADOUT_TRACK));
		toc.tracks[toc.lastTrack - toc.firstTrack + 1].trackNumber = 0xAA;
		toc.tracks[toc.lastTrack - toc.firstTrack + 1].rsvd2	   = 0;
		toc.tracks[toc.lastTrack - toc.firstTrack + 1].addr	   = htonl(cdio_get_track_lsn(cd, CDIO_CDROM_LEADOUT_TRACK));

		Buffer<UnsignedByte>	 buffer(ntohs(toc.tocLen) + 2);

		memcpy(buffer, &toc, ntohs(toc.tocLen) + 2);

		mcdi.SetData(buffer);

		cdio_destroy(cd);
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
		CdIo_t		*cd = cdio_open(driveName, DRIVER_UNKNOWN);
		cdio_hwinfo_t	 device;

		if (cd == NIL) continue;

		cdio_get_hwinfo(cd, &device);
		
		Device	 drive;

		drive.type = DEVICE_CDROM;
		drive.name = String(device.psz_vendor).Append(" ").Append(device.psz_model).Append(" ").Append(device.psz_revision);

		drive.canOpenTray = True;

		devices.Add(drive);

		cdio_destroy(cd);
	}
}
