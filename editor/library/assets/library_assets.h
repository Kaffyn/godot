/**************************************************************************/
/*  library_assets.h                                                      */
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

#include "scene/gui/box_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"

class EditorFileSystemDirectory;
class EditorInspector;

class LibraryAssets : public VBoxContainer {
	GDCLASS(LibraryAssets, VBoxContainer);

	LineEdit *assets_search;
	ItemList *assets_list;

	// Internal data for filtering
	struct AssetData {
		String path;
		String name;
		String type;
		Ref<Texture2D> icon;
	};
	Vector<AssetData> all_assets;

	EditorInspector *workbench_inspector_ref = nullptr; // Reference to workbench for double-click action

	void _scan_recursive(EditorFileSystemDirectory *p_dir);
	void _scan_internal_resources(const String &p_path);

	void _update_assets_list();
	void _on_assets_search_text_changed(const String &p_text);
	void _on_asset_item_activated(int p_index);
	Variant _get_drag_data_fw(const Point2 &p_point, Control *p_from);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void scan_project();
	void set_workbench_inspector(EditorInspector *p_inspector);

	LibraryAssets();
	~LibraryAssets();
};
