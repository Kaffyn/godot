/**************************************************************************/
/*  virtual_touch_pad.cpp                                                 */
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

#include "virtual_touch_pad.h"

#include "core/config/engine.h"
#include "core/input/input.h"
#include "scene/theme/theme_db.h"

void VirtualTouchPad::_update_theme_item_cache() {
	VirtualDevice::_update_theme_item_cache();

	theme_cache.style_panel = get_theme_stylebox(SNAME("panel"), SNAME("VirtualTouchPad"));
	theme_cache.trace_color = get_theme_color(SNAME("trace_color"), SNAME("VirtualTouchPad"));
	theme_cache.trace_texture = get_theme_icon(SNAME("trace_texture"), SNAME("VirtualTouchPad"));

	// Integer theme items are not standard for width, using "constant" usually works for int values
	theme_cache.trace_width = get_theme_constant(SNAME("trace_width"), SNAME("VirtualTouchPad"));
}

void VirtualTouchPad::_notification(int p_what) {
	VirtualDevice::_notification(p_what);
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			// Draw Background (only in editor)
			if (Engine::get_singleton()->is_editor_hint()) {
				if (theme_cache.style_panel.is_valid()) {
					theme_cache.style_panel->draw(get_canvas_item(), Rect2(Point2(), get_size()));
				} else {
					// Fallback editor visual
					draw_rect(Rect2(Point2(), get_size()), Color(0.5, 0.5, 0.5, 0.2), true);
					draw_rect(Rect2(Point2(), get_size()), Color(1, 1, 1, 0.5), false);
				}
			}

			// Draw Trace
			if (trace_visible && is_pressed()) {
				// We can draw a line or a texture pattern
				if (theme_cache.trace_texture.is_valid()) {
					// Draw texture at current pos (like a brush)
					Vector2 size = theme_cache.trace_texture->get_size();
					draw_texture_rect(theme_cache.trace_texture, Rect2(current_pos - size / 2, size), false, theme_cache.trace_color);
				} else {
					draw_line(last_pos, current_pos, theme_cache.trace_color, (float)MAX(1, theme_cache.trace_width));
					draw_circle(current_pos, (float)MAX(1, theme_cache.trace_width) * 2.0, theme_cache.trace_color);
				}
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_theme_item_cache();
			queue_redraw();
		} break;
	}
}

// Update FX: Sync child particles to touch position
void _update_fx(Node *p_root, const Vector2 &p_pos, bool p_emitting) {
	for (int i = 0; i < p_root->get_child_count(); i++) {
		Node *child = p_root->get_child(i);
		if (!child) {
			continue;
		}

		// Try CPUParticles2D
		if (child->is_class("CPUParticles2D")) {
			child->call("set_emitting", p_emitting);
			if (p_emitting) {
				child->call("set_position", p_pos);
			}
		}
		// Try GPUParticles2D
		else if (child->is_class("GPUParticles2D")) {
			child->call("set_emitting", p_emitting);
			if (p_emitting) {
				child->call("set_position", p_pos);
			}
		}
		// Recurse? Usually FX are direct children.
	}
}

void VirtualTouchPad::_on_drag(int p_index, const Vector2 &p_pos, const Vector2 &p_relative) {
	// ... (Input movement logic same as before) ...
	// Calculate axes based on hand
	int axis_x = (hand == HAND_LEFT) ? 0 : 2;
	int axis_y = (hand == HAND_LEFT) ? 1 : 3;

	if (p_relative.x != 0) {
		Ref<InputEventVirtualMotion> ie_x;
		ie_x.instantiate();
		ie_x->set_device(get_device());
		ie_x->set_axis(axis_x);
		ie_x->set_axis_value(p_relative.x * sensitivity);
		Input::get_singleton()->parse_input_event(ie_x);
	}

	if (p_relative.y != 0) {
		Ref<InputEventVirtualMotion> ie_y;
		ie_y.instantiate();
		ie_y->set_device(get_device());
		ie_y->set_axis(axis_y);
		ie_y->set_axis_value(p_relative.y * sensitivity);
		Input::get_singleton()->parse_input_event(ie_y);
	}

	// Update trail positions
	last_pos = current_pos;
	current_pos = p_pos;

	_update_fx(this, current_pos, true);

	queue_redraw();
}

void VirtualTouchPad::_reset_touchpad() {
	int axis_x = (hand == HAND_LEFT) ? 0 : 2;
	int axis_y = (hand == HAND_LEFT) ? 1 : 3;

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

	// Turn off FX
	_update_fx(this, current_pos, false);
}

void VirtualTouchPad::pressed_state_changed() {
	if (!is_pressed()) {
		_reset_touchpad();
	}
}

void VirtualTouchPad::set_hand(TouchPadHand p_hand) {
	hand = p_hand;
}

void VirtualTouchPad::set_trace_visible(bool p_visible) {
	trace_visible = p_visible;
	queue_redraw();
}

bool VirtualTouchPad::is_trace_visible() const {
	return trace_visible;
}

void VirtualTouchPad::_on_touch_down(int p_index, const Vector2 &p_pos) {
	last_pos = p_pos;
	current_pos = p_pos;
	_update_fx(this, current_pos, true);
	queue_redraw();
}

void VirtualTouchPad::_on_touch_up(int p_index, const Vector2 &p_pos) {
	_reset_touchpad();
	queue_redraw();
}

void VirtualTouchPad::set_sensitivity(float p_sensitivity) {
	sensitivity = p_sensitivity;
}

float VirtualTouchPad::get_sensitivity() const {
	return sensitivity;
}

VirtualTouchPad::VirtualTouchPad() {
}

Size2 VirtualTouchPad::get_minimum_size() const {
	return Size2(20, 20);
}

void VirtualTouchPad::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sensitivity", "sensitivity"), &VirtualTouchPad::set_sensitivity);
	ClassDB::bind_method(D_METHOD("get_sensitivity"), &VirtualTouchPad::get_sensitivity);
	ClassDB::bind_method(D_METHOD("set_hand", "hand"), &VirtualTouchPad::set_hand);
	ClassDB::bind_method(D_METHOD("get_hand"), &VirtualTouchPad::get_hand);

	ClassDB::bind_method(D_METHOD("set_trace_visible", "visible"), &VirtualTouchPad::set_trace_visible);
	ClassDB::bind_method(D_METHOD("is_trace_visible"), &VirtualTouchPad::is_trace_visible);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sensitivity"), "set_sensitivity", "get_sensitivity");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trace_visible"), "set_trace_visible", "is_trace_visible");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hand", PROPERTY_HINT_ENUM, "Left,Right"), "set_hand", "get_hand");

	BIND_ENUM_CONSTANT(HAND_LEFT);
	BIND_ENUM_CONSTANT(HAND_RIGHT);

	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, VirtualTouchPad, style_panel);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, VirtualTouchPad, trace_width); // Using constant
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_COLOR, VirtualTouchPad, trace_color, "trace_color");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_ICON, VirtualTouchPad, trace_texture, "trace_texture");
}
