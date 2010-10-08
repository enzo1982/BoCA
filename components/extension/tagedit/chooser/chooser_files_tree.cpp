 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "chooser_files_tree.h"

Signal1<Void, const Directory &>	 BoCA::ChooserFilesTree::onSelectDirectory;

BoCA::ChooserFilesTree::ChooserFilesTree(const Directory &iDirectory) : Tree(iDirectory.GetDirectoryName())
{
	directory = iDirectory;

	if (directory.GetDirectoryName() == NIL) SetText(directory);

	onOpen.Connect(&ChooserFilesTree::OnOpen, this);
	onClose.Connect(&ChooserFilesTree::OnClose, this);

	onSelect.Connect(&ChooserFilesTree::OnSelect, this);
}

BoCA::ChooserFilesTree::~ChooserFilesTree()
{
	foreach (Tree *tree, trees) DeleteObject(tree);
}

/* Called when the tree is opened.
 * ----
 * Fills the tree with subdirectory trees.
 */
Void BoCA::ChooserFilesTree::OnOpen()
{
	const Array<Directory>	&subDirectories = directory.GetDirectories();

	foreach (const Directory &currentDirectory, subDirectories)
	{
		Tree	*newTree = new ChooserFilesTree(currentDirectory);

		trees.Add(newTree);

		Add(newTree);
	}
}

/* Called when the tree is closed.
 * ----
 * Cleans up subdirectory trees.
 */
Void BoCA::ChooserFilesTree::OnClose()
{
	foreach (Tree *tree, trees) DeleteObject(tree);

	trees.RemoveAll();
}

/* Called when the entry is selected.
 * ----
 * Fires the onSelectDirectory signal.
 */
Void BoCA::ChooserFilesTree::OnSelect()
{
	onSelectDirectory.Emit(directory);;
}
