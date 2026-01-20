/**************************************************************************/
/*  ability_system_component.cpp                                          */
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

#include "ability_system_component.h"

// AbilitySystemComponent

void AbilitySystemComponent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_attribute_set", "set"), &AbilitySystemComponent::set_attribute_set);
	ClassDB::bind_method(D_METHOD("get_attribute_set"), &AbilitySystemComponent::get_attribute_set);

	ClassDB::bind_method(D_METHOD("grant_ability", "ability"), &AbilitySystemComponent::grant_ability);
	ClassDB::bind_method(D_METHOD("try_activate_ability", "tag_name"), &AbilitySystemComponent::try_activate_ability);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "attribute_set", PROPERTY_HINT_RESOURCE_TYPE, "AttributeSet"), "set_attribute_set", "get_attribute_set");
}

void AbilitySystemComponent::set_attribute_set(const Ref<AttributeSet> &p_set) {
	attribute_set = p_set;
}

void AbilitySystemComponent::grant_ability(const Ref<GameplayAbility> &p_ability) {
	if (p_ability.is_null()) {
		return;
	}
	granted_abilities.push_back(p_ability);
	_update_inverted_index();
}

void AbilitySystemComponent::remove_ability(const Ref<GameplayAbility> &p_ability) {
	granted_abilities.erase(p_ability);
	_update_inverted_index();
}

void AbilitySystemComponent::_update_inverted_index() {
	tag_to_ability_map.clear();
	for (const Ref<GameplayAbility> &ability : granted_abilities) {
		if (ability.is_null()) {
			continue;
		}
		for (const GameplayTag &tag : ability->get_ability_tags()->get_all_tags()) {
			if (!tag_to_ability_map.has(tag.id)) {
				tag_to_ability_map[tag.id] = Vector<Ref<GameplayAbility>>();
			}
			tag_to_ability_map[tag.id].push_back(ability);
		}
	}
}

bool AbilitySystemComponent::try_activate_ability(const StringName &p_tag_name) {
	GameplayTag tag = GameplayTagManager::get_singleton()->get_tag(p_tag_name);
	return try_activate_ability_by_tag(tag);
}

bool AbilitySystemComponent::try_activate_ability_by_tag(const GameplayTag &p_tag) {
	if (!p_tag.is_valid() || !tag_to_ability_map.has(p_tag.id)) {
		return false;
	}

	const Vector<Ref<GameplayAbility>> &abilities = tag_to_ability_map[p_tag.id];
	bool triggered = false;
	for (const Ref<GameplayAbility> &ability : abilities) {
		if (ability->can_activate()) {
			ability->activate();
			triggered = true;
		}
	}
	return triggered;
}

void AbilitySystemComponent::apply_gameplay_effect(const Ref<GameplayEffect> &p_effect) {
	if (p_effect.is_null() || attribute_set.is_null()) {
		return;
	}

	// Simple Instant implementation for now
	if (p_effect->get_duration_policy() == GameplayEffect::DURATION_INSTANT) {
		for (const GameplayEffect::Modifier &m : p_effect->get_modifiers()) {
			float val = attribute_set->get_attribute_current(m.attribute);
			switch (m.op) {
				case GameplayEffect::MODIFIER_ADD:
					val += m.magnitude;
					break;
				case GameplayEffect::MODIFIER_MULTIPLY:
					val *= m.magnitude;
					break;
				case GameplayEffect::MODIFIER_OVERRIDE:
					val = m.magnitude;
					break;
			}
			attribute_set->set_attribute_current(m.attribute, val);
		}
	}
}

AbilitySystemComponent::AbilitySystemComponent() {
	active_tags.instantiate();
}
