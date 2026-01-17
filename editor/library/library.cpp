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
#include "editor/editor_node.h"
#include "scene/gui/label.h"

void Library::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update_folders_tree"), &Library::_update_folders_tree);
	ClassDB::bind_method(D_METHOD("_on_folder_selected"), &Library::_on_folder_selected);
	ClassDB::bind_method(D_METHOD("_on_asset_item_selected", "index"), &Library::_on_asset_item_selected);
	ClassDB::bind_method(D_METHOD("_on_asset_item_clicked", "index", "at_position", "mouse_button_index"), &Library::_on_asset_item_clicked);
	ClassDB::bind_method(D_METHOD("_on_context_menu_id_pressed", "id"), &Library::_on_context_menu_id_pressed);
	ClassDB::bind_method(D_METHOD("_on_delete_confirmed"), &Library::_on_delete_confirmed);
	ClassDB::bind_method(D_METHOD("_on_rename_confirmed"), &Library::_on_rename_confirmed);
}

void Library::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_folders_tree();
			_update_file_list("res://");
		} break;
	}
}

void Library::_update_folders_tree() {
	if (!folders_tree) {
		return;
	}
	folders_tree->clear();
	TreeItem *root = folders_tree->create_item();
	root->set_text(0, "res://");
	root->set_metadata(0, "res://");
	if (has_theme_icon("Folder", "EditorIcons")) {
		root->set_icon(0, get_theme_icon("Folder", "EditorIcons"));
	}

	_scan_folders("res://", root);
}

void Library::_scan_folders(const String &p_path, TreeItem *p_parent) {
	Ref<DirAccess> da = DirAccess::open(p_path);
	if (da.is_null()) {
		return;
	}

	da->list_dir_begin();
	String f = da->get_next();

	List<String> folders;

	while (!f.is_empty()) {
		if (f == "." || f == ".." || f == ".import") {
			f = da->get_next();
			continue;
		}

		if (da->current_is_dir()) {
			folders.push_back(f);
		}
		f = da->get_next();
	}
	da->list_dir_end();

	folders.sort();

	for (const String &E : folders) {
		TreeItem *ti = folders_tree->create_item(p_parent);
		ti->set_text(0, E);
		String full_path = p_path.path_join(E);
		ti->set_metadata(0, full_path);
		ti->set_collapsed(true);
		if (has_theme_icon("Folder", "EditorIcons")) {
			ti->set_icon(0, get_theme_icon("Folder", "EditorIcons"));
		}
		_scan_folders(full_path, ti);
	}
}

void Library::_update_file_list(const String &p_dir_path) {
	current_path = p_dir_path;
	assets_list->clear();

	Ref<DirAccess> da = DirAccess::open(p_dir_path);
	if (da.is_null()) {
		return;
	}

	da->list_dir_begin();
	String f = da->get_next();

	List<String> files;

	while (!f.is_empty()) {
		if (f == "." || f == ".." || f == ".import") {
			f = da->get_next();
			continue;
		}

		if (!da->current_is_dir()) {
			if (!f.ends_with(".import") && (f.ends_with(".tres") || f.ends_with(".res"))) {
				files.push_back(f);
			}
		}
		f = da->get_next();
	}
	da->list_dir_end();

	files.sort();

	for (const String &E : files) {
		int idx = assets_list->add_item(E);
		String full_path = p_dir_path.path_join(E);
		assets_list->set_item_metadata(idx, full_path);

		Ref<Texture2D> icon;
		// Try to load resource to get type icon (expensive?)
		// For now, generic Resource icon or File
		if (has_theme_icon("Resource", "EditorIcons")) {
			icon = get_theme_icon("Resource", "EditorIcons");
		} else {
			icon = get_theme_icon("File", "EditorIcons");
		}
		assets_list->set_item_icon(idx, icon);
	}
}

void Library::_on_folder_selected() {
	if (!folders_tree) {
		return;
	}
	TreeItem *ti = folders_tree->get_selected();
	if (!ti) {
		return;
	}

	String path = ti->get_metadata(0);
	_update_file_list(path);
}

void Library::_on_asset_item_selected(int p_index) {
	String path = assets_list->get_item_metadata(p_index);
	if (path.is_empty()) {
		return;
	}

	if (ResourceLoader::exists(path)) {
		Ref<Resource> res = ResourceLoader::load(path);
		if (res.is_valid() && workbench_inspector) {
			workbench_inspector->edit(res.ptr());
		}
	}
}

void Library::_on_asset_item_clicked(int p_index, const Vector2 &p_pos, MouseButton p_mouse_button_index) {
	if (p_mouse_button_index == MouseButton::RIGHT) {
		assets_list->select(p_index);
		_on_asset_item_selected(p_index);
		context_menu->set_position(get_screen_position() + get_local_mouse_position());
		// Note: get_local_mouse_position is relative to Library, but click is on List.
		// Use global mouse pos usually, or calculate offset.
		// For simplicity:
		context_menu->set_position(assets_list->get_screen_position() + p_pos);
		context_menu->popup();
	}
}

void Library::_on_context_menu_id_pressed(int p_id) {
	if (assets_list->get_selected_items().size() == 0) {
		return;
	}
	int idx = assets_list->get_selected_items()[0];
	String path = assets_list->get_item_metadata(idx);

	switch (p_id) {
		case 0: // Rename
			rename_edit->set_text(path.get_file());
			rename_dialog->popup_centered(Size2(300, 80));
			break;
		case 1: // Delete
			delete_dialog->set_text("Are you sure you want to delete " + path.get_file() + "?");
			delete_dialog->popup_centered();
			break;
	}
}

void Library::_on_delete_confirmed() {
	if (assets_list->get_selected_items().size() == 0) {
		return;
	}
	int idx = assets_list->get_selected_items()[0];
	String path = assets_list->get_item_metadata(idx);

	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	if (da->remove(path) == OK) {
		_update_file_list(current_path);
	} else {
		// Error handling
	}
}

void Library::_on_rename_confirmed() {
	if (assets_list->get_selected_items().size() == 0) {
		return;
	}
	int idx = assets_list->get_selected_items()[0];
	String path = assets_list->get_item_metadata(idx);
	String new_name = rename_edit->get_text();
	String new_path = path.get_base_dir().path_join(new_name);

	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	if (da->rename(path, new_path) == OK) {
		_update_file_list(current_path);
	}
}

Library::Library() {
	current_path = "res://";

	tabs = memnew(TabContainer);
	tabs->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(tabs);

	// Tab 1: Assets (Split View)
	HSplitContainer *assets_tab = memnew(HSplitContainer);
	assets_tab->set_name("Assets");

	folders_tree = memnew(Tree);
	folders_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	folders_tree->set_custom_minimum_size(Size2(150, 0));
	folders_tree->connect("cell_selected", callable_mp(this, &Library::_on_folder_selected));
	assets_tab->add_child(folders_tree);

	assets_list = memnew(ItemList);
	assets_list->set_v_size_flags(SIZE_EXPAND_FILL);
	assets_list->set_h_size_flags(SIZE_EXPAND_FILL);
	assets_list->set_icon_mode(ItemList::ICON_MODE_TOP);
	assets_list->set_max_columns(0); // Auto
	assets_list->set_fixed_icon_size(Size2i(64, 64));
	assets_list->connect("item_selected", callable_mp(this, &Library::_on_asset_item_selected));
	assets_list->connect("item_clicked", callable_mp(this, &Library::_on_asset_item_clicked));
	assets_tab->add_child(assets_list);

	tabs->add_child(assets_tab);

	// Context Menu
	context_menu = memnew(PopupMenu);
	context_menu->add_item("Rename", 0);
	context_menu->add_item("Delete", 1);
	context_menu->connect("id_pressed", callable_mp(this, &Library::_on_context_menu_id_pressed));
	add_child(context_menu);

	delete_dialog = memnew(ConfirmationDialog);
	delete_dialog->connect("confirmed", callable_mp(this, &Library::_on_delete_confirmed));
	add_child(delete_dialog);

	rename_dialog = memnew(ConfirmationDialog);
	VBoxContainer *vb = memnew(VBoxContainer);
	rename_dialog->add_child(vb);
	rename_edit = memnew(LineEdit);
	vb->add_child(rename_edit);
	rename_dialog->connect("confirmed", callable_mp(this, &Library::_on_rename_confirmed));
	add_child(rename_dialog);

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
