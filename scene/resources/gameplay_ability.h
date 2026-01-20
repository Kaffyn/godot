/**************************************************************************/
/*  gameplay_ability.h                                                    */
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

#include "core/gameplay_tags.h"
#include "core/io/resource.h"

class GameplayAbility : public Resource {
	GDCLASS(GameplayAbility, Resource);

	Ref<GameplayTagContainer> ability_tags;
	Ref<GameplayTagContainer> cancel_tags;
	Ref<GameplayTagContainer> block_tags;
	Ref<GameplayTagContainer> activation_required_tags;
	Ref<GameplayTagContainer> activation_blocked_tags;

protected:
	static void _bind_methods();

public:
	GDVIRTUAL0(_activate_ability)
	GDVIRTUAL0RC(bool, _can_activate_ability)

	virtual bool can_activate() const;
	virtual void activate();
	virtual void commit();
	virtual void end();

	Ref<GameplayTagContainer> get_ability_tags() const { return ability_tags; }
	Ref<GameplayTagContainer> get_cancel_tags() const { return cancel_tags; }
	Ref<GameplayTagContainer> get_block_tags() const { return block_tags; }

	GameplayAbility();
};
