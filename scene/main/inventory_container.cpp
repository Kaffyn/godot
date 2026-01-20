/**************************************************************************/
/*  inventory_container.cpp                                               */
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

#include "inventory_container.h"

void InventoryContainer::set_capacity(int p_capacity) {
	capacity = p_capacity;
	slots.resize(capacity);
}

int InventoryContainer::get_capacity() const {
	return capacity;
}

Error InventoryContainer::add_item(Ref<ItemResource> p_item, int p_amount) {
	// Simple implementation
	for (int i = 0; i < slots.size(); i++) {
		if (slots[i].item.is_null()) {
			slots.write[i].item = p_item;
			slots.write[i].amount = p_amount;
			return OK;
		} else if (slots[i].item == p_item) {
			slots.write[i].amount += p_amount;
			return OK;
		}
	}
	return ERR_OUT_OF_MEMORY;
}

Error InventoryContainer::remove_item(Ref<ItemResource> p_item, int p_amount) {
	for (int i = 0; i < slots.size(); i++) {
		if (slots[i].item == p_item) {
			if (slots[i].amount >= p_amount) {
				slots.write[i].amount -= p_amount;
				if (slots[i].amount == 0) {
					slots.write[i].item = Ref<ItemResource>();
				}
				return OK;
			}
		}
	}
	return ERR_INVALID_DATA;
}

void InventoryContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_capacity", "capacity"), &InventoryContainer::set_capacity);
	ClassDB::bind_method(D_METHOD("get_capacity"), &InventoryContainer::get_capacity);
	ClassDB::bind_method(D_METHOD("add_item", "item", "amount"), &InventoryContainer::add_item, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("remove_item", "item", "amount"), &InventoryContainer::remove_item, DEFVAL(1));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "capacity"), "set_capacity", "get_capacity");
}

InventoryContainer::InventoryContainer() {
	slots.resize(capacity);
}
