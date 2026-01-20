/**************************************************************************/
/*  attribute_set.cpp                                                     */
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

#include "attribute_set.h"

void AttributeSet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_attribute_base", "name"), &AttributeSet::get_attribute_base);
	ClassDB::bind_method(D_METHOD("get_attribute_current", "name"), &AttributeSet::get_attribute_current);
	ClassDB::bind_method(D_METHOD("set_attribute_base", "name", "value"), &AttributeSet::set_attribute_base);
	ClassDB::bind_method(D_METHOD("set_attribute_current", "name", "value"), &AttributeSet::set_attribute_current);
	ClassDB::bind_method(D_METHOD("has_attribute", "name"), &AttributeSet::has_attribute);
}

float AttributeSet::get_attribute_base(const StringName &p_name) const {
	if (attributes.has(p_name)) {
		return attributes[p_name].base_value;
	}
	return 0.0f;
}

float AttributeSet::get_attribute_current(const StringName &p_name) const {
	if (attributes.has(p_name)) {
		return attributes[p_name].current_value;
	}
	return 0.0f;
}

void AttributeSet::set_attribute_base(const StringName &p_name, float p_val) {
	if (!attributes.has(p_name)) {
		register_attribute(p_name);
	}
	attributes[p_name].base_value = p_val;
	// When base changes, usually we want to emit a signal, handled by ASC
}

void AttributeSet::set_attribute_current(const StringName &p_name, float p_val) {
	if (!attributes.has(p_name)) {
		register_attribute(p_name);
	}
	attributes[p_name].current_value = p_val;
}

bool AttributeSet::has_attribute(const StringName &p_name) const {
	return attributes.has(p_name);
}

void AttributeSet::register_attribute(const StringName &p_name, float p_initial_value) {
	AttributeData data;
	data.base_value = p_initial_value;
	data.current_value = p_initial_value;
	attributes[p_name] = data;
}
