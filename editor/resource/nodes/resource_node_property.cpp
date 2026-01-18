/**************************************************************************/
/*  resource_node_property.cpp                                            */
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

#include "resource_node_property.h"
#include "editor/editor_node.h"
#include "editor/themes/editor_scale.h"

void ResourceNodeProperty::set_property(const Ref<Resource> &p_resource, const String &p_name, const Variant &p_value, const String &p_type_hint) {
	target_resource = p_resource;
	property_name = p_name;
	updating = true;

	name_label->set_text(p_name.capitalize());

	// Icon handling
	if (p_value.get_type() == Variant::OBJECT) {
		Object *obj = p_value;
		if (obj) {
			icon->set_texture(EditorNode::get_singleton()->get_object_icon(obj, "Object"));
			icon->show();
		} else {
			// Try to resolve class from type hint if possible, otherwise generic Object
			icon->set_texture(EditorNode::get_singleton()->get_class_icon("Object", "Object"));
			icon->show();
		}
	} else {
		icon->set_texture(EditorNode::get_singleton()->get_class_icon(Variant::get_type_name(p_value.get_type()), "Variant"));
		icon->show();
	}

	// Clear previous editor
	if (editor_widget) {
		remove_child(editor_widget);
		memdelete(editor_widget);
		editor_widget = nullptr;
	}

	// Instantiate Editor Widget based on Type
	switch (p_value.get_type()) {
		case Variant::BOOL: {
			CheckBox *cb = memnew(CheckBox);
			cb->set_text((bool)p_value ? "True" : "False");
			cb->set_pressed(p_value);
			cb->connect("toggled", callable_mp(this, &ResourceNodeProperty::_value_changed));
			editor_widget = cb;
		} break;

		case Variant::INT: {
			SpinBox *sb = memnew(SpinBox);
			sb->set_min(-100000); // TODO: Check range hints
			sb->set_max(100000);
			sb->set_step(1);
			sb->set_value(p_value);
			sb->connect("value_changed", callable_mp(this, &ResourceNodeProperty::_value_changed));
			editor_widget = sb;
		} break;

		case Variant::FLOAT: {
			SpinBox *sb = memnew(SpinBox);
			sb->set_min(-100000);
			sb->set_max(100000);
			sb->set_step(0.001);
			sb->set_value(p_value);
			sb->connect("value_changed", callable_mp(this, &ResourceNodeProperty::_value_changed));
			editor_widget = sb;
		} break;

		case Variant::STRING:
		case Variant::STRING_NAME: {
			LineEdit *le = memnew(LineEdit);
			le->set_text(p_value);
			le->connect("text_submitted", callable_mp(this, &ResourceNodeProperty::_text_changed));
			le->connect("focus_exited", callable_mp(this, &ResourceNodeProperty::_text_changed).bind("")); // Hack to save on focus loss, assumes text hasn't changed if empty arg passed? No, bind overrides.
			// Simplified:
			le->connect("text_changed", callable_mp(this, &ResourceNodeProperty::_text_changed));
			editor_widget = le;
		} break;

		case Variant::COLOR: {
			ColorPickerButton *cp = memnew(ColorPickerButton);
			cp->set_pick_color(p_value);
			cp->connect("color_changed", callable_mp(this, &ResourceNodeProperty::_color_changed));
			cp->set_custom_minimum_size(Size2(32, 0) * EDSCALE);
			editor_widget = cp;
		} break;

		default: {
			Label *l = memnew(Label);
			String val_str = String(p_value);
			if (val_str.length() > 20) {
				val_str = val_str.substr(0, 20) + "...";
			}
			l->set_text(val_str);
			l->set_modulate(Color(0.7, 0.7, 0.7));
			editor_widget = l;
		} break;
	}

	if (editor_widget) {
		editor_widget->set_h_size_flags(SIZE_EXPAND_FILL);
		add_child(editor_widget);
	}

	updating = false;
}

void ResourceNodeProperty::_value_changed(const Variant &p_value) {
	if (updating || target_resource.is_null()) {
		return;
	}
	target_resource->set(property_name, p_value);

	// Update label for bools
	if (CheckBox *cb = Object::cast_to<CheckBox>(editor_widget)) {
		cb->set_text((bool)p_value ? "True" : "False");
	}
}

void ResourceNodeProperty::_text_changed(const String &p_text) {
	if (updating || target_resource.is_null()) {
		return;
	}
	// For LineEdit text_changed, we update immediately.
	// For optimization, maybe only on submit/focus loss, but for now immediate is fine for simple resources.
	target_resource->set(property_name, p_text);
}

void ResourceNodeProperty::_color_changed(const Color &p_color) {
	if (updating || target_resource.is_null()) {
		return;
	}
	target_resource->set(property_name, p_color);
}

ResourceNodeProperty::ResourceNodeProperty() {
	icon = memnew(TextureRect);
	icon->set_custom_minimum_size(Size2(16, 16) * EDSCALE);
	icon->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	icon->hide();
	add_child(icon);

	name_label = memnew(Label);
	add_child(name_label);

	Label *sep = memnew(Label);
	sep->set_text(":");
	add_child(sep);
}
