/**************************************************************************/
/*  virtual_gesture_area.h                                                */
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

#ifndef VIRTUAL_GESTURE_AREA_H
#define VIRTUAL_GESTURE_AREA_H

#include "core/math/gesture_recognizer.h"
#include "scene/gui/virtual_device.h"

class VirtualGestureArea : public VirtualDevice {
	GDCLASS(VirtualGestureArea, VirtualDevice);

	Ref<GestureRecognizer> recognizer;
	Vector<Vector<Vector2>> current_strokes;
	uint64_t stroke_start_time = 0;
	float multistroke_timer = 0.0f;
	bool is_waiting_for_multistroke = false;

	// Settings
	float swipe_min_distance = 50.0f;
	float swipe_max_time = 0.3f; // seconds
	float multistroke_timeout = 0.5f; // seconds to wait for next stroke

	// Trail visualization
	Vector2 last_pos;
	Vector2 current_pos;

protected:
	void _notification(int p_what);
	static void _bind_methods();

	virtual void _on_touch_down(int p_index, const Vector2 &p_pos) override;
	virtual void _on_touch_up(int p_index, const Vector2 &p_pos) override;
	virtual void _on_drag(int p_index, const Vector2 &p_pos, const Vector2 &p_relative) override;
	virtual Size2 get_minimum_size() const override;
	// _gui_input override would go here to handle multi-touch

public:
	void set_min_confidence(float p_confidence);
	float get_min_confidence() const;

	void set_resample_points(int p_points);
	int get_resample_points() const;

	void set_swipe_min_distance(float p_distance);
	float get_swipe_min_distance() const;

	void set_swipe_max_time(float p_time);
	float get_swipe_max_time() const;

	void set_multistroke_timeout(float p_timeout);
	float get_multistroke_timeout() const;

	void _perform_recognition();

	VirtualGestureArea();
};

#endif // VIRTUAL_GESTURE_AREA_H
