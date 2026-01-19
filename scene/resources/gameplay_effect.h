/**************************************************************************/
/*  gameplay_effect.h                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2025-present Kaffyn and Zyris contributors              */
/* Copyright (c) 2014-present Godot Engine contributors                  */
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
#include "core/gameplay_tags.h"

class GameplayEffect : public Resource {
	GDCLASS(GameplayEffect, Resource);

public:
	enum DurationPolicy {
		DURATION_INSTANT,
		DURATION_INFINITE,
		DURATION_HAS_DURATION
	};

	enum ModifierOp {
		MODIFIER_ADD,
		MODIFIER_MULTIPLY,
		MODIFIER_OVERRIDE
	};

	struct Modifier {
		StringName attribute;
		ModifierOp op = MODIFIER_ADD;
		float magnitude = 0.0f;
	};

private:
	DurationPolicy duration_policy = DURATION_INSTANT;
	float duration = 0.0f;
	Vector<Modifier> modifiers;
	Ref<GameplayTagContainer> granted_tags;

protected:
	static void _bind_methods();

public:
	void set_duration_policy(DurationPolicy p_policy) { duration_policy = p_policy; }
	DurationPolicy get_duration_policy() const { return duration_policy; }

	void set_duration(float p_duration) { duration = p_duration; }
	float get_duration() const { return duration; }

	void add_modifier(const StringName &p_attr, ModifierOp p_op, float p_mag);
	Vector<Modifier> get_modifiers() const { return modifiers; }

	GameplayEffect() {}
};

VARIANT_ENUM_CAST(GameplayEffect::DurationPolicy);
VARIANT_ENUM_CAST(GameplayEffect::ModifierOp);
