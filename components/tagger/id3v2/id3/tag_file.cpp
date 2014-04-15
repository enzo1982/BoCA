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

#include <stdio.h>  // for BUFSIZ and functions remove & rename
#include <id3/writers.h>
#include <id3/io_strings.h>
#include "tag_impl.h"

using namespace dami;

#if !defined HAVE_MKSTEMP
#	include <stdio.h>
#endif

#if defined HAVE_UNISTD_H
#	include <unistd.h>
#endif

#if defined HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif

#if defined WIN32
#	include <windows.h>

static int truncate(const char *path, size_t length)
{
	int	 result = -1;
	HANDLE	 fh;

	fh = ::CreateFileA(path,
			   GENERIC_WRITE | GENERIC_READ,
			   0,
			   NULL,
			   OPEN_EXISTING,
			   FILE_ATTRIBUTE_NORMAL,
			   NULL);

	if (INVALID_HANDLE_VALUE != fh)
	{
		SetFilePointer(fh, length, NULL, FILE_BEGIN);
		SetEndOfFile(fh);
		CloseHandle(fh);

		result = 0;
	}

	return result;
}

#elif defined(macintosh)

static int truncate(const char *path, size_t length)
{
	/* Not implemented on the Mac.
	 */
	return -1;
}

#endif

size_t ID3_TagImpl::Link(const char *fileInfo, bool parseID3v1, bool parseLyrics3)
{
	flags_t	 tt = ID3TT_NONE;

	if (parseID3v1)	  tt |= ID3TT_ID3V1;
	if (parseLyrics3) tt |= ID3TT_LYRICS;

	return this->Link(fileInfo, tt);
}

size_t ID3_TagImpl::Link(const char *fileInfo, flags_t tag_types)
{
	_tags_to_parse.set(tag_types);

	if (NULL == fileInfo) return 0;

	_file_name = fileInfo;
	_changed = true;

	this->ParseFile();

	return this->GetPrependedBytes();
}

/* Used for streaming:
 */
size_t ID3_TagImpl::Link(ID3_Reader &reader, flags_t tag_types)
{
	_tags_to_parse.set(tag_types);

	_file_name = "";
	_changed = true;

	this->ParseReader(reader);

	return this->GetPrependedBytes();
}

size_t RenderV1ToFile(ID3_TagImpl &tag, fstream &file)
{
  if (!file) return 0;

  // Heck no, this is stupid.  If we do not read in an initial V1(.1)
  // header then we are constantly appending new V1(.1) headers. Files
  // can get very big that way if we never overwrite the old ones.
  //  if (ID3_V1_LEN > tag.GetAppendedBytes())   - Daniel Hazelbaker
  if (ID3_V1_LEN > tag.GetFileSize())
  {
    file.seekp(0, ios::end);
  }
  else
  {
    // We want to check if there is already an id3v1 tag, so we can write over
    // it.  First, seek to the beginning of any possible id3v1 tag
    file.seekg(0-ID3_V1_LEN, ios::end);
    char sID[ID3_V1_LEN_ID];

    // Read in the TAG characters
    file.read(sID, ID3_V1_LEN_ID);

    // If those three characters are TAG, then there's a preexisting id3v1 tag,
    // so we should set the file cursor so we can overwrite it with a new tag.
    if (memcmp(sID, "TAG", ID3_V1_LEN_ID) == 0)
    {
      file.seekp(0-ID3_V1_LEN, ios::end);
    }
    // Otherwise, set the cursor to the end of the file so we can append on
    // the new tag.
    else
    {
      file.seekp(0, ios::end);
    }
  }

  ID3_IOStreamWriter out(file);

  id3::v1::render(out, tag);

  return ID3_V1_LEN;
}

size_t RenderV2ToFile(const ID3_TagImpl &tag, fstream &file)
{
  ID3_Err err = ID3E_NoError;

  ID3D_NOTICE( "RenderV2ToFile: starting" );
  if (!file)
  {
    ID3D_WARNING("RenderV2ToFile: error in file");
    return 0;
  }

  String tagString;
  io::StringWriter writer(tagString);
  err = id3::v2::render(writer, tag);
  if (err != ID3E_NoError)
  {
    return (size_t)err; //impossible size, will make caller be able to set _last_error
  }

  ID3D_NOTICE( "RenderV2ToFile: rendered v2" );

  const char* tagData = tagString.data();
  size_t tagSize = tagString.size();
  // if the new tag fits perfectly within the old and the old one
  // actually existed (ie this isn't the first tag this file has had)
  if ((!tag.GetPrependedBytes() && !ID3_GetDataSize(tag)) ||
      (tagSize == tag.GetPrependedBytes()))
  {
    file.seekp(0, ios::beg);
    file.write(tagData, tagSize);
  }
  else
  {
    String filename = tag.GetFileName();
    String sTmpSuffix = ".XXXXXX";
    if (filename.size() + sTmpSuffix.size() >= ID3_PATH_LENGTH)
    {
      /* TODO: log this
       */
      return (size_t)ID3E_NoFile;
    }
    char sTempFile[ID3_PATH_LENGTH];
    strcpy(sTempFile, filename.c_str());
    strcat(sTempFile, sTmpSuffix.c_str());

#if ((defined(__GNUC__) && __GNUC__ >= 3  ) || !defined(HAVE_MKSTEMP))
    // This section is for Windows folk && gcc 3.x folk
    fstream tmpOut;
    err = createFile(sTempFile, tmpOut);
    if (err != ID3E_NoError)
    {
      tmpOut.close();
      remove(sTempFile);
      return (size_t)err; //impossible size, will make caller be able to set _last_error
    }

    tmpOut.write(tagData, tagSize);
    file.seekg(tag.GetPrependedBytes(), ios::beg);
    char *tmpBuffer[BUFSIZ];
    while (!file.eof())
    {
      file.read((char *)tmpBuffer, BUFSIZ);
      size_t nBytes = file.gcount();
      tmpOut.write((char *)tmpBuffer, nBytes);
    }

#else //((defined(__GNUC__) && __GNUC__ >= 3  ) || !defined(HAVE_MKSTEMP))

    // else we gotta make a temp file, copy the tag into it, copy the
    // rest of the old file after the tag, delete the old file, rename
    // this new file to the old file's name and update the handle

    int fd = mkstemp(sTempFile);
    if (fd < 0)
    {
      remove(sTempFile);
      return (size_t)ID3E_NoFile; //impossible size, will make caller be able to set _last_error
    }

    ofstream tmpOut(fd);
    if (!tmpOut)
    {
      tmpOut.close();
      remove(sTempFile);
      /* TODO: log this
       */
      return (size_t)ID3E_ReadOnly; //impossible size, will make caller be able to set _last_error
    }

    tmpOut.write(tagData, tagSize);
    file.seekg(tag.GetPrependedBytes(), ios::beg);
    uchar tmpBuffer[BUFSIZ];
    while (file)
    {
      file.read(tmpBuffer, BUFSIZ);
      size_t nBytes = file.gcount();
      tmpOut.write(tmpBuffer, nBytes);
    }

    close(fd); //closes the file

#endif ////((defined(__GNUC__) && __GNUC__ >= 3  ) || !defined(HAVE_MKSTEMP))

    tmpOut.close();
    file.close();

    // the following sets the permissions of the new file
    // to be the same as the original
#if defined(HAVE_SYS_STAT_H)
    struct stat fileStat;
    if(stat(filename.c_str(), &fileStat) == 0)
    {
#endif //defined(HAVE_SYS_STAT_H)
      remove(filename.c_str());
      rename(sTempFile, filename.c_str());
#if defined(HAVE_SYS_STAT_H)
      chmod(filename.c_str(), fileStat.st_mode);
    }
#endif //defined(HAVE_SYS_STAT_H)

//    file = tmpOut;
    file.clear();//to clear the eof mark
    err = openWritableFile(filename, file);
    if (err != ID3E_NoError)
    {
      return (size_t)err; //impossible size, will make caller be able to set _last_error
    }
  }

  return tagSize;
}


flags_t ID3_TagImpl::Update(flags_t ulTagFlag)
{
  flags_t tags = ID3TT_NONE;

  fstream file;
  String filename = this->GetFileName();
  _last_error = openWritableFile(filename, file);
  _file_size = getFileSize(file);

  if (_last_error == ID3E_NoFile)
  {
    _last_error = createFile(filename, file);
  }

  if (_last_error == ID3E_ReadOnly)
  {
    return tags;
  }

  if ((ulTagFlag & ID3TT_ID3V2) && this->HasChanged())
  {
    ID3_V2Spec spec2use;
    if (this->UserUpdatedSpec) // if the spec is too old, upgrade anyway. And never use experimental ones
      spec2use = this->GetSpec() < ID3V2_3_0 ? ID3V2_LATEST : this->GetSpec();
    else
      spec2use = ID3V2_LATEST; //write ID3V2_LATEST as default

    this->SetSpec(spec2use);
    this->checkFrames();
    _prepended_bytes = RenderV2ToFile(*this, file);
    if (_prepended_bytes < 17) //17 = minimal tag size, errors should not be higher numbered than 16
    {
      //must be an error
      _last_error = (ID3_Err)_prepended_bytes;
      _prepended_bytes = 0;
    }
    if (_prepended_bytes)
    {
      tags |= ID3TT_ID3V2;
    }
  }

  if ((ulTagFlag & ID3TT_ID3V1) &&
      (!this->HasTagType(ID3TT_ID3V1) || this->HasChanged()))
  {
    size_t tag_bytes = RenderV1ToFile(*this, file);
    if (tag_bytes)
    {
      // only add the tag_bytes if there wasn't an id3v1 tag before
      if (! _file_tags.test(ID3TT_ID3V1))
      {
        _appended_bytes += tag_bytes;
      }
      tags |= ID3TT_ID3V1;
    }
  }
  _changed = false;
  _file_tags.add(tags);
  _file_size = getFileSize(file);
  file.close();
  return tags;
}

flags_t ID3_TagImpl::Strip(flags_t ulTagFlag)
{
  flags_t ulTags = ID3TT_NONE;
  const size_t data_size = ID3_GetDataSize(*this);

  // First remove the v2 tag, if requested
  if (ulTagFlag & ID3TT_PREPENDED & _file_tags.get())
  {
    fstream file;
    _last_error = openWritableFile(this->GetFileName(), file);
    if (ID3E_NoError != _last_error)
    {
      return ulTags;
    }
    _file_size = getFileSize(file);

    // We will remove the id3v2 tag in place: since it comes at the beginning
    // of the file, we'll effectively move all the data that comes after the
    // tag back n bytes, where n is the size of the id3v2 tag.  Once we've
    // copied the data, we'll truncate the file.
    file.seekg(this->GetPrependedBytes(), ios::beg);

    uchar aucBuffer[BUFSIZ];

    // The nBytesRemaining variable indicates how many bytes are to be copied
    size_t nBytesToCopy = data_size;

    // Here we increase the nBytesToCopy by the size of any tags that appear
    // at the end of the file if we don't want to strip them
    if (!(ulTagFlag & ID3TT_APPENDED))
    {
      nBytesToCopy += this->GetAppendedBytes();
    }

    // The nBytesRemaining variable indicates how many bytes are left to be
    // moved in the actual file.
    // The nBytesCopied variable keeps track of how many actual bytes were
    // copied (or moved) so far.
    size_t nBytesRemaining = nBytesToCopy,
    nBytesCopied = 0;
    while (!file.eof())
    {
      size_t nBytesToRead = min((unsigned int)(nBytesRemaining - nBytesCopied), (unsigned int)BUFSIZ);

      file.read((char *)aucBuffer, nBytesToRead);
      size_t nBytesRead = file.gcount();

      if (nBytesRead != nBytesToRead)
      {
        // TODO: log this
        //cerr << "--- attempted to write " << nBytesRead << " bytes, "
        //     << "only wrote " << nBytesWritten << endl;
      }
      if (nBytesRead > 0)
      {
        long offset = nBytesRead + this->GetPrependedBytes();
        file.seekp(-offset, ios::cur);
        file.write((char *)aucBuffer, nBytesRead);
        file.seekg(this->GetPrependedBytes(), ios::cur);
        nBytesCopied += nBytesRead;
      }

      if (nBytesCopied == nBytesToCopy || nBytesToRead < BUFSIZ)
      {
        break;
      }
    }
    file.close();
  }

  size_t nNewFileSize = data_size;

  if ((_file_tags.get() & ID3TT_APPENDED) && (ulTagFlag & ID3TT_APPENDED))
  {
    ulTags |= _file_tags.get() & ID3TT_APPENDED;
  }
  else
  {
    // if we're not stripping the appended tags, be sure to increase the file
    // size by those bytes
    nNewFileSize += this->GetAppendedBytes();
  }

  if ((ulTagFlag & ID3TT_PREPENDED) && (_file_tags.get() & ID3TT_PREPENDED))
  {
    // If we're stripping the ID3v2 tag, there's no need to adjust the new
    // file size, since it doesn't account for the ID3v2 tag size
    ulTags |= _file_tags.get() & ID3TT_PREPENDED;
  }
  else
  {
    // add the original prepended tag size since we don't want to delete it,
    // and the new file size represents the file size _not_ counting the ID3v2
    // tag
    nNewFileSize += this->GetPrependedBytes();
  }

  if (ulTags && (truncate(_file_name.c_str(), nNewFileSize) == -1))
  {
    /* TODO: log this
     */
    _last_error = ID3E_NoFile;
    return 0;
  }

  _prepended_bytes = (ulTags & ID3TT_PREPENDED) ? 0 : _prepended_bytes;
  _appended_bytes  = (ulTags & ID3TT_APPENDED)  ? 0 : _appended_bytes;
  _file_size = data_size + _prepended_bytes + _appended_bytes;

  _changed = _file_tags.remove(ulTags) || _changed;

  return ulTags;
}
