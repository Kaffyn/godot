/**************************************************************************/
/*  neural_emitter.cpp                                                    */
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

#include "neural_emitter.h"

void NeuralEmitter::set_stimulus_type(NeuralServer::StimulusType p_type) {
	type = p_type;
}

NeuralServer::StimulusType NeuralEmitter::get_stimulus_type() const {
	return type;
}

void NeuralEmitter::set_intensity(real_t p_intensity) {
	intensity = p_intensity;
}

real_t NeuralEmitter::get_intensity() const {
	return intensity;
}

void NeuralEmitter::set_faction(const StringName &p_faction) {
	faction = p_faction;
}

StringName NeuralEmitter::get_faction() const {
	return faction;
}

void NeuralEmitter::set_tags(const PackedStringArray &p_tags) {
	tags = p_tags;
}

PackedStringArray NeuralEmitter::get_tags() const {
	return tags;
}

void NeuralEmitter::set_active(bool p_active) {
	active = p_active;
	set_physics_process(active);
}

bool NeuralEmitter::is_active() const {
	return active;
}

void NeuralEmitter::emit() {
	if (!is_inside_tree() || !active) {
		return;
	}

	NeuralServer::Stimulus S;
	S.type = type;
	S.position = get_global_transform().origin;
	S.intensity = intensity;
	S.faction = faction;
	S.tags = tags;
	S.emitter_id = get_instance_id();
	S.lifetime = 0; // Transient by default

	NeuralServer::get_singleton()->emit_stimulus(S);
}

void NeuralEmitter::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PHYSICS_PROCESS: {
			if (pulse_interval <= 0) {
				emit();
			} else {
				time_since_pulse += get_physics_process_delta_time();
				if (time_since_pulse >= pulse_interval) {
					emit();
					time_since_pulse = 0;
				}
			}
		} break;
	}
}

void NeuralEmitter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_stimulus_type", "type"), &NeuralEmitter::set_stimulus_type);
	ClassDB::bind_method(D_METHOD("get_stimulus_type"), &NeuralEmitter::get_stimulus_type);
	ClassDB::bind_method(D_METHOD("set_intensity", "intensity"), &NeuralEmitter::set_intensity);
	ClassDB::bind_method(D_METHOD("get_intensity"), &NeuralEmitter::get_intensity);
	ClassDB::bind_method(D_METHOD("set_faction", "faction"), &NeuralEmitter::set_faction);
	ClassDB::bind_method(D_METHOD("get_faction"), &NeuralEmitter::get_faction);
	ClassDB::bind_method(D_METHOD("set_tags", "tags"), &NeuralEmitter::set_tags);
	ClassDB::bind_method(D_METHOD("get_tags"), &NeuralEmitter::get_tags);
	ClassDB::bind_method(D_METHOD("set_active", "active"), &NeuralEmitter::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &NeuralEmitter::is_active);
	ClassDB::bind_method(D_METHOD("emit"), &NeuralEmitter::emit);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "stimulus_type", PROPERTY_HINT_ENUM, "Visual,Auditory,Olfactory,Thermal,Vibrational"), "set_stimulus_type", "get_stimulus_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "intensity"), "set_intensity", "get_intensity");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "faction"), "set_faction", "get_faction");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "tags"), "set_tags", "get_tags");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "is_active");
}

NeuralEmitter::NeuralEmitter() {
	set_physics_process(true);
}
