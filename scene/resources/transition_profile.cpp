/**************************************************************************/
/*  transition_profile.cpp                                                */
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

#include "transition_profile.h"

void TransitionProfile::set_duration(float p_duration) {
	duration = p_duration;
}

float TransitionProfile::get_duration() const {
	return duration;
}

void TransitionProfile::set_fade_color(const Color &p_color) {
	fade_color = p_color;
}

Color TransitionProfile::get_fade_color() const {
	return fade_color;
}

void TransitionProfile::set_transition_material(const Ref<ShaderMaterial> &p_material) {
	transition_material = p_material;
}

Ref<ShaderMaterial> TransitionProfile::get_transition_material() const {
	return transition_material;
}

void TransitionProfile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_duration", "duration"), &TransitionProfile::set_duration);
	ClassDB::bind_method(D_METHOD("get_duration"), &TransitionProfile::get_duration);

	ClassDB::bind_method(D_METHOD("set_fade_color", "color"), &TransitionProfile::set_fade_color);
	ClassDB::bind_method(D_METHOD("get_fade_color"), &TransitionProfile::get_fade_color);

	ClassDB::bind_method(D_METHOD("set_transition_material", "material"), &TransitionProfile::set_transition_material);
	ClassDB::bind_method(D_METHOD("get_transition_material"), &TransitionProfile::get_transition_material);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration", PROPERTY_HINT_RANGE, "0,10,0.01"), "set_duration", "get_duration");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "fade_color"), "set_fade_color", "get_fade_color");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "transition_material", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial"), "set_transition_material", "get_transition_material");
}

TransitionProfile::TransitionProfile() {
}
