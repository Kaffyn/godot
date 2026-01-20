/**************************************************************************/
/*  lss_server.cpp                                                        */
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

#include "lss_server.h"
#include "core/config/engine.h"
#include "core/io/resource_loader.h"
#include "save_server.h"
#include "scene/resources/universe_state.h"

LSSServer *LSSServer::singleton = nullptr;

LSSServer *LSSServer::get_singleton() {
	return singleton;
}

void LSSServer::change_state(GameState p_new_state, const Dictionary &p_params) {
	if (current_state == p_new_state) {
		return;
	}

	current_state = p_new_state;
	emit_signal("state_changed", current_state, p_params);
}

LSSServer::GameState LSSServer::get_current_state() const {
	return current_state;
}

void LSSServer::set_universe_state(const Ref<UniverseState> &p_universe) {
	current_universe = p_universe;
}

Ref<UniverseState> LSSServer::get_universe_state() const {
	return current_universe;
}

void LSSServer::load_universe(const String &p_path) {
	ERR_FAIL_COND_MSG(is_loading, "Already loading a universe.");

	// Coordinate with SaveServer if needed before transition
	if (SaveServer::get_singleton()) {
		// SaveServer::get_singleton()->save_game_async("auto_transition_backup");
	}

	loading_universe_path = p_path;
	is_loading = true;

	change_state(STATE_LOADING);

	Error err = ResourceLoader::load_threaded_request(p_path, "UniverseState");
	if (err != OK) {
		ERR_PRINT("Failed to start threaded load for universe: " + p_path);
		is_loading = false;
		change_state(STATE_TITLE); // Revert or handle error
	}
}

void LSSServer::request_save(const String &p_slot) {
	if (SaveServer::get_singleton()) {
		SaveServer::get_singleton()->save_game_async(p_slot);
	}
}

float LSSServer::get_loading_progress() const {
	if (!is_loading) {
		return 1.0f;
	}
	float progress = 0.0f;
	ResourceLoader::load_threaded_get_status(loading_universe_path, &progress);
	return progress;
}

void LSSServer::update() {
	if (!is_loading) {
		return;
	}

	float progress = 0.0f;
	ResourceLoader::ThreadLoadStatus status = ResourceLoader::load_threaded_get_status(loading_universe_path, &progress);

	switch (status) {
		case ResourceLoader::THREAD_LOAD_LOADED: {
			Ref<UniverseState> loaded_universe = ResourceLoader::load_threaded_get(loading_universe_path);
			set_universe_state(loaded_universe);
			is_loading = false;
			loading_universe_path = "";
			change_state(STATE_GAMEPLAY);
		} break;
		case ResourceLoader::THREAD_LOAD_FAILED:
		case ResourceLoader::THREAD_LOAD_INVALID_RESOURCE: {
			ERR_PRINT("Failed to load universe: " + loading_universe_path);
			is_loading = false;
			loading_universe_path = "";
			change_state(STATE_TITLE);
		} break;
		default:
			break;
	}
}

void LSSServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("change_state", "new_state", "params"), &LSSServer::change_state, DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("get_current_state"), &LSSServer::get_current_state);

	ClassDB::bind_method(D_METHOD("set_universe_state", "universe"), &LSSServer::set_universe_state);
	ClassDB::bind_method(D_METHOD("get_universe_state"), &LSSServer::get_universe_state);

	ClassDB::bind_method(D_METHOD("load_universe", "path"), &LSSServer::load_universe);
	ClassDB::bind_method(D_METHOD("get_loading_progress"), &LSSServer::get_loading_progress);
	ClassDB::bind_method(D_METHOD("request_save", "slot"), &LSSServer::request_save);
	ClassDB::bind_method(D_METHOD("update"), &LSSServer::update);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "universe_state", PROPERTY_HINT_RESOURCE_TYPE, "UniverseState"), "set_universe_state", "get_universe_state");

	ADD_SIGNAL(MethodInfo("state_changed", PropertyInfo(Variant::INT, "new_state"), PropertyInfo(Variant::DICTIONARY, "params")));

	BIND_ENUM_CONSTANT(STATE_BOOT);
	BIND_ENUM_CONSTANT(STATE_TITLE);
	BIND_ENUM_CONSTANT(STATE_GAMEPLAY);
	BIND_ENUM_CONSTANT(STATE_LOADING);
	BIND_ENUM_CONSTANT(STATE_PAUSE);
}

LSSServer::LSSServer() {
	singleton = this;
}

LSSServer::~LSSServer() {
	singleton = nullptr;
}
