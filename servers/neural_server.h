/**************************************************************************/
/*  neural_server.h                                                       */
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

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/templates/hash_map.h"
#include "core/templates/list.h"
#include "core/templates/local_vector.h"
#include "core/templates/vector.h"
#include "core/variant/variant.h"

// Forward declarations
class NeuralAgent;

struct NeuralStimulus {
	enum Type {
		STIMULUS_VISUAL,
		STIMULUS_AUDITORY,
		STIMULUS_OLFACTORY,
		STIMULUS_THERMAL,
		STIMULUS_VIBRATIONAL,
		STIMULUS_MAX
	};

	Type type = STIMULUS_VISUAL;
	Vector3 position;
	real_t intensity = 1.0;
	StringName faction;
	PackedStringArray tags;
	ObjectID emitter_id;
	real_t lifetime = 0.0; // 0 = transient (1 frame)
};

class NeuralServer : public Object {
	GDCLASS(NeuralServer, Object);

public:
	typedef NeuralStimulus::Type StimulusType;
	typedef NeuralStimulus Stimulus;

	static NeuralServer *singleton;

private:
	HashMap<ObjectID, Vector<Stimulus>> active_stimuli;
	Vector<NeuralAgent *> registered_agents;

protected:
	static void _bind_methods();

public:
	static NeuralServer *get_singleton();

	// Perception API
	void emit_stimulus(const Stimulus &p_stimulus);
	Vector<Stimulus> query_stimuli(const Vector3 &p_position, real_t p_radius, uint32_t p_type_mask = 0xFFFFFFFF);

	// Agent Registry
	void register_agent(NeuralAgent *p_agent);
	void unregister_agent(NeuralAgent *p_agent);

	// Inference API
	void request_inference(ObjectID p_agent_id, const String &p_model_path, const Dictionary &p_inputs);

	void update(double p_delta);

	NeuralServer();
	~NeuralServer();
};

VARIANT_ENUM_CAST(NeuralServer::StimulusType);

#define Neural NeuralServer::get_singleton()
