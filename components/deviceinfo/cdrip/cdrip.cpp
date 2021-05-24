 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

#include <time.h>

#include "cdrip.h"

const String &BoCA::DeviceInfoCDRip::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (cdripdll != NIL && ex_CR_GetNumCDROM() >= 1)
	{
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>CDRip Ripper Device Info Component</name>	\
		    <version>1.0</version>				\
		    <id>cdrip-info</id>					\
		    <type>deviceinfo</type>				\
		  </component>						\
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
		Long		 error	= ex_CR_Init(True);

		if	(error == CDEX_ACCESSDENIED)   BoCA::Utilities::ErrorMessage("Access to CD-ROM drives was denied by Windows.\n\nPlease contact your system administrator in order\nto be granted the right to access the CD-ROM drive.");
		else if (error != CDEX_OK &&
			 error != CDEX_NOCDROMDEVICES) BoCA::Utilities::ErrorMessage("Unable to load NT SCSI drivers! CD ripping disabled!");

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

namespace BoCA
{
	static Bool OpenDriveTray(Int n)
	{
		Int	 nDrives = ex_CR_GetNumCDROM();

		if (n >= nDrives) return False;

		CDROMDRIVE	*cd = ex_CR_OpenCDROM(n);

		ex_CR_EjectCD(cd, True);
		ex_CR_CloseCDROM(cd);

		return True;
	}
};

BoCA::DeviceInfoCDRip::DeviceInfoCDRip()
{
	CollectDriveInfo();
}

BoCA::DeviceInfoCDRip::~DeviceInfoCDRip()
{
}

Bool BoCA::DeviceInfoCDRip::IsNthDeviceTrayOpen(Int n)
{
	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n >= nDrives) return False;

	CDROMDRIVE	*cd = ex_CR_OpenCDROM(n);
	CDMEDIASTATUS	 status;

	ex_CR_IsMediaLoaded(cd, status);
	ex_CR_CloseCDROM(cd);

	return (status == CDMEDIA_NOT_PRESENT_TRAY_OPEN);
}

Bool BoCA::DeviceInfoCDRip::OpenNthDeviceTray(Int n)
{
	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n >= nDrives) return False;

	NonBlocking1<Int>(&OpenDriveTray).Call(n);

	return True;
}

Bool BoCA::DeviceInfoCDRip::CloseNthDeviceTray(Int n)
{
	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n >= nDrives) return False;

	CDROMDRIVE	*cd = ex_CR_OpenCDROM(n);

	ex_CR_EjectCD(cd, False);
	ex_CR_CloseCDROM(cd);

	return True;
}

const Array<String> &BoCA::DeviceInfoCDRip::GetNthDeviceTrackList(Int n)
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

const BoCA::MCDI &BoCA::DeviceInfoCDRip::GetNthDeviceMCDI(Int n)
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

	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n < nDrives)
	{
		CDROMDRIVE	*cd = ex_CR_OpenCDROM(n);

		if (ex_CR_ReadToc(cd) == CDEX_OK)
		{
			Int			 numTocEntries = ex_CR_GetNumTocEntries(cd);
			Buffer<UnsignedByte>	 buffer(4 + 8 * numTocEntries + 8);

			((UnsignedInt16 *) (UnsignedByte *) buffer)[0] = htons(buffer.Size() - 2);

			buffer[2] = ex_CR_GetTocEntry(cd, 0).btTrackNumber;
			buffer[3] = ex_CR_GetTocEntry(cd, numTocEntries - 1).btTrackNumber;

			for (Int i = 0; i <= numTocEntries; i++)
			{
				TOCENTRY	 entry = ex_CR_GetTocEntry(cd, i);

				buffer[4 + 8 * i + 0] = 0;
				buffer[4 + 8 * i + 1] = entry.btFlag;
				buffer[4 + 8 * i + 2] = entry.btTrackNumber;
				buffer[4 + 8 * i + 3] = 0;

				Int32		 address = entry.dwStartSector;

				if (address < 0) address = ~((-address) - 1) & ((1 << 24) - 1);

				((UnsignedInt32 *) (UnsignedByte *) buffer)[1 + 2 * i + 1] = htonl(address);
			}

			mcdi.SetData(buffer);
		}

		ex_CR_CloseCDROM(cd);
	}

	lastDrive  = n;
	lastAccess = S::System::System::Clock();

	return mcdi;
}

Void BoCA::DeviceInfoCDRip::CollectDriveInfo()
{
	static Bool	 initialized = False;

	if (initialized) return;

	Int	 nDrives = ex_CR_GetNumCDROM();

	for (Int i = 0; i < nDrives; i++)
	{
		CDROMDRIVE	*cd = ex_CR_OpenCDROM(i);
		CDROMPARAMS	 params;

		if (cd == NIL) continue;

		ex_CR_GetCDROMParameters(cd, &params);

		Device	 drive;

		drive.type	  = DEVICE_CDROM;

		drive.vendor	  = String(params.lpszCDROMID).SubString(0,  8).Trim();
		drive.model	  = String(params.lpszCDROMID).SubString(8, 16).Trim();

		drive.path	  = String::FromInt(params.btAdapterID).Append(":").Append(String::FromInt(params.btTargetID)).Append(":").Append(String::FromInt(params.btLunID));

		drive.canOpenTray = True;

		devices.Add(drive);

		ex_CR_CloseCDROM(cd);
	}

	initialized = True;
}
