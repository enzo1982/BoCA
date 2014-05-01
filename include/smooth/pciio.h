 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_PCIIO
#define H_OBJSMOOTH_PCIIO

#include "definitions.h"
#include "misc/string.h"
#include "graphics/bitmap.h"

#include "io/instream.h"
#include "io/outstream.h"

#define PCIMajorVersion		1
#define PCIMinorVersion		2

#define PCIOut	S::IO::OutStream	*
#define PCIIn	S::IO::InStream		*

// tag identifiers

enum PCITagID
{
	PCIF_TAG = 1,	// identifies the PCIF tag
	RESO_TAG,	// resolution tag
	FORM_TAG,	// format tag
	DESC_TAG,	// description tag
	DATA_TAG	// data tag
};

// compression identifiers

enum
{
	UNCOMPRESSED = 0,	// raw image data
	RAW	     = 0,	// raw image data
	STANDARD     = 1,	// PCI compressed data
	PCI	     = 1,	// PCI compressed data
	JPEG,			// JPEG compressed data
	RLE,			// RLE compressed data
	BZIP2			// BZIP2 compressed data
};

// structure definitions

class SMOOTHAPI PCIIO
{
	private:
		int				 majorversion;	// major version of PIC file format
		int				 minorversion;	// minor version of PCI file format
		S::GUI::Bitmap			 bmp;
		int				 sizex;		// horizontal pixels
		int				 sizey;		// vertical pixels
		int				 dpix;		// horizontal dots per inch
		int				 dpiy;		// vertical dots per inch
		S::String			 description;	// image description
		S::String			 imagename;	// name of the image (can be used to identify an image)
		int				 compression;	// the compression type
		int				 bpcc;		// bits per color channel
		S::GUI::Color::ColorSpace	 colorspace;	// colorspace (implicates nof color channels)
		int				 quality;	// image quality for JPEG compression
		int				 rlebits;	// number of bits used for the runlength in RLE
		bool				 overwrite;	// overwrite file or append to PCI file
		int				 imageid;	// image ID in file

		bool				 WritePCIFTAG(PCIOut);
		bool				 WriteIMAGTAG(PCIOut);
		bool				 WriteRESOTAG(PCIOut);
		bool				 WriteFORMTAG(PCIOut);
		bool				 WriteDESCTAG(PCIOut);
		bool				 WriteNAMETAG(PCIOut);
		bool				 WriteDATATAG(PCIOut);

		bool				 ReadPCIFTAG(PCIIn);
		bool				 ReadIMAGTAG(PCIIn);
		bool				 ReadRESOTAG(PCIIn);
		bool				 ReadFORMTAG(PCIIn);
		bool				 ReadDESCTAG(PCIIn);
		bool				 ReadNAMETAG(PCIIn);
		bool				 ReadDATATAG(PCIIn);

		bool				 FindImageID(PCIIn);

		bool				 WriteLine(PCIOut, int);
		bool				 ReadLine(PCIIn, int);

		S::Bool				 CompressPCI(PCIOut);
		S::Bool				 DecompressPCI(PCIIn);
	public:
						 PCIIO(const S::GUI::Bitmap &);
						 PCIIO();
						~PCIIO();

		bool				 WritePCI(PCIOut);
		bool				 ReadPCI(PCIIn);

		void				 SetCompressionType(int);
		void				 SetColorspace(S::GUI::Color::ColorSpace);
		void				 SetBitsPerChannel(int);
		void				 SetQuality(int);
		void				 SetDescription(const S::String &);
		void				 SetRLEBits(int);
		void				 SelectImage(int);
		void				 SelectImage(const S::String &);
		void				 SetImageID(int);
		void				 SetImageName(const S::String &);

		S::GUI::Bitmap			&GetBitmap();
};

SMOOTHAPI PCIOut	 CreatePCI(const S::String &);		// creates a new PCI file
SMOOTHAPI PCIOut	 OpenPCIForOutput(const S::String &);	// opens a PCI file
SMOOTHAPI PCIIn		 OpenPCIForInput(const S::String &);	// opens a PCI file
SMOOTHAPI bool		 ClosePCI(PCIIn);			// closes a PCI file
SMOOTHAPI bool		 ClosePCI(PCIOut);			// closes a PCI file

#endif
