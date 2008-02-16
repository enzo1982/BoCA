 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_PCIIO_
#define _H_OBJSMOOTH_PCIIO_

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

const int	 PCIF_TAG	= 1;	// identifies the PCIF tag
const int	 RESO_TAG	= 2;	// resolution tag
const int	 FORM_TAG	= 3;	// format tag
const int	 DESC_TAG	= 4;	// description tag
const int	 DATA_TAG	= 5;	// data tag

// compression identifiers

const int	 UNCOMPRESSED	= 0;	// raw image data
const int	 RAW		= 0;	// raw image data
const int	 STANDARD	= 1;	// PCI compressed data
const int	 PCI		= 1;	// PCI compressed data
const int	 JPEG		= 2;	// JPEG compressed data
const int	 RLE		= 3;	// RLE compressed data
const int	 BZIP2		= 4;	// BZIP2 compressed data

// color format identifiers

extern S::String	 cfNames[10];
extern S::String	 cpNames[5];

// structure definitions

class SMOOTHAPI PCIIO
{
	friend SMOOTHAPI bool	 WritePCI(PCIOut, PCIIO &);
	friend bool	 WritePCIFTAG(PCIOut, PCIIO &);
	friend bool	 WriteIMAGTAG(PCIOut, PCIIO &);
	friend bool	 WriteRESOTAG(PCIOut, PCIIO &);
	friend bool	 WriteFORMTAG(PCIOut, PCIIO &);
	friend bool	 WriteDESCTAG(PCIOut, PCIIO &);
	friend bool	 WriteNAMETAG(PCIOut, PCIIO &);
	friend bool	 WriteDATATAG(PCIOut, PCIIO &);
	friend SMOOTHAPI bool	 ReadPCI(PCIIn, PCIIO &);
	friend bool	 ReadPCIFTAG(PCIIn, PCIIO &);
	friend bool	 ReadIMAGTAG(PCIIn, PCIIO &);
	friend bool	 ReadRESOTAG(PCIIn, PCIIO &);
	friend bool	 ReadFORMTAG(PCIIn, PCIIO &);
	friend bool	 ReadDESCTAG(PCIIn, PCIIO &);
	friend bool	 ReadNAMETAG(PCIIn, PCIIO &);
	friend bool	 ReadDATATAG(PCIIn, PCIIO &);
	friend bool	 FindImageID(PCIIn, PCIIO &);
	friend bool	 WriteLine(PCIOut, PCIIO &, int);
	friend bool	 ReadLine(PCIIn, PCIIO &, int);
	friend bool	 CompressPCI(PCIOut, PCIIO &);
	friend bool	 DecompressPCI(PCIIn, PCIIO &);
	private:
		int		 majorversion;	// major version of PIC file format
		int		 minorversion;	// minor version of PCI file format
		S::GUI::Bitmap	 bmp;
		int		 sizex;		// horizontal pixels
		int		 sizey;		// vertical pixels
		int		 dpix;		// horizontal dots per inch
		int		 dpiy;		// vertical dots per inch
		S::String	 description;	// image description
		S::String	 imagename;	// name of the image (can be used to identify an image)
		int		 compression;	// the compression type
		int		 bpcc;		// bits per color channel
		int		 colorspace;	// colorspace (implicates nof color channels)
		int		 quality;	// image quality for JPEG compression
		int		 rlebits;	// number of bits used for the runlength in RLE
		bool		 overwrite;	// overwrite file or append to PCI file
		int		 imageid;	// image ID in file
	public:
				 PCIIO(const S::GUI::Bitmap &);
				 PCIIO();
				~PCIIO();
		void		 SetCompressionType(int);
		void		 SetColorspace(int);
		void		 SetBitsPerChannel(int);
		void		 SetQuality(int);
		void		 SetDescription(const S::String &);
		void		 SetRLEBits(int);
		void		 SelectImage(int);
		void		 SelectImage(const S::String &);
		void		 SetImageID(int);
		void		 SetImageName(const S::String &);
		S::GUI::Bitmap	&GetBitmap();
};

SMOOTHAPI PCIOut	 CreatePCI(const S::String &);		// creates a new PCI file
SMOOTHAPI PCIOut	 OpenPCIForOutput(const S::String &);	// opens a PCI file
SMOOTHAPI PCIIn		 OpenPCIForInput(const S::String &);	// opens a PCI file
SMOOTHAPI bool		 ClosePCI(PCIIn);			// closes a PCI file
SMOOTHAPI bool		 ClosePCI(PCIOut);			// closes a PCI file
SMOOTHAPI bool		 WritePCI(PCIOut, PCIIO &);
SMOOTHAPI bool		 ReadPCI(PCIIn, PCIIO &);

bool	 WritePCIFTAG(PCIOut, PCIIO &);	// creates PCIF tag
bool	 WriteIMAGTAG(PCIOut, PCIIO &);
bool	 WriteRESOTAG(PCIOut, PCIIO &);	// creates RESO tag
bool	 WriteFORMTAG(PCIOut, PCIIO &);
bool	 WriteDESCTAG(PCIOut, PCIIO &);
bool	 WriteNAMETAG(PCIOut, PCIIO &);
bool	 WriteDATATAG(PCIOut, PCIIO &);
bool	 ReadPCIFTAG(PCIIn, PCIIO &);
bool	 ReadIMAGTAG(PCIIn, PCIIO &);
bool	 ReadRESOTAG(PCIIn, PCIIO &);
bool	 ReadFORMTAG(PCIIn, PCIIO &);
bool	 ReadDESCTAG(PCIIn, PCIIO &);
bool	 ReadNAMETAG(PCIIn, PCIIO &);
bool	 ReadDATATAG(PCIIn, PCIIO &);
bool	 FindImageID(PCIIn, PCIIO &);

#endif
