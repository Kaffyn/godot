/**************************************************************************/
/*  virtual_joystick_region.cpp                                           */
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

#include "virtual_joystick_region.h"

#include "core/config/engine.h"

void VirtualJoystickRegion::_notification(int p_what) {
	// Delegate to parent for runtime behavior and other notifications
	VirtualJoystick::_notification(p_what);
}

void VirtualJoystickRegion::_draw_joystick() {
	if (Engine::get_singleton()->is_editor_hint()) {
		// In editor, only draw the debug region, never the joystick itself
		draw_rect(Rect2(Point2(), get_size()), Color(1, 1, 1, 0.2), true);
		draw_rect(Rect2(Point2(), get_size()), Color(1, 1, 1, 0.5), false);
		return;
	}
	// At runtime, we need to override the drawing to use our joystick_size
	if (!Engine::get_singleton()->is_editor_hint() && !is_pressed() && get_joystick_mode() == JOYSTICK_MODE_DYNAMIC) {
		// Don't draw dynamic joystick if not touched (runtime)
		return;
	}

	Size2 s = get_size();
	// joystick_size is the diameter, so radius is half
	float radius = joystick_size / 2.0f;

	Vector2 drawing_base = get_base_pos();
	Vector2 drawing_tip = is_pressed() ? get_tip_pos() : drawing_base;

	// Access parent's theme cache through a local variable reference
	auto &cache = VirtualJoystick::theme_cache;

	// Draw Base
	if (cache.base_texture.is_valid()) {
		Size2 base_size = Size2(radius * 2, radius * 2);
		draw_texture_rect(cache.base_texture, Rect2(drawing_base - base_size / 2, base_size), false, cache.base_color);
	} else {
		draw_circle(drawing_base, radius, cache.base_color);
	}

	// Draw Tip
	if (cache.tip_texture.is_valid()) {
		float tip_scale = 0.4;
		Size2 tip_size = Size2(radius * 2 * tip_scale, radius * 2 * tip_scale);
		draw_texture_rect(cache.tip_texture, Rect2(drawing_tip - tip_size / 2, tip_size), false, cache.tip_color);
	} else {
		draw_circle(drawing_tip, radius * 0.4, cache.tip_color);
	}
}

void VirtualJoystickRegion::set_joystick_size(float p_size) {
	joystick_size = p_size;
	// Sync clamp zone to half the diameter (the radius)
	// This allows tip center to reach the edge, so half the tip extends beyond
	clamp_zone_size = p_size / 2.0f;
	queue_redraw();
}

float VirtualJoystickRegion::get_joystick_size() const {
	return joystick_size;
}

void VirtualJoystickRegion::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_joystick_size", "size"), &VirtualJoystickRegion::set_joystick_size);
	ClassDB::bind_method(D_METHOD("get_joystick_size"), &VirtualJoystickRegion::get_joystick_size);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "joystick_size"), "set_joystick_size", "get_joystick_size");
}

VirtualJoystickRegion::VirtualJoystickRegion() {
	// Defaults to Dynamic mode to act as a region that spawns the joystick
	joystick_mode = JOYSTICK_MODE_DYNAMIC;

	// Sync clamp zone with joystick radius (half the diameter)
	clamp_zone_size = joystick_size / 2.0f;

	// Default to a transparent control that covers an area
	// Users should likely set the rect to be large (e.g. half screen)
}
