/**************************************************************************/
/*  library_assets.cpp                                                    */
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

#include "library_assets.h"

#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/resource_loader.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/file_system/editor_file_system.h"
#include "editor/inspector/editor_inspector.h"
#include "editor/resource/resource_editor.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/label.h"
#include "scene/gui/texture_rect.h"

void LibraryAssets::_bind_methods() {
	ClassDB::bind_method(D_METHOD("scan_project"), &LibraryAssets::scan_project);
	ClassDB::bind_method(D_METHOD("_update_assets_list"), &LibraryAssets::_update_assets_list);
	ClassDB::bind_method(D_METHOD("_on_assets_search_text_changed", "text"), &LibraryAssets::_on_assets_search_text_changed);
	ClassDB::bind_method(D_METHOD("_on_asset_item_activated", "index"), &LibraryAssets::_on_asset_item_activated);
	ClassDB::bind_method(D_METHOD("_get_drag_data_fw", "point", "from"), &LibraryAssets::_get_drag_data_fw);
}

void LibraryAssets::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (EditorFileSystem::get_singleton()) {
				EditorFileSystem::get_singleton()->connect("filesystem_changed", callable_mp(this, &LibraryAssets::scan_project));
			}
			scan_project();
		} break;
	}
}

void LibraryAssets::scan_project() {
	all_assets.clear();
	EditorFileSystemDirectory *fs = EditorFileSystem::get_singleton()->get_filesystem();
	if (fs) {
		_scan_recursive(fs);
	}
	_update_assets_list();
}

void LibraryAssets::_scan_recursive(EditorFileSystemDirectory *p_dir) {
	for (int i = 0; i < p_dir->get_subdir_count(); i++) {
		_scan_recursive(p_dir->get_subdir(i));
	}

	for (int i = 0; i < p_dir->get_file_count(); i++) {
		String path = p_dir->get_file_path(i);
		String type = p_dir->get_file_type(i);
		String ext = path.get_extension().to_lower();

		if ((type == "Resource" || ClassDB::is_parent_class(type, "Resource")) && (ext == "tres" || ext == "res")) {
			AssetData asset_data;
			asset_data.path = path;
			asset_data.name = path.get_file();
			asset_data.type = type;
			asset_data.icon = EditorNode::get_singleton()->get_class_icon(type, "Resource");
			all_assets.push_back(asset_data);

			// Scan for internal resources in text files if needed
			if (path.ends_with(".tscn") || path.ends_with(".tres")) {
				_scan_internal_resources(path);
			}
		}
	}
}

void LibraryAssets::_scan_internal_resources(const String &p_path) {
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	if (f.is_null()) {
		return;
	}

	while (!f->eof_reached()) {
		String line = f->get_line().strip_edges();
		if (line.begins_with("[sub_resource")) {
			// Basic parser for [sub_resource type="ClassName" id="ID"]
			int id_pos = line.find("id=\"");
			int type_pos = line.find("type=\"");

			if (id_pos != -1 && type_pos != -1) {
				String id = line.substr(id_pos + 4).get_slice("\"", 0);
				String type = line.substr(type_pos + 6).get_slice("\"", 0);

				AssetData asset_data;
				asset_data.path = p_path + "::" + id;
				asset_data.name = p_path.get_file() + "::" + id;
				asset_data.type = type;
				asset_data.icon = EditorNode::get_singleton()->get_class_icon(type, "Resource");
				all_assets.push_back(asset_data);
			}
		}
	}
}

void LibraryAssets::_update_assets_list() {
	assets_list->clear();
	String filter = assets_search->get_text();

	for (const AssetData &asset_data : all_assets) {
		if (!filter.is_empty() && asset_data.name.findn(filter) == -1 && asset_data.type.findn(filter) == -1) {
			continue;
		}

		int idx = assets_list->add_item(asset_data.name, asset_data.icon);
		assets_list->set_item_tooltip(idx, asset_data.path + "\nType: " + asset_data.type);
		assets_list->set_item_metadata(idx, asset_data.path);
	}
}

void LibraryAssets::_on_assets_search_text_changed(const String &p_text) {
	_update_assets_list();
}

void LibraryAssets::_on_asset_item_activated(int p_index) {
	String path = assets_list->get_item_metadata(p_index);
	if (path.is_empty()) {
		return;
	}

	Ref<Resource> res = ResourceLoader::load(path);
	if (res.is_valid()) {
		// Open in Inspector
		EditorInterface::get_singleton()->edit_resource(res);
		// Open in ResourceEditor
		if (ResourceEditor::get_singleton()) {
			ResourceEditor::get_singleton()->edit(res);
			ResourceEditor::get_singleton()->show(); // Ensure visible
		}

		// Optionally, if we want to show it in the Workbench tab
		if (workbench_inspector_ref) {
			workbench_inspector_ref->edit(res.ptr());
			// Tab switching should be handled by the parent if needed, or we just edit it here.
			// The parent Library class will handle the visibility of the workbench tab if it detects a signal?
			// For now, let's just edit it.
			// Ideally, we emit a signal "resource_selected" and let Library handle it.
			// But for strict refactor matching original:
			// tabs->set_current_tab(1); // logic is lost here as we don't have access to tabs.
		}
	} else {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Could not load resource: %s"), path));
	}
}

Variant LibraryAssets::_get_drag_data_fw(const Point2 &p_point, Control *p_from) {
	if (p_from == assets_list) {
		int idx = assets_list->get_item_at_position(p_point, true);
		if (idx != -1) {
			String path = assets_list->get_item_metadata(idx);
			if (path.is_empty()) {
				return Variant();
			}

			Dictionary drag_data;
			drag_data["type"] = "files";
			Vector<String> files;
			files.push_back(path);
			drag_data["files"] = files;
			drag_data["from"] = this;

			// Preview
			HBoxContainer *preview = memnew(HBoxContainer);
			TextureRect *icon = memnew(TextureRect);
			icon->set_texture(assets_list->get_item_icon(idx));
			preview->add_child(icon);
			Label *label = memnew(Label);
			label->set_text(assets_list->get_item_text(idx));
			preview->add_child(label);
			assets_list->set_drag_preview(preview);

			return drag_data;
		}
	}
	return Variant();
}

void LibraryAssets::set_workbench_inspector(EditorInspector *p_inspector) {
	workbench_inspector_ref = p_inspector;
}

LibraryAssets::LibraryAssets() {
	set_name(TTR("Resources"));

	assets_search = memnew(LineEdit);
	assets_search->set_placeholder(TTR("Search Resources (Recursive)..."));
	assets_search->set_clear_button_enabled(true);
	assets_search->connect("text_changed", callable_mp(this, &LibraryAssets::_on_assets_search_text_changed));
	add_child(assets_search);

	assets_list = memnew(ItemList);
	assets_list->set_v_size_flags(SIZE_EXPAND_FILL);
	assets_list->set_icon_mode(ItemList::ICON_MODE_TOP);
	assets_list->set_max_columns(0);
	assets_list->set_same_column_width(true);
	assets_list->set_fixed_icon_size(Size2(64, 64) * EDSCALE);
	assets_list->connect("item_activated", callable_mp(this, &LibraryAssets::_on_asset_item_activated));
	assets_list->set_drag_forwarding(callable_mp(this, &LibraryAssets::_get_drag_data_fw), Callable(), Callable());
	add_child(assets_list);
}

LibraryAssets::~LibraryAssets() {
}
