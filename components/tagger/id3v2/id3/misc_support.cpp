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

#include <id3/misc_support.h>
#include <id3/field.h>
#include <id3/utils.h>

// using namespace dami;

char *ID3_GetString(const ID3_Frame *frame, ID3_FieldID fldName)
{
	if (frame == NULL) return NULL;

	char		*text = NULL;
	ID3_Field	*fld  = frame->GetField(fldName);

	if (fld != NULL)
	{
		ID3_TextEnc	 enc   = fld->GetEncoding();

		fld->SetEncoding(ID3TE_ISO8859_1);

		size_t		 nText = fld->Size();

		text = new char[nText + 1];

		fld->Get(text, nText + 1);
		fld->SetEncoding(enc);
	}

	return text;
}

char *ID3_GetString(const ID3_Frame *frame, ID3_FieldID fldName, size_t nIndex)
{
	char	*text = NULL;

	if (NULL != frame)
	{
		size_t	 nText = frame->GetField(fldName)->Size();

		text = new char[nText + 1];

		frame->GetField(fldName)->Get(text, nText + 1, nIndex);
	}

	return text;
}

void ID3_FreeString(char *str)
{
	if (str != NULL) delete [] str;
}

char *ID3_GetArtist(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sArtist = NULL;
	ID3_Frame	*frame	 = NULL;

	if ((frame = tag->Find(ID3FID_LEADARTIST)) ||
	    (frame = tag->Find(ID3FID_BAND))       ||
	    (frame = tag->Find(ID3FID_CONDUCTOR))  ||
	    (frame = tag->Find(ID3FID_COMPOSER)))
	{
		sArtist = ID3_GetString(frame, ID3FN_TEXT);
	}

	return sArtist;
}

ID3_Frame *ID3_AddArtist(ID3_Tag *tag, const char *text, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		if (replace) ID3_RemoveArtists(tag);

		if (replace || (tag->Find(ID3FID_LEADARTIST) == NULL &&
				tag->Find(ID3FID_BAND)       == NULL &&
				tag->Find(ID3FID_CONDUCTOR)  == NULL &&
				tag->Find(ID3FID_COMPOSER)   == NULL))
		{
			frame = new ID3_Frame(ID3FID_LEADARTIST);

			if (frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

size_t ID3_RemoveArtists(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_LEADARTIST)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	while ((frame = tag->Find(ID3FID_BAND)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	while ((frame = tag->Find(ID3FID_CONDUCTOR)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	while ((frame = tag->Find(ID3FID_COMPOSER)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetAlbum(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sAlbum = NULL;
	ID3_Frame	*frame  = tag->Find(ID3FID_ALBUM);

	if (frame != NULL) sAlbum = ID3_GetString(frame, ID3FN_TEXT);

	return sAlbum;
}

ID3_Frame *ID3_AddAlbum(ID3_Tag *tag, const char *text, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		if (replace) ID3_RemoveAlbums(tag);

		if (replace || tag->Find(ID3FID_ALBUM) == NULL)
		{
			frame = new ID3_Frame(ID3FID_ALBUM);

			if (frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

size_t ID3_RemoveAlbums(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_ALBUM)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetTitle(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sTitle = NULL;
	ID3_Frame	*frame  = tag->Find(ID3FID_TITLE);

	if (frame != NULL) sTitle = ID3_GetString(frame, ID3FN_TEXT);

	return sTitle;
}

ID3_Frame *ID3_AddTitle(ID3_Tag *tag, const char *text, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		if (replace) ID3_RemoveTitles(tag);

		if (replace || tag->Find(ID3FID_TITLE) == NULL)
		{
			frame = new ID3_Frame(ID3FID_TITLE);

			if (frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

size_t ID3_RemoveTitles(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_TITLE)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetYear(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sYear = NULL;
	ID3_Frame	*frame = tag->Find(ID3FID_YEAR);

	if (frame != NULL) sYear = ID3_GetString(frame, ID3FN_TEXT);

	return sYear;
}

ID3_Frame *ID3_AddYear(ID3_Tag *tag, const char *text, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		if (replace) ID3_RemoveYears(tag);

		if (replace || tag->Find(ID3FID_YEAR) == NULL)
		{
			frame = new ID3_Frame(ID3FID_YEAR);

			if (NULL != frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

size_t ID3_RemoveYears(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_YEAR)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetComment(const ID3_Tag *tag, const char* desc)
{
	if (NULL == tag) return NULL;

	char		*comment = NULL;
	ID3_Frame	*frame	 = NULL;

	if (desc)
	{
		frame = tag->Find(ID3FID_COMMENT, ID3FN_DESCRIPTION, desc);
	}
	else
	{
		frame = tag->Find(ID3FID_COMMENT);

		if (frame == tag->Find(ID3FID_COMMENT, ID3FN_DESCRIPTION, STR_V1_COMMENT_DESC)) frame = tag->Find(ID3FID_COMMENT);
	}

	if (frame) comment = ID3_GetString(frame, ID3FN_TEXT);

	return comment;
}

ID3_Frame *ID3_AddComment(ID3_Tag *tag, const char *text, bool replace)
{
	return ID3_AddComment(tag, text, "", replace);
}

ID3_Frame *ID3_AddComment(ID3_Tag *tag, const char *text, const char *desc, bool replace)
{
	return ID3_AddComment(tag, text, desc, "XXX", replace);
}

ID3_Frame *ID3_AddComment(ID3_Tag *tag, const char *text, const char *desc, const char *lang, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag  &&
	    NULL != text &&
	    NULL != desc && strlen(text) > 0)
	{
		bool	 bAdd = true;

		if (replace)
		{
			ID3_RemoveComments(tag, desc);
		}
		else
		{
			// See if there is already a comment with this description
			ID3_Tag::Iterator	*iter  = tag->CreateIterator();
			ID3_Frame		*frame = NULL;

			while ((frame = iter->GetNext()) != NULL)
			{
				if (frame->GetID() == ID3FID_COMMENT)
				{
					char *tmp_desc = ID3_GetString(frame, ID3FN_DESCRIPTION);

					if (strcmp(tmp_desc, desc) == 0) bAdd = false;

					delete [] tmp_desc;

					if (!bAdd) break;
				}
			}

			delete iter;
		}

		if (bAdd)
		{
			frame = new ID3_Frame(ID3FID_COMMENT);

			if (NULL != frame)
			{
				frame->GetField(ID3FN_LANGUAGE)->Set(lang);
				frame->GetField(ID3FN_DESCRIPTION)->Set(desc);
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

// Remove all comments with the given description (remove all comments if
// desc is NULL)
size_t ID3_RemoveComments(ID3_Tag *tag, const char *desc)
{
	size_t	 num_removed = 0;

	if (NULL == tag) return num_removed;

	ID3_Tag::Iterator	*iter  = tag->CreateIterator();
	ID3_Frame		*frame = NULL;

	while ((frame = iter->GetNext()) != NULL)
	{
		if (frame->GetID() == ID3FID_COMMENT)
		{
			bool	 remove = false;

			// A null description means remove all comments
			if (NULL == desc)
			{
				remove = true;
			}
			else
			{
				// See if the description we have matches the description of the
				// current comment.  If so, set the "remove the comment" flag to true.
				char	*tmp_desc = ID3_GetString(frame, ID3FN_DESCRIPTION);

				remove = (strcmp(tmp_desc, desc) == 0);

				delete [] tmp_desc;
			}

			if (remove)
			{
				frame = tag->RemoveFrame(frame);
				delete frame;
				num_removed++;
			}
		}
	}

	delete iter;

	return num_removed;
}

char *ID3_GetTrack(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sTrack = NULL;
	ID3_Frame	*frame  = tag->Find(ID3FID_TRACKNUM);

	if (frame != NULL) sTrack = ID3_GetString(frame, ID3FN_TEXT);

	return sTrack;
}

size_t ID3_GetTrackNum(const ID3_Tag *tag)
{
	char	*sTrack = ID3_GetTrack(tag);
	size_t	 nTrack = 0;

	if (NULL != sTrack)
	{
		nTrack = atoi(sTrack);
		delete [] sTrack;
	}

	return nTrack;
}

ID3_Frame *ID3_AddTrack(ID3_Tag *tag, uchar trk, uchar ttl, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && trk > 0)
	{
		if (replace) ID3_RemoveTracks(tag);

		if (replace || NULL == tag->Find(ID3FID_TRACKNUM))
		{
			frame = new ID3_Frame(ID3FID_TRACKNUM);

			if (frame)
			{
				char	*sTrack = NULL;

				if (0 == ttl)
				{
					sTrack = new char[4];
					sprintf(sTrack, "%lu", (luint) trk);
				}
				else
				{
					sTrack = new char[8];
					sprintf(sTrack, "%lu/%lu", (luint) trk, (luint) ttl);
				}

				frame->GetField(ID3FN_TEXT)->Set(sTrack);

				delete [] sTrack;

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

// following routine courtesy of John George
int ID3_GetPictureData(const ID3_Tag *tag, const char *TempPicPath)
{
	if (NULL == tag) return 0;

	ID3_Frame	*frame = tag->Find(ID3FID_PICTURE);

	if (frame == NULL) return 0;

	ID3_Field	*myField = frame->GetField(ID3FN_DATA);

	if (myField == NULL) return 0;

	myField->ToFile(TempPicPath);

	return (int) myField->Size();
}

// following routine courtesy of John George
char *ID3_GetPictureMimeType(const ID3_Tag *tag)
{
	  if (NULL == tag) return NULL;

	  char		*sPicMimetype = NULL;
	  ID3_Frame	*frame	      = tag->Find(ID3FID_PICTURE);

	  if (frame != NULL) sPicMimetype = ID3_GetString(frame, ID3FN_MIMETYPE);

	  return sPicMimetype;
}

// following routine courtesy of John George
bool ID3_HasPicture(const ID3_Tag *tag)
{
	if (NULL == tag) return false;

	ID3_Frame	*frame = tag->Find(ID3FID_PICTURE);

	if (frame == NULL) return false;

	ID3_Field	*myField = frame->GetField(ID3FN_DATA);

	if (myField != NULL) return true;
	else		     return false;
}

// following routine courtesy of John George
ID3_Frame *ID3_AddPicture(ID3_Tag *tag, const char *TempPicPath, const char *MimeType, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag)
	{
		if (replace) ID3_RemovePictures(tag);

		if (replace || NULL == tag->Find(ID3FID_PICTURE))
		{
			frame = new ID3_Frame(ID3FID_PICTURE);

			if (NULL != frame)
			{
				frame->GetField(ID3FN_DATA)->FromFile(TempPicPath);
				frame->GetField(ID3FN_MIMETYPE)->Set(MimeType);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

// following routine courtesy of John George
size_t ID3_RemovePictures(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_PICTURE)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

// following routine courtesy of John George
size_t ID3_RemovePictureType(ID3_Tag* tag, ID3_PictureType pictype)
{
	size_t		 bremoved = 0;
	ID3_Frame	*frame	  = NULL;

	if (NULL == tag) return bremoved;

	ID3_Tag::Iterator	*iter = tag->CreateIterator();

	while (NULL != (frame = iter->GetNext()))
	{
		if (frame->GetID() == ID3FID_PICTURE)
		{
			if (frame->GetField(ID3FN_PICTURETYPE)->Get() == (uint32) pictype) break;
		}
	}

	delete iter;

	if (NULL != frame)
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		bremoved = 1;
	}

	return bremoved;
}

// following routine courtesy of John George
ID3_Frame *ID3_AddPicture(ID3_Tag *tag, const char *TempPicPath, const char *MimeType, ID3_PictureType pictype, const char *Description, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag)
	{
		if (replace) ID3_RemovePictureType(tag, pictype);

		if (replace || NULL == tag->Find(ID3FID_PICTURE, ID3FN_PICTURETYPE, (uint32)pictype))
		{
			frame = new ID3_Frame(ID3FID_PICTURE);

			if (NULL != frame)
			{
				frame->GetField(ID3FN_DATA)->FromFile(TempPicPath);
				frame->GetField(ID3FN_MIMETYPE)->Set(MimeType);
				frame->GetField(ID3FN_PICTURETYPE)->Set((uint32)pictype);
				frame->GetField(ID3FN_DESCRIPTION)->Set(Description);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

// following routine courtesy of John George
size_t ID3_GetPictureDataOfPicType(ID3_Tag *tag, const char *TempPicPath, ID3_PictureType pictype)
{
	if (NULL == tag) return 0;

	ID3_Frame		*frame = NULL;
	ID3_Tag::Iterator	*iter  = tag->CreateIterator();

	while (NULL != (frame = iter->GetNext() ))
	{
		if(frame->GetID() == ID3FID_PICTURE)
		{
			if(frame->GetField(ID3FN_PICTURETYPE)->Get() == (uint32) pictype) break;
		}
	}

	delete iter;

	if (frame == NULL) return 0;

	ID3_Field	*myField = frame->GetField(ID3FN_DATA);

	if (myField == NULL) return 0;

	myField->ToFile(TempPicPath);

	return (size_t) myField->Size();
}

// following routine courtesy of John George
char *ID3_GetMimeTypeOfPicType(ID3_Tag *tag, ID3_PictureType pictype)
{
	if (NULL == tag) return NULL;

	char			*sPicMimetype = NULL;
	ID3_Frame		*frame	      = NULL;
	ID3_Tag::Iterator	*iter	      = tag->CreateIterator();

	while (NULL != (frame = iter->GetNext()))
	{
		if(frame->GetID() == ID3FID_PICTURE)
		{
			if(frame->GetField(ID3FN_PICTURETYPE)->Get() == (uint32) pictype) break;
		}
	}

	delete iter;

	if (frame != NULL) sPicMimetype = ID3_GetString(frame, ID3FN_MIMETYPE);

	return sPicMimetype;
}

// following routine courtesy of John George
char *ID3_GetDescriptionOfPicType(ID3_Tag *tag, ID3_PictureType pictype)
{
	if (NULL == tag) return NULL;

	char			*sPicDescription = NULL;
	ID3_Frame		*frame		 = NULL;
	ID3_Tag::Iterator	*iter		 = tag->CreateIterator();

	while (NULL != (frame = iter->GetNext()))
	{
		if(frame->GetID() == ID3FID_PICTURE)
		{
			if(frame->GetField(ID3FN_PICTURETYPE)->Get() == (uint32)pictype) break;
		}
	}

	delete iter;

	if (frame != NULL) sPicDescription = ID3_GetString(frame, ID3FN_DESCRIPTION);

	return sPicDescription;
}


size_t ID3_RemoveTracks(ID3_Tag* tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_TRACKNUM)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetGenre(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sGenre = NULL;
	ID3_Frame	*frame	= tag->Find(ID3FID_CONTENTTYPE);

	if (frame != NULL) sGenre = ID3_GetString(frame, ID3FN_TEXT);

	return sGenre;
}

size_t ID3_GetGenreNum(const ID3_Tag *tag)
{
	char	*sGenre	 = ID3_GetGenre(tag);
	size_t	 ulGenre = 0xFF;

	if (NULL == sGenre) return ulGenre;

	// If the genre string begins with "(ddd)", where "ddd" is a number, then
	// "ddd" is the genre number---get it
	if (sGenre[0] == '(')
	{
		char	*pCur = &sGenre[1];

		while (isdigit(*pCur)) pCur++;

		if (*pCur == ')')
		{
			// if the genre number is greater than 255, its invalid.
			ulGenre = dami::min(0xFF, atoi(&sGenre[1]));
		}
	}

	delete [] sGenre;

	return ulGenre;
}

// following routine courtesy of John George
ID3_Frame *ID3_AddGenre(ID3_Tag *tag, const char *genre, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && NULL != genre && strlen(genre) > 0)
	{
		if (replace) ID3_RemoveGenres(tag);

		if (replace || NULL == tag->Find(ID3FID_CONTENTTYPE))
		{
			frame = new ID3_Frame(ID3FID_CONTENTTYPE);

			if (NULL != frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(genre);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

ID3_Frame *ID3_AddGenre(ID3_Tag *tag, size_t genreNum, bool replace)
{
	if (0xFF != genreNum)
	{
		char	 sGenre[6];

		sprintf(sGenre, "(%lu)", (luint) genreNum);

		return(ID3_AddGenre(tag, sGenre, replace));
	}
	else
	{
		return(NULL);
	}
}

size_t ID3_RemoveGenres(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_CONTENTTYPE)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetLyrics(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sLyrics = NULL;
	ID3_Frame	*frame	 = tag->Find(ID3FID_UNSYNCEDLYRICS);

	if (frame != NULL) sLyrics = ID3_GetString(frame, ID3FN_TEXT);

	return sLyrics;
}

ID3_Frame *ID3_AddLyrics(ID3_Tag *tag, const char *text, bool replace)
{
	return ID3_AddLyrics(tag, text, "", replace);
}

ID3_Frame *ID3_AddLyrics(ID3_Tag *tag, const char *text, const char *desc, bool replace)
{
	return ID3_AddLyrics(tag, text, desc, "XXX", replace);
}

ID3_Frame *ID3_AddLyrics(ID3_Tag *tag, const char *text, const char *desc, const char *lang, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && strlen(text) > 0)
	{
		if (replace) ID3_RemoveLyrics(tag);

		if (replace || tag->Find(ID3FID_UNSYNCEDLYRICS) == NULL)
		{
			frame = new ID3_Frame(ID3FID_UNSYNCEDLYRICS);

			if (NULL != frame)
			{
				frame->GetField(ID3FN_LANGUAGE)->Set(lang);
				frame->GetField(ID3FN_DESCRIPTION)->Set(desc);
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

size_t ID3_RemoveLyrics(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_UNSYNCEDLYRICS)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

char *ID3_GetLyricist(const ID3_Tag *tag)
{
	if (NULL == tag) return NULL;

	char		*sLyricist = NULL;
	ID3_Frame	*frame	   = tag->Find(ID3FID_LYRICIST);

	if (frame != NULL) sLyricist = ID3_GetString(frame, ID3FN_TEXT);

	return sLyricist;
}

ID3_Frame *ID3_AddLyricist(ID3_Tag *tag, const char *text, bool replace)
{
	ID3_Frame	*frame = NULL;

	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		if (replace) ID3_RemoveLyricist(tag);

		if (replace || (tag->Find(ID3FID_LYRICIST) == NULL))
		{
			frame = new ID3_Frame(ID3FID_LYRICIST);

			if (frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);

				if (!tag->AttachFrame(frame)) return NULL;
			}
		}
	}

	return frame;
}

size_t ID3_RemoveLyricist(ID3_Tag *tag)
{
	size_t		 num_removed = 0;
	ID3_Frame	*frame	     = NULL;

	if (NULL == tag) return num_removed;

	while ((frame = tag->Find(ID3FID_LYRICIST)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
}

ID3_Frame *ID3_AddSyncLyrics(ID3_Tag *tag, const uchar *data, size_t datasize, ID3_TimeStampFormat format, bool replace)
{
	return ID3_AddSyncLyrics(tag, data, datasize, format, "", replace);
}

ID3_Frame *ID3_AddSyncLyrics(ID3_Tag *tag, const uchar *data, size_t datasize, ID3_TimeStampFormat format, const char *desc, bool replace)
{
	return ID3_AddSyncLyrics(tag, data, datasize, format, desc, "XXX", replace);
}

ID3_Frame *ID3_AddSyncLyrics(ID3_Tag *tag, const uchar *data, size_t datasize, ID3_TimeStampFormat format, const char *desc, const char *lang, bool replace)
{
	return ID3_AddSyncLyrics(tag, data, datasize, format, desc, lang, ID3CT_LYRICS, replace);
}

ID3_Frame *ID3_AddSyncLyrics(ID3_Tag *tag, const uchar *data, size_t datasize, ID3_TimeStampFormat format, const char *desc, const char *lang, ID3_ContentType type, bool replace)
{
	if (tag == NULL) return NULL;

	ID3_Frame	*frame = NULL;

	// language and descriptor should be mandatory
	if ((lang == NULL) || (desc == NULL)) return NULL;

	// check if a SYLT frame of this language or descriptor already exists
	ID3_Frame	*frmExist = tag->Find(ID3FID_SYNCEDLYRICS, ID3FN_LANGUAGE, lang);

	if (!frmExist) frmExist = tag->Find(ID3FID_SYNCEDLYRICS, ID3FN_DESCRIPTION, desc);

	if (data != NULL)
	{
		if (replace && frmExist)
		{
			frmExist = tag->RemoveFrame (frmExist);

			delete frmExist;

			frmExist = NULL;
		}

		// if the frame still exist, cannot continue
		if (frmExist) return NULL;

		frame = new ID3_Frame(ID3FID_SYNCEDLYRICS);

		if (frame != NULL)
		{
			frame->GetField(ID3FN_LANGUAGE)->Set(lang);
			frame->GetField(ID3FN_DESCRIPTION)->Set(desc);
			frame->GetField(ID3FN_TIMESTAMPFORMAT)->Set(format);
			frame->GetField(ID3FN_CONTENTTYPE)->Set(type);
			frame->GetField(ID3FN_DATA)->Set(data, datasize);

			if (!tag->AttachFrame(frame)) return NULL;
		}
	}

	return frame;
}

ID3_Frame *ID3_GetSyncLyricsInfo(const ID3_Tag *tag, const char *desc, const char *lang, ID3_TimeStampFormat &format, ID3_ContentType &type, size_t &size)
{
	// check if a SYLT frame of this language or descriptor exists
	ID3_Frame	*frmExist = NULL;

	if (NULL != lang)
	{
		// search through language
		frmExist = tag->Find(ID3FID_SYNCEDLYRICS, ID3FN_LANGUAGE, lang);
	}
	else if (NULL != desc)
	{
		// search through descriptor
		frmExist = tag->Find(ID3FID_SYNCEDLYRICS, ID3FN_DESCRIPTION, desc);
	}
	else
	{
		// both language and description not specified, search the first SYLT frame
		frmExist = tag->Find(ID3FID_SYNCEDLYRICS);
	}

	if (!frmExist) return NULL;

	// get the lyrics time stamp format
	format = static_cast<ID3_TimeStampFormat>(frmExist->GetField(ID3FN_TIMESTAMPFORMAT)->Get ());

	// get the lyrics content type
	type = static_cast<ID3_ContentType>(frmExist->GetField(ID3FN_CONTENTTYPE)->Get ());

	// get the lyrics size
	size = frmExist->GetField (ID3FN_DATA)->Size ();

	// return the frame pointer for further uses
	return frmExist;
}

ID3_Frame *ID3_GetSyncLyrics(const ID3_Tag *tag, const char *lang, const char *desc, const uchar *&pData, size_t &size)
{
	// check if a SYLT frame of this language or descriptor exists
	ID3_Frame	*frmExist = NULL;

	if (NULL != lang)
	{
		// search through language
		frmExist = tag->Find(ID3FID_SYNCEDLYRICS, ID3FN_LANGUAGE, lang);
	}
	else if (NULL != desc)
	{
		// search through descriptor
		frmExist = tag->Find(ID3FID_SYNCEDLYRICS, ID3FN_DESCRIPTION, desc);
	}
	else
	{
		// both language and description not specified, search the first SYLT frame
		frmExist = tag->Find(ID3FID_SYNCEDLYRICS);
	}

	if (NULL == frmExist) return NULL;

	// get the lyrics size
	size = dami::min(size, frmExist->GetField(ID3FN_DATA)->Size());

	// get the lyrics data
	pData = frmExist->GetField (ID3FN_DATA)->GetRawBinary();

	// return the frame pointer for further uses
	return frmExist;
}
