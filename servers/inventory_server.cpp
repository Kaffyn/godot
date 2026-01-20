/**************************************************************************/
/*  inventory_server.cpp                                                  */
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

#include "inventory_server.h"

InventoryServer *InventoryServer::singleton = nullptr;

InventoryServer *InventoryServer::get_singleton() {
	return singleton;
}

bool InventoryServer::validate_transaction(Object *p_from, Object *p_to, Ref<ItemResource> p_item, int p_amount) {
	// Logic to validate if item can be moved (weight, space, etc.)
	return true;
}

void InventoryServer::execute_transaction(Object *p_from, Object *p_to, Ref<ItemResource> p_item, int p_amount) {
	if (validate_transaction(p_from, p_to, p_item, p_amount)) {
		// Logic to remove from p_from and add to p_to
	}
}

void InventoryServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("validate_transaction", "from", "to", "item", "amount"), &InventoryServer::validate_transaction);
	ClassDB::bind_method(D_METHOD("execute_transaction", "from", "to", "item", "amount"), &InventoryServer::execute_transaction);
}

InventoryServer::InventoryServer() {
	singleton = this;
}

InventoryServer::~InventoryServer() {
	singleton = nullptr;
}
