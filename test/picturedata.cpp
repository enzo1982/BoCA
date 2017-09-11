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
#include <smooth/main.h>

#include <boca.h>

#include <time.h>

using namespace smooth;
using namespace smooth::Threads;

using namespace BoCA;

/* This tests thread safety of the PictureData store.
 */
static int		 srandCount  = 0;

static const Int	 numThreads  = 4;
static const Int	 numPictures = 100;
static const Int	 bufferSize  = 100;

Void Worker()
{
	srand(time(0) + srandCount++);

	/* Allocate picture data.
	 */
	Array<PictureData *>	 pictures;
	Buffer<UnsignedByte>	 buffer(bufferSize);

	for (Int i = 0; i < numPictures; i++)
	{
		for (Int i = 0; i < bufferSize; i++) buffer[i] = rand() % 256;

		PictureData	*picture = new PictureData();

		picture->Set(buffer, buffer.Size());

		pictures.Add(picture);
	}

	/* Do some stuff.
	 */
	for (Int i = 0; i < 100000; i++)
	{
		Int		 n	 = rand() % numPictures;
		PictureData	*picture = pictures.GetNth(n);

		picture->Size();

		if (*picture != *pictures.GetNth(rand() % numPictures))
		{
			for (Int i = 0; i < bufferSize; i++) buffer[i] = ((const Buffer<UnsignedByte> &) *picture)[i];
			for (Int i = 0; i < bufferSize; i++) buffer[i] = ((const UnsignedByte *) *picture)[i];

			delete picture;

			pictures.RemoveNth(n);

			picture = new PictureData();
			picture->Set(buffer, buffer.Size());

			pictures.Add(picture);
		}
	}

	/* Free picture data.
	 */
	foreach (PictureData *picture, pictures) delete picture;

	pictures.RemoveAll();
}

/* Start threads and wait.
 */
Int S::Main()
{
	Array<Thread *>	 threads;

	for (Int i = 0; i < numThreads; i++) threads.Add(NonBlocking0<Void>(&Worker).Call());
	for (Int i = 0; i < numThreads; i++) threads.GetNth(i)->Wait();
	for (Int i = 0; i < numThreads; i++) delete threads.GetNth(i);

	threads.RemoveAll();

	return 0;
}
