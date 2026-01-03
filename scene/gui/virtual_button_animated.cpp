/**************************************************************************/
/*  virtual_button_animated.cpp                                           */
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

#include "virtual_button_animated.h"
#include "core/config/engine.h"
#include "core/string/string_name.h"

void VirtualButtonAnimated::set_value(float p_value) {
	value = CLAMP(p_value, 0.0f, 1.0f);
	queue_redraw();
}

float VirtualButtonAnimated::get_value() const {
	return value;
}

void VirtualButtonAnimated::set_fill_mode(FillMode p_mode) {
	fill_mode = p_mode;
	queue_redraw();
}

VirtualButtonAnimated::FillMode VirtualButtonAnimated::get_fill_mode() const {
	return fill_mode;
}

void VirtualButtonAnimated::set_show_progress(bool p_show) {
	show_progress = p_show;
	queue_redraw();
}

bool VirtualButtonAnimated::is_showing_progress() const {
	return show_progress;
}

void VirtualButtonAnimated::_notification(int p_what) {
	if (p_what == NOTIFICATION_DRAW) {
		if (show_progress && (value < 1.0f || Engine::get_singleton()->is_editor_hint())) {
			Rect2 progress_rect = Rect2(Point2(), get_size());

			switch (fill_mode) {
				case FILL_LEFT_TO_RIGHT:
					progress_rect.size.x *= value;
					break;
				case FILL_RIGHT_TO_LEFT:
					progress_rect.position.x += progress_rect.size.x * (1.0 - value);
					progress_rect.size.x *= value;
					break;
				case FILL_TOP_TO_BOTTOM:
					progress_rect.size.y *= value;
					break;
				case FILL_BOTTOM_TO_TOP:
					progress_rect.position.y += progress_rect.size.y * (1.0 - value);
					progress_rect.size.y *= value;
					break;
			}

			if (theme_cache.progress_style.is_valid()) {
				theme_cache.progress_style->draw(get_canvas_item(), progress_rect);
			} else {
				draw_rect(progress_rect, theme_cache.progress_color);
			}
		}
	}

	// VirtualButton handles the rest of drawing (text, icon, texture)
	VirtualButton::_notification(p_what);
}

void VirtualButtonAnimated::_update_theme_item_cache() {
	VirtualButton::_update_theme_item_cache();

	theme_cache.progress_style = get_theme_stylebox(SNAME("progress_style"), SNAME("ProgressBar"));
	theme_cache.progress_color = get_theme_color(SNAME("progress_color"), SNAME("VirtualButtonAnimated"));
}

void VirtualButtonAnimated::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_value", "value"), &VirtualButtonAnimated::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &VirtualButtonAnimated::get_value);
	ClassDB::bind_method(D_METHOD("set_fill_mode", "mode"), &VirtualButtonAnimated::set_fill_mode);
	ClassDB::bind_method(D_METHOD("get_fill_mode"), &VirtualButtonAnimated::get_fill_mode);
	ClassDB::bind_method(D_METHOD("set_show_progress", "show"), &VirtualButtonAnimated::set_show_progress);
	ClassDB::bind_method(D_METHOD("is_showing_progress"), &VirtualButtonAnimated::is_showing_progress);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "value", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_value", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fill_mode", PROPERTY_HINT_ENUM, "Left to Right,Right to Left,Top to Bottom,Bottom to Top"), "set_fill_mode", "get_fill_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_progress"), "set_show_progress", "is_showing_progress");
}

VirtualButtonAnimated::VirtualButtonAnimated() {
	// Base constructor already sets defaults
}
