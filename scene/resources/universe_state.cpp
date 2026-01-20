/**************************************************************************/
/*  universe_state.cpp                                                    */
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

#include "universe_state.h"

void UniverseState::set_world_name(const String &p_name) {
	world_name = p_name;
}

String UniverseState::get_world_name() const {
	return world_name;
}

void UniverseState::set_initial_map_path(const String &p_path) {
	initial_map_path = p_path;
}

String UniverseState::get_initial_map_path() const {
	return initial_map_path;
}

void UniverseState::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_world_name", "name"), &UniverseState::set_world_name);
	ClassDB::bind_method(D_METHOD("get_world_name"), &UniverseState::get_world_name);

	ClassDB::bind_method(D_METHOD("set_initial_map_path", "path"), &UniverseState::set_initial_map_path);
	ClassDB::bind_method(D_METHOD("get_initial_map_path"), &UniverseState::get_initial_map_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "world_name"), "set_world_name", "get_world_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "initial_map_path", PROPERTY_HINT_FILE, "*.tscn"), "set_initial_map_path", "get_initial_map_path");
}

UniverseState::UniverseState() {
}
