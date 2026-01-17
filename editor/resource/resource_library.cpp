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
#include "resource_server.h"

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

	String search_term = search_box->get_text();

	// Create sections
	TreeItem *zyris_root = item_tree->create_item(root);
	zyris_root->set_text(0, "Zyris Resources");
	zyris_root->set_selectable(0, false);
	zyris_root->set_collapsed(false);

	if (ResourceServer::get_singleton()) {
		// Use Registered Domains and Resources from SonharServer/ResourceServer

		// 1. Domains (Main types)
		Array domains = ResourceServer::get_singleton()->get_registered_domains();
		for (int i = 0; i < domains.size(); i++) {
			String domain_name = domains[i];
			Dictionary info = ResourceServer::get_singleton()->get_domain_info(domain_name);
			String type_name = info.get("resource_type", "");

			if (type_name.is_empty()) {
				continue;
			}

			if (!search_term.is_empty() && type_name.findn(search_term) == -1 && domain_name.findn(search_term) == -1) {
				continue;
			}

			TreeItem *item = item_tree->create_item(zyris_root);
			item->set_text(0, domain_name + " (" + type_name + ")");

			Ref<Texture2D> icon = info.get("icon", Variant());
			if (icon.is_valid()) {
				item->set_icon(0, icon);
			} else {
				if (has_theme_icon(type_name, "EditorIcons")) {
					item->set_icon(0, get_theme_icon(type_name, "EditorIcons"));
				} else {
					item->set_icon(0, get_theme_icon("Object", "EditorIcons"));
				}
			}
		}

		// 2. We might want to list other registered types if ResourceServer exposed them.
		// For now, sticking to Domains as they represent the "Craftable" high-level systems.
	} else {
		// Fallback if Server not ready (shouldn't happen in editor usually)
		TreeItem *err = item_tree->create_item(zyris_root);
		err->set_text(0, "ResourceServer not active.");
	}
}

ResourceLibrary::ResourceLibrary() {
	search_box = memnew(LineEdit);
	search_box->set_placeholder(TTR("Search Zyris Resources..."));
	search_box->connect("text_changed", callable_mp(this, &ResourceLibrary::_search_text_changed));
	add_child(search_box);

	item_tree = memnew(Tree);
	item_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	item_tree->set_hide_root(true);
	add_child(item_tree);
}

ResourceLibrary::~ResourceLibrary() {
}
