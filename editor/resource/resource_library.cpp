/**************************************************************************/
/*  resource_library.cpp                                                  */
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

#include "resource_library.h"
#include "core/object/class_db.h"
#include "editor/themes/editor_scale.h"

void ResourceLibrary::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_search_text_changed", "text"), &ResourceLibrary::_search_text_changed);
}

void ResourceLibrary::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_tree();
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			if (search_box) {
				search_box->set_right_icon(get_theme_icon("Search", "EditorIcons"));
			}
		} break;
	}
}

void ResourceLibrary::_search_text_changed(const String &p_text) {
	_update_tree();
}

void ResourceLibrary::_update_tree() {
	item_tree->clear();
	TreeItem *root = item_tree->create_item();

	List<StringName> class_list;
	ClassDB::get_class_list(&class_list);
	class_list.sort_custom<StringName::AlphCompare>();

	String search_term = search_box->get_text();

	// Create sections
	TreeItem *resources_root = item_tree->create_item(root);
	resources_root->set_text(0, TTR("Resources"));
	resources_root->set_selectable(0, false);
	resources_root->set_collapsed(false);

	for (const StringName &E : class_list) {
		if (ClassDB::is_parent_class(E, "Resource")) {
			if (!search_term.is_empty() && String(E).findn(search_term) == -1) {
				continue;
			}

			// Skip abstract classes or internal ones usually
			if (!ClassDB::can_instantiate(E)) {
				continue;
			}

			TreeItem *item = item_tree->create_item(resources_root);
			item->set_text(0, E);

			// Try to get an icon
			if (has_theme_icon(E, "EditorIcons")) {
				item->set_icon(0, get_theme_icon(E, "EditorIcons"));
			} else {
				item->set_icon(0, get_theme_icon("Object", "EditorIcons"));
			}
		}
	}
}

ResourceLibrary::ResourceLibrary() {
	search_box = memnew(LineEdit);
	search_box->set_placeholder(TTR("Search Resources..."));
	search_box->connect("text_changed", callable_mp(this, &ResourceLibrary::_search_text_changed));
	add_child(search_box);

	item_tree = memnew(Tree);
	item_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	item_tree->set_hide_root(true);
	add_child(item_tree);
}

ResourceLibrary::~ResourceLibrary() {
}
