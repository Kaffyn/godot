/**************************************************************************/
/*  attribute_set.h                                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2025-present Kaffyn and Zyris contributors              */
/* Copyright (c) 2014-present Godot Engine contributors                  */
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

#pragma once

#include "core/io/resource.h"
#include "core/templates/hash_map.h"

struct AttributeData {
	float base_value = 0.0f;
	float current_value = 0.0f;

	void set_base(float p_val) {
		base_value = p_val;
		current_value = p_val; // Reset current when base changes? Or keep delta? Usually reset for init.
	}
};

class AttributeSet : public Resource {
	GDCLASS(AttributeSet, Resource);

	HashMap<StringName, AttributeData> attributes;

protected:
	static void _bind_methods();

public:
	float get_attribute_base(const StringName &p_name) const;
	float get_attribute_current(const StringName &p_name) const;

	void set_attribute_base(const StringName &p_name, float p_val);
	void set_attribute_current(const StringName &p_name, float p_val);

	bool has_attribute(const StringName &p_name) const;
	void register_attribute(const StringName &p_name, float p_initial_value = 0.0f);

	AttributeSet() {}
};

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	float get_##PropertyName() const { return get_attribute_current(#PropertyName); } \
	void set_##PropertyName(float p_val) { set_attribute_current(#PropertyName, p_val); } \
	void init_##PropertyName(float p_val) { set_attribute_base(#PropertyName, p_val); }
