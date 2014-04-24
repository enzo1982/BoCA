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

#include "tag_impl.h"
#include <id3/writers.h>
#include <memory.h>

using namespace dami;

/** \mainpage id3lib Library Documentation
 **
 ** \section tutorial Quick Tutorial
 **
 ** This tutorial will quickly get you up and running with id3lib.
 **
 ** \subsection download Downloading id3lib
 **
 ** First, id3lib must be a part of your development environment.  The latest
 ** files can always be downloaded from the <a href="http://id3lib.sourceforge.net">id3lib homepage</a>.
 **
 ** \subsection include Preparing your source code
 **
 ** To use the basic functionality of id3lib in your C++ code, a single
 ** \c #include is necessary.
 **
 ** \code
 **   #include <id3/tag.h>
 ** \endcode
 **
 ** There are other files that must be included to access more advanced
 ** functionality, but this will do most of the core functionality.
 **
 ** In straight C the \c #include is slightly different:
 **
 ** \code
 **   #include <id3.h>
 ** \endcode
 **
 ** \subsection creation Creating a tag
 **
 ** Almost all functionality occurs via an ID3_Tag object.  An ID3_Tag object
 ** basically encapsulates two things: a collection of ID3_Frame objects and
 ** file information.  The goal is to populate an ID3_Tag object with ID3_Frame
 ** objects, and the easiest way to do this is to associate the tag with a
 ** file.  This is done primarily via the ID3_Tag constructor, like so:
 **
 ** \code
 **   ID3_Tag myTag("song.mp3");
 ** \endcode
 **
 ** This constructor links, or associates, the object \c myTag with the file
 ** "song.mp3".  In doing so, the tagging information from "song.mp3" is parsed
 ** and added to \c myTag.  This association can also be accomplished by creating
 ** an empty tag and making an explicit call to Link().
 **
 ** \code
 **   ID3_Tag myTag;
 **   myTag.Link("song.mp3");
 ** \endcode
 **
 ** This is similar to the interface in straight C, where the tag creation and
 ** tag link must be done separately:
 **
 ** \code
 **   ID3Tag *myTag = ID3Tag_New();
 **   ID3Tag_Link(myTag, "song.mp3");
 ** \endcode
 **
 ** The default behavior of Link() is to parse all possible tagging information
 ** and convert it into ID3v2 frames.  The tagging information parsed can be
 ** limited to a particular type (or types) of tag by passing an ID3_TagType
 ** (or combination of ID3_TagTypes).  For example, to read only the ID3v1
 ** tag, pass in the constant ID3TT_ID3V1.
 **
 ** \code
 **   myTag.Link("song.mp3", ID3TT_ID3V1);
 ** \endcode
 **
 ** The C equivalent is this:
 **
 ** \code
 **   ID3Tag_LinkWithFlags(myTag, "song.mp3", ID3TT_ID3V1);
 ** \endcode
 **
 ** Another example would be to read in all tags that could possibly appear at
 ** the end of the file.  Here are the C++ and C examples:
 **
 ** \code
 **   myTag.Link("song.mp3", ID3TT_ID3V1 | ID3TT_LYRICS3V2 | ID3TT_MUSICMATCH); // C++
 **   ID3Tag_LinkWithFlags(myTag, "song.mp3", ID3TT_ID3V1 | ID3TT_LYRICS3V2 | ID3TT_MUSICMATCH); // C
 ** \endcode
 **
 ** \section accessing Accessing the Tag Data
 **
 ** After linking with a file, the object \c myTag now contains some or all of
 ** the tagging information present in the file "song.mp3", represented as
 ** ID3v2 frames.  How can that information be accessed?  There are a variety of
 ** ways to do this.  One is to iterate through all the frames in the tag.
 **
 ** \code
 **   // use an std::auto_ptr here to handle object cleanup automatically
 **   ID3_Tag::Iterator* iter = myTag.CreateIterator();
 **   ID3_Frame* myFrame = NULL;
 **   while (NULL != (myFrame = iter->GetNext()))
 **   {
 **     // do something with myFrame
 **   }
 **   delete iter;
 ** \endcode
 **
 ** Another way to access tagging information is by searching for specific
 ** frames using the Find() method.  For example, the album frame can be found
 ** in the following manner:
 **
 ** \code
 **   ID3_Frame* myFrame = myTag.Find(ID3FID_ALBUM);
 **   if (NULL != myFrame)
 **   {
 **     // do something with myFrame
 **   }
 ** \endcode
 **
 ** The Find() method can be used to search for frames with specific
 ** information.  For example, the following code can be used to find the frame
 ** with the title "Nirvana".
 **
 ** \code
 **   ID3_Frame* myFrame = myTag.Find(ID3FID_TITLE, ID3FN_TEXT, "Nirvana")));
 **   if (NULL != myFrame)
 **   {
 **     // do something with myFrame
 **   }
 ** \endcode
 **
 ** As indicated, the Find() method will return a NULL pointer if no such frame
 ** can be found.  If more than one frame meets the search criteria, subsequent
 ** calls to Find() with the same parameters will return the other matching
 ** frames.  The Find() method is guaranteed to return all matching frames
 ** before it wraps around to return the first matching frame.
 **
 ** All ID3_Frame objects are comprised of a collection of ID3_Field objects.
 ** These fields can represent text, numbers, or binary data.  As with frames,
 ** fields can be accessed in a variety of manners.  The fields of a frame
 ** can be iterated over in much the same manner of the frames of a tag.
 **
 ** \code
 **   // use an std::auto_ptr here to handle object cleanup automatically
 **   ID3_Frame::Iterator* iter = myFrame->CreateIterator();
 **   ID3_Field* myField = NULL;
 **   while (NULL != (myField = iter->GetNext()))
 **   {
 **     // do something with myField
 **   }
 **   delete iter;
 ** \endcode
 **
 ** If you know which field type you're looking for, you can access it
 ** directly.
 **
 ** \code
 **   ID3_Field* myField = myFrame->GetField(ID3FN_TEXT);
 **   while (NULL != myField)
 **   {
 **     // do something with myField
 **   }
 ** \endcode
 **
 ** Note: The ID3_FrameInfo class provides information about the frame types known
 ** to id3lib.
 **
 ** The ID3_Field represents a single piece of data within an ID3v2 frame.  As
 ** mentioned, an ID3_Field can represent three possible types of
 ** data: integers, binary data, and text strings.  The type of a particular
 ** field object is immutable; it is determined at the time of its construction
 ** (almost always when a frame is constructed) and can't be changed.  If in
 ** doubt, the field type can be accessed through its GetType() method.
 **
 ** Having an ID3_Field object isn't much use if you cannot access and/or
 ** alter its data.  Luckily, the id3lib API provides overloaded \c Set and
 ** \c Get methods for all data types.
 **
 ** If the field is an integer, the following methods can be used to access
 ** the data.
 **
 ** \code
 **   uint32 val = myField->Get();
 **   myField->Set(5);
 **   (*myField) = 10;
 ** \endcode
 **
 ** All text data is accessed in a slightly different manner.  The following
 ** code example best illustrates these differences.
 **
 ** \code
 **   // for ascii strings
 **   char str1[1024];
 **   const char* p1 = "My String";
 **   const char* p2 = "My Other String";
 **
 **   myField->Set(p1);
 **   (*myField) = p2;  // equivalent to Set
 **
 **   myField->Get(str1, 1024); // copies up to 1024 bytes of the field data into str1
 **   p1 = myField->GetRawText(); // returns a pointer to the internal string
 ** \endcode
 **
 ** Binary data is similar to text data, except that its base type is a pointer
 ** to an unsigned, rather than a signed, char.
 **
 ** \code
 **   // for binary strings
 **   uchar data[1024];
 **   const uchar *p1 = getBinaryData(); // not an id3lib function
 **   size_t size = getBinarySize();     // not an id3lib function
 **
 **   myField->Set(p1, size);
 **
 **   myField->Get(data, 1024); // copies up to 1024 bytes of the field data into str1
 **   p1 = myField->GetRawBinary(); // returns a pointer to the internal string
 ** \endcode
 **
 ** \section updating Updating the Tag
 **
 ** When you're ready to save your changes back to the file, a single call to
 ** Update() is sufficient.
 **
 ** \code
 **   tag.Update();
 ** \endcode
 **
 **/

/** \class ID3_Tag tag.h id3/tag.h
 ** \brief The representative class of an id3 tag.
 **
 ** The ID3_Tag is, at its simplest, a container for ID3v2 frames.  At its
 ** most complicated, it's a kitchen-sink, monolithic "catch-all" class for
 ** handling almost every task associated with creating, parsing, rendering,
 ** and manipulating digital audio data using id3lib.
 **
 ** This simple \c #include does it all.  In order to read an existing tag, do
 ** the following:
 **
 ** \code
 **   ID3_Tag myTag;
 **   myTag.Link("something.mp3");
 ** \endcode
 **
 ** That is all there is to it.  Now all you have to do is use the Find()
 ** method to locate the frames you are interested in is the following:
 **
 ** \code
 **   ID3_Frame* myFrame = myTag.Find(ID3FID_TITLE);
 **   if (NULL != myFrame)
 **   {
 **     const char* title = myFrame->GetField(ID3FN_TEXT)->GetText();
 **     cout << "Title: " << title << endl;
 **   }
 ** \endcode
 **
 ** This code snippet locates the ID3FID_TITLE frame and displays the
 ** text field.
 **
 ** When using the ID3_Tag::Link() method, you automatically gain access to any
 ** ID3v1/1.1, ID3v2, Lyrics3 v2.0, and MusicMatch tags present in the file.
 ** The class will automaticaly parse and convert any of these foreign tag
 ** formats into ID3v2 tags.  Also, id3lib will correctly parse any correctly
 ** formatted 'CDM' frames from the unreleased ID3v2 2.01 draft specification.
 **
 ** \author Dirk Mahoney
 ** \version $Id$
 ** \sa ID3_Frame
 ** \sa ID3_Field
 ** \sa ID3_Err
 **/

/** Default constructor; it can accept an optional filename as a parameter.
 **
 ** If this file exists, it will be opened and all id3lib-supported tags will
 ** be parsed and converted to ID3v2 if necessary.  After the conversion, the
 ** file will remain unchanged, and will continue to do so until you use the
 ** Update() method on the tag (if you choose to Update() at all).
 **
 ** \param name The filename of the mp3 file to link to
 **/
ID3_Tag::ID3_Tag(const char *name, flags_t flags) : ID3_Container(new ID3_TagImpl(name, flags))
{
	_impl = (ID3_TagImpl *) ID3_Container::_impl;
}

/** Standard copy constructor.
 **
 ** \param tag What is copied into this tag
 **/
ID3_Tag::ID3_Tag(const ID3_Tag &tag) : ID3_Container(new ID3_TagImpl(tag))
{
	_impl = (ID3_TagImpl *) ID3_Container::_impl;
}

ID3_Tag::~ID3_Tag()
{
	delete _impl;
}

/** Turns unsynchronization on or off, dependant on the value of the boolean
 ** parameter.
 **
 ** If you call this method with 'false' as the parameter, the
 ** binary tag will not be unsync'ed, regardless of whether the tag should
 ** be.  This option is useful when the file is only going to be used by
 ** ID3v2-compliant software.  See the ID3v2 standard document for futher
 ** details on unsync.
 **
 ** Be default, tags are created without unsync.
 **
 ** \code
 **   myTag.SetUnsync(false);
 ** \endcode
 **
 ** \param bSync Whether the tag should be unsynchronized
 **/
bool ID3_Tag::SetUnsync(bool b)
{
  return _impl->SetUnsync(b);
}


/** Turns extended header rendering on or off, dependant on the value of the
 ** boolean parameter.
 **
 ** This option is currently ignored as id3lib doesn't yet create extended
 ** headers.  This option only applies when rendering tags for ID3v2 versions
 ** that support extended headers.
 **
 ** \code
 **   myTag.SetExtendedHeader(true);
 ** \endcode
 **
 ** \param bExt Whether to render an extended header
 **/
bool ID3_Tag::SetExtendedHeader(bool ext)
{
  return _impl->SetExtended(ext);
}

/** Turns padding on or off, dependant on the value of the boolean
 ** parameter.
 **
 ** When using ID3v2 tags in association with files, id3lib can optionally
 ** add padding to the tags to ensure minmal file write times when updating
 ** the tag in the future.
 **
 ** When the padding option is switched on, id3lib automatically creates
 ** padding according to the 'ID3v2 Programming Guidelines'.  Specifically,
 ** enough padding will be added to round out the entire file (song plus
 ** tag) to an even multiple of 2K.  Padding will only be created when the
 ** tag is attached to a file and that file is not empty (aside from a
 ** pre-existing tag).
 **
 ** id3lib's addition to the guidelines for padding, is that if frames are
 ** removed from a pre-existing tag (or the tag simply shrinks because of
 ** other reasons), the new tag will continue to stay the same size as the
 ** old tag (with padding making the difference of course) until such time as
 ** the padding is greater than 4K.  When this happens, the padding will be
 ** reduced and the new tag will be smaller than the old.
 **
 ** By default, padding is switched on.
 **
 ** \code
 **   myTag.SetPadding(false);
 ** \endcode
 **
 ** \param bPad Whether or not render the tag with padding.
 **/
bool ID3_Tag::SetPadding(bool pad)
{
  return _impl->SetPadding(pad);
}

bool ID3_Tag::SetExperimental(bool exp)
{
  return _impl->SetExperimental(exp);
}

bool ID3_Tag::GetUnsync() const
{
  return _impl->GetUnsync();
}

bool ID3_Tag::GetExtendedHeader() const
{
  return _impl->GetExtended();
}

bool ID3_Tag::GetExperimental() const
{
  return _impl->GetExperimental();
}

bool ID3_Tag::Parse(ID3_Reader& reader)
{
  return id3::v2::parse(*_impl, reader);
}

size_t ID3_Tag::Parse(const uchar* buffer, size_t bytes)
{
  ID3_MemoryReader mr(buffer, bytes);
  ID3_Reader::pos_type beg = mr.getCur();
  id3::v2::parse(*_impl, mr);
  return mr.getEnd() - beg;
}

/** Renders the tag and writes it to the attached file; the type of tag
 ** rendered can be specified as a parameter.  The default is to update only
 ** the ID3v2 tag.  See the ID3_TagType enumeration for the constants that
 ** can be used.
 **
 ** Make sure the rendering parameters are set before calling the method.
 ** See the Link documentation for an example of this method in use.
 **
 ** \sa ID3_TagType
 ** \sa Link
 ** \param tt The type of tag to update.
 **/
/** Renders a binary image of the tag into the supplied buffer.
 **
 ** See Size() for an example.  This method returns the actual number of the
 ** bytes of the buffer used to store the tag.  This will be no more than the
 ** size of the buffer itself, because Size() over estimates the required
 ** buffer size when padding is enabled.
 **
 ** Before calling this method, it is advisable to call HasChanged() first as
 ** this will let you know whether you should bother rendering the tag.
 **
 ** @see    ID3_IsTagHeader
 ** @see    ID3_Tag#HasChanged
 ** @return The actual number of the bytes of the buffer used to store the
 **         tag
 ** @param  buffer The buffer that will contain the rendered tag.
 **/
size_t ID3_Tag::Render(uchar* buffer, ID3_TagType tt) const
{
  ID3_MemoryWriter mw(buffer, -1);
  return this->Render(mw, tt);
}

size_t ID3_Tag::Render(ID3_Writer& writer, ID3_TagType tt) const
{
  ID3_Writer::pos_type beg = writer.getCur();
  if (ID3TT_ID3V2 & tt)
  {
    ID3_Err err = id3::v2::render(writer, *this);
    if (err != ID3E_NoError)
      _impl->SetLastError(err);
  }
  else if (ID3TT_ID3V1 & tt)
  {
    id3::v1::render(writer, *this);
  }
  return writer.getCur() - beg;
}


/** Attaches a file to the tag, parses the file, and adds any tag information
 ** found in the file to the tag.
 **
 ** Use this method if you created your ID3_Tag object without supplying a
 ** parameter to the constructor (maybe you created an array of ID3_Tag
 ** pointers).  This is the preferred method of interacting with files, since
 ** id3lib can automatically do things like parse foreign tag formats and
 ** handle padding when linked to a file.  When a tag is linked to a file, you
 ** do not need to use the Size(), Render(const uchar*, size_t), or
 ** Parse(ID3_Reader&) methods or the IsV2Tag(ID3_Reader&) static function--
 ** id3lib will take care of those details for you.  The single parameter is a
 ** pointer to a file name.
 **
 ** Link returns the size of the the ID3v2 tag (if any) that begins the file.
 **
 ** \code
 **   ID3_Tag myTag;
 **   myTag.Link("mysong.mp3");
 **
 **   // do whatever we want with the tag
 **   // ...
 **
 **   // setup all our rendering parameters
 **   myTag->SetUnsync(false);
 **   myTag->SetExtendedHeader(true);
 **   myTag->SetPadding(true);
 **
 **   // write any changes to the file
 **   myTag->Update()
 **
 ** \endcode
 **
 ** @see IsV2Tag
 ** @param fileInfo The filename of the file to link to.
 **/
size_t ID3_Tag::Link(const char *fileInfo, flags_t flags)
{
  return _impl->Link(fileInfo, flags);
}

/**
 ** Same as above, but takes a ID3_Reader as argument.
 */
size_t ID3_Tag::Link(ID3_Reader &reader, flags_t flags)
{
  return _impl->Link(reader, flags);
}

flags_t ID3_Tag::Update(flags_t flags)
{
  return _impl->Update(flags);
}

/**
 ** Get's the mp3 Info like bitrate, mpeg version, etc.
 ** Can be run after Link(<filename>)
 **
 **/
const Mp3_Headerinfo* ID3_Tag::GetMp3HeaderInfo() const
{
  return _impl->GetMp3HeaderInfo();
}

/**
 ** Returns the last error
 ** Can be run after Link() and Update()
 ** Will be reset to ID3E_NoError after being called for.
 **
 **/
ID3_Err ID3_Tag::GetLastError()
{
  return _impl->GetLastError();
}

/** Strips the tag(s) from the attached file. The type of tag stripped
 ** can be specified as a parameter.  The default is to strip all tag types.
 **
 ** \param tt The type of tag to strip
 ** \sa ID3_TagType
 **/
flags_t ID3_Tag::Strip(flags_t flags)
{
  return _impl->Strip(flags);
}

size_t ID3_Tag::GetPrependedBytes() const
{
  return _impl->GetPrependedBytes();
}

size_t ID3_Tag::GetAppendedBytes() const
{
  return _impl->GetAppendedBytes();
}

size_t ID3_Tag::GetFileSize() const
{
  return _impl->GetFileSize();
}

const char* ID3_Tag::GetFileName() const
{
  return _impl->GetFileName().c_str();
}

ID3_Tag& ID3_Tag::operator=( const ID3_Tag &rTag )
{
  if (this != &rTag)
  {
    *_impl = rTag;
  }
  return *this;
}

bool ID3_Tag::HasTagType(ID3_TagType tt) const
{
  return _impl->HasTagType(tt);
}

/** Analyses a buffer to determine if we have a valid ID3v2 tag header.
 ** If so, return the total number of bytes (including the header) to
 ** read so we get all of the tag
 **/
size_t ID3_Tag::IsV2Tag(const uchar* const data)
{
  ID3_MemoryReader mr(data, ID3_TagHeader::SIZE);
  return ID3_TagImpl::IsV2Tag(mr);
}

size_t ID3_Tag::IsV2Tag(ID3_Reader& reader)
{
  return ID3_TagImpl::IsV2Tag(reader);
}
