/**************************************************************************/
/*  virtual_joystick.cpp                                                  */
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

#include "virtual_joystick.h"
#include "core/input/input.h"
#include "core/config/engine.h"

void VirtualJoystick::_notification(int p_what) {
	VirtualDevice::_notification(p_what);
	switch (p_what) {
		case NOTIFICATION_READY: {
			original_base_pos = get_size() / 2.0;
			_reset_joystick();
		} break;
		case NOTIFICATION_DRAW: {
			_draw_joystick();
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			theme_cache.base_texture = get_theme_icon(SNAME("joystick_base"));
			theme_cache.tip_texture = get_theme_icon(SNAME("joystick_tip"));
			theme_cache.base_color = get_theme_color(SNAME("joystick_base_color"));
			theme_cache.tip_color = get_theme_color(SNAME("joystick_tip_color"));
			queue_redraw();
		} break;
		case NOTIFICATION_RESIZED: {
			if (!is_pressed()) {
				original_base_pos = get_size() / 2.0;
				_reset_joystick();
			}
			queue_redraw();
		} break;
	}
}

void VirtualJoystick::_draw_joystick() {
	if (joystick_mode == JOYSTICK_MODE_DYNAMIC && !is_pressed() && !Engine::get_singleton()->is_editor_hint()) {
		// Don't draw dynamic joystick if not touched (runtime)
		return;
	}

	Size2 s = get_size();
	float radius = MIN(s.x, s.y) / 2.0;

	if (joystick_mode == JOYSTICK_MODE_DYNAMIC || joystick_mode == JOYSTICK_MODE_FOLLOWING) {
		radius = clamp_zone_size;
	}

	// In FIXED mode, we draw at the center of the control
	Vector2 drawing_base = (joystick_mode == JOYSTICK_MODE_FIXED) ? (s / 2.0) : base_pos;
	Vector2 drawing_tip = (joystick_mode == JOYSTICK_MODE_FIXED && !is_pressed()) ? drawing_base : tip_pos;

	// Draw Base
	if (theme_cache.base_texture.is_valid()) {
		Size2 base_size = Size2(radius * 2, radius * 2);
		draw_texture_rect(theme_cache.base_texture, Rect2(drawing_base - base_size / 2, base_size), false, theme_cache.base_color);
	} else {
		draw_circle(drawing_base, radius, theme_cache.base_color);
	}

	// Draw Tip
	if (theme_cache.tip_texture.is_valid()) {
		float tip_scale = 0.4;
		Size2 tip_size = Size2(radius * 2 * tip_scale, radius * 2 * tip_scale);
		draw_texture_rect(theme_cache.tip_texture, Rect2(drawing_tip - tip_size / 2, tip_size), false, theme_cache.tip_color);
	} else {
		draw_circle(drawing_tip, radius * 0.4, theme_cache.tip_color);
	}
}

void VirtualJoystick::_on_touch_down(int p_index, const Vector2 &p_pos) {
	if (joystick_mode == JOYSTICK_MODE_DYNAMIC) {
		base_pos = p_pos;
		tip_pos = p_pos;
	} else {
		// FIXED or FOLLOWING (starts at fixed, moves if following)
		// Check distance to center? Or if inside rect?
		// BaseButton handles gui_input if inside control rect, so p_pos is local.

		// If clicking very far from center in Fixed mode, maybe we strictly respect control bounds.
		// Since we are here, we are already inside control rect (Base logic).

		// We just assume the user touched the "handle".
		tip_pos = p_pos;
	}
	_update_input_vector();
	queue_redraw();
}

void VirtualJoystick::_on_touch_up(int p_index, const Vector2 &p_pos) {
	_reset_joystick();
	queue_redraw();
}

void VirtualJoystick::_on_drag(int p_index, const Vector2 &p_pos, const Vector2 &p_relative) {
	tip_pos = p_pos;

	if (joystick_mode == JOYSTICK_MODE_FOLLOWING) {
		if (base_pos.distance_to(tip_pos) > clamp_zone_size) {
			Vector2 dir = (tip_pos - base_pos).normalized();
			base_pos = tip_pos - dir * clamp_zone_size;
		}
	}

	_update_input_vector();
	queue_redraw();
}

void VirtualJoystick::_update_input_vector() {
	Size2 s = get_size();
	float radius = MIN(s.x, s.y) / 2.0;
	if (joystick_mode == JOYSTICK_MODE_DYNAMIC || joystick_mode == JOYSTICK_MODE_FOLLOWING) {
		radius = clamp_zone_size;
	}

	Vector2 current_base = (joystick_mode == JOYSTICK_MODE_FIXED) ? (s / 2.0) : base_pos;
	Vector2 diff = tip_pos - current_base;
	float dist = diff.length();

	if (dist <= deadzone_size) {
		input_vector = Vector2();
	} else {
		// Clamp visual tip
		if (dist > radius) {
			diff = diff.normalized() * radius;
			tip_pos = current_base + diff; // Update visual position too
		}
		input_vector = diff / radius;
	}

	// Emit Input Event
	// Map to JoyAxis: LEFT_X/Y (0/1) or RIGHT_X/Y (2/3) based on hand configuration.
	int axis_x = (joystick_hand == JOYSTICK_HAND_LEFT) ? 0 : 2;
	int axis_y = (joystick_hand == JOYSTICK_HAND_LEFT) ? 1 : 3;

	// X Axis
	Ref<InputEventVirtualMotion> ie_x;
	ie_x.instantiate();
	ie_x->set_device(get_device());
	ie_x->set_axis(axis_x);
	ie_x->set_axis_value(input_vector.x);
	Input::get_singleton()->parse_input_event(ie_x);

	// Y Axis
	Ref<InputEventVirtualMotion> ie_y;
	ie_y.instantiate();
	ie_y->set_device(get_device());
	ie_y->set_axis(axis_y);
	ie_y->set_axis_value(input_vector.y);
	Input::get_singleton()->parse_input_event(ie_y);
}

void VirtualJoystick::_reset_joystick() {
	input_vector = Vector2();
	base_pos = original_base_pos;
	tip_pos = original_base_pos;

	int axis_x = (joystick_hand == JOYSTICK_HAND_LEFT) ? 0 : 2;
	int axis_y = (joystick_hand == JOYSTICK_HAND_LEFT) ? 1 : 3;

	// Reset inputs to 0
	Ref<InputEventVirtualMotion> ie_x;
	ie_x.instantiate();
	ie_x->set_device(get_device());
	ie_x->set_axis(axis_x);
	ie_x->set_axis_value(0.0);
	Input::get_singleton()->parse_input_event(ie_x);

	Ref<InputEventVirtualMotion> ie_y;
	ie_y.instantiate();
	ie_y->set_device(get_device());
	ie_y->set_axis(axis_y);
	ie_y->set_axis_value(0.0);
	Input::get_singleton()->parse_input_event(ie_y);
}

void VirtualJoystick::set_deadzone_size(float p_size) {
	deadzone_size = p_size;
}

float VirtualJoystick::get_deadzone_size() const {
	return deadzone_size;
}

void VirtualJoystick::set_clamp_zone_size(float p_size) {
	clamp_zone_size = p_size;
	queue_redraw();
}

float VirtualJoystick::get_clamp_zone_size() const {
	return clamp_zone_size;
}

Vector2 VirtualJoystick::get_output() const {
	return input_vector;
}

Size2 VirtualJoystick::get_minimum_size() const {
	return Size2(20, 20);
}

void VirtualJoystick::set_joystick_mode(JoystickMode p_mode) {
	joystick_mode = p_mode;
	queue_redraw();
}

void VirtualJoystick::set_joystick_hand(JoystickHand p_hand) {
	joystick_hand = p_hand;
}

void VirtualJoystick::_update_theme_item_cache() {
	// VirtualDevice::_update_theme_item_cache(); // Assuming this is not needed or handled differently

	theme_cache.base_style = get_theme_stylebox(SNAME("base_style"), SNAME("VirtualJoystick")); // Custom type for styling
	theme_cache.tip_style = get_theme_stylebox(SNAME("tip_style"), SNAME("VirtualJoystick"));

	theme_cache.base_texture = get_theme_icon(SNAME("base"), SNAME("VirtualJoystick"));
	theme_cache.tip_texture = get_theme_icon(SNAME("tip"), SNAME("VirtualJoystick"));
}

VirtualJoystick::VirtualJoystick() {
	set_mouse_filter(MOUSE_FILTER_PASS); // Let users click
}

void VirtualJoystick::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_deadzone_size", "deadzone_size"), &VirtualJoystick::set_deadzone_size);
	ClassDB::bind_method(D_METHOD("get_deadzone_size"), &VirtualJoystick::get_deadzone_size);

	ClassDB::bind_method(D_METHOD("set_clamp_zone_size", "size"), &VirtualJoystick::set_clamp_zone_size);
	ClassDB::bind_method(D_METHOD("get_clamp_zone_size"), &VirtualJoystick::get_clamp_zone_size);

	ClassDB::bind_method(D_METHOD("get_output"), &VirtualJoystick::get_output);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "deadzone_size"), "set_deadzone_size", "get_deadzone_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "clamp_zone_size"), "set_clamp_zone_size", "get_clamp_zone_size");

	BIND_ENUM_CONSTANT(JOYSTICK_MODE_FIXED);
	BIND_ENUM_CONSTANT(JOYSTICK_MODE_DYNAMIC);
	BIND_ENUM_CONSTANT(JOYSTICK_MODE_FOLLOWING);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "joystick_mode", PROPERTY_HINT_ENUM, "Fixed,Dynamic,Following"), "set_joystick_mode", "get_joystick_mode");

	ClassDB::bind_method(D_METHOD("set_joystick_mode", "mode"), &VirtualJoystick::set_joystick_mode);
	ClassDB::bind_method(D_METHOD("get_joystick_mode"), &VirtualJoystick::get_joystick_mode);

	ClassDB::bind_method(D_METHOD("set_joystick_hand", "hand"), &VirtualJoystick::set_joystick_hand);
	ClassDB::bind_method(D_METHOD("get_joystick_hand"), &VirtualJoystick::get_joystick_hand);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "joystick_hand", PROPERTY_HINT_ENUM, "Left,Right"), "set_joystick_hand", "get_joystick_hand");

	BIND_ENUM_CONSTANT(JOYSTICK_HAND_LEFT);
	BIND_ENUM_CONSTANT(JOYSTICK_HAND_RIGHT);
}
