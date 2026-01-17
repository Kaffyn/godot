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
#include "scene/2d/cpu_particles_2d.h"
#include "scene/2d/gpu_particles_2d.h"
#include "scene/main/timer.h"
#include "scene/theme/theme_db.h"

void VirtualTouchPad::_update_theme_item_cache() {
	VirtualDevice::_update_theme_item_cache();

	theme_cache.style_panel = get_theme_stylebox(SNAME("panel"), SNAME("VirtualTouchPad"));
	theme_cache.trace_color = get_theme_color(SNAME("trace_color"), SNAME("VirtualTouchPad"));
	if (has_theme_icon(SNAME("trace_texture"), SNAME("VirtualTouchPad"))) {
		theme_cache.trace_texture = get_theme_icon(SNAME("trace_texture"), SNAME("VirtualTouchPad"));
	} else {
		theme_cache.trace_texture = Ref<Texture2D>();
	}

	theme_cache.trace_width = get_theme_constant(SNAME("trace_width"), SNAME("VirtualTouchPad"));
}

void VirtualTouchPad::_notification(int p_what) {
	VirtualDevice::_notification(p_what);
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (is_pressed()) {
				Vector2 deflection;
				float d = get_process_delta_time();
				if (d > 0) {
					Size2 viewport_size = get_viewport_rect().size;
					float ref_side = MIN(viewport_size.x, viewport_size.y);
					if (ref_side > 0) {
						Vector2 vel = accumulated_relative / (ref_side * d);
						deflection = vel * sensitivity;
					}
				}

				deflection.x = CLAMP(deflection.x, -1.0, 1.0);
				deflection.y = CLAMP(deflection.y, -1.0, 1.0);

				if (deflection != last_sent_deflection || deflection != Vector2()) {
					_send_axis_events(deflection);
					last_sent_deflection = deflection;
				}

				accumulated_relative = Vector2();
			}
		} break;
		case NOTIFICATION_DRAW: {
			if (Engine::get_singleton()->is_editor_hint()) {
				if (theme_cache.style_panel.is_valid()) {
					theme_cache.style_panel->draw(get_canvas_item(), Rect2(Point2(), get_size()));
				} else {
					draw_rect(Rect2(Point2(), get_size()), Color(0.5, 0.5, 0.5, 0.2), true);
					draw_rect(Rect2(Point2(), get_size()), Color(1, 1, 1, 0.5), false);
				}
			}

			if (trace_visible) {
				if (theme_cache.trace_texture.is_valid()) {
					if (is_pressed()) {
						Vector2 size = theme_cache.trace_texture->get_size();
						draw_texture_rect(theme_cache.trace_texture, Rect2(current_pos - size / 2, size), false, theme_cache.trace_color);
					}
				} else if (trace_points.size() >= 2) {
					Color col = (base_color.a == 0) ? theme_cache.trace_color : base_color;
					float width = (base_width <= 0) ? (float)theme_cache.trace_width : base_width;

					int point_count = trace_points.size();
					for (int i = 0; i < point_count - 1; i++) {
						float alpha = (float)(i + 1) / point_count;
						Color c = col;
						c.a *= alpha;

						float w = width;
						if (tapering) {
							w *= alpha;
						}

						draw_line(trace_points[i], trace_points[i + 1], c, MAX(1.0f, w), true);
					}
				}
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_theme_item_cache();
			queue_redraw();
		} break;
	}
}

void _update_fx(Node *p_root, const Vector2 &p_pos, bool p_emitting) {
	for (int i = 0; i < p_root->get_child_count(); i++) {
		Node *child = p_root->get_child(i);
		if (!child) {
			continue;
		}

		if (CPUParticles2D *cpu_part = Object::cast_to<CPUParticles2D>(child)) {
			cpu_part->set_emitting(p_emitting);
			if (p_emitting) {
				cpu_part->set_position(p_pos);
			}
		} else if (GPUParticles2D *gpu_part = Object::cast_to<GPUParticles2D>(child)) {
			gpu_part->set_emitting(p_emitting);
			if (p_emitting) {
				gpu_part->set_position(p_pos);
			}
		}
	}
}

void VirtualTouchPad::_on_drag(int p_index, const Vector2 &p_pos, const Vector2 &p_relative) {
	accumulated_relative += p_relative;

	last_pos = current_pos;
	current_pos = p_pos;

	trace_points.push_back(p_pos);
	if (trace_points.size() > trace_length) {
		trace_points.remove_at(0);
	}

	_update_fx(this, current_pos, true);

	queue_redraw();
}

void VirtualTouchPad::_send_axis_events(const Vector2 &p_deflection) {
	int axis_x = (hand == HAND_LEFT) ? 0 : 2;
	int axis_y = (hand == HAND_LEFT) ? 1 : 3;

	Ref<InputEventVirtualMotion> ie_x;
	ie_x.instantiate();
	ie_x->set_device(get_device());
	ie_x->set_axis(axis_x);
	ie_x->set_axis_value(p_deflection.x);
	Input::get_singleton()->parse_input_event(ie_x);

	Ref<InputEventVirtualMotion> ie_y;
	ie_y.instantiate();
	ie_y->set_device(get_device());
	ie_y->set_axis(axis_y);
	ie_y->set_axis_value(p_deflection.y);
	Input::get_singleton()->parse_input_event(ie_y);
}

void VirtualTouchPad::_reset_touchpad() {
	_send_axis_events(Vector2());
	last_sent_deflection = Vector2();
	accumulated_relative = Vector2();
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
	accumulated_relative = Vector2();
	trace_points.clear();
	trace_points.push_back(p_pos);
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

void VirtualTouchPad::_on_fade_timer_timeout() {
	if (trace_points.size() > 0) {
		trace_points.remove_at(0);
		queue_redraw();
	}
}

void VirtualTouchPad::_update_timer_interval() {
	if (fade_timer) {
		float interval = fade_duration / (float)MAX(1, trace_length);
		fade_timer->set_wait_time(MAX(0.001f, interval));
	}
}

void VirtualTouchPad::set_trace_length(int p_length) {
	trace_length = MAX(2, p_length);
	_update_timer_interval();
}

int VirtualTouchPad::get_trace_length() const {
	return trace_length;
}

void VirtualTouchPad::set_fade_duration(float p_duration) {
	fade_duration = MAX(0.01f, p_duration);
	_update_timer_interval();
}

float VirtualTouchPad::get_fade_duration() const {
	return fade_duration;
}

void VirtualTouchPad::set_base_color(const Color &p_color) {
	base_color = p_color;
	queue_redraw();
}

Color VirtualTouchPad::get_base_color() const {
	return base_color;
}

void VirtualTouchPad::set_base_width(float p_width) {
	base_width = p_width;
	queue_redraw();
}

float VirtualTouchPad::get_base_width() const {
	return base_width;
}

void VirtualTouchPad::set_tapering(bool p_tapering) {
	tapering = p_tapering;
	queue_redraw();
}

bool VirtualTouchPad::is_tapering() const {
	return tapering;
}

VirtualTouchPad::VirtualTouchPad() {
	fade_timer = memnew(Timer);
	_update_timer_interval();
	fade_timer->connect("timeout", callable_mp(this, &VirtualTouchPad::_on_fade_timer_timeout));
	fade_timer->set_autostart(true);
	add_child(fade_timer);

	set_process_internal(true);
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

	ClassDB::bind_method(D_METHOD("set_trace_length", "length"), &VirtualTouchPad::set_trace_length);
	ClassDB::bind_method(D_METHOD("get_trace_length"), &VirtualTouchPad::get_trace_length);

	ClassDB::bind_method(D_METHOD("set_fade_duration", "duration"), &VirtualTouchPad::set_fade_duration);
	ClassDB::bind_method(D_METHOD("get_fade_duration"), &VirtualTouchPad::get_fade_duration);

	ClassDB::bind_method(D_METHOD("set_base_color", "color"), &VirtualTouchPad::set_base_color);
	ClassDB::bind_method(D_METHOD("get_base_color"), &VirtualTouchPad::get_base_color);

	ClassDB::bind_method(D_METHOD("set_base_width", "width"), &VirtualTouchPad::set_base_width);
	ClassDB::bind_method(D_METHOD("get_base_width"), &VirtualTouchPad::get_base_width);

	ClassDB::bind_method(D_METHOD("set_tapering", "tapering"), &VirtualTouchPad::set_tapering);
	ClassDB::bind_method(D_METHOD("is_tapering"), &VirtualTouchPad::is_tapering);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sensitivity"), "set_sensitivity", "get_sensitivity");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hand", PROPERTY_HINT_ENUM, "Left,Right"), "set_hand", "get_hand");

	ADD_GROUP("Trace", "trace_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trace_visible"), "set_trace_visible", "is_trace_visible");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "trace_length", PROPERTY_HINT_RANGE, "2,100,1"), "set_trace_length", "get_trace_length");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trace_fade_duration", PROPERTY_HINT_RANGE, "0.01,5.0,0.01,suffix:s"), "set_fade_duration", "get_fade_duration");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "trace_base_color"), "set_base_color", "get_base_color");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trace_base_width", PROPERTY_HINT_RANGE, "0,50,0.1,or_greater"), "set_base_width", "get_base_width");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trace_tapering"), "set_tapering", "is_tapering");

	BIND_ENUM_CONSTANT(HAND_LEFT);
	BIND_ENUM_CONSTANT(HAND_RIGHT);

	BIND_THEME_ITEM(Theme::DATA_TYPE_STYLEBOX, VirtualTouchPad, style_panel);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, VirtualTouchPad, trace_width);
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_COLOR, VirtualTouchPad, trace_color, "trace_color");
	BIND_THEME_ITEM_CUSTOM(Theme::DATA_TYPE_ICON, VirtualTouchPad, trace_texture, "trace_texture");
}
