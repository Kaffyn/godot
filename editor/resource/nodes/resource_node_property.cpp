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

void ResourceNodeProperty::set_property(const String &p_name, const Variant &p_value, const String &p_type_hint) {
	name_label->set_text(p_name.capitalize());

	String val_str = String(p_value);
	if (val_str.length() > 20) {
		val_str = val_str.substr(0, 20) + "...";
	}
	value_label->set_text(val_str);

	// Icon handling
	if (p_value.get_type() == Variant::OBJECT) {
		Object *obj = p_value;
		if (obj) {
			icon->set_texture(EditorNode::get_singleton()->get_object_icon(obj, "Object"));
			icon->show();
		} else {
			// Null object, try to infer from type hint or just show generic
			icon->hide(); // Or show null icon
		}
	} else {
		// Basic types
		icon->set_texture(EditorNode::get_singleton()->get_class_icon(Variant::get_type_name(p_value.get_type()), "Variant"));
		icon->show();
	}
}

ResourceNodeProperty::ResourceNodeProperty() {
	icon = memnew(TextureRect);
	icon->set_custom_minimum_size(Size2(16, 16) * EDSCALE);
	icon->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	icon->hide();
	add_child(icon);

	name_label = memnew(Label);
	add_child(name_label);

	separator = memnew(Label);
	separator->set_text(":");
	add_child(separator);

	value_label = memnew(Label);
	value_label->set_modulate(Color(0.7, 0.7, 0.7)); // Dimmed value
	add_child(value_label);
}
