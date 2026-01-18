/**************************************************************************/
/*  resource_editor.cpp                                                   */
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

#include "resource_editor.h"
#include "core/io/resource_saver.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/gui/code_editor.h"
#include "editor/themes/editor_scale.h"
#include "resource_server.h"

ResourceEditor *ResourceEditor::singleton = nullptr;

void ResourceEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_mode_visual_pressed"), &ResourceEditor::_on_mode_visual_pressed);
	ClassDB::bind_method(D_METHOD("_on_mode_code_pressed"), &ResourceEditor::_on_mode_code_pressed);
	ClassDB::bind_method(D_METHOD("_on_edit_script_pressed"), &ResourceEditor::_on_edit_script_pressed);

	ClassDB::bind_method(D_METHOD("_on_resource_selected", "index"), &ResourceEditor::_on_resource_selected);
	ClassDB::bind_method(D_METHOD("_on_filter_changed", "text"), &ResourceEditor::_on_filter_changed);
}
void ResourceEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			if (is_inside_tree()) {
				// Update icons if needed
				if (filter_txt) {
					filter_txt->set_right_icon(get_theme_icon("Search", "EditorIcons"));
				}
			}
		} break;
	}
}

void ResourceEditor::_update_mode() {
	if (current_resource.is_null()) {
		if (graph_edit) {
			graph_edit->hide();
		}
		if (code_editor) {
			code_editor->hide();
		}
		if (custom_editor) {
			custom_editor->hide();
		}
		if (edit_script_button) {
			edit_script_button->hide();
		}
		return;
	}

	if (edit_script_button) {
		Ref<Script> scr = current_resource->get_script();
		edit_script_button->set_visible(scr.is_valid());
	}

	bool visual = mode_visual_button->is_pressed();

	// Clear previous custom editor state if switching or refreshing
	if (custom_editor) {
		memdelete(custom_editor);
		custom_editor = nullptr;
	}

	if (visual) {
		code_editor->hide();

		bool handled_by_domain = false;
		if (current_resource.is_valid()) {
			String domain = ResourceServer::get_singleton()->get_domain_for_resource(current_resource->get_class());
			if (!domain.is_empty()) {
				Dictionary info = ResourceServer::get_singleton()->get_domain_info(domain);
				String visual_class = info.get("visual_class_name", "");
				if (ClassDB::class_exists(visual_class)) {
					Object *obj = ClassDB::instantiate(visual_class);
					Control *ctrl = Object::cast_to<Control>(obj);
					if (ctrl) {
						custom_editor = ctrl;
						// Fix sizing
						custom_editor->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
						editor_container->add_child(custom_editor);

						if (ctrl->has_method("bind_data")) {
							ctrl->call("bind_data", current_resource);
						}

						graph_edit->hide();
						handled_by_domain = true;
					} else {
						if (obj) {
							memdelete(obj);
						}
					}
				}
			}
		}

		if (!handled_by_domain) {
			graph_edit->show();

			// Logic to populate graph
			graph_edit->clear_connections();
			// Remove all child nodes (graph nodes)
			for (int i = graph_edit->get_child_count() - 1; i >= 0; i--) {
				GraphNode *gn = Object::cast_to<GraphNode>(graph_edit->get_child(i));
				if (gn) {
					memdelete(gn);
				}
			}

			if (current_resource.is_valid()) {
				GraphNode *root_node = memnew(GraphNode);
				root_node->set_title(current_resource->get_class());
				root_node->set_position_offset(Vector2(50, 50));
				graph_edit->add_child(root_node);

				List<PropertyInfo> props;
				current_resource->get_property_list(&props);

				int slot_idx = 0;
				for (const PropertyInfo &E : props) {
					// Filter exactly like EditorInspector
					if (!(E.usage & PROPERTY_USAGE_EDITOR)) {
						continue;
					}
					if (E.name == "script" || E.name == "resource_name" || E.name == "resource_path" || E.name == "resource_local_to_scene" || E.name.begins_with("metadata/_")) {
						continue;
					}

					Label *prop_label = memnew(Label);
					Variant val = current_resource->get(E.name);
					String val_str = String(val);

					// Truncate long strings
					if (val_str.length() > 50) {
						val_str = val_str.substr(0, 50) + "...";
					}

					prop_label->set_text(E.name + ": " + val_str);
					root_node->add_child(prop_label);

					// Enable right slot for Resources (output)
					if (E.type == Variant::OBJECT) {
						root_node->set_slot(slot_idx, false, 0, Color(1, 1, 1), true, 0, Color(0, 1, 0));
					}
					slot_idx++;
				}
			}
		}

	} else {
		graph_edit->hide();
		if (custom_editor) {
			custom_editor->hide();
		}
		code_editor->show();

		if (current_resource.is_valid()) {
			String code_dump = "";

			List<PropertyInfo> props;
			current_resource->get_property_list(&props);

			for (const PropertyInfo &E : props) {
				// Filter exactly like EditorInspector
				if (!(E.usage & PROPERTY_USAGE_EDITOR)) {
					continue;
				}
				if (E.name == "script" || E.name == "resource_name" || E.name == "resource_path" || E.name == "resource_local_to_scene" || E.name.begins_with("metadata/_")) {
					continue;
				}

				code_dump += String(E.name) + " = " + String(current_resource->get(E.name)) + "\n";
			}

			code_editor->get_text_editor()->set_text(code_dump);
		}
	}
}

void ResourceEditor::_on_edit_script_pressed() {
	if (current_resource.is_valid()) {
		Ref<Script> scr = current_resource->get_script();
		if (scr.is_valid()) {
			EditorInterface::get_singleton()->edit_resource(scr);
		}
	}
}

void ResourceEditor::_on_mode_visual_pressed() {
	mode_visual_button->set_pressed(true);
	mode_code_button->set_pressed(false);
	_update_mode();
}

void ResourceEditor::_on_mode_code_pressed() {
	mode_visual_button->set_pressed(false);
	mode_code_button->set_pressed(true);
	_update_mode();
}

void ResourceEditor::edit(const Ref<Resource> &p_resource) {
	if (p_resource.is_null()) {
		return;
	}

	current_resource = p_resource;

	// Add to open resources if not present
	bool found = false;
	for (const Ref<Resource> &res : open_resources) {
		if (res == p_resource) {
			found = true;
			break;
		}
	}
	if (!found) {
		open_resources.push_back(p_resource);
	}

	_update_resource_list();
	_update_mode();
}

void ResourceEditor::_update_resource_list() {
	resource_list->clear();

	String filter = filter_txt->get_text();

	int index = 0;
	for (const Ref<Resource> &res : open_resources) {
		if (res.is_null()) {
			continue;
		}

		String name = res->get_path().get_file();
		if (name.is_empty()) {
			name = res->get_name();
		}
		if (name.is_empty()) {
			name = res->get_class();
		}

		if (!filter.is_empty() && name.findn(filter) == -1) {
			continue;
		}

		resource_list->add_item(name);

		// Icon
		Ref<Texture2D> icon = EditorNode::get_singleton()->get_object_icon(res.ptr(), "Object");
		resource_list->set_item_icon(index, icon);

		// Metadata (using index in list as ID? No, safer to store Ref or rely on order if filtered)
		// For simplicity, we store the ID or rely on syncing.
		// Since we filter, we can't just use index.
		// Let's store the index in the original list as metadata?
		// Or just re-find it.
		// Actually, ItemList metadata is a Variant. We can't store Ref<Resource> directly easily without cast.
		// We'll use the resource pointer ID or similar.
		// But for now, let's just assume list order matches if no filter.
		// With filter, it's trickier.
		// BETTER: Store the index of the resource in the `open_resources` list.

		// Find index in open_resources
		int original_idx = 0;
		for (List<Ref<Resource>>::Element *E = open_resources.front(); E; E = E->next()) {
			if (E->get() == res) {
				break;
			}
			original_idx++;
		}
		resource_list->set_item_metadata(index, original_idx);

		if (res == current_resource) {
			resource_list->select(index);
		}

		index++;
	}
}

void ResourceEditor::_on_resource_selected(int p_index) {
	int original_idx = resource_list->get_item_metadata(p_index);

	if (original_idx >= 0 && original_idx < open_resources.size()) {
		Ref<Resource> res = open_resources.get(original_idx);
		if (res.is_valid() && res != current_resource) {
			current_resource = res;
			_update_mode();
		}
	}
}

void ResourceEditor::_on_filter_changed(const String &p_text) {
	_update_resource_list();
}

Ref<Resource> ResourceEditor::get_edited_resource() const {
	return current_resource;
}

ResourceEditor::ResourceEditor() {
	singleton = this;
	memnew(ResourceServer); // Initialize the server singleton

	// Main Layout
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(main_split);

	// Sidebar
	sidebar = memnew(VBoxContainer);
	sidebar->set_custom_minimum_size(Size2(200 * EDSCALE, 0));
	sidebar->set_v_size_flags(SIZE_EXPAND_FILL);
	main_split->add_child(sidebar);

	filter_txt = memnew(LineEdit);
	filter_txt->set_placeholder(TTR("Filter Resources"));
	filter_txt->set_right_icon(EditorNode::get_singleton()->get_gui_base()->get_theme_icon("Search", "EditorIcons"));
	filter_txt->connect("text_changed", callable_mp(this, &ResourceEditor::_on_filter_changed));
	sidebar->add_child(filter_txt);

	resource_list = memnew(ItemList);
	resource_list->set_v_size_flags(SIZE_EXPAND_FILL);
	resource_list->connect("item_selected", callable_mp(this, &ResourceEditor::_on_resource_selected));
	sidebar->add_child(resource_list);

	// Editor Layout (Right)
	editor_layout = memnew(VBoxContainer);
	editor_layout->set_h_size_flags(SIZE_EXPAND_FILL);
	editor_layout->set_v_size_flags(SIZE_EXPAND_FILL);
	main_split->add_child(editor_layout);

	// Toolbar
	toolbar = memnew(HBoxContainer);
	// toolbar->set_alignment(BoxContainer::ALIGNMENT_CENTER); // We use spacers now
	editor_layout->add_child(toolbar);

	Control *spacer_l = memnew(Control);
	spacer_l->set_h_size_flags(SIZE_EXPAND_FILL);
	toolbar->add_child(spacer_l);

	mode_visual_button = memnew(Button);
	mode_visual_button->set_toggle_mode(true);
	mode_visual_button->set_pressed(true);
	mode_visual_button->set_text(TTR("Visual"));
	mode_visual_button->connect("pressed", callable_mp(this, &ResourceEditor::_on_mode_visual_pressed));
	toolbar->add_child(mode_visual_button);

	mode_code_button = memnew(Button);
	mode_code_button->set_toggle_mode(true);
	mode_code_button->set_text(TTR("Code"));
	mode_code_button->connect("pressed", callable_mp(this, &ResourceEditor::_on_mode_code_pressed));
	toolbar->add_child(mode_code_button);

	Control *spacer_r = memnew(Control);
	spacer_r->set_h_size_flags(SIZE_EXPAND_FILL);
	toolbar->add_child(spacer_r);

	edit_script_button = memnew(Button);
	edit_script_button->set_flat(true);
	edit_script_button->set_text(TTR("Edit Script"));
	edit_script_button->connect("pressed", callable_mp(this, &ResourceEditor::_on_edit_script_pressed));
	edit_script_button->hide();
	toolbar->add_child(edit_script_button);

	// Editor Container
	editor_container = memnew(Control);
	editor_container->set_v_size_flags(SIZE_EXPAND_FILL);
	editor_layout->add_child(editor_container);

	// Tools
	custom_editor = nullptr;

	graph_edit = memnew(GraphEdit);
	graph_edit->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	editor_container->add_child(graph_edit);

	code_editor = memnew(CodeTextEditor);
	code_editor->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	code_editor->hide();
	editor_container->add_child(code_editor);
}

ResourceEditor::~ResourceEditor() {
	if (ResourceServer::get_singleton()) {
		memdelete(ResourceServer::get_singleton());
	}
	singleton = nullptr;
}
