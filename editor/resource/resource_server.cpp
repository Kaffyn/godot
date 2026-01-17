/**************************************************************************/
/*  resource_server.cpp                                                   */
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

#include "resource_server.h"

ResourceServer *ResourceServer::singleton = nullptr;

ResourceServer *ResourceServer::get_singleton() {
	return singleton;
}

void ResourceServer::register_domain(const String &p_name, const String &p_resource_type, const String &p_visual_class_name, const Ref<Texture2D> &p_icon, const Dictionary &p_rules) {
	if (domains.has(p_name)) {
		WARN_PRINT("ResourceServer: overwriting existing domain: " + p_name);
	}
	DomainDef def;
	def.name = p_name;
	def.resource_type = p_resource_type;
	def.visual_class_name = p_visual_class_name;
	def.icon = p_icon;
	def.rules = p_rules;

	domains[p_name] = def;
}

Dictionary ResourceServer::get_domain_info(const String &p_name) const {
	if (!domains.has(p_name)) {
		return Dictionary();
	}
	const DomainDef &def = domains[p_name];
	Dictionary dict;
	dict["name"] = def.name;
	dict["resource_type"] = def.resource_type;
	dict["visual_class_name"] = def.visual_class_name;
	dict["icon"] = def.icon;
	dict["rules"] = def.rules;
	return dict;
}

Array ResourceServer::get_registered_domains() const {
	Array arr;
	for (const KeyValue<String, DomainDef> &E : domains) {
		arr.push_back(E.key);
	}
	return arr;
}

String ResourceServer::get_domain_for_resource(const String &p_resource_type) const {
	// Simple matching: exact type or inheritance (not checked here for simplicity, but could use ClassDB)
	for (const KeyValue<String, DomainDef> &E : domains) {
		if (E.value.resource_type == p_resource_type || ClassDB::is_parent_class(p_resource_type, E.value.resource_type)) {
			return E.key;
		}
	}
	return String();
}

void ResourceServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_domain", "name", "resource_type", "visual_class_name", "icon", "rules"), &ResourceServer::register_domain, DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("get_domain_info", "name"), &ResourceServer::get_domain_info);
	ClassDB::bind_method(D_METHOD("get_registered_domains"), &ResourceServer::get_registered_domains);
	ClassDB::bind_method(D_METHOD("get_domain_for_resource", "resource_type"), &ResourceServer::get_domain_for_resource);
}

ResourceServer::ResourceServer() {
	singleton = this;
}

ResourceServer::~ResourceServer() {
	if (singleton == this) {
		singleton = nullptr;
	}
}
