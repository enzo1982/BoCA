 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

#include "chooser_files.h"
#include "chooser_files_tree.h"
#include "chooser_files_utilities.h"

BoCA::ChooserFiles::ChooserFiles() : Chooser("Files")
{
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Extensions::Tag Editor");

	SetText(i18n->TranslateString("Files"));

	list_directories= new ListBox(Point(7,7), Size(150, 150));

	div_split	= new Divider(160, OR_VERT | DIV_MOVABLE);
	div_split->onDrag.Connect(&ChooserFiles::OnDragDivider, this);

	edit_directory	= new EditBox(NIL, Point(165, 7), Size(100, 0));
	edit_directory->Deactivate();

	list_files	= new ListBox(Point(165, 34), Size(100, 150));
	list_files->AddTab(i18n->TranslateString("File"));
	list_files->Deactivate();
	list_files->onSelectEntry.Connect(&ChooserFiles::OnSelectFile, this);

	text_nofiles	= new Text(i18n->TranslateString("no audio files found"), Point());
	text_nofiles->SetFont(GUI::Font(GUI::Font::Default, 12, GUI::Font::Bold, 0, Setup::GrayTextColor));

	btn_save	= new Button(i18n->TranslateString("Save"), NIL, Point(176, 30), Size());
	btn_save->SetOrientation(OR_LOWERRIGHT);
	btn_save->Deactivate();
	btn_save->onAction.Connect(&ChooserFiles::OnSave, this);

	btn_saveall	= new Button(i18n->TranslateString("Save all"), NIL, Point(88, 30), Size());
	btn_saveall->SetOrientation(OR_LOWERRIGHT);
	btn_saveall->Deactivate();
	btn_saveall->onAction.Connect(&ChooserFiles::OnSaveAll, this);

	ChooserFilesTree::onSelectDirectory.Connect(&ChooserFiles::OnSelectDirectory, this);

#ifdef __WIN32__
	/* Add local drives.
	 */
	for (Int i = 0; i < 26; i++)
	{
		char		 name[4] = { 'A' + i, ':', '\\', 0};
		Directory	 drive(name);

		if (drive.Exists())
		{
			Tree		*tree = new ChooserFilesTree(drive);

			trees.Add(tree);

			list_directories->Add(tree);
		}
	}
#else
	/* Add root directory.
	 */
	Tree	*root = new ChooserFilesTree(Directory("/"));

	trees.Add(root);

	list_directories->Add(root);
#endif

	Add(list_directories);

	Add(div_split);

	Add(edit_directory);
	Add(list_files);
	Add(text_nofiles);

	Add(btn_save);
	Add(btn_saveall);

	onChangeSize.Connect(&ChooserFiles::OnChangeSize, this);
}

BoCA::ChooserFiles::~ChooserFiles()
{
	ChooserFilesTree::onSelectDirectory.Disconnect(&ChooserFiles::OnSelectDirectory, this);

	list_files->RemoveAllEntries();

	foreach (Tree *tree, trees) DeleteObject(tree);

	DeleteObject(list_directories);

	DeleteObject(div_split);

	DeleteObject(edit_directory);
	DeleteObject(list_files);
	DeleteObject(text_nofiles);

	DeleteObject(btn_save);
	DeleteObject(btn_saveall);
}

/* Called when component canvas size changes.
 * ----
 */
Void BoCA::ChooserFiles::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_directories->SetHeight(clientSize.cy - 15);

	edit_directory->SetWidth(clientSize.cx - edit_directory->GetX() - 8);
	list_files->SetSize(Size(clientSize.cx - list_files->GetX() - 8, clientSize.cy - 72));
	text_nofiles->SetPosition(list_files->GetPosition() + Point((list_files->GetWidth() - text_nofiles->textSize.cx) / 2, (list_files->GetHeight() - text_nofiles->textSize.cy) / 2));
}

/* Called when the splitting divider is dragged with the mouse.
 * ----
 */
Void BoCA::ChooserFiles::OnDragDivider(Int pos)
{
	Rect	 clientRect = Rect(GetRealPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	if (pos > clientSize.cx - 300) pos = clientSize.cx - 300;
	if (pos <		  150) pos =		     150;

	Surface	*surface = GetDrawSurface();

	surface->StartPaint(clientRect);

	div_split->Hide();
	list_directories->Hide();
	edit_directory->Hide();
	list_files->Hide();

	list_directories->SetWidth(pos - 10);

	edit_directory->SetWidth(clientSize.cx - pos - 13);
	edit_directory->SetX(pos + 5);

	list_files->SetWidth(clientSize.cx - pos - 13);
	list_files->SetX(pos + 5);

	div_split->SetPos(pos);

	text_nofiles->SetPosition(list_files->GetPosition() + Point((list_files->GetWidth() - text_nofiles->textSize.cx) / 2, (list_files->GetHeight() - text_nofiles->textSize.cy) / 2));

	div_split->Show();
	list_directories->Show();
	edit_directory->Show();
	list_files->Show();

	if (text_nofiles->IsVisible()) text_nofiles->Paint(SP_PAINT);

	surface->EndPaint();
}

/* Called when a directory entry is selected.
 * ----
 * Reads files in the selected directory and adds them to the files list.
 */
Void BoCA::ChooserFiles::OnSelectDirectory(const Directory &directory)
{
	if (String(directory) == edit_directory->GetText()) return;

	GetSupportedFileExtensions();

	if (modified.Length() > 0)
	{
		if (IDYES == Dialogs::QuickMessage("There are unsaved files in this directory. Would you like to save them now?", "Save changes", MB_YESNO, IDI_QUESTION)) OnSaveAll();
	}

	edit_directory->SetText(String(directory).Append(Directory::GetDirectoryDelimiter()));

	if (IsRegistered()) container->GetDrawSurface()->StartPaint(Rect(list_files->GetRealPosition(), list_files->GetSize()));

	list_files->RemoveAllEntries();

	const Array<File>	&files = directory.GetFiles();

	foreach (const File &file, files)
	{
		String	 filename = file.GetFileName();
		String	 extension = filename.Tail(filename.Length() - filename.FindLast(".") - 1).ToLower();

		if (extensions.Get(extension.ComputeCRC32()) != NIL) list_files->AddEntry(filename);
	}

	if (list_files->Length() == 0)
	{
		list_files->Deactivate();

		if (text_nofiles->IsVisible())	text_nofiles->Paint(SP_PAINT);
		else				text_nofiles->Show();
	}
	else
	{
		text_nofiles->Hide();
		list_files->Activate();
	}

	if (IsRegistered()) container->GetDrawSurface()->EndPaint();

	btn_save->Deactivate();
	btn_saveall->Deactivate();

	onSelectNone.Emit();

	tracks.RemoveAll();
	modified.RemoveAll();
}

/* Called when a file entry is selected.
 * ----
 * Tries to open the file and generate a track object.
 */
Void BoCA::ChooserFiles::OnSelectFile(ListEntry *entry)
{
	if (tracks.Get(list_files->GetSelectedEntryNumber()) != NIL)
	{
		if (modified.Get(list_files->GetSelectedEntryNumber()) != NIL)	btn_save->Activate();
		else								btn_save->Deactivate();

		onSelectTrack.Emit(tracks.Get(list_files->GetSelectedEntryNumber()));

		return;
	}

	File			 file(entry->GetText(), edit_directory->GetText());
	DecoderComponent	*decoder = ChooserFilesUtilities::CreateDecoderComponent(file);

	if (decoder == NIL)
	{
		I18n	*i18n	= I18n::Get();

		Utilities::ErrorMessage("Unable to open file: %1\n\nError: %2", file.GetFileName(), i18n->TranslateString("Unknown file type", "Messages"));

		return;
	}

	Track	 track;
	Error	 error = decoder->GetStreamInfo(file, track);
	String	 errorString = decoder->GetErrorString();

	Registry::Get().DeleteComponent(decoder);

	if (error == Error())
	{
		I18n	*i18n	= I18n::Get();

		Utilities::ErrorMessage("Unable to open file: %1\n\nError: %2", file.GetFileName(), i18n->TranslateString(errorString, "Messages"));

		return;
	}

	track.origFilename = file;

	btn_save->Deactivate();

	onSelectTrack.Emit(track);

	tracks.Add(track, list_files->GetSelectedEntryNumber());
}

/* Called when a track is modified.
 * ----
 * Finds the track and marks it as modified.
 */
Void BoCA::ChooserFiles::OnModifyTrack(const Track &track)
{
	for (Int i = 0; i < tracks.Length(); i++)
	{
		if (tracks.GetNth(i).GetTrackID() == track.GetTrackID())
		{
			GUI::Font	 font = list_files->GetNthEntry(tracks.GetNthIndex(i))->GetFont();

			font.SetColor(Color(0, 0, 255));

			list_files->GetNthEntry(tracks.GetNthIndex(i))->SetFont(font);

			/* Update track.
			 */
			tracks.GetNthReference(i) = track;

			/* Add it to the list of modified tracks.
			 */
			modified.Add(track.GetTrackID(), tracks.GetNthIndex(i));

			if (tracks.GetNthIndex(i) == list_files->GetSelectedEntryNumber()) btn_save->Activate();

			btn_saveall->Activate();

			return;
		}
	}
}

/* Called when the "Save" button is clicked.
 * ----
 * Invokes SaveFileTag() for the selected file.
 */
Void BoCA::ChooserFiles::OnSave()
{
	Int	 i = list_files->GetSelectedEntryNumber();

	if (SaveFileTag(tracks.Get(i)) != Success()) return;

	GUI::Font	 font = list_files->GetNthEntry(i)->GetFont();

	font.SetColor(Color(0, 0, 0));

	list_files->GetNthEntry(i)->SetFont(font);

	modified.Remove(i);

	btn_save->Deactivate();

	if (modified.Length() == 0) btn_saveall->Deactivate();
}

/* Called when the "Save all" button is clicked.
 * ----
 * Invokes SaveFileTag() for all modified files.
 */
Void BoCA::ChooserFiles::OnSaveAll()
{
	Int	 failed = 0;

	while (modified.Length() - failed > 0)
	{
		Int	 i = modified.GetNthIndex(0 + failed);

		if (SaveFileTag(tracks.Get(i)) != Success()) { failed++; continue; }

		GUI::Font	 font = list_files->GetNthEntry(i)->GetFont();

		font.SetColor(Color(0, 0, 0));

		list_files->GetNthEntry(i)->SetFont(font);

		modified.Remove(i);
	}

	if (modified.Get(list_files->GetSelectedEntryNumber()) == NIL) btn_save->Deactivate();

	if (modified.Length() == 0) btn_saveall->Deactivate();
}

/* Called when the currently selected entry needs to be selected again.
 * ----
 */
Void BoCA::ChooserFiles::ReselectEntry()
{
	if (list_files->GetSelectedEntry() == NIL) return;

	OnSelectFile(list_files->GetSelectedEntry());
}

Int BoCA::ChooserFiles::SaveFileTag(const Track &track)
{
	Config	*config = Config::Get();

	Int	 error	     = Error();
	String	 errorString = "Unknown error";

	/* Get tagger mode, format and ID
	 */
	DecoderComponent		*decoder = ChooserFilesUtilities::CreateDecoderComponent(track.origFilename);
	const Array<FileFormat *>	&formats = decoder->GetFormats();

	Int	 tagMode = TAG_MODE_NONE;
	String	 tagFormat;
	String	 taggerID;

	String	 lcURI = track.origFilename.ToLower();

	foreach (FileFormat *format, formats)
	{
		foreach (const String &extension, format->GetExtensions())
		{
			if (lcURI.EndsWith(String(".").Append(extension)))
			{
				tagMode	  = format->GetTagMode();
				tagFormat = format->GetTagFormat();
				taggerID  = format->GetTaggerID();

				break;
			}
		}
	}

	Registry::Get().DeleteComponent(decoder);

	/* Update tag.
	 */
	if (tagMode != TAG_MODE_NONE)
	{
		Registry	&boca = AS::Registry::Get();
		TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID(taggerID);

		if (tagger != NIL)
		{
			foreach (TagFormat *tag, tagger->GetTagFormats())
			{
				if (tag->GetName() != tagFormat) continue;

				/* Set to Success() by default, so we won't report an
				 * error if the tag format is simply deactivated.
				 */
				error = Success();

				if (config->GetIntValue("Tags", String("Enable").Append(String(tagFormat).Replace(" ", NIL)), tag->IsDefault()))
				{
					error	    = tagger->UpdateStreamInfo(track.origFilename, track);
					errorString = tagger->GetErrorString();
				}

				break;
			}

			boca.DeleteComponent(tagger);
		}
		else
		{
			error	    = Error();
			errorString = "Not supported";
		}
	}

	/* Check for error.
	 */
	if (error == Error())
	{
		Utilities::ErrorMessage(String(BoCA::I18n::Get()->TranslateString("Unable to update tag: %1\n\nError: %2")).Replace("%1", track.origFilename).Replace("%2", BoCA::I18n::Get()->TranslateString(errorString)));

		return Error();
	}
	
	return Success();
}

Void BoCA::ChooserFiles::GetSupportedFileExtensions()
{
	if (extensions.Length() > 0) return;

	Registry		&boca = Registry::Get();

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_DECODER) continue;

		const Array<FileFormat *>	&formats = boca.GetComponentFormats(i);

		foreach (FileFormat *format, formats)
		{
			const Array<String>	&extensions = format->GetExtensions();

			foreach (const String &extension, extensions) this->extensions.Add(extension.ToLower(), extension.ToLower().ComputeCRC32());
		}
	}
}
