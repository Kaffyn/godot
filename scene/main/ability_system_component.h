/**************************************************************************/
/*  ability_system_component.h                                            */
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
#include "scene/resources/attribute_set.h"
#include "scene/resources/gameplay_ability.h"
#include "scene/resources/gameplay_effect.h"

class AbilitySystemComponent : public Node {
	GDCLASS(AbilitySystemComponent, Node);

	Ref<AttributeSet> attribute_set;
	Vector<Ref<GameplayAbility>> granted_abilities;
	Ref<GameplayTagContainer> active_tags;

	// O(1) Candidate Selection Maps
	HashMap<uint32_t, Vector<Ref<GameplayAbility>>> tag_to_ability_map;

protected:
	static void _bind_methods();
	void _update_inverted_index();

public:
	void set_attribute_set(const Ref<AttributeSet> &p_set);
	Ref<AttributeSet> get_attribute_set() const { return attribute_set; }

	void grant_ability(const Ref<GameplayAbility> &p_ability);
	void remove_ability(const Ref<GameplayAbility> &p_ability);

	bool try_activate_ability(const StringName &p_tag_name);
	bool try_activate_ability_by_tag(const GameplayTag &p_tag);
	void apply_gameplay_effect(const Ref<GameplayEffect> &p_effect);

	AbilitySystemComponent();
};
