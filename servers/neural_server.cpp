/**************************************************************************/
/*  neural_server.cpp                                                     */
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

#include "neural_server.h"
#include "core/config/engine.h"
#include "core/templates/list.h"
#include "scene/main/neural_agent.h"

NeuralServer *NeuralServer::singleton = nullptr;

NeuralServer *NeuralServer::get_singleton() {
	return singleton;
}

void NeuralServer::emit_stimulus(const Stimulus &p_stimulus) {
	active_stimuli[p_stimulus.emitter_id].push_back(p_stimulus);
}

Vector<NeuralServer::Stimulus> NeuralServer::query_stimuli(const Vector3 &p_position, real_t p_radius, uint32_t p_type_mask) {
	Vector<NeuralServer::Stimulus> found;
	real_t radius_sq = p_radius * p_radius;

	for (const KeyValue<ObjectID, Vector<Stimulus>> &E : active_stimuli) {
		for (const Stimulus &S : E.value) {
			if (!(p_type_mask & (1 << S.type))) {
				continue;
			}

			if (S.position.distance_squared_to(p_position) <= radius_sq) {
				found.push_back(S);
			}
		}
	}

	return found;
}

void NeuralServer::register_agent(NeuralAgent *p_agent) {
	if (registered_agents.find(p_agent) == -1) {
		registered_agents.push_back(p_agent);
	}
}

void NeuralServer::unregister_agent(NeuralAgent *p_agent) {
	registered_agents.erase(p_agent);
}

void NeuralServer::request_inference(ObjectID p_agent_id, const String &p_model_path, const Dictionary &p_inputs) {
	// Future ONNX implementation
}

void NeuralServer::update(double p_delta) {
	// 1. Collect keys to process
	LocalVector<ObjectID> keys;
	for (const KeyValue<ObjectID, Vector<Stimulus>> &E : active_stimuli) {
		keys.push_back(E.key);
	}

	// 2. Update/Cleanup stimuli using mutable access
	LocalVector<ObjectID> to_remove;
	for (const ObjectID &id : keys) {
		Vector<Stimulus> *v = active_stimuli.getptr(id);
		if (!v) {
			continue;
		}

		// Use write pointer for mutation
		Stimulus *stimuli = v->ptrw();
		int count = v->size();

		for (int i = count - 1; i >= 0; i--) {
			if (stimuli[i].lifetime <= 0) {
				v->remove_at(i);
				// After remove_at, ptrw may be invalidated, update it
				stimuli = v->ptrw();
			} else {
				stimuli[i].lifetime -= p_delta;
			}
		}

		if (v->is_empty()) {
			to_remove.push_back(id);
		}
	}

	for (const ObjectID &id : to_remove) {
		active_stimuli.erase(id);
	}

	// 3. We don't act directly on agents here; agents poll the server or are notified.
}

void NeuralServer::_bind_methods() {
	// Enum constants are exposed via VARIANT_ENUM_CAST(NeuralServer::StimulusType) in neural_server.h
}

NeuralServer::NeuralServer() {
	singleton = this;
}

NeuralServer::~NeuralServer() {
	singleton = nullptr;
}
