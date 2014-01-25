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
}

#include <glob.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#ifdef __linux__
#	include <linux/cdrom.h>
#else
#	include <sys/cdio.h>
#endif

#include "cdparanoia.h"

static Int	 numDrives = 0;

const String &BoCA::DeviceInfoCDParanoia::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (numDrives >= 1)
	{
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>cdparanoia Device Info Component</name>	\
		    <version>1.0</version>				\
		    <id>cdrip-info</id>					\
		    <type>deviceinfo</type>				\
		  </component>						\
									\
		";
	}

	return componentSpecs;
}

const Array<String> &BoCA::DeviceInfoCDParanoia::FindDrives()
{
#if defined __linux__
	static const char	*deviceNames[] = { "/dev/hd?", "/dev/sr?", "/dev/scd?", NIL };
#elif defined __FreeBSD__
	static const char	*deviceNames[] = { "/dev/acd?", "/dev/cd?", NIL };
#elif defined __OpenBSD__
	static const char	*deviceNames[] = { "/dev/cd?c", NIL };
#elif defined __NetBSD__
	static const char	*deviceNames[] = { "/dev/cd?d", NIL };
#else
	static const char	*deviceNames[] = { "/dev/cdrom?", NIL };
#endif

	static Array<String>	 driveNames;
	static Bool		 initialized = False;

	if (initialized) return driveNames;

	for (Int i = 0; deviceNames[i] != NIL; i++)
	{
		glob_t	*fileData = new glob_t;

		if (glob(deviceNames[i], 0, NIL, fileData) == 0)
		{
			for (UnsignedInt n = 0; n < fileData->gl_pathc; n++)
			{
				cdrom_drive	*cd = cdda_identify(fileData->gl_pathv[n], CDDA_MESSAGE_FORGETIT, NIL);

				if (cd != NIL)
				{
					driveNames.Add(fileData->gl_pathv[n]);

					cdda_close(cd);
				}
			}
		}

		globfree(fileData);
	}

	initialized = True;

	return driveNames;
}

Void smooth::AttachDLL(Void *instance)
{
	BoCA::Config		*config	    = BoCA::Config::Get();
	const Array<String>	&driveNames = BoCA::DeviceInfoCDParanoia::FindDrives();

	numDrives = driveNames.Length();

	/* ToDo: Remove next line once config->cdrip_numdrives becomes unnecessary.
	 */
	config->cdrip_numdrives = numDrives;

	if (numDrives <= config->GetIntValue("Ripper", "ActiveDrive", 0)) config->SetIntValue("Ripper", "ActiveDrive", 0);
}

Void smooth::DetachDLL()
{
}

Bool	 BoCA::DeviceInfoCDParanoia::initialized = False;

BoCA::DeviceInfoCDParanoia::DeviceInfoCDParanoia()
{
	if (!initialized)
	{
		CollectDriveInfo();

		initialized = True;
	}
}

BoCA::DeviceInfoCDParanoia::~DeviceInfoCDParanoia()
{
}

Bool BoCA::DeviceInfoCDParanoia::OpenNthDeviceTray(Int n)
{
	const Array<String>	&driveNames = FindDrives();
	cdrom_drive		*cd	    = cdda_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

	if (cd != NIL)
	{
		cdda_open(cd);

		/* Unlock tray, then eject.
		 */
#if defined __linux__
		ioctl(cd->ioctl_fd, CDROM_LOCKDOOR, 0);
		ioctl(cd->ioctl_fd, CDROMEJECT);
#else
		ioctl(cd->ioctl_fd, CDIOCALLOW);
		ioctl(cd->ioctl_fd, CDIOCEJECT);
#endif

		cdda_close(cd);

		return True;
	}

	return False;
}

Bool BoCA::DeviceInfoCDParanoia::CloseNthDeviceTray(Int n)
{
	const Array<String>	&driveNames = FindDrives();
	cdrom_drive		*cd	    = cdda_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

	if (cd != NIL)
	{
		cdda_open(cd);

		/* Close tray.
		 */
#if defined __linux__
		ioctl(cd->ioctl_fd, CDROMCLOSETRAY);
#else
		ioctl(cd->ioctl_fd, CDIOCCLOSE);
#endif

		cdda_close(cd);

		return True;
	}

	return False;
}

const Array<String> &BoCA::DeviceInfoCDParanoia::GetNthDeviceTrackList(Int n)
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
			/* Look for hidden track before the first track (HTOA).
			 */
			if (i == 0 && mcdi.GetNthEntryOffset(i) >= 450)
			{
				trackList.Add(String("device://cdda:")
					     .Append(String::FromInt(n))
					     .Append("/")
					     .Append(String::FromInt(0)));
			}

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

const BoCA::MCDI &BoCA::DeviceInfoCDParanoia::GetNthDeviceMCDI(Int n)
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
	cdrom_drive		*cd	    = cdda_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

	if (cd != NIL)
	{
		typedef struct
		{
			uint8_t		 rsvd;
			uint8_t		 ADR;
			uint8_t		 trackNumber;
			uint8_t		 rsvd2;
			uint32_t	 addr;
		} __attribute__((__packed__)) TOCTRACK;

		typedef struct
		{
			uint16_t	 tocLen;
			uint8_t		 firstTrack;
			uint8_t		 lastTrack;
			TOCTRACK	 tracks[100];
		} __attribute__((__packed__)) TOC;

		if (cdda_open(cd) == 0)
		{
			TOC	 toc;

			toc.tocLen	= htons(2 + cd->tracks * 8 + 8);
			toc.firstTrack	= 1;
			toc.lastTrack	= cd->tracks;

			for (Int i = 0; i <= cd->tracks; i++)
			{
				toc.tracks[i].rsvd	  = 0;
				toc.tracks[i].ADR	  = cd->disc_toc[i].bFlags;
				toc.tracks[i].trackNumber = cd->disc_toc[i].bTrack;
				toc.tracks[i].rsvd2	  = 0;
				toc.tracks[i].addr	  = htonl(cd->disc_toc[i].dwStartSector);

#ifndef __FreeBSD__
				/* cdparanoia accounts for the session gap on mixed mode discs, so we need to undo that to build the
				 * correct MCDI. The FreeBSD port has the corresponding code removed, so this is not necessary there.
				 */
				if ((i > 1 && (cd->disc_toc[i - 1].bFlags & 4) != (cd->disc_toc[i].bFlags & 4) && cd->disc_toc[i].bTrack != 0xAA) ||
				    (i < cd->tracks && cd->disc_toc[i + 1].dwStartSector - cd->disc_toc[i].dwStartSector <= 0))
				{
					toc.tracks[i].addr = htonl(cd->disc_toc[i].dwStartSector + 11400);
				}
#endif
			}

			Buffer<UnsignedByte>	 buffer(ntohs(toc.tocLen) + 2);

			memcpy(buffer, &toc, ntohs(toc.tocLen) + 2);

			mcdi.SetData(buffer);
		}

		cdda_close(cd);
	}

	lastDrive  = n;
	lastAccess = S::System::System::Clock();

	return mcdi;
}

Void BoCA::DeviceInfoCDParanoia::CollectDriveInfo()
{
	const Array<String>	&driveNames = FindDrives();

	foreach (const String &driveName, driveNames)
	{
		cdrom_drive	*cd = cdda_identify(driveName, CDDA_MESSAGE_FORGETIT, NIL);

		if (cd == NIL) continue;

		Device	 drive;

		drive.type = DEVICE_CDROM;
		drive.name = cd->drive_model;

		drive.canOpenTray = True;

		devices.Add(drive);

		cdda_close(cd);
	}
}
