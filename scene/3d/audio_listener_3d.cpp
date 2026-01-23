/**************************************************************************/
/*  audio_listener_3d.cpp                                                 */
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

#include "audio_listener_3d.h"

#include "scene/main/neural_agent.h"
#include "scene/main/viewport.h"
#include "servers/neural_server.h"

void AudioListener3D::_update_audio_listener_state() {
}

void AudioListener3D::_request_listener_update() {
	_update_listener();
}

bool AudioListener3D::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "current") {
		if (p_value.operator bool()) {
			make_current();
		} else {
			clear_current();
		}
	} else {
		return false;
	}

	return true;
}

bool AudioListener3D::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "current") {
		if (is_part_of_edited_scene()) {
			r_ret = current;
		} else {
			r_ret = is_current();
		}
	} else {
		return false;
	}

	return true;
}

void AudioListener3D::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::BOOL, PNAME("current")));
}

void AudioListener3D::_update_listener() {
	if (is_inside_tree() && is_current()) {
		get_viewport()->_listener_transform_3d_changed_notify();
	}
}

void AudioListener3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			scenario_id = get_world_3d()->get_scenario();
			_update_listener();
			_update_neural_agent_cache();
		} break;

		case NOTIFICATION_ENTER_WORLD: {
			bool first_listener = get_viewport()->_audio_listener_3d_add(this);
			if (!is_part_of_edited_scene() && (current || first_listener)) {
				make_current();
			}
		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			_request_listener_update();
			if (doppler_tracking != DOPPLER_TRACKING_DISABLED) {
				velocity_tracker->update_position(get_global_transform().origin);
			}
		} break;

		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (neural_agent_path.is_empty()) {
				break;
			}

			if (!neural_agent_cache) {
				_update_neural_agent_cache();
			}

			if (neural_agent_cache) {
				Vector<NeuralServer::Stimulus> stimuli = NeuralServer::get_singleton()->query_stimuli(get_global_position(), hearing_range, 1 << NeuralStimulus::STIMULUS_AUDITORY);
				for (int i = 0; i < stimuli.size(); i++) {
					neural_agent_cache->add_stimulus(stimuli[i]);
				}
			}
		} break;

		case NOTIFICATION_EXIT_WORLD: {
			if (!is_part_of_edited_scene()) {
				if (is_current()) {
					clear_current();
					current = true; //keep it true

				} else {
					current = false;
				}
			}

			get_viewport()->_audio_listener_3d_remove(this);
		} break;

		case NOTIFICATION_EXIT_TREE: {
			scenario_id = RID();
			set_physics_process_internal(false);
		} break;
	}
}

Transform3D AudioListener3D::get_listener_transform() const {
	return get_global_transform().orthonormalized();
}

void AudioListener3D::make_current() {
	current = true;

	if (!is_inside_tree()) {
		return;
	}

	get_viewport()->_audio_listener_3d_set(this);
}

void AudioListener3D::clear_current() {
	current = false;
	if (!is_inside_tree()) {
		return;
	}

	if (get_viewport()->get_audio_listener_3d() == this) {
		get_viewport()->_audio_listener_3d_set(nullptr);
		get_viewport()->_audio_listener_3d_make_next_current(this);
	}
}

bool AudioListener3D::is_current() const {
	if (is_inside_tree() && !is_part_of_edited_scene()) {
		return get_viewport()->get_audio_listener_3d() == this;
	} else {
		return current;
	}
}

void AudioListener3D::set_doppler_tracking(DopplerTracking p_tracking) {
	if (doppler_tracking == p_tracking) {
		return;
	}

	doppler_tracking = p_tracking;
	if (p_tracking != DOPPLER_TRACKING_DISABLED) {
		velocity_tracker->set_track_physics_step(doppler_tracking == DOPPLER_TRACKING_PHYSICS_STEP);
		if (is_inside_tree()) {
			velocity_tracker->reset(get_global_transform().origin);
		}
	}
}

AudioListener3D::DopplerTracking AudioListener3D::get_doppler_tracking() const {
	return doppler_tracking;
}

void AudioListener3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("make_current"), &AudioListener3D::make_current);
	ClassDB::bind_method(D_METHOD("clear_current"), &AudioListener3D::clear_current);
	ClassDB::bind_method(D_METHOD("is_current"), &AudioListener3D::is_current);
	ClassDB::bind_method(D_METHOD("get_listener_transform"), &AudioListener3D::get_listener_transform);
	ClassDB::bind_method(D_METHOD("set_doppler_tracking", "mode"), &AudioListener3D::set_doppler_tracking);
	ClassDB::bind_method(D_METHOD("get_doppler_tracking"), &AudioListener3D::get_doppler_tracking);

	ClassDB::bind_method(D_METHOD("set_neural_agent_path", "path"), &AudioListener3D::set_neural_agent_path);
	ClassDB::bind_method(D_METHOD("get_neural_agent_path"), &AudioListener3D::get_neural_agent_path);

	ClassDB::bind_method(D_METHOD("set_hearing_range", "range"), &AudioListener3D::set_hearing_range);
	ClassDB::bind_method(D_METHOD("get_hearing_range"), &AudioListener3D::get_hearing_range);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "doppler_tracking", PROPERTY_HINT_ENUM, "Disabled,Idle,Physics"), "set_doppler_tracking", "get_doppler_tracking");

	ADD_GROUP("Neural", "neural_");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "neural_agent_path"), "set_neural_agent_path", "get_neural_agent_path");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "neural_hearing_range", PROPERTY_HINT_RANGE, "0,1000,0.1,or_greater"), "set_hearing_range", "get_hearing_range");

	BIND_ENUM_CONSTANT(DOPPLER_TRACKING_DISABLED);
	BIND_ENUM_CONSTANT(DOPPLER_TRACKING_IDLE_STEP);
	BIND_ENUM_CONSTANT(DOPPLER_TRACKING_PHYSICS_STEP);
}

Vector3 AudioListener3D::get_doppler_tracked_velocity() const {
	if (doppler_tracking != DOPPLER_TRACKING_DISABLED) {
		return velocity_tracker->get_tracked_linear_velocity();
	} else {
		return Vector3();
	}
}

AudioListener3D::AudioListener3D() {
	set_notify_transform(true);
	velocity_tracker.instantiate();
}

AudioListener3D::~AudioListener3D() {
}

void AudioListener3D::set_neural_agent_path(const NodePath &p_path) {
	neural_agent_path = p_path;
	neural_agent_cache = nullptr;
	set_physics_process_internal(!neural_agent_path.is_empty());
}

NodePath AudioListener3D::get_neural_agent_path() const {
	return neural_agent_path;
}

void AudioListener3D::set_hearing_range(float p_range) {
	hearing_range = p_range;
}

float AudioListener3D::get_hearing_range() const {
	return hearing_range;
}

void AudioListener3D::_update_neural_agent_cache() {
	if (!is_inside_tree()) {
		return;
	}
	Node *n = get_node_or_null(neural_agent_path);
	neural_agent_cache = Object::cast_to<NeuralAgent>(n);
}
