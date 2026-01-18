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
#include "editor/resource/resource_editor.h"
#include "editor/resource/resource_factory.h"
#include "editor/resource/resource_server.h"

#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/file_system/editor_file_system.h"
#include "scene/gui/label.h"

void Library::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_scan_project"), &Library::_scan_project);
	ClassDB::bind_method(D_METHOD("_update_assets_list"), &Library::_update_assets_list);
	ClassDB::bind_method(D_METHOD("_on_assets_search_text_changed", "text"), &Library::_on_assets_search_text_changed);
	ClassDB::bind_method(D_METHOD("_on_asset_item_activated", "index"), &Library::_on_asset_item_activated);
	ClassDB::bind_method(D_METHOD("_get_drag_data_fw", "point", "from"), &Library::_get_drag_data_fw);

	ClassDB::bind_method(D_METHOD("_update_craft_tree"), &Library::_update_craft_tree);
	ClassDB::bind_method(D_METHOD("_on_craft_search_text_changed", "text"), &Library::_on_craft_search_text_changed);
	ClassDB::bind_method(D_METHOD("_on_craft_item_activated"), &Library::_on_craft_item_activated);
	ClassDB::bind_method(D_METHOD("_on_creation_file_selected", "path"), &Library::_on_creation_file_selected);
}

void Library::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (EditorFileSystem::get_singleton()) {
				EditorFileSystem::get_singleton()->connect("filesystem_changed", callable_mp(this, &Library::_scan_project));
			}
			_scan_project();
			_update_craft_tree();
		} break;
	}
}

void Library::_scan_project() {
	all_assets.clear();
	EditorFileSystemDirectory *fs = EditorFileSystem::get_singleton()->get_filesystem();
	if (fs) {
		_scan_recursive(fs);
	}
	_update_assets_list();
}

void Library::_scan_recursive(EditorFileSystemDirectory *p_dir) {
	for (int i = 0; i < p_dir->get_subdir_count(); i++) {
		_scan_recursive(p_dir->get_subdir(i));
	}

	for (int i = 0; i < p_dir->get_file_count(); i++) {
		String path = p_dir->get_file_path(i);
		String type = p_dir->get_file_type(i);

		if (type == "Resource" || ClassDB::is_parent_class(type, "Resource")) {
			AssetData data;
			data.path = path;
			data.name = path.get_file();
			data.type = type;
			data.icon = EditorNode::get_singleton()->get_class_icon(type, "Resource");
			all_assets.push_back(data);

			// Scan for internal resources in text files if needed
			if (path.ends_with(".tscn") || path.ends_with(".tres")) {
				_scan_internal_resources(path);
			}
		}
	}
}

void Library::_scan_internal_resources(const String &p_path) {
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

				AssetData data;
				data.path = p_path + "::" + id;
				data.name = p_path.get_file() + "::" + id;
				data.type = type;
				data.icon = EditorNode::get_singleton()->get_class_icon(type, "Resource");
				all_assets.push_back(data);
			}
		}
	}
}

void Library::_update_assets_list() {
	assets_list->clear();
	String filter = assets_search->get_text();

	for (const AssetData &data : all_assets) {
		if (!filter.is_empty() && data.name.findn(filter) == -1 && data.type.findn(filter) == -1) {
			continue;
		}

		int idx = assets_list->add_item(data.name, data.icon);
		assets_list->set_item_tooltip(idx, data.path + "\nType: " + data.type);
		assets_list->set_item_metadata(idx, data.path);
	}
}

void Library::_on_assets_search_text_changed(const String &p_text) {
	_update_assets_list();
}

void Library::_on_asset_item_activated(int p_index) {
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
		if (workbench_inspector) {
			workbench_inspector->edit(res.ptr());
			tabs->set_current_tab(1); // Switch to Workbench
		}
	} else {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Could not load resource: %s"), path));
	}
}

Variant Library::_get_drag_data_fw(const Point2 &p_point, Control *p_from) {
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

// ... (CraftTable methods remain largely the same, logic reused) ...

void Library::_update_craft_tree() {
	craft_tree->clear();
	TreeItem *root = craft_tree->create_item();

	String search_term = craft_search->get_text();

	if (ResourceServer::get_singleton()) {
		Array domains = ResourceServer::get_singleton()->get_registered_domains();
		for (int i = 0; i < domains.size(); i++) {
			String domain_name = domains[i];
			Dictionary info = ResourceServer::get_singleton()->get_domain_info(domain_name);
			String type = info.get("resource_type", "");
			Ref<Texture2D> icon = info.get("icon", Ref<Texture2D>());

			if (!search_term.is_empty() && domain_name.findn(search_term) == -1 && type.findn(search_term) == -1) {
				continue;
			}

			TreeItem *item = craft_tree->create_item(root);
			item->set_text(0, domain_name);
			item->set_metadata(0, domain_name);

			if (icon.is_valid()) {
				item->set_icon(0, icon);
			} else {
				// Fallback icon
				item->set_icon(0, get_theme_icon("Resource", "EditorIcons"));
			}
			item->set_tooltip_text(0, "Creates a new " + type);
		}
	}
}

void Library::_on_craft_search_text_changed(const String &p_text) {
	_update_craft_tree();
}

void Library::_on_craft_item_activated() {
	TreeItem *item = craft_tree->get_selected();
	if (!item) {
		return;
	}

	String domain_name = item->get_metadata(0);
	current_creation_domain = domain_name;

	// Show Save Dialog
	creation_dialog->set_current_file(domain_name + ".tres");
	creation_dialog->popup_centered_ratio();
}

void Library::_on_creation_file_selected(const String &p_path) {
	if (current_creation_domain.is_empty()) {
		return;
	}

	Ref<Resource> res = ResourceFactory::create_resource_from_domain(current_creation_domain);
	if (res.is_valid()) {
		Error err = ResourceSaver::save(res, p_path);
		if (err == OK) {
			_scan_project(); // Refresh list
			// Open
			EditorInterface::get_singleton()->edit_resource(res);
			if (ResourceEditor::get_singleton()) {
				ResourceEditor::get_singleton()->edit(res);
			}
		} else {
			EditorNode::get_singleton()->show_warning(TTR("Error saving resource to disk."));
		}
	} else {
		EditorNode::get_singleton()->show_warning(TTR("Failed to create resource instance."));
	}
	current_creation_domain = "";
}

Library::Library() {
	tabs = memnew(TabContainer);
	tabs->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(tabs);

	// Tab 1: Assets (Flat List)
	VBoxContainer *assets_tab = memnew(VBoxContainer);
	assets_tab->set_name("Assets");

	assets_search = memnew(LineEdit);
	assets_search->set_placeholder(TTR("Search Resources (Recursive)..."));
	assets_search->set_clear_button_enabled(true);
	assets_search->connect("text_changed", callable_mp(this, &Library::_on_assets_search_text_changed));
	assets_tab->add_child(assets_search);

	assets_list = memnew(ItemList);
	assets_list->set_v_size_flags(SIZE_EXPAND_FILL);
	assets_list->connect("item_activated", callable_mp(this, &Library::_on_asset_item_activated));
	assets_list->set_drag_forwarding(callable_mp(this, &Library::_get_drag_data_fw), Callable(), Callable());
	assets_tab->add_child(assets_list);

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

	craft_search = memnew(LineEdit);
	craft_search->set_placeholder(TTR("Search Zyris Resources..."));
	craft_search->connect("text_changed", callable_mp(this, &Library::_on_craft_search_text_changed));
	craft_tab->add_child(craft_search);

	craft_tree = memnew(Tree);
	craft_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	craft_tree->set_hide_root(true);
	craft_tree->connect("item_activated", callable_mp(this, &Library::_on_craft_item_activated));
	craft_tab->add_child(craft_tree);

	tabs->add_child(craft_tab);

	// Creation Dialog
	creation_dialog = memnew(EditorFileDialog);
	creation_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	creation_dialog->add_filter("*.tres", "Resource");
	creation_dialog->connect("file_selected", callable_mp(this, &Library::_on_creation_file_selected));
	add_child(creation_dialog);

	context_menu = nullptr;
	delete_dialog = nullptr;
	rename_dialog = nullptr;
	rename_edit = nullptr;
}

Library::~Library() {
}
