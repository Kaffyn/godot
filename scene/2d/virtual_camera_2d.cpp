/**************************************************************************/
/*  virtual_camera_2d.cpp                                                 */
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

#include "virtual_camera_2d.h"

#include "servers/camera_server.h"

void VirtualCamera2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				CameraServer::get_singleton()->register_vcam_2d(get_instance_id(), priority);
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				CameraServer::get_singleton()->unregister_vcam_2d(get_instance_id());
			}
		} break;
	}
}

void VirtualCamera2D::set_priority(int p_priority) {
	priority = p_priority;
	if (is_inside_tree() && !Engine::get_singleton()->is_editor_hint()) {
		CameraServer::get_singleton()->update_vcam_2d_priority(get_instance_id(), priority);
	}
}

int VirtualCamera2D::get_priority() const {
	return priority;
}

void VirtualCamera2D::set_zoom(const Vector2 &p_zoom) {
	zoom = p_zoom;
}

Vector2 VirtualCamera2D::get_zoom() const {
	return zoom;
}

void VirtualCamera2D::set_blending_time(float p_time) {
	blending_time = p_time;
}

float VirtualCamera2D::get_blending_time() const {
	return blending_time;
}

void VirtualCamera2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_priority", "priority"), &VirtualCamera2D::set_priority);
	ClassDB::bind_method(D_METHOD("get_priority"), &VirtualCamera2D::get_priority);
	ClassDB::bind_method(D_METHOD("set_zoom", "zoom"), &VirtualCamera2D::set_zoom);
	ClassDB::bind_method(D_METHOD("get_zoom"), &VirtualCamera2D::get_zoom);
	ClassDB::bind_method(D_METHOD("set_blending_time", "time"), &VirtualCamera2D::set_blending_time);
	ClassDB::bind_method(D_METHOD("get_blending_time"), &VirtualCamera2D::get_blending_time);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "priority"), "set_priority", "get_priority");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "zoom"), "set_zoom", "get_zoom");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "blending_time", PROPERTY_HINT_RANGE, "0,10,0.01,suffix:s"), "set_blending_time", "get_blending_time");
}

VirtualCamera2D::VirtualCamera2D() {
}
