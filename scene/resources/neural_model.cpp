/**************************************************************************/
/*  neural_model.cpp                                                      */
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

#include "neural_model.h"

void NeuralModel::set_model_data(const PackedByteArray &p_data) {
	model_data = p_data;
}

PackedByteArray NeuralModel::get_model_data() const {
	return model_data;
}

void NeuralModel::set_model_format(const String &p_format) {
	model_format = p_format;
}

String NeuralModel::get_model_format() const {
	return model_format;
}

void NeuralModel::set_metadata(const Dictionary &p_metadata) {
	metadata = p_metadata;
}

Dictionary NeuralModel::get_metadata() const {
	return metadata;
}

void NeuralModel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_model_data", "data"), &NeuralModel::set_model_data);
	ClassDB::bind_method(D_METHOD("get_model_data"), &NeuralModel::get_model_data);
	ClassDB::bind_method(D_METHOD("set_model_format", "format"), &NeuralModel::set_model_format);
	ClassDB::bind_method(D_METHOD("get_model_format"), &NeuralModel::get_model_format);
	ClassDB::bind_method(D_METHOD("set_metadata", "metadata"), &NeuralModel::set_metadata);
	ClassDB::bind_method(D_METHOD("get_metadata"), &NeuralModel::get_metadata);

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "model_data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_model_data", "get_model_data");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "model_format"), "set_model_format", "get_model_format");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "metadata"), "set_metadata", "get_metadata");
}

NeuralModel::NeuralModel() {
}
