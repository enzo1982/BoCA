 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_CHOOSER_FILES_TREE_
#define _H_TAGEDIT_CHOOSER_FILES_TREE_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ChooserFilesTree : public Tree
	{
		private:
			Directory				 directory;
			Array<Tree *>				 trees;
		public:
			static Signal1<Void, const Directory &>	 onSelectDirectory;

								 ChooserFilesTree(const Directory &);
								~ChooserFilesTree();
		slots:
			Void					 OnOpen();
			Void					 OnClose();

			Void					 OnSelect();
	};
};

#endif
