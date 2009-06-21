 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_CHOOSER_FILES_
#define _H_TAGEDIT_CHOOSER_FILES_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

#include "chooser.h"

namespace BoCA
{
	class ChooserFiles : public Chooser
	{
		private:
			Array<Track>	 tracks;
			Array<Int>	 modified;

			Array<String>	 extensions;

			Array<Tree *>	 trees;

			ListBox		*list_directories;

			EditBox		*edit_directory;
			ListBox		*list_files;
			Text		*text_nofiles;

			Button		*btn_save;
			Button		*btn_saveall;

			Int		 SaveFileTag(const Track &);

			Void		 GetSupportedFileExtensions();
		slots:
			Void		 OnChangeSize(const Size &);

			Void		 OnSelectDirectory(const Directory &);
			Void		 OnSelectFile(ListEntry *);

			Void		 OnModifyTrack(const Track &);

			Void		 OnSave();
			Void		 OnSaveAll();
		public:
					 ChooserFiles();
					~ChooserFiles();
	};
};

#endif
