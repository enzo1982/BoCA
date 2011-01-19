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
}

#include <glob.h>
#include <arpa/inet.h>

#ifdef __linux__
#	include <stropts.h>
#	include <linux/cdrom.h>
#else
#	include <sys/cdio.h>
#endif

#include "cdparanoia_info.h"

static Int	 numDrives = 0;

const String &BoCA::CDParanoiaInfo::GetComponentSpecs()
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

const Array<String> &BoCA::CDParanoiaInfo::FindDrives()
{
#if defined __linux__
	static const char	*deviceNames[] = { "/dev/hd?", "/dev/scd?", NIL };
#elif defined __FreeBSD__
	static const char	*deviceNames[] = { "/dev/acd?", "/dev/cd?", NIL };
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

				if (cd != NIL) driveNames.Add(fileData->gl_pathv[n]);
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
	const Array<String>	&driveNames = BoCA::CDParanoiaInfo::FindDrives();

	numDrives = driveNames.Length();

	/* ToDo: Remove next line once config->cdrip_numdrives becomes unnecessary.
	 */
	config->cdrip_numdrives = numDrives;

	if (numDrives <= config->GetIntValue("Ripper", "ActiveDrive", 0)) config->SetIntValue("Ripper", "ActiveDrive", 0);
}

Void smooth::DetachDLL()
{
}

Bool	 BoCA::CDParanoiaInfo::initialized = False;

BoCA::CDParanoiaInfo::CDParanoiaInfo()
{
	if (!initialized)
	{
		CollectDriveInfo();

		initialized = True;
	}
}

BoCA::CDParanoiaInfo::~CDParanoiaInfo()
{
}

Bool BoCA::CDParanoiaInfo::OpenNthDeviceTray(Int n)
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

Bool BoCA::CDParanoiaInfo::CloseNthDeviceTray(Int n)
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

const Array<String> &BoCA::CDParanoiaInfo::GetNthDeviceTrackList(Int n)
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

const BoCA::MCDI &BoCA::CDParanoiaInfo::GetNthDeviceMCDI(Int n)
{
	static MCDI	 mcdi = MCDI(Buffer<UnsignedByte>());

	/* Do not read the TOC again if the last
	 * request was less than half a second ago.
	 */
	{
		static Int		 lastDrive = -1;
		static UnsignedInt64	 lastAccess = 0;

		UnsignedInt64	 clockValue = S::System::System::Clock();

		if (lastDrive == n && clockValue - lastAccess < 500)
		{
			lastAccess = clockValue;

			return mcdi;
		}

		lastDrive = n;
		lastAccess = clockValue;
	}

	mcdi.SetData(Buffer<UnsignedByte>());

	const Array<String>	&driveNames = FindDrives();
	cdrom_drive		*cd	    = cdda_identify(driveNames.GetNth(n), CDDA_MESSAGE_FORGETIT, NIL);

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

		if (cdda_open(cd) == 0)
		{
			TOC	 toc;

			toc.tocLen	= htons(2 + cd->tracks * 8 + 8);
			toc.firstTrack	= 1;
			toc.lastTrack	= cd->tracks;

			for (Int i = 0; i < cd->tracks; i++)
			{
				toc.tracks[i].rsvd	  = 0;
				toc.tracks[i].ADR	  = cd->disc_toc[i].bFlags;
				toc.tracks[i].trackNumber = i + 1;
				toc.tracks[i].rsvd2	  = 0;
				toc.tracks[i].addr	  = htonl(cd->disc_toc[i].dwStartSector);
			}

			toc.tracks[cd->tracks].rsvd	   = 0;
			toc.tracks[cd->tracks].ADR	   = cd->disc_toc[cd->tracks].bFlags;
			toc.tracks[cd->tracks].trackNumber = 0xAA;
			toc.tracks[cd->tracks].rsvd2	   = 0;
			toc.tracks[cd->tracks].addr	   = htonl(cd->disc_toc[cd->tracks].dwStartSector);

			cdda_close(cd);

			Buffer<UnsignedByte>	 buffer(ntohs(toc.tocLen) + 2);

			memcpy(buffer, &toc, ntohs(toc.tocLen) + 2);

			mcdi.SetData(buffer);
		}
	}

	return mcdi;
}

Void BoCA::CDParanoiaInfo::CollectDriveInfo()
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
	}
}
