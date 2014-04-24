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
#include <id3/writers.h>
#include <memory.h>

using namespace dami;

/** Default constructor.
 **/
ID3_Container::ID3_Container() : _impl(new ID3_ContainerImpl()),
				 _own_impl(true)
{
}

/** Constructor that accepts a implementation as a parameter.
 **
 ** The implementation pointed to by impl will be used instead
 ** of a newly created implementation.
 **
 ** \param impl The implementation object to use
 **/
ID3_Container::ID3_Container(ID3_ContainerImpl *impl) : _impl(impl),
							_own_impl(false)
{
}

/** Standard copy constructor.
 **
 ** \param tag What is copied into this tag
 **/
ID3_Container::ID3_Container(const ID3_Container &container) : _impl(new ID3_ContainerImpl(container)),
							       _own_impl(true)
{
}

ID3_Container::~ID3_Container()
{
	if (_own_impl) delete _impl;
}

/** Clears the object and disassociates it from any files.
 **
 ** Frees any resources for which the object is responsible, including all
 ** frames and files.  After a call to Clear(), the object can be used
 ** again for any new or existing tag.
 **/
void ID3_Container::Clear()
{
	_impl->Clear();
}


/** Indicates whether the tag has been altered since the last parse, render,
 ** or update.
 **
 ** If you have a tag linked to a file, you do not need this method since the
 ** Update() method will check for changes before writing the tag.
 **
 ** This method is primarily intended as a status indicator for applications
 ** and for applications that use the Parse() and Render() methods.
 **
 ** Setting a field, changed the ID of an attached frame, setting or grouping
 ** or encryption IDs, and clearing a frame or field all constitute a change
 ** to the tag, as do calls to the SetUnsync(), SetExtendedHeader(), and
 ** SetPadding() methods.
 **
 ** \code
 **   if (myTag.HasChanged())
 **   {
 **     // render and output the tag
 **   }
 ** \endcode
 **
 ** \return Whether or not the tag has been altered.
 **/
bool ID3_Container::HasChanged() const
{
	return _impl->HasChanged();
}

/** Returns an over estimate of the number of bytes required to store a
 ** binary version of a tag.
 **
 ** When using Render() to render a binary tag to a
 ** memory buffer, first use the result of this call to allocate a buffer of
 ** unsigned chars.
 **
 ** \code
 **   if (myTag.HasChanged())
 **   {
 **     size_t tagSize; = myTag.Size();
 **     if (tagSize > 0)
 **     {
 **       uchar *buffer = new uchar[tagSize];
 **       if (NULL != buffer)
 **       {
 **         size_t actualSize = myTag.Render(buffer);
 **         // do something useful with the first
 **         // 'actualSize' bytes of the buffer,
 **         // like push it down a socket
 **         delete [] buffer;
 **       }
 **     }
 **   }
 ** \endcode
 **
 ** @see #Render
 ** @return The (overestimated) number of bytes required to store a binary
 **         version of a tag
 **/
size_t ID3_Container::Size() const
{
	return _impl->Size();
}

void ID3_Container::AddFrame(const ID3_Frame& frame)
{
  _impl->AddFrame(frame);
}

/** Attaches a frame to the tag; the tag doesn't take responsibility for
 ** releasing the frame's memory when tag goes out of scope.
 **
 ** Optionally, operator<< can also be used to attach a frame to a tag.  To
 ** use, simply supply its parameter a pointer to the ID3_Frame object you wish
 ** to attach.
 **
 ** \code
 **   ID3_Frame myFrame;
 **   myTag.AddFrame(&myFrame);
 ** \endcode
 **
 ** As stated, this method attaches the frames to the tag---the tag does
 ** not create its own copy of the frame.  Frames created by an application
 ** must exist until the frame is removed or the tag is finished with it.
 **
 ** \param pFrame A pointer to the frame that is being added to the tag.
 ** \sa ID3_Frame
 **/
void ID3_Container::AddFrame(const ID3_Frame* frame)
{
  _impl->AddFrame(frame);
}

/** Attaches a frame to the tag; the tag takes responsibility for
 ** releasing the frame's memory when tag goes out of scope.
 **
 ** This method accepts responsibility for the attached frame's memory, and
 ** will delete the frame and its contents when the tag goes out of scope or is
 ** deleted.  Therefore, be sure the frame isn't "Attached" to other tags.
 **
 ** \code
 **   ID3_Frame *frame = new ID3_Frame;
 **   myTag.AttachFrame(frame);
 ** \endcode
 **
 ** \param frame A pointer to the frame that is being added to the tag.
 **/
bool ID3_Container::AttachFrame(ID3_Frame *frame)
{
  return _impl->AttachFrame(frame);
}


/** Removes a frame from the tag.
 **
 ** If you already own the frame object in question, then you should already
 ** have a pointer to the frame you want to delete.  If not, or if you wish to
 ** delete a pre-existing frame (from a tag you have parsed, for example), the
 ** use one of the Find methods to obtain a frame pointer to pass to this
 ** method.
 **
 ** \code
 **   ID3_Frame *someFrame;
 **   if (someFrame = myTag.Find(ID3FID_TITLE))
 **   {
 **     myTag.RemoveFrame(someFrame);
 **   }
 ** \endcode
 **
 ** \sa ID3_Tag#Find
 ** \param pOldFrame A pointer to the frame that is to be removed from the
 **                  tag
 **/
ID3_Frame* ID3_Container::RemoveFrame(const ID3_Frame *frame)
{
  return _impl->RemoveFrame(frame);
}


/// Finds frame with given frame id
  /** Returns a pointer to the next ID3_Frame with the given ID3_FrameID;
   ** returns NULL if no such frame found.
   **
   ** If there are multiple frames in the tag with the same ID (which, for some
   ** frames, is allowed), then subsequent calls to Find() will return
   ** subsequent frame pointers, wrapping if necessary.
   **
   ** \code
   **   ID3_Frame *myFrame;
   **   if (myFrame = myTag.Find(ID3FID_TITLE))
   **   {
   **     // do something with the frame, like copy
   **     // the contents into a buffer, display the
   **     // contents in a window, etc.
   **     // ...
   **   }
   ** \endcode
   **
   ** You may optionally supply to more parameters ot this method, being an
   ** ID3_FieldID and a value of some sort.  Depending on the field name/ID you
   ** supply, you may supply an integer, a char* or a unicode_t* as the third
   ** parameter.  If you supply an ID3_FrameID, you must also supply a data
   ** value to compare against.
   **
   ** This method will then return the first frame that has a matching frame
   ** ID, and which has a field with the same name as that which you supplied
   ** in the second parameter, whose calue matches that which you supplied as
   ** the third parameter.  For example:
   **
   ** \code
   **   ID3_Frame *myFrame;
   **   if (myFrame = myTag.Find(ID3FID_TITLE, ID3FN_TEXT, "Nirvana"))
   **   {
   **     // found it, do something with it.
   **     // ...
   **   }
   ** \endcode
   **
   ** This example will return the first TITLE frame and whose TEXT field is
   ** 'Nirvana'.  Currently there is no provision for things like 'contains',
   ** 'greater than', or 'less than'.  If there happens to be more than one of
   ** these frames, subsequent calls to the Find() method will return
   ** subsequent frames and will wrap around to the beginning.
   **
   ** Another example...
   **
   ** \code
   **   ID3_Frame *myFrame;
   **   if (myFrame = myTag.Find(ID3FID_COMMENT, ID3FN_TEXTENC, ID3TE_UNICODE))
   **   {
   **     // found it, do something with it.
   **     // ...
   **   }
   ** \endcode
   **
   ** This returns the first COMMENT frame that uses Unicode as its text
   ** encdoing.
   **
   ** @name   Find
   ** @param  id The ID of the frame that is to be located
   ** @return A pointer to the first frame found that has the given frame id,
   **         or NULL if no such frame.
   **/
ID3_Frame* ID3_Container::Find(ID3_FrameID id) const
{
  return _impl->Find(id);
}

/// Finds frame with given frame id, fld id, and integer data
ID3_Frame* ID3_Container::Find(ID3_FrameID id, ID3_FieldID fld, uint32 data) const
{
  return _impl->Find(id, fld, data);
}

/// Finds frame with given frame id, fld id, and ascii data
ID3_Frame* ID3_Container::Find(ID3_FrameID id, ID3_FieldID fld, const char* data) const
{
  String str(data);
  return _impl->Find(id, fld, str);
}

/// Finds frame with given frame id, fld id, and unicode data
ID3_Frame* ID3_Container::Find(ID3_FrameID id, ID3_FieldID fld, const unicode_t* data) const
{
  WString str = toWString(data, ucslen(data));
  return _impl->Find(id, fld, str);
}

/** Returns the number of frames present in the tag object.
 **
 ** This includes only those frames that id3lib recognises.  This is used as
 ** the upper bound on calls to the GetFrame() and operator[]() methods.
 **
 ** \return The number of frames present in the tag object.
 **/
size_t ID3_Container::NumFrames() const
{
  return _impl->NumFrames();
}

ID3_Container& ID3_Container::operator=( const ID3_Container &rContainer )
{
  if (this != &rContainer)
  {
    *_impl = rContainer;
  }
  return *this;
}

ID3_V2Spec ID3_Container::GetSpec() const
{
  return _impl->GetSpec();
}

bool ID3_Container::SetSpec(ID3_V2Spec spec)
{
  //a user cannot set a spec lower than ID3V2_3_0, it's obsolete!
  ID3_V2Spec spec2use = spec < ID3V2_3_0 ? ID3V2_LATEST : spec;
  _impl->UserUpdatedSpec = _impl->GetSpec() != spec2use;
  return _impl->SetSpec(spec2use);
}


namespace
{
  class IteratorImpl : public ID3_Container::Iterator
  {
    ID3_ContainerImpl::iterator _cur;
    ID3_ContainerImpl::iterator _end;
  public:
    IteratorImpl(ID3_ContainerImpl& container)
      : _cur(container.begin()), _end(container.end())
    {
    }

    ID3_Frame* GetNext()
    {
      ID3_Frame* next = NULL;
      while (next == NULL && _cur != _end)
      {
        next = *_cur;
        ++_cur;
      }
      return next;
    }
  };


  class ConstIteratorImpl : public ID3_Container::ConstIterator
  {
    ID3_ContainerImpl::const_iterator _cur;
    ID3_ContainerImpl::const_iterator _end;
  public:
    ConstIteratorImpl(ID3_ContainerImpl& container)
      : _cur(container.begin()), _end(container.end())
    {
    }
    const ID3_Frame* GetNext()
    {
      ID3_Frame* next = NULL;
      while (next == NULL && _cur != _end)
      {
        next = *_cur;
        ++_cur;
      }
      return next;
    }
  };
}

ID3_Container::Iterator *ID3_Container::CreateIterator()
{
	return new IteratorImpl(*_impl);
}

ID3_Container::ConstIterator *ID3_Container::CreateIterator() const
{
	return new ConstIteratorImpl(*_impl);
}
