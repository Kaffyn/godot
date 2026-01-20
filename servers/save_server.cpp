/**************************************************************************/
/*  save_server.cpp                                                       */
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

#include "save_server.h"
#include "core/io/dir_access.h"
#include "core/io/resource_saver.h"

SaveServer *SaveServer::singleton = nullptr;

SaveServer *SaveServer::get_singleton() {
	return singleton;
}

void SaveServer::set_current_data(const Ref<SaveData> &p_data) {
	current_data = p_data;
}

Ref<SaveData> SaveServer::get_current_data() const {
	return current_data;
}

void SaveServer::set_encryption_password(const String &p_password) {
	encryption_password = p_password;
}

String SaveServer::get_encryption_password() const {
	return encryption_password;
}

void SaveServer::set_use_encryption(bool p_enable) {
	use_encryption = p_enable;
}

bool SaveServer::is_using_encryption() const {
	return use_encryption;
}

String SaveServer::get_save_path(const String &p_slot) const {
	return save_dir.path_join(p_slot + ".tres");
}

Error SaveServer::save_game(const String &p_slot) {
	ERR_FAIL_COND_V_MSG(current_data.is_null(), ERR_INVALID_DATA, "No save data to save.");

	if (!DirAccess::exists(save_dir)) {
		DirAccess::make_dir_recursive_absolute(save_dir);
	}

	String path = get_save_path(p_slot);
	Error err = ResourceSaver::save(current_data, path);

	if (err != OK) {
		ERR_PRINT("Failed to save game to: " + path);
	}

	return err;
}

Error SaveServer::load_game(const String &p_slot) {
	String path = get_save_path(p_slot);

	if (!FileAccess::exists(path)) {
		return ERR_FILE_NOT_FOUND;
	}

	Ref<SaveData> loaded_data = ResourceLoader::load(path);
	if (loaded_data.is_null()) {
		ERR_PRINT("Failed to load game from: " + path);
		return ERR_CANT_OPEN;
	}

	current_data = loaded_data;
	return OK;
}

void SaveServer::save_game_async(const String &p_slot) {
	ERR_FAIL_COND_MSG(current_data.is_null(), "No save data to save.");

	SaveTaskArgs *args = memnew(SaveTaskArgs);
	args->data = current_data;
	args->path = get_save_path(p_slot);
	args->password = encryption_password;
	args->encrypt = use_encryption;

	WorkerThreadPool::get_singleton()->add_native_task(&SaveServer::_save_task, args, true);
}

void SaveServer::_save_task(void *p_userdata) {
	SaveTaskArgs *args = (SaveTaskArgs *)p_userdata;

	if (!DirAccess::dir_exists_absolute(args->path.get_base_dir())) {
		DirAccess::make_dir_recursive_absolute(args->path.get_base_dir());
	}

	// For now, simple save. Encryption logic would go here by saving to buffer and then encrypted file.
	// Future phase will implement full AES-256 binary serialization.
	ResourceSaver::save(args->data, args->path);

	memdelete(args);
}

void SaveServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_current_data", "data"), &SaveServer::set_current_data);
	ClassDB::bind_method(D_METHOD("get_current_data"), &SaveServer::get_current_data);

	ClassDB::bind_method(D_METHOD("set_encryption_password", "password"), &SaveServer::set_encryption_password);
	ClassDB::bind_method(D_METHOD("get_encryption_password"), &SaveServer::get_encryption_password);

	ClassDB::bind_method(D_METHOD("set_use_encryption", "enable"), &SaveServer::set_use_encryption);
	ClassDB::bind_method(D_METHOD("is_using_encryption"), &SaveServer::is_using_encryption);

	ClassDB::bind_method(D_METHOD("save_game", "slot"), &SaveServer::save_game);
	ClassDB::bind_method(D_METHOD("load_game", "slot"), &SaveServer::load_game);
	ClassDB::bind_method(D_METHOD("save_game_async", "slot"), &SaveServer::save_game_async);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_data", PROPERTY_HINT_RESOURCE_TYPE, "SaveData"), "set_current_data", "get_current_data");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "encryption_password"), "set_encryption_password", "get_encryption_password");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_encryption"), "set_use_encryption", "is_using_encryption");
}

SaveServer::SaveServer() {
	singleton = this;
}

SaveServer::~SaveServer() {
	singleton = nullptr;
}
