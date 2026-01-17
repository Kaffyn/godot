/**************************************************************************/
/*  resource_server.h                                                     */
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

#include "core/object/object.h"
#include "scene/resources/texture.h"

class ResourceServer : public Object {
	GDCLASS(ResourceServer, Object);

	static ResourceServer *singleton;

	// Domain Definition
	struct DomainDef {
		String name;
		String resource_type;
		String visual_class_name;
		Ref<Texture2D> icon;
		Dictionary rules;
	};

	// Maps "DomainName" -> DomainDef
	HashMap<String, DomainDef> domains;

protected:
	static void _bind_methods();

public:
	static ResourceServer *get_singleton();

	// Domain Injection API
	void register_domain(const String &p_name, const String &p_resource_type, const String &p_visual_class_name, const Ref<Texture2D> &p_icon, const Dictionary &p_rules = Dictionary());
	Dictionary get_domain_info(const String &p_name) const;
	Array get_registered_domains() const;

	// Helper to find domain by resource type
	String get_domain_for_resource(const String &p_resource_type) const;

	ResourceServer();
	~ResourceServer();
};
