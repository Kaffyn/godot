/**************************************************************************/
/*  library.cpp                                                           */
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

#include "library.h"
#include "editor/resource/resource_library.h"

#include "core/io/dir_access.h"
#include "core/io/resource_loader.h"
#include "scene/gui/label.h"

void Library::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update_assets_tree"), &Library::_update_assets_tree);
	ClassDB::bind_method(D_METHOD("_on_asset_selected"), &Library::_on_asset_selected);
}

void Library::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_assets_tree();
		} break;
	}
}

void Library::_update_assets_tree() {
	if (!assets_tree) {
		return;
	}
	assets_tree->clear();
	TreeItem *root = assets_tree->create_item();
	root->set_text(0, "res://");
	root->set_metadata(0, "res://");
	if (has_theme_icon("Folder", "EditorIcons")) {
		root->set_icon(0, get_theme_icon("Folder", "EditorIcons"));
	}

	_scan_dir("res://", root);
}

void Library::_scan_dir(const String &p_path, TreeItem *p_parent) {
	Ref<DirAccess> da = DirAccess::open(p_path);
	if (da.is_null()) {
		return;
	}

	da->list_dir_begin();
	String f = da->get_next();

	// Separate folders and files for sorting
	List<String> folders;
	List<String> files;

	while (!f.is_empty()) {
		if (f == "." || f == ".." || f == ".import") {
			f = da->get_next();
			continue;
		}

		if (da->current_is_dir()) {
			folders.push_back(f);
		} else {
			if (!f.ends_with(".import")) {
				files.push_back(f);
			}
		}
		f = da->get_next();
	}
	da->list_dir_end();

	folders.sort();
	files.sort();

	for (const String &E : folders) {
		TreeItem *ti = assets_tree->create_item(p_parent);
		ti->set_text(0, E);
		String full_path = p_path.path_join(E);
		ti->set_metadata(0, full_path);
		ti->set_collapsed(true);
		if (has_theme_icon("Folder", "EditorIcons")) {
			ti->set_icon(0, get_theme_icon("Folder", "EditorIcons"));
		}
		_scan_dir(full_path, ti);
	}

	for (const String &E : files) {
		TreeItem *ti = assets_tree->create_item(p_parent);
		ti->set_text(0, E);
		String full_path = p_path.path_join(E);
		ti->set_metadata(0, full_path);
		if (has_theme_icon("File", "EditorIcons")) {
			ti->set_icon(0, get_theme_icon("File", "EditorIcons"));
		}
	}
}

void Library::_on_asset_selected() {
	if (!assets_tree) {
		return;
	}
	TreeItem *ti = assets_tree->get_selected();
	if (!ti) {
		return;
	}
	String path = ti->get_metadata(0);
	if (path.is_empty()) {
		return;
	}

	// Simple check if it's a file by checking extension
	// (Directories usually don't have extension or we can check DirAccess)
	// But simpler: try to load if it looks like a resource.
	if (ResourceLoader::exists(path)) {
		Ref<Resource> res = ResourceLoader::load(path);
		if (res.is_valid() && workbench_inspector) {
			workbench_inspector->edit(res.ptr());
		}
	}
}

Library::Library() {
	tabs = memnew(TabContainer);
	tabs->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(tabs);

	// Tab 1: Assets
	VBoxContainer *assets_tab = memnew(VBoxContainer);
	assets_tab->set_name("Assets");

	assets_tree = memnew(Tree);
	assets_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	assets_tree->connect("item_selected", callable_mp(this, &Library::_on_asset_selected));
	assets_tab->add_child(assets_tree);

	tabs->add_child(assets_tab);

	// Tab 2: Workbench
	VBoxContainer *workbench_tab = memnew(VBoxContainer);
	workbench_tab->set_name("Workbench");

	workbench_inspector = memnew(EditorInspector);
	workbench_inspector->set_v_size_flags(SIZE_EXPAND_FILL);
	workbench_tab->add_child(workbench_inspector);

	tabs->add_child(workbench_tab);

	// Tab 3: CraftTable
	VBoxContainer *craft_tab = memnew(VBoxContainer);
	craft_tab->set_name("CraftTable");
	ResourceLibrary *resource_lib = memnew(ResourceLibrary);
	resource_lib->set_v_size_flags(SIZE_EXPAND_FILL);
	craft_tab->add_child(resource_lib);
	tabs->add_child(craft_tab);
}

Library::~Library() {
}
