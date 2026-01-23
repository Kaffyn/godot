/**************************************************************************/
/*  neural_agent.cpp                                                      */
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

#include "neural_agent.h"

void NeuralAgent::set_faction(const StringName &p_faction) {
	agent_faction = p_faction;
}

StringName NeuralAgent::get_faction() const {
	return agent_faction;
}

void NeuralAgent::set_model(const Ref<Resource> &p_model) {
	neural_model = p_model;
}

Ref<Resource> NeuralAgent::get_model() const {
	return neural_model;
}

void NeuralAgent::add_stimulus(const NeuralServer::Stimulus &p_stimulus) {
	perceived_stimuli.push_back(p_stimulus);
}

Array NeuralAgent::get_perceived_stimuli() const {
	Array arr;
	for (const NeuralServer::Stimulus &S : perceived_stimuli) {
		Dictionary d;
		d["type"] = S.type;
		d["position"] = S.position;
		d["intensity"] = S.intensity;
		d["faction"] = S.faction;
		d["tags"] = S.tags;
		d["emitter_id"] = S.emitter_id;
		arr.push_back(d);
	}
	return arr;
}

Dictionary NeuralAgent::get_blackboard() const {
	return blackboard;
}

void NeuralAgent::set_blackboard_value(const StringName &p_key, const Variant &p_value) {
	blackboard[p_key] = p_value;
}

void NeuralAgent::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			Neural->register_agent(this);
		} break;
		case NOTIFICATION_EXIT_TREE: {
			Neural->unregister_agent(this);
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			// Clear stimuli for next frame
			perceived_stimuli.clear();
		} break;
	}
}

void NeuralAgent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_faction", "faction"), &NeuralAgent::set_faction);
	ClassDB::bind_method(D_METHOD("get_faction"), &NeuralAgent::get_faction);
	ClassDB::bind_method(D_METHOD("set_model", "model"), &NeuralAgent::set_model);
	ClassDB::bind_method(D_METHOD("get_model"), &NeuralAgent::get_model);
	ClassDB::bind_method(D_METHOD("get_perceived_stimuli"), &NeuralAgent::get_perceived_stimuli);
	ClassDB::bind_method(D_METHOD("set_blackboard_value", "key", "value"), &NeuralAgent::set_blackboard_value);
	ClassDB::bind_method(D_METHOD("get_blackboard"), &NeuralAgent::get_blackboard);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "faction"), "set_faction", "get_faction");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "NeuralModel"), "set_model", "get_model");
}

NeuralAgent::NeuralAgent() {
	set_physics_process(true);
}

NeuralAgent::~NeuralAgent() {
}
