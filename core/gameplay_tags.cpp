/**************************************************************************/
/*  gameplay_tags.cpp                                                     */
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

#include "gameplay_tags.h"

// GameplayTag

String GameplayTag::get_tag_name() const {
	if (id == 0) {
		return "";
	}
	return GameplayTagManager::get_singleton()->get_tag_name(id);
}

bool GameplayTag::matches(const GameplayTag &p_other) const {
	if (id == p_other.id) {
		return true;
	}
	return GameplayTagManager::get_singleton()->is_child_of(id, p_other.id);
}

bool GameplayTag::matches_any(const Vector<GameplayTag> &p_container) const {
	for (const GameplayTag &tag : p_container) {
		if (matches(tag)) {
			return true;
		}
	}
	return false;
}

// GameplayTagManager

GameplayTagManager *GameplayTagManager::singleton = nullptr;

void GameplayTagManager::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("register_tag", "name"), &GameplayTagManager::register_tag); // Cannot bind return type GameplayTag
	// ClassDB::bind_method(D_METHOD("get_tag", "name"), &GameplayTagManager::get_tag); // Cannot bind return type GameplayTag
	ClassDB::bind_method(D_METHOD("get_tag_name", "id"), &GameplayTagManager::get_tag_name);
}

GameplayTag GameplayTagManager::register_tag(const StringName &p_name) {
	if (name_to_id.has(p_name)) {
		return GameplayTag(name_to_id[p_name]);
	}

	last_id++;
	uint32_t new_id = last_id;
	name_to_id[p_name] = new_id;

	TagNode node;
	node.name = p_name;
	node.id = new_id;

	// Handle Hierarchy (A.B.C)
	String full_name = String(p_name);
	int last_dot = full_name.rfind(".");
	if (last_dot != -1) {
		StringName parent_name = full_name.substr(0, last_dot);
		GameplayTag parent_tag = register_tag(parent_name);
		node.parent_id = parent_tag.id;
		tags[parent_tag.id].children.push_back(new_id);
	}

	tags[new_id] = node;
	return GameplayTag(new_id);
}

GameplayTag GameplayTagManager::get_tag(const StringName &p_name) const {
	if (name_to_id.has(p_name)) {
		return GameplayTag(name_to_id[p_name]);
	}
	return GameplayTag(0);
}

StringName GameplayTagManager::get_tag_name(uint32_t p_id) const {
	if (tags.has(p_id)) {
		return tags[p_id].name;
	}
	return StringName();
}

bool GameplayTagManager::is_child_of(uint32_t p_tag_id, uint32_t p_parent_id) const {
	if (p_tag_id == 0 || p_parent_id == 0) {
		return false;
	}
	uint32_t current = p_tag_id;
	while (current != 0) {
		if (tags.has(current)) {
			current = tags[current].parent_id;
			if (current == p_parent_id) {
				return true;
			}
		} else {
			break;
		}
	}
	return false;
}

GameplayTagManager::GameplayTagManager() {
	singleton = this;
}

GameplayTagManager::~GameplayTagManager() {
	singleton = nullptr;
}

// GameplayTagContainer

void GameplayTagContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_tag", "name"), &GameplayTagContainer::add_tag_name);
	ClassDB::bind_method(D_METHOD("remove_tag", "name"), &GameplayTagContainer::remove_tag_name);
	ClassDB::bind_method(D_METHOD("has_tag", "name"), &GameplayTagContainer::has_tag_name);
}

void GameplayTagContainer::add_tag(const GameplayTag &p_tag) {
	if (p_tag.is_valid() && !tags.has(p_tag)) {
		tags.push_back(p_tag);
	}
}

void GameplayTagContainer::add_tag_name(const StringName &p_name) {
	GameplayTag tag = GameplayTagManager::get_singleton()->get_tag(p_name);
	add_tag(tag);
}

void GameplayTagContainer::remove_tag(const GameplayTag &p_tag) {
	tags.erase(p_tag);
}

void GameplayTagContainer::remove_tag_name(const StringName &p_name) {
	GameplayTag tag = GameplayTagManager::get_singleton()->get_tag(p_name);
	remove_tag(tag);
}

bool GameplayTagContainer::has_tag(const GameplayTag &p_tag) const {
	for (const GameplayTag &t : tags) {
		if (t.matches(p_tag)) {
			return true;
		}
	}
	return false;
}

bool GameplayTagContainer::has_tag_name(const StringName &p_name) const {
	GameplayTag tag = GameplayTagManager::get_singleton()->get_tag(p_name);
	return has_tag(tag);
}

bool GameplayTagContainer::has_any(const Ref<GameplayTagContainer> &p_other) const {
	if (p_other.is_null()) {
		return false;
	}
	for (const GameplayTag &t : p_other->get_all_tags()) {
		if (has_tag(t)) {
			return true;
		}
	}
	return false;
}

bool GameplayTagContainer::has_all(const Ref<GameplayTagContainer> &p_other) const {
	if (p_other.is_null()) {
		return true;
	}
	for (const GameplayTag &t : p_other->get_all_tags()) {
		if (!has_tag(t)) {
			return false;
		}
	}
	return true;
}
