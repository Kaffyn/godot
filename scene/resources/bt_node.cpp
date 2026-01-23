/**************************************************************************/
/*  bt_node.cpp                                                           */
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

#include "bt_node.h"
#include "scene/main/ability_system_component.h"
#include "scene/main/bt_agent.h"
#include "servers/neural_server.h"

// --- BTNode ---

BTNode::Status BTNode::tick(BTAgent *p_agent, double p_delta) {
	return STATUS_SUCCESS;
}

void BTNode::open(BTAgent *p_agent) {
}

void BTNode::close(BTAgent *p_agent, Status p_status) {
}

void BTNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("tick", "agent", "delta"), &BTNode::tick);
	ClassDB::bind_method(D_METHOD("open", "agent"), &BTNode::open);
	ClassDB::bind_method(D_METHOD("close", "agent", "status"), &BTNode::close);

	BIND_ENUM_CONSTANT(STATUS_IDLE);
	BIND_ENUM_CONSTANT(STATUS_RUNNING);
	BIND_ENUM_CONSTANT(STATUS_SUCCESS);
	BIND_ENUM_CONSTANT(STATUS_FAILURE);
}

BTNode::BTNode() {
}

// --- BTDecorator ---

void BTDecorator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_child", "child"), &BTDecorator::set_child);
	ClassDB::bind_method(D_METHOD("get_child"), &BTDecorator::get_child);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "child", PROPERTY_HINT_RESOURCE_TYPE, "BTNode"), "set_child", "get_child");
}

// --- BTComposite ---

void BTComposite::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_children", "children"), &BTComposite::set_children);
	ClassDB::bind_method(D_METHOD("get_children"), &BTComposite::get_children);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "children", PROPERTY_HINT_ARRAY_TYPE, "BTNode"), "set_children", "get_children");
}

// --- BTSequence ---

BTNode::Status BTSequence::tick(BTAgent *p_agent, double p_delta) {
	TypedArray<BTNode> l_children = get_children();
	for (int i = 0; i < l_children.size(); i++) {
		Ref<BTNode> child_node = l_children[i];
		if (child_node.is_null()) {
			continue;
		}
		Status s = child_node->tick(p_agent, p_delta);
		if (s != STATUS_SUCCESS) {
			return s;
		}
	}
	return STATUS_SUCCESS;
}

// --- BTSelector ---

BTNode::Status BTSelector::tick(BTAgent *p_agent, double p_delta) {
	TypedArray<BTNode> l_children = get_children();
	for (int i = 0; i < l_children.size(); i++) {
		Ref<BTNode> child_node = l_children[i];
		if (child_node.is_null()) {
			continue;
		}
		Status s = child_node->tick(p_agent, p_delta);
		if (s != STATUS_FAILURE) {
			return s;
		}
	}
	return STATUS_FAILURE;
}

// --- BTCheckPerception ---

void BTCheckPerception::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_stimulus_type", "type"), &BTCheckPerception::set_stimulus_type);
	ClassDB::bind_method(D_METHOD("get_stimulus_type"), &BTCheckPerception::get_stimulus_type);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &BTCheckPerception::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &BTCheckPerception::get_radius);
	ClassDB::bind_method(D_METHOD("set_required_tags", "tags"), &BTCheckPerception::set_required_tags);
	ClassDB::bind_method(D_METHOD("get_required_tags"), &BTCheckPerception::get_required_tags);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "stimulus_type", PROPERTY_HINT_ENUM, "Visual,Auditory,Olfactory,Thermal,Vibrational"), "set_stimulus_type", "get_stimulus_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "required_tags"), "set_required_tags", "get_required_tags");
}

BTNode::Status BTCheckPerception::tick(BTAgent *p_agent, double p_delta) {
	Vector<NeuralServer::Stimulus> stimuli = NeuralServer::get_singleton()->query_stimuli(p_agent->get_agent_position(), radius, (1 << stimulus_type));

	bool found = false;
	if (required_tags.is_empty()) {
		found = !stimuli.is_empty();
	} else {
		for (const NeuralServer::Stimulus &S : stimuli) {
			bool all_tags = true;
			for (int i = 0; i < required_tags.size(); i++) {
				if (S.tags.find(required_tags[i]) == -1) {
					all_tags = false;
					break;
				}
			}
			if (all_tags) {
				found = true;
				break;
			}
		}
	}

	if (found) {
		Ref<BTNode> child_node = get_child();
		if (child_node.is_valid()) {
			return child_node->tick(p_agent, p_delta);
		}
		return STATUS_SUCCESS;
	}
	return STATUS_FAILURE;
}

BTCheckPerception::BTCheckPerception() {}

// --- BTActionAbility ---

void BTActionAbility::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ability_tag", "tag"), &BTActionAbility::set_ability_tag);
	ClassDB::bind_method(D_METHOD("get_ability_tag"), &BTActionAbility::get_ability_tag);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ability_tag"), "set_ability_tag", "get_ability_tag");
}

BTNode::Status BTActionAbility::tick(BTAgent *p_agent, double p_delta) {
	AbilitySystemComponent *asc = p_agent->get_asc();
	if (!asc) {
		return STATUS_FAILURE;
	}

	if (asc->try_activate_ability(ability_tag)) {
		return STATUS_SUCCESS;
	}
	return STATUS_FAILURE;
}

BTActionAbility::BTActionAbility() {}
