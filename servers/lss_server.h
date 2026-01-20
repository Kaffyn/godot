/**************************************************************************/
/*  lss_server.h                                                          */
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

#include "core/variant/dictionary.h"
#include "scene/resources/universe_state.h"

class LSSServer : public Object {
	GDCLASS(LSSServer, Object);

public:
	enum GameState {
		STATE_BOOT,
		STATE_TITLE,
		STATE_GAMEPLAY,
		STATE_LOADING,
		STATE_PAUSE
	};

private:
	static LSSServer *singleton;

	GameState current_state = STATE_BOOT;
	Ref<UniverseState> current_universe;

	String loading_universe_path;
	bool is_loading = false;

protected:
	static void _bind_methods();

public:
	static LSSServer *get_singleton();

	void change_state(GameState p_new_state, const Dictionary &p_params = Dictionary());
	GameState get_current_state() const;

	void set_universe_state(const Ref<UniverseState> &p_universe);
	Ref<UniverseState> get_universe_state() const;

	void load_universe(const String &p_path);
	float get_loading_progress() const;

	void request_save(const String &p_slot);
	void update(); // To be called from LSSRoot or MainLoop

	LSSServer();
	~LSSServer();
};

VARIANT_ENUM_CAST(LSSServer::GameState);

#define LSS LSSServer::get_singleton()
