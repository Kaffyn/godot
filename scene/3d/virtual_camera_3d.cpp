/**************************************************************************/
/*  virtual_camera_3d.cpp                                                 */
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

#include "virtual_camera_3d.h"

#include "scene/main/viewport.h"
#include "servers/camera_server.h"
#include "servers/physics_server_3d.h"

void VirtualCamera3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				CameraServer::get_singleton()->register_vcam_3d(get_instance_id(), priority);
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				CameraServer::get_singleton()->unregister_vcam_3d(get_instance_id());
			}
		} break;
	}
}

void VirtualCamera3D::set_priority(int p_priority) {
	priority = p_priority;
	if (is_inside_tree() && !Engine::get_singleton()->is_editor_hint()) {
		CameraServer::get_singleton()->update_vcam_3d_priority(get_instance_id(), priority);
	}
}

int VirtualCamera3D::get_priority() const {
	return priority;
}

void VirtualCamera3D::set_fov(real_t p_fov) {
	fov = p_fov;
}

real_t VirtualCamera3D::get_fov() const {
	return fov;
}

void VirtualCamera3D::set_near(real_t p_near) {
	near_clip = p_near;
}

real_t VirtualCamera3D::get_near() const {
	return near_clip;
}

void VirtualCamera3D::set_far(real_t p_far) {
	far_clip = p_far;
}

real_t VirtualCamera3D::get_far() const {
	return far_clip;
}

void VirtualCamera3D::set_blending_time(float p_time) {
	blending_time = p_time;
}

float VirtualCamera3D::get_blending_time() const {
	return blending_time;
}

void VirtualCamera3D::set_enable_avoidance(bool p_enable) {
	enable_avoidance = p_enable;
}

bool VirtualCamera3D::get_enable_avoidance() const {
	return enable_avoidance;
}

void VirtualCamera3D::set_avoidance_distance(real_t p_distance) {
	avoidance_distance = MAX(0.1, p_distance);
}

real_t VirtualCamera3D::get_avoidance_distance() const {
	return avoidance_distance;
}

void VirtualCamera3D::set_collision_mask(uint32_t p_mask) {
	collision_mask = p_mask;
}

uint32_t VirtualCamera3D::get_collision_mask() const {
	return collision_mask;
}

void VirtualCamera3D::update_avoidance(const Vector3 &p_target_position) {
	if (!enable_avoidance) {
		compensated_position = get_global_transform().origin;
		is_obstructed = false;
		return;
	}

	// Raycast from target to camera position
	Vector3 camera_pos = get_global_transform().origin;
	Vector3 direction = camera_pos - p_target_position;
	real_t distance = direction.length();

	if (distance < 0.01) {
		compensated_position = camera_pos;
		is_obstructed = false;
		return;
	}

	direction = direction.normalized();

	// Physics query
	PhysicsDirectSpaceState3D *space_state = get_world_3d()->get_direct_space_state();
	if (!space_state) {
		compensated_position = camera_pos;
		is_obstructed = false;
		return;
	}

	PhysicsDirectSpaceState3D::RayParameters params;
	params.from = p_target_position;
	params.to = camera_pos;
	params.collision_mask = collision_mask;

	PhysicsDirectSpaceState3D::RayResult result;
	if (space_state->intersect_ray(params, result)) {
		// Hit something - compensate position
		is_obstructed = true;
		// Pull camera closer to avoid clipping
		real_t safe_distance = result.position.distance_to(p_target_position) - 0.2; // 20cm margin
		safe_distance = MAX(0.5, safe_distance); // Minimum 50cm from target
		compensated_position = p_target_position + direction * safe_distance;
	} else {
		// Clear path
		is_obstructed = false;
		compensated_position = camera_pos;
	}
}

void VirtualCamera3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_priority", "priority"), &VirtualCamera3D::set_priority);
	ClassDB::bind_method(D_METHOD("get_priority"), &VirtualCamera3D::get_priority);
	ClassDB::bind_method(D_METHOD("set_fov", "fov"), &VirtualCamera3D::set_fov);
	ClassDB::bind_method(D_METHOD("get_fov"), &VirtualCamera3D::get_fov);
	ClassDB::bind_method(D_METHOD("set_near", "near"), &VirtualCamera3D::set_near);
	ClassDB::bind_method(D_METHOD("get_near"), &VirtualCamera3D::get_near);
	ClassDB::bind_method(D_METHOD("set_far", "far"), &VirtualCamera3D::set_far);
	ClassDB::bind_method(D_METHOD("get_far"), &VirtualCamera3D::get_far);
	ClassDB::bind_method(D_METHOD("set_blending_time", "time"), &VirtualCamera3D::set_blending_time);
	ClassDB::bind_method(D_METHOD("get_blending_time"), &VirtualCamera3D::get_blending_time);

	// Avoidance
	ClassDB::bind_method(D_METHOD("set_enable_avoidance", "enable"), &VirtualCamera3D::set_enable_avoidance);
	ClassDB::bind_method(D_METHOD("get_enable_avoidance"), &VirtualCamera3D::get_enable_avoidance);
	ClassDB::bind_method(D_METHOD("set_avoidance_distance", "distance"), &VirtualCamera3D::set_avoidance_distance);
	ClassDB::bind_method(D_METHOD("get_avoidance_distance"), &VirtualCamera3D::get_avoidance_distance);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), &VirtualCamera3D::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &VirtualCamera3D::get_collision_mask);
	ClassDB::bind_method(D_METHOD("get_compensated_position"), &VirtualCamera3D::get_compensated_position);
	ClassDB::bind_method(D_METHOD("is_position_obstructed"), &VirtualCamera3D::is_position_obstructed);
	ClassDB::bind_method(D_METHOD("update_avoidance", "target_position"), &VirtualCamera3D::update_avoidance);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "priority"), "set_priority", "get_priority");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fov", PROPERTY_HINT_RANGE, "1,179,0.1,degrees"), "set_fov", "get_fov");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "near", PROPERTY_HINT_RANGE, "0.001,10,0.001,or_greater,exp,suffix:m"), "set_near", "get_near");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "far", PROPERTY_HINT_RANGE, "0.01,4000,0.01,or_greater,exp,suffix:m"), "set_far", "get_far");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "blending_time", PROPERTY_HINT_RANGE, "0,10,0.01,suffix:s"), "set_blending_time", "get_blending_time");

	ADD_GROUP("Avoidance", "avoidance_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_avoidance"), "set_enable_avoidance", "get_enable_avoidance");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "avoidance_distance", PROPERTY_HINT_RANGE, "0.1,100,0.1,suffix:m"), "set_avoidance_distance", "get_avoidance_distance");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");
}

VirtualCamera3D::VirtualCamera3D() {
}
