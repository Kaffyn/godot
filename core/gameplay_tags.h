/**************************************************************************/
/*  gameplay_tags.h                                                       */
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

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/string/ustring.h"
#include "core/string/string_name.h"
#include "core/templates/hash_map.h"
#include "core/templates/vector.h"

struct GameplayTag {
	uint32_t id = 0;

	_FORCE_INLINE_ bool is_valid() const { return id != 0; }
	_FORCE_INLINE_ bool operator==(const GameplayTag &p_other) const { return id == p_other.id; }
	_FORCE_INLINE_ bool operator!=(const GameplayTag &p_other) const { return id != p_other.id; }
	_FORCE_INLINE_ bool operator<(const GameplayTag &p_other) const { return id < p_other.id; }

	String get_tag_name() const;
	bool matches(const GameplayTag &p_other) const;
	bool matches_any(const Vector<GameplayTag> &p_container) const;

	GameplayTag() {}
	GameplayTag(uint32_t p_id) :
			id(p_id) {}
};

class GameplayTagManager : public Object {
	GDCLASS(GameplayTagManager, Object);

	static GameplayTagManager *singleton;

	struct TagNode {
		StringName name;
		uint32_t id;
		uint32_t parent_id = 0;
		Vector<uint32_t> children;
	};

	HashMap<StringName, uint32_t> name_to_id;
	HashMap<uint32_t, TagNode> tags;
	uint32_t last_id = 0;

protected:
	static void _bind_methods();

public:
	static GameplayTagManager *get_singleton() { return singleton; }

	GameplayTag register_tag(const StringName &p_name);
	GameplayTag get_tag(const StringName &p_name) const;
	StringName get_tag_name(uint32_t p_id) const;

	bool is_child_of(uint32_t p_tag_id, uint32_t p_parent_id) const;

	GameplayTagManager();
	~GameplayTagManager();
};

// Helper for bitmask or container operations if needed
class GameplayTagContainer : public RefCounted {
	GDCLASS(GameplayTagContainer, RefCounted);

	Vector<GameplayTag> tags;

protected:
	static void _bind_methods();

public:
	void add_tag(const GameplayTag &p_tag);
	void add_tag_name(const StringName &p_name);
	void remove_tag(const GameplayTag &p_tag);
	void remove_tag_name(const StringName &p_name);

	bool has_tag(const GameplayTag &p_tag) const;
	bool has_tag_name(const StringName &p_name) const;

	bool has_any(const Ref<GameplayTagContainer> &p_other) const;
	bool has_all(const Ref<GameplayTagContainer> &p_other) const;

	Vector<GameplayTag> get_all_tags() const { return tags; }

	GameplayTagContainer() {}
};
