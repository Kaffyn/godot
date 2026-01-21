/**************************************************************************/
/*  virtual_camera_3d.h                                                   */
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

#pragma once

#include "scene/3d/node_3d.h"

class VirtualCamera3D : public Node3D {
	GDCLASS(VirtualCamera3D, Node3D);

private:
	int priority = 0;
	real_t fov = 75.0;
	real_t blending_time = 1.0;

	// Avoidance System
	bool enable_avoidance = false;
	real_t avoidance_distance = 5.0;
	uint32_t collision_mask = 1;
	Vector3 compensated_position;
	bool is_obstructed = false;

protected:
	static void _bind_methods();

public:
	void set_priority(int p_priority);
	int get_priority() const { return priority; }

	void set_fov(real_t p_fov);
	real_t get_fov() const { return fov; }

	void set_blending_time(real_t p_time);
	real_t get_blending_time() const { return blending_time; }

	// Avoidance
	void set_enable_avoidance(bool p_enable);
	bool get_enable_avoidance() const { return enable_avoidance; }

	void set_avoidance_distance(real_t p_distance);
	real_t get_avoidance_distance() const { return avoidance_distance; }

	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() const { return collision_mask; }

	Vector3 get_compensated_position() const { return compensated_position; }
	bool is_position_obstructed() const { return is_obstructed; }

	void update_avoidance(const Vector3 &p_target_position);

	VirtualCamera3D();
};
