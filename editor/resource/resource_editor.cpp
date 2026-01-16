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
#include "editor/editor_node.h"
#include "editor/gui/code_editor.h"
#include "editor/themes/editor_scale.h"

ResourceEditor *ResourceEditor::singleton = nullptr;

void ResourceEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_mode_visual_pressed"), &ResourceEditor::_on_mode_visual_pressed);
	ClassDB::bind_method(D_METHOD("_on_mode_code_pressed"), &ResourceEditor::_on_mode_code_pressed);
}

void ResourceEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			if (is_inside_tree()) {
				// Icons might need to be fetched from EditorNode or Theme
				// Using standard icons for now
				// mode_visual_button->set_button_icon(get_theme_icon("GraphEdit", "EditorIcons"));
				// mode_code_button->set_button_icon(get_theme_icon("Script", "EditorIcons"));
			}
		} break;
	}
}

void ResourceEditor::_update_mode() {
	bool visual = mode_visual_button->is_pressed();
	if (visual) {
		graph_edit->show();
		code_editor->hide();

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
				if (E.usage & PROPERTY_USAGE_STORAGE) {
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
		code_editor->show();

		if (current_resource.is_valid()) {
			String code_dump = "";

			List<PropertyInfo> props;
			current_resource->get_property_list(&props);

			for (const PropertyInfo &E : props) {
				if (E.usage & PROPERTY_USAGE_STORAGE) {
					if (E.name == "resource_local_to_scene" || E.name == "resource_name" || E.name == "script" || E.name.begins_with("metadata/")) {
						continue;
					}
					code_dump += String(E.name) + " = " + String(current_resource->get(E.name)) + "\n";
				}
			}

			code_editor->get_text_editor()->set_text(code_dump);
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
	current_resource = p_resource;
	if (current_resource.is_valid()) {
		// Update title or info
	}
	_update_mode();
}

Ref<Resource> ResourceEditor::get_edited_resource() const {
	return current_resource;
}

ResourceEditor::ResourceEditor() {
	singleton = this;

	toolbar = memnew(HBoxContainer);
	add_child(toolbar);

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

	editor_container = memnew(Control);
	editor_container->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(editor_container);

	graph_edit = memnew(GraphEdit);
	graph_edit->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	editor_container->add_child(graph_edit);

	code_editor = memnew(CodeTextEditor);
	code_editor->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	code_editor->hide();
	editor_container->add_child(code_editor);
}

ResourceEditor::~ResourceEditor() {
	singleton = nullptr;
}
