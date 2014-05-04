// $Id$

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug
// Copyright 2002 Thijmen Klok (thijmen@id3lib.org)

/* This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* The id3lib authors encourage improvements and optimisations to be sent to
 * the id3lib coordinator.  Please see the README file for details on where to
 * send such submissions.  See the AUTHORS file for a list of people who have
 * contributed to id3lib.  See the ChangeLog file for a list of changes to
 * id3lib.  These files are distributed with id3lib at
 * http://download.sourceforge.net/id3lib/
 */

#include "container_impl.h"

#include <id3/io_decorators.h>

using namespace dami;

bool id3::v2::parseFrames(ID3_ContainerImpl &container, ID3_Reader &rdr)
{
	ID3_Reader::pos_type	 beg = rdr.getCur();
	io::ExitTrigger		 et(rdr, beg);
	ID3_Reader::pos_type	 last_pos = beg;
	size_t			 totalSize = 0;
	size_t			 frameSize = 0;

	while (!rdr.atEnd() && rdr.peekChar() != '\0')
	{
		ID3D_NOTICE("id3::v2::parseFrames(): rdr.getBeg() = " << rdr.getBeg());
		ID3D_NOTICE("id3::v2::parseFrames(): rdr.getCur() = " << rdr.getCur());
		ID3D_NOTICE("id3::v2::parseFrames(): rdr.getEnd() = " << rdr.getEnd());

		last_pos = rdr.getCur();

		ID3_Frame	*f = new ID3_Frame;

		f->SetSpec(container.GetSpec());

		bool		 goodParse = f->Parse(rdr);

		frameSize = rdr.getCur() - last_pos;

		ID3D_NOTICE("id3::v2::parseFrames(): frameSize = " << frameSize);

		totalSize += frameSize;

		if (frameSize == 0)
		{
			/* There is a problem.
			 * If the frame size is 0, then we can't progress.
			 */
			ID3D_WARNING("id3::v2::parseFrames(): frame size is 0, can't " << "continue parsing frames");

			delete f;

			/* Break for now.
			 */
			break;
		}
		else if (!goodParse)
		{
			/* Bad parse! We can't attach this frame.
			 */
			ID3D_WARNING("id3::v2::parseFrames(): bad parse, deleting frame");

			delete f;
		}
		else if (f->GetID() != ID3FID_METACOMPRESSION)
		{
			ID3D_NOTICE("id3::v2::parseFrames(): attaching non-compressed " << "frame");

			/* A good, uncompressed frame. Attach away!
			 */
			container.AttachFrame(f);
		}
		else
		{
			ID3D_NOTICE("id3::v2::parseFrames(): parsing ID3v2.2.1 " << "compressed frame");

			/* Hmm. An ID3v2.2.1 compressed frame. It contains 1 or more
			 * compressed frames. Uncompress and call parseFrames recursively.
			 */
			ID3_Field	*fld = f->GetField(ID3FN_DATA);

			if (fld)
			{
				ID3_MemoryReader	 mr(fld->GetRawBinary(), fld->BinSize());
				ID3_Reader::char_type	 ch = mr.readChar();

				if (ch != 'z')
				{
					/* Unknown compression method.
					 */
					ID3D_WARNING("id3::v2::parseFrames(): unknown compression id " << " = '" << ch << "'");
				}
				else
				{
					uint32			 newSize = io::readBENumber(mr, sizeof(uint32));
				//	size_t			 oldSize = f->GetDataSize() - sizeof(uint32) - 1;
					io::CompressedReader	 cr(mr, newSize);

					parseFrames(container, cr);

					if (!cr.atEnd())
					{
						/* Hmm. It didn't parse the entire uncompressed data.
						 * Wonder why.
						 */
						ID3D_WARNING("id3::v2::parseFrames(): didn't parse entire " << "id3v2.2.1 compressed memory stream");
					}
				}
			}

			delete f;
		}

		et.setExitPos(rdr.getCur());
	}

	if (rdr.peekChar() == '\0')
	{
		ID3D_NOTICE("id3::v2::parseFrames: done parsing, padding at postion " << rdr.getCur());
	}
	else
	{
		ID3D_NOTICE("id3::v2::parseFrames: done parsing, [cur, end] = [" << rdr.getCur() << ", " << rdr.getEnd() << "]");
	}

	return true;
}
