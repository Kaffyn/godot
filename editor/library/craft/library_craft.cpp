/**************************************************************************/
/*  library_craft.cpp                                                     */
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

#include "library_craft.h"
#include "core/io/resource_saver.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/resource/resource_editor.h"
#include "editor/resource/resource_server.h"

void LibraryCraft::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_update_craft_tree"), &LibraryCraft::_update_craft_tree);
	ClassDB::bind_method(D_METHOD("_on_craft_search_text_changed", "text"), &LibraryCraft::_on_craft_search_text_changed);
	ClassDB::bind_method(D_METHOD("_on_craft_item_activated"), &LibraryCraft::_on_craft_item_activated);
	ClassDB::bind_method(D_METHOD("_on_creation_file_selected", "path"), &LibraryCraft::_on_creation_file_selected);
}

void LibraryCraft::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_craft_tree();
		} break;
	}
}

void LibraryCraft::_update_craft_tree() {
	craft_tree->clear();
	TreeItem *root = craft_tree->create_item();

	String search_term = craft_search->get_text();

	if (ResourceServer::get_singleton()) {
		Array domains = ResourceServer::get_singleton()->get_registered_domains();
		HashMap<String, TreeItem *> categories;

		for (int i = 0; i < domains.size(); i++) {
			String domain_name = domains[i];
			Dictionary info = ResourceServer::get_singleton()->get_domain_info(domain_name);
			String type = info.get("resource_type", "");
			Ref<Texture2D> icon = info.get("icon", Ref<Texture2D>());
			Dictionary rules = info.get("rules", Dictionary());
			String category_name = rules.get("category", "General");

			if (!search_term.is_empty() && domain_name.findn(search_term) == -1 && type.findn(search_term) == -1 && category_name.findn(search_term) == -1) {
				continue;
			}

			TreeItem *cat_item = nullptr;
			if (categories.has(category_name)) {
				cat_item = categories[category_name];
			} else {
				cat_item = craft_tree->create_item(root);
				cat_item->set_text(0, category_name);
				cat_item->set_selectable(0, false);
				cat_item->set_custom_color(0, get_theme_color("accent_color", "Editor"));
				categories[category_name] = cat_item;
			}

			TreeItem *item = craft_tree->create_item(cat_item);
			item->set_text(0, domain_name);
			item->set_metadata(0, domain_name);

			if (icon.is_valid()) {
				item->set_icon(0, icon);
			} else {
				// Fallback icon
				item->set_icon(0, get_theme_icon("Resource", "EditorIcons"));
			}
			item->set_tooltip_text(0, vformat(TTR("Creates a new %s"), type));
		}
	}
}

void LibraryCraft::_on_craft_search_text_changed(const String &p_text) {
	_update_craft_tree();
}

void LibraryCraft::_on_craft_item_activated() {
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

void LibraryCraft::_on_creation_file_selected(const String &p_path) {
	if (current_creation_domain.is_empty()) {
		return;
	}

	Ref<Resource> res = create_resource_from_domain(current_creation_domain);
	if (res.is_valid()) {
		Error err = ResourceSaver::save(res, p_path);
		if (err == OK) {
			if (on_resource_created_callback.is_valid()) {
				on_resource_created_callback.call();
			}

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

void LibraryCraft::set_on_resource_created_callback(const Callable &p_callback) {
	on_resource_created_callback = p_callback;
}

Ref<Resource> LibraryCraft::create_resource_from_domain(const StringName &p_domain_name) {
	if (!ResourceServer::get_singleton()) {
		ERR_PRINT("ResourceServer singleton is not available.");
		return Ref<Resource>();
	}

	Dictionary info = ResourceServer::get_singleton()->get_domain_info(p_domain_name);
	String resource_class_name = info.get("resource_type", "");

	if (resource_class_name.is_empty()) {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Resource domain '%s' not found or not registered correctly."), p_domain_name));
		return Ref<Resource>();
	}

	return create_resource_by_class(resource_class_name);
}

Ref<Resource> LibraryCraft::create_resource_by_class(const StringName &p_class_name) {
	if (!ClassDB::class_exists(p_class_name)) {
		ERR_PRINT(vformat("Class '%s' does not exist in ClassDB.", p_class_name));
		return Ref<Resource>();
	}
	if (!ClassDB::is_parent_class(p_class_name, "Resource")) {
		ERR_PRINT(vformat("Class '%s' does not inherit from Resource.", p_class_name));
		return Ref<Resource>();
	}
	if (!ClassDB::can_instantiate(p_class_name)) {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Class '%s' cannot be instantiated. Check if it's abstract or if it requires specific setup."), p_class_name));
		return Ref<Resource>();
	}

	Object *obj = ClassDB::instantiate(p_class_name);
	if (!obj) {
		EditorNode::get_singleton()->show_warning(vformat(TTR("Failed to instantiate class '%s'."), p_class_name));
		return Ref<Resource>();
	}

	Resource *res = Object::cast_to<Resource>(obj);
	if (!res) {
		memdelete(obj); // Clean up if cast fails
		EditorNode::get_singleton()->show_warning(vformat(TTR("Instantiated object for class '%s' is not a Resource."), p_class_name));
		return Ref<Resource>();
	}

	Ref<Resource> resource = res;
	return resource;
}

LibraryCraft::LibraryCraft() {
	set_name(TTR("CraftTable"));

	craft_search = memnew(LineEdit);
	craft_search->set_placeholder(TTR("Search Zyris Resources..."));
	craft_search->connect("text_changed", callable_mp(this, &LibraryCraft::_on_craft_search_text_changed));
	add_child(craft_search);

	craft_tree = memnew(Tree);
	craft_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	craft_tree->set_hide_root(true);
	craft_tree->connect("item_activated", callable_mp(this, &LibraryCraft::_on_craft_item_activated));
	add_child(craft_tree);

	// Creation Dialog
	creation_dialog = memnew(EditorFileDialog);
	creation_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	creation_dialog->add_filter("*.tres", TTR("Resource"));
	creation_dialog->connect("file_selected", callable_mp(this, &LibraryCraft::_on_creation_file_selected));
	add_child(creation_dialog);
}

LibraryCraft::~LibraryCraft() {
}
