/**************************************************************************/
/*  virtual_gesture_area.cpp                                              */
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

#include "virtual_gesture_area.h"
#include "core/config/engine.h"
#include "core/input/input.h"
#include "core/os/time.h"

// ARCHITECTURE NOTE:
// VirtualGestureArea is strictly designed for Discrete Pattern Recognition (Spells/Commands).
// It emits InputEventVirtualGesture after processing strokes.
// It does NOT emit continuous motion events. For camera/aiming, use VirtualTouchPad.

VirtualGestureArea::VirtualGestureArea() {
	recognizer.instantiate();
}

void VirtualGestureArea::_notification(int p_what) {
	VirtualDevice::_notification(p_what);
	switch (p_what) {
		case NOTIFICATION_PROCESS: {
			if (is_waiting_for_multistroke) {
				multistroke_timer += get_process_delta_time();
				if (multistroke_timer >= multistroke_timeout) {
					_perform_recognition();
				}
			}
		} break;
		case NOTIFICATION_DRAW: {
			if (Engine::get_singleton()->is_editor_hint()) {
				draw_rect(Rect2(Point2(), get_size()), Color(0.2, 0.8, 0.2, 0.2), true);
				draw_rect(Rect2(Point2(), get_size()), Color(0.2, 0.8, 0.2, 0.5), false);
			} else {
				// Draw movement trail from strokes
				for (int i = 0; i < current_strokes.size(); i++) {
					const Vector<Vector2> &s = current_strokes[i];
					if (s.size() < 2) continue;
					for (int j = 1; j < s.size(); j++) {
						draw_line(s[j - 1], s[j], Color(0.2, 1.0, 0.2, 0.6), 2.0);
					}
					// Draw current point if pressed
					if (i == current_strokes.size() - 1 && is_pressed()) {
						draw_circle(s[s.size() - 1], 5.0, Color(0.2, 1.0, 0.2, 0.8));
					}
				}
			}
		} break;
	}
}

void VirtualGestureArea::_on_touch_down(int p_index, const Vector2 &p_pos) {
	is_waiting_for_multistroke = false;
	multistroke_timer = 0.0f;

	if (current_strokes.is_empty()) {
		stroke_start_time = Time::get_singleton()->get_ticks_msec();
	}

	current_strokes.push_back(Vector<Vector2>());
	current_strokes.write[current_strokes.size() - 1].push_back(p_pos);

	queue_redraw();
}

void VirtualGestureArea::_on_touch_up(int p_index, const Vector2 &p_pos) {
	if (current_strokes.is_empty()) return;

	current_strokes.write[current_strokes.size() - 1].push_back(p_pos);

	// Start waiting for potentially more strokes
	is_waiting_for_multistroke = true;
	multistroke_timer = 0.0f;
	set_process(true);

	queue_redraw();
}

void VirtualGestureArea::_perform_recognition() {
	is_waiting_for_multistroke = false;
	set_process(false);

	if (current_strokes.is_empty()) return;

	uint64_t duration_msec = Time::get_singleton()->get_ticks_msec() - stroke_start_time;
	float duration_sec = duration_msec / 1000.0f;

	Dictionary final_result;
	final_result["type"] = GestureRecognizer::GESTURE_UNKNOWN;
	final_result["confidence"] = 0.0f;

	// 1. Check for simple SWIPE
	if (current_strokes.size() == 1 && duration_sec <= swipe_max_time) {
		const Vector<Vector2> &s = current_strokes[0];
		GestureRecognizer::GestureType swipe = GestureRecognizer::get_swipe_direction(s[0], s[s.size() - 1], swipe_min_distance);
		if (swipe != GestureRecognizer::GESTURE_UNKNOWN) {
			final_result["type"] = swipe;
			final_result["confidence"] = 1.0f;
			final_result["start_position"] = s[0];
			final_result["end_position"] = s[s.size() - 1];
		}
	}

	// 2. Math Recognizer
	if ((int)final_result["type"] == GestureRecognizer::GESTURE_UNKNOWN) {
		if (current_strokes.size() == 1) {
			PackedVector2Array pa;
			for(int i=0; i<current_strokes[0].size(); i++) pa.push_back(current_strokes[0][i]);
			final_result = recognizer->recognize(pa);
		} else {
			TypedArray<PackedVector2Array> strokes_arr;
			for(int i=0; i<current_strokes.size(); i++) {
				PackedVector2Array pa;
				for(int j=0; j<current_strokes[i].size(); j++) pa.push_back(current_strokes[i][j]);
				strokes_arr.push_back(pa);
			}
			final_result = recognizer->recognize_multistroke(strokes_arr);
		}
	}

	// Parse event
	Ref<InputEventVirtualGesture> ev;
	ev.instantiate();
	ev->set_device(get_device());
	ev->set_gesture_type(final_result["type"]);
	const Vector<Vector2> &last_stroke = current_strokes[current_strokes.size() - 1];
	ev->set_position(last_stroke.is_empty() ? Vector2() : last_stroke[last_stroke.size() - 1]);

	if (final_result.has("start_position")) {
		ev->set_start_position(final_result["start_position"]);
		ev->set_delta((Vector2)final_result["end_position"] - (Vector2)final_result["start_position"]);
	}

	ev->set_value(final_result["confidence"]);

	// Include raw points
	PackedVector2Array all_points;
	for (int i = 0; i < current_strokes.size(); i++) {
		for (int j = 0; j < current_strokes[i].size(); j++) {
			all_points.push_back(current_strokes[i][j]);
		}
	}
	ev->set_points(all_points);

	Input::get_singleton()->parse_input_event(ev);

	current_strokes.clear();
	stroke_start_time = 0;
	queue_redraw();
}

void VirtualGestureArea::_on_drag(int p_index, const Vector2 &p_pos, const Vector2 &p_relative) {
	if (current_strokes.is_empty()) return;

	current_strokes.write[current_strokes.size() - 1].push_back(p_pos);
	queue_redraw();
}

Size2 VirtualGestureArea::get_minimum_size() const {
	return Size2(20, 20);
}

void VirtualGestureArea::set_min_confidence(float p_confidence) {
	recognizer->set_min_confidence(p_confidence);
}

float VirtualGestureArea::get_min_confidence() const {
	return recognizer->get_min_confidence();
}

void VirtualGestureArea::set_resample_points(int p_points) {
	recognizer->set_resample_points(p_points);
}

int VirtualGestureArea::get_resample_points() const {
	return recognizer->get_resample_points();
}

void VirtualGestureArea::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_min_confidence", "confidence"), &VirtualGestureArea::set_min_confidence);
	ClassDB::bind_method(D_METHOD("get_min_confidence"), &VirtualGestureArea::get_min_confidence);
	ClassDB::bind_method(D_METHOD("set_resample_points", "points"), &VirtualGestureArea::set_resample_points);
	ClassDB::bind_method(D_METHOD("get_resample_points"), &VirtualGestureArea::get_resample_points);

	ClassDB::bind_method(D_METHOD("set_swipe_min_distance", "distance"), &VirtualGestureArea::set_swipe_min_distance);
	ClassDB::bind_method(D_METHOD("get_swipe_min_distance"), &VirtualGestureArea::get_swipe_min_distance);
	ClassDB::bind_method(D_METHOD("set_swipe_max_time", "time"), &VirtualGestureArea::set_swipe_max_time);
	ClassDB::bind_method(D_METHOD("get_swipe_max_time"), &VirtualGestureArea::get_swipe_max_time);

	ClassDB::bind_method(D_METHOD("set_multistroke_timeout", "time"), &VirtualGestureArea::set_multistroke_timeout);
	ClassDB::bind_method(D_METHOD("get_multistroke_timeout"), &VirtualGestureArea::get_multistroke_timeout);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_confidence", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_min_confidence", "get_min_confidence");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "resample_points", PROPERTY_HINT_RANGE, "8,128,1"), "set_resample_points", "get_resample_points");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "swipe_min_distance"), "set_swipe_min_distance", "get_swipe_min_distance");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "swipe_max_time"), "set_swipe_max_time", "get_swipe_max_time");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "multistroke_timeout"), "set_multistroke_timeout", "get_multistroke_timeout");
}

void VirtualGestureArea::set_swipe_min_distance(float p_distance) {
	swipe_min_distance = p_distance;
}

float VirtualGestureArea::get_swipe_min_distance() const {
	return swipe_min_distance;
}

void VirtualGestureArea::set_swipe_max_time(float p_time) {
	swipe_max_time = p_time;
}

float VirtualGestureArea::get_swipe_max_time() const {
	return swipe_max_time;
}

void VirtualGestureArea::set_multistroke_timeout(float p_time) {
	multistroke_timeout = p_time;
}

float VirtualGestureArea::get_multistroke_timeout() const {
	return multistroke_timeout;
}
