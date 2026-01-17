/**************************************************************************/
/*  library.h                                                             */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "editor/inspector/editor_inspector.h"
#include "scene/gui/box_container.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/popup_menu.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"

class Library : public VBoxContainer {
	GDCLASS(Library, VBoxContainer);

	TabContainer *tabs;
	Tree *folders_tree;
	ItemList *assets_list;
	EditorInspector *workbench_inspector;
	PopupMenu *context_menu;
	ConfirmationDialog *delete_dialog;
	ConfirmationDialog *rename_dialog;
	LineEdit *rename_edit;

	String current_path;

	// CraftTable Components
	LineEdit *craft_search;
	Tree *craft_tree;

	void _update_folders_tree();
	void _scan_folders(const String &p_path, TreeItem *p_parent);
	void _update_file_list(const String &p_dir_path);

	// CraftTable Methods
	void _update_craft_tree();
	void _on_craft_search_text_changed(const String &p_text);

	void _on_folder_selected();
	void _on_asset_item_selected(int p_index);
	void _on_asset_item_clicked(int p_index, const Vector2 &p_pos, MouseButton p_mouse_button_index);
	void _on_context_menu_id_pressed(int p_id);
	void _on_delete_confirmed();
	void _on_rename_confirmed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Library();
	~Library();
};
