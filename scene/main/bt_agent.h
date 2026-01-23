/**************************************************************************/
/*  bt_agent.h                                                            */
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

#include "scene/main/node.h"
#include "scene/resources/bt_node.h"

class AbilitySystemComponent;

class BTAgent : public Node {
	GDCLASS(BTAgent, Node);

	Ref<BTNode> root_node;
	Dictionary blackboard;
	AbilitySystemComponent *asc_cache = nullptr;
	bool active = true;

	void _update_asc_cache();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_root_node(const Ref<BTNode> &p_node);
	Ref<BTNode> get_root_node() const;

	void set_blackboard_value(const StringName &p_key, const Variant &p_value);
	Variant get_blackboard_value(const StringName &p_key, const Variant &p_default = Variant()) const;

	void set_active(bool p_active);
	bool is_active() const;

	AbilitySystemComponent *get_asc() const { return asc_cache; }
	Vector3 get_agent_position() const;

	BTAgent();
};
