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

#include "tocplist.h"

const String &BoCA::TaggerTOCPList::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>.TOC.plist Parser</name>		\
	    <version>1.0</version>			\
	    <id>tocplist-tag</id>			\
	    <type>tagger</type>				\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::TaggerTOCPList::TaggerTOCPList()
{
}

BoCA::TaggerTOCPList::~TaggerTOCPList()
{
}

Error BoCA::TaggerTOCPList::ParseStreamInfo(const String &fileName, Track &track)
{
	String		 pListFile = fileName.Head(fileName.FindLast(Directory::GetDirectoryDelimiter()) + 1).Append(".TOC.plist");

	if (!File(pListFile).Exists()) return Success();

	Bool		 error = False;
	XML::Document	*pList = new XML::Document();

	if (pList->LoadFile(pListFile) == Success())
	{
		XML::Node	*root = pList->GetRootNode();
		XML::Node	*dict = root->GetNodeByName("dict");

		for (Int i = 0; i < dict->GetNOfNodes(); i++)
		{
			/* Look for sessions key.
			 */
			XML::Node	*node = dict->GetNthNode(i);

			if (node->GetName() != "key" || node->GetContent() != "Sessions") continue;

			/* Parse sessions and build MCDI.
			 */
			Buffer<UnsignedByte>	 mcdi(4);
			Int32			 address;
			XML::Node		*array = dict->GetNthNode(++i);

			if (array->GetName() == "array") ReadSessions(array, mcdi, address);

			if (mcdi.Size() > 4)
			{
				((UnsignedByte *) mcdi)[0] = ((mcdi.Size() + 6) >> 8) & 255;
				((UnsignedByte *) mcdi)[1] =  (mcdi.Size() + 6)	      & 255;
				((UnsignedByte *) mcdi)[2] = ((UnsignedByte *) mcdi)[6];
				((UnsignedByte *) mcdi)[3] = ((UnsignedByte *) mcdi)[mcdi.Size() - 6];

				/* Add lead-out entry.
				 */
				Int	 offset = mcdi.Size();

				mcdi.Resize(offset + 8);

				((UnsignedByte *) mcdi)[offset + 0] = 0;
				((UnsignedByte *) mcdi)[offset + 1] = 16;
				((UnsignedByte *) mcdi)[offset + 2] = 170;
				((UnsignedByte *) mcdi)[offset + 3] = 0;

				((UnsignedByte *) mcdi)[offset + 4] = (address >> 24) & 255;
				((UnsignedByte *) mcdi)[offset + 5] = (address >> 16) & 255;
				((UnsignedByte *) mcdi)[offset + 6] = (address >>  8) & 255;
				((UnsignedByte *) mcdi)[offset + 7] =  address 	      & 255;

				/* Set MCDI in track info.
				 */
				Info	 info = track.GetInfo();

				info.track = File(fileName).GetFileName().Head(2).ToInt();

				info.mcdi.SetData(mcdi);

				track.SetInfo(info);
			}
		}
	}

	delete pList;

	if (error) return Error();
	else	   return Success();
}

Bool BoCA::TaggerTOCPList::ReadSessions(XML::Node *sessions, Buffer<UnsignedByte> &mcdi, Int32 &leadOut)
{
	for (Int i = 0; i < sessions->GetNOfNodes(); i++)
	{
		XML::Node	*session = sessions->GetNthNode(i);

		if (session->GetName() == "dict") ReadSession(session, mcdi, leadOut);
	}

	return True;
}

Bool BoCA::TaggerTOCPList::ReadSession(XML::Node *session, Buffer<UnsignedByte> &mcdi, Int32 &leadOut)
{
	for (Int i = 0; i < session->GetNOfNodes(); i++)
	{
		XML::Node	*node = session->GetNthNode(i);

		if (node->GetName() == "key" && node->GetContent() == "Leadout Block") leadOut = session->GetNthNode(++i)->GetContent().ToInt() - 150;

		if (node->GetName() != "key" || node->GetContent() != "Track Array") continue;

		XML::Node	*array = session->GetNthNode(++i);

		if (array->GetName() == "array") ReadTracks(array, mcdi);
	}

	return True;
}

Bool BoCA::TaggerTOCPList::ReadTracks(XML::Node *tracks, Buffer<UnsignedByte> &mcdi)
{
	for (Int i = 0; i < tracks->GetNOfNodes(); i++)
	{
		XML::Node	*track = tracks->GetNthNode(i);

		if (track->GetName() == "dict") ReadTrack(track, mcdi);
	}

	return True;
}

Bool BoCA::TaggerTOCPList::ReadTrack(XML::Node *track, Buffer<UnsignedByte> &mcdi)
{
	Bool	 data	 = False;
	Bool	 preemp	 = False;
	Int	 number	 = 0;
	Int32	 address = 0;

	for (Int i = 0; i < track->GetNOfNodes(); i++)
	{
		XML::Node	*node = track->GetNthNode(i);

		if (node->GetName() == "key")
		{
			if	(node->GetContent() == "Data")		       data    = track->GetNthNode(++i)->GetName() == "true" ? True : False;
			else if (node->GetContent() == "Pre-Emphasis Enabled") preemp  = track->GetNthNode(++i)->GetName() == "true" ? True : False;
			else if (node->GetContent() == "Point")		       number  = track->GetNthNode(++i)->GetContent().ToInt();
			else if (node->GetContent() == "Start Block")	       address = track->GetNthNode(++i)->GetContent().ToInt() - 150;
		}
	}

	Int	 offset = mcdi.Size();

	mcdi.Resize(offset + 8);

	((UnsignedByte *) mcdi)[offset + 0] = 0;
	((UnsignedByte *) mcdi)[offset + 1] = 16 + (data ? 4 : 0) + (preemp ? 1 : 0);
	((UnsignedByte *) mcdi)[offset + 2] = number;
	((UnsignedByte *) mcdi)[offset + 3] = 0;

	((UnsignedByte *) mcdi)[offset + 4] = (address >> 24) & 255;
	((UnsignedByte *) mcdi)[offset + 5] = (address >> 16) & 255;
	((UnsignedByte *) mcdi)[offset + 6] = (address >>  8) & 255;
	((UnsignedByte *) mcdi)[offset + 7] =  address 	      & 255;

	return True;
}
