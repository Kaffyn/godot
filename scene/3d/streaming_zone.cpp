/**************************************************************************/
/*  streaming_zone.cpp                                                    */
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

#include "streaming_zone.h"
#include "scene/3d/camera_3d.h"
#include "scene/main/viewport.h"
#include "servers/lss_server.h"

void StreamingZone::set_scenes_to_load(const Vector<String> &p_scenes) {
	scenes_to_load = p_scenes;
}

Vector<String> StreamingZone::get_scenes_to_load() const {
	return scenes_to_load;
}

void StreamingZone::set_scenes_to_unload(const Vector<String> &p_scenes) {
	scenes_to_unload = p_scenes;
}

Vector<String> StreamingZone::get_scenes_to_unload() const {
	return scenes_to_unload;
}

void StreamingZone::set_radius(float p_radius) {
	radius = p_radius;
	update_gizmos();
}

float StreamingZone::get_radius() const {
	return radius;
}

void StreamingZone::set_active(bool p_active) {
	active = p_active;
}

bool StreamingZone::is_active() const {
	return active;
}

void StreamingZone::_notification(int p_what) {
	if (!active) {
		return;
	}

	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			// Basic distance check for now, later we integrate with Physics/Area
			// For simplicity in this core implementation, we check distance to active camera
			Viewport *vp = get_viewport();
			if (!vp) {
				return;
			}

			Camera3D *cam = vp->get_camera_3d();
			if (!cam) {
				return;
			}

			float dist = get_global_position().distance_to(cam->get_global_position());
			if (dist < radius) {
				// Potential load trigger
				// LSSServer::get_singleton()->request_chunks(scenes_to_load);
			}
		} break;
		case NOTIFICATION_ENTER_TREE: {
			set_process_internal(true);
		} break;
	}
}

void StreamingZone::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_scenes_to_load", "scenes"), &StreamingZone::set_scenes_to_load);
	ClassDB::bind_method(D_METHOD("get_scenes_to_load"), &StreamingZone::get_scenes_to_load);

	ClassDB::bind_method(D_METHOD("set_scenes_to_unload", "scenes"), &StreamingZone::set_scenes_to_unload);
	ClassDB::bind_method(D_METHOD("get_scenes_to_unload"), &StreamingZone::get_scenes_to_unload);

	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &StreamingZone::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &StreamingZone::get_radius);

	ClassDB::bind_method(D_METHOD("set_active", "active"), &StreamingZone::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &StreamingZone::is_active);

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "scenes_to_load"), "set_scenes_to_load", "get_scenes_to_load");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "scenes_to_unload"), "set_scenes_to_unload", "get_scenes_to_unload");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius", PROPERTY_HINT_RANGE, "0.1,4096,0.1,or_greater"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "is_active");
}

StreamingZone::StreamingZone() {
}
