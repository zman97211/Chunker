#include "nbt.h"

using namespace nbt;

tag_ptr dispatch_parse(const std::vector<uint8_t> data, int& position, const uint8_t tag_type, bool suppress_name = false);

std::string parse_string(const std::vector<uint8_t> data, int& position) {
	uint16_t string_length = (data[position] << 8) + data[position + 1];
	position += 2;
	std::string s;
	for (int i = 0; i < string_length; i++)
		s.push_back(data[position++]);
	return s;
}

tag_ptr parse_tag_byte(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<int8_t> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	tag.value = data[position++];
	return std::make_shared<nbt::tag<int8_t>>(tag);
}

tag_ptr parse_tag_short(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<int16_t> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	tag.value = (data[position] << 8) + data[position + 1];
	position += 2;
	return std::make_shared<nbt::tag<int16_t>>(tag);
}

tag_ptr parse_tag_int(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<int32_t> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	tag.value = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + (data[position + 3]);
	position += 4;
	return std::make_shared<nbt::tag<int32_t>>(tag);
}

tag_ptr parse_tag_long(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<int64_t> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	tag.value = (static_cast<int64_t>(data[position]) << 56) + (static_cast<int64_t>(data[position + 1]) << 48) + (static_cast<int64_t>(data[position + 2]) << 40) + (static_cast<int64_t>(data[position + 3]) << 32)
		+ (static_cast<int64_t>(data[position + 4]) << 24) + (static_cast<int64_t>(data[position + 5]) << 16) + (static_cast<int64_t>(data[position + 6]) << 8) + static_cast<int64_t>(data[position + 7]);
	position += 8;
	return std::make_shared<nbt::tag<int64_t>>(tag);
}

tag_ptr parse_tag_float(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<float> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	uint32_t tmp = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	tag.value = *reinterpret_cast<float*>(&tmp);
	position += 4;
	return std::make_shared<nbt::tag<float>>(tag);
}

tag_ptr parse_tag_double(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<double> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);

	uint64_t tmp = (static_cast<uint64_t>(data[position]) << 56) + (static_cast<uint64_t>(data[position + 1]) << 48) + (static_cast<uint64_t>(data[position + 2]) << 40) + (static_cast<uint64_t>(data[position + 3]) << 32)
		+ (static_cast<uint64_t>(data[position + 4]) << 24) + (static_cast<uint64_t>(data[position + 5]) << 16) + (static_cast<uint64_t>(data[position + 6]) << 8) + static_cast<uint64_t>(data[position + 7]);
	tag.value = *reinterpret_cast<double*>(&tmp);

	position += 8;
	return std::make_shared<nbt::tag<double>>(tag);
}

tag_ptr parse_tag_byte_array(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag_byte_array tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	int32_t length = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	position += 4;
	for (int i = 0; i < length; i++)
		tag.data.push_back(data[position++]);
	return std::make_shared<tag_byte_array>(tag);
}

tag_ptr parse_tag_int_array(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag_int_array tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	int32_t length = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	position += 4;
	for (int i = 0; i < length; i++) {
		int32_t value = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + (data[position + 3]);
		tag.data.push_back(value);
		position += 4;
	}
	return std::make_shared<tag_int_array>(tag);
}

tag_ptr parse_tag_string(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag<std::string> tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	tag.value = parse_string(data, position);
	return std::make_shared<nbt::tag<std::string>>(tag);
}

tag_ptr parse_tag_list(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag_list tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);
	uint8_t list_tag_type = data[position++];
	int32_t length = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	position += 4;
	for (int i = 0; i < length; i++)
		tag.items.push_back(dispatch_parse(data, position, list_tag_type, true));
	return std::make_shared<tag_list>(tag);
}

tag_ptr parse_tag_compound(const std::vector<uint8_t> data, int& position, bool suppress_name = false) {
	tag_compound tag;
	if (!suppress_name)
		tag.name = parse_string(data, position);

	uint8_t next_tag_type = 0;
	do {
		next_tag_type = data[position++];
		if (next_tag_type != 0) // not at end of compound tag
			tag.items.push_back(dispatch_parse(data, position, next_tag_type));
	} while (next_tag_type != 0);

	return std::make_shared<tag_compound>(tag);
}

tag_ptr dispatch_parse(const std::vector<uint8_t> data, int& position, const uint8_t tag_type, bool suppress_name) {
	switch (tag_type) {
	case 0:
		return std::make_shared<tag_end>(tag_end{});
		break;
	case 1:
		return parse_tag_byte(data, position, suppress_name);
		break;
	case 2:
		return parse_tag_short(data, position, suppress_name);
		break;
	case 3:
		return parse_tag_int(data, position, suppress_name);
		break;
	case 4:
		return parse_tag_long(data, position, suppress_name);
		break;
	case 5:
		return parse_tag_float(data, position, suppress_name);
		break;
	case 6:
		return parse_tag_double(data, position, suppress_name);
		break;
	case 7:
		return parse_tag_byte_array(data, position, suppress_name);
		break;
	case 8:
		return parse_tag_string(data, position, suppress_name);
		break;
	case 9:
		return parse_tag_list(data, position, suppress_name);
		break;
	case 10:
		return parse_tag_compound(data, position, suppress_name);
		break;
	case 11:
		return parse_tag_int_array(data, position, suppress_name);
		break;
	default:
		throw std::runtime_error{ "Encountered an unknown NBT tag type." };
		break;
	}
}

tag_ptr nbt::parse_nbt(const std::vector<uint8_t> data) {
	if (data[0] != 10)
		throw std::runtime_error{ "Did not see a compound chunk tag when it was expected." };

	int position = 1;
	return parse_tag_compound(data, position);
}

void nbt::tag_end::accept(tag_visitor* v) {
	v->visit(this);
}

template <typename T>
void nbt::tag<T>::accept(tag_visitor* v) {
	v->visit(this);
}

void nbt::tag_byte_array::accept(tag_visitor* v) {
	v->visit(this);
}

void nbt::tag_int_array::accept(tag_visitor* v) {
	v->visit(this);
}

void nbt::tag_list::accept(tag_visitor* v) {
	v->visit(this);
}

void nbt::tag_compound::accept(tag_visitor* v) {
	v->visit(this);
}

