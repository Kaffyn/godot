/**************************************************************************/
/*  bt_node.h                                                             */
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

#include "core/io/resource.h"
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"

class BTAgent;

// --- BTNode ---

class BTNode : public Resource {
	GDCLASS(BTNode, Resource);

public:
	enum Status {
		STATUS_IDLE,
		STATUS_RUNNING,
		STATUS_SUCCESS,
		STATUS_FAILURE
	};

protected:
	static void _bind_methods();

public:
	virtual Status tick(BTAgent *p_agent, double p_delta);
	virtual void open(BTAgent *p_agent);
	virtual void close(BTAgent *p_agent, Status p_status);

	BTNode();
};

VARIANT_ENUM_CAST(BTNode::Status);

// --- BTLeaf ---

class BTLeaf : public BTNode {
	GDCLASS(BTLeaf, BTNode);

public:
	BTLeaf() {}
};

// --- BTDecorator ---

class BTDecorator : public BTNode {
	GDCLASS(BTDecorator, BTNode);
	Ref<BTNode> child;

protected:
	static void _bind_methods();

public:
	void set_child(const Ref<BTNode> &p_child) { child = p_child; }
	Ref<BTNode> get_child() const { return child; }

	BTDecorator() {}
};

// --- BTComposite ---

class BTComposite : public BTNode {
	GDCLASS(BTComposite, BTNode);
	TypedArray<BTNode> children;

protected:
	static void _bind_methods();

public:
	void set_children(const TypedArray<BTNode> &p_children) { children = p_children; }
	TypedArray<BTNode> get_children() const { return children; }

	BTComposite() {}
};

// --- Composites ---

class BTSequence : public BTComposite {
	GDCLASS(BTSequence, BTComposite);

public:
	virtual Status tick(BTAgent *p_agent, double p_delta) override;
};

class BTSelector : public BTComposite {
	GDCLASS(BTSelector, BTComposite);

public:
	virtual Status tick(BTAgent *p_agent, double p_delta) override;
};

// --- Decorators ---

class BTCheckPerception : public BTDecorator {
	GDCLASS(BTCheckPerception, BTDecorator);
	int stimulus_type = 0; // NeuralServer::STIMULUS_VISUAL
	float radius = 10.0;
	PackedStringArray required_tags;

protected:
	static void _bind_methods();

public:
	virtual Status tick(BTAgent *p_agent, double p_delta) override;

	void set_stimulus_type(int p_type) { stimulus_type = p_type; }
	int get_stimulus_type() const { return stimulus_type; }

	void set_radius(float p_radius) { radius = p_radius; }
	float get_radius() const { return radius; }

	void set_required_tags(const PackedStringArray &p_tags) { required_tags = p_tags; }
	PackedStringArray get_required_tags() const { return required_tags; }

	BTCheckPerception();
};

// --- Actions ---

class BTActionAbility : public BTLeaf {
	GDCLASS(BTActionAbility, BTLeaf);
	StringName ability_tag;

protected:
	static void _bind_methods();

public:
	virtual Status tick(BTAgent *p_agent, double p_delta) override;

	void set_ability_tag(const StringName &p_tag) { ability_tag = p_tag; }
	StringName get_ability_tag() const { return ability_tag; }

	BTActionAbility();
};
