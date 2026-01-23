/**************************************************************************/
/*  bt_agent.cpp                                                          */
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

#include "bt_agent.h"
#include "scene/3d/node_3d.h"
#include "scene/main/ability_system_component.h"

void BTAgent::set_root_node(const Ref<BTNode> &p_node) {
	root_node = p_node;
}

Ref<BTNode> BTAgent::get_root_node() const {
	return root_node;
}

void BTAgent::set_blackboard_value(const StringName &p_key, const Variant &p_value) {
	blackboard[p_key] = p_value;
}

Variant BTAgent::get_blackboard_value(const StringName &p_key, const Variant &p_default) const {
	return blackboard.get(p_key, p_default);
}

void BTAgent::set_active(bool p_active) {
	active = p_active;
	set_process(active);
}

bool BTAgent::is_active() const {
	return active;
}

void BTAgent::_update_asc_cache() {
	asc_cache = nullptr;
	Node *p = get_parent();
	if (!p) {
		return;
	}

	// Check parent
	asc_cache = Object::cast_to<AbilitySystemComponent>(p);
	if (asc_cache) {
		return;
	}

	// Check siblings
	for (int i = 0; i < p->get_child_count(); i++) {
		AbilitySystemComponent *asc = Object::cast_to<AbilitySystemComponent>(p->get_child(i));
		if (asc) {
			asc_cache = asc;
			return;
		}
	}
}

Vector3 BTAgent::get_agent_position() const {
	Node3D *n3d = Object::cast_to<Node3D>(get_parent());
	if (n3d) {
		return n3d->get_global_position();
	}
	return Vector3();
}

void BTAgent::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_asc_cache();
			if (active) {
				set_process(true);
			}
		} break;
		case NOTIFICATION_PROCESS: {
			if (root_node.is_valid()) {
				root_node->tick(this, get_process_delta_time());
			}
		} break;
	}
}

void BTAgent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_root_node", "node"), &BTAgent::set_root_node);
	ClassDB::bind_method(D_METHOD("get_root_node"), &BTAgent::get_root_node);

	ClassDB::bind_method(D_METHOD("set_blackboard_value", "key", "value"), &BTAgent::set_blackboard_value);
	ClassDB::bind_method(D_METHOD("get_blackboard_value", "key", "default"), &BTAgent::get_blackboard_value, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("set_active", "active"), &BTAgent::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &BTAgent::is_active);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root_node", PROPERTY_HINT_RESOURCE_TYPE, "BTNode"), "set_root_node", "get_root_node");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "is_active");
}

BTAgent::BTAgent() {
}
