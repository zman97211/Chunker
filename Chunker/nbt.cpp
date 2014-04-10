#include "nbt.h"

using namespace nbt;

tag_ptr dispatch_parse(const std::vector<uint8_t> data, int& position, const uint8_t tag_type);

std::string parse_string(const std::vector<uint8_t> data, int& position) {
	uint16_t string_length = (data[position] << 8) + data[position + 1];
	position += 2;
	std::string s;
	for (int i = 0; i < string_length; i++)
		s.push_back(data[position++]);
	return s;
}

tag_ptr parse_tag_byte(const std::vector<uint8_t> data, int& position) {
	position++;
	tag<uint8_t> tag;
	tag.name = parse_string(data, position);
	tag.value = data[position++];
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_short(const std::vector<uint8_t> data, int& position) {
	position++;
	tag<uint16_t> tag;
	tag.name = parse_string(data, position);
	tag.value = (data[position] << 8) + data[position + 1];
	position += 2;
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_int(const std::vector<uint8_t> data, int& position) {
	position++;
	tag<uint32_t> tag;
	tag.name = parse_string(data, position);
	tag.value = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + (data[position + 3]);
	position += 4;
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_long(const std::vector<uint8_t> data, int& position) {
	position++;
	tag<uint64_t> tag;
	tag.name = parse_string(data, position);
	tag.value = (static_cast<uint64_t>(data[position]) << 56) + (static_cast<uint64_t>(data[position + 1]) << 48) + (static_cast<uint64_t>(data[position + 2]) << 40) + (static_cast<uint64_t>(data[position + 3]) << 32)
		+ (static_cast<uint64_t>(data[position]) << 24) + (static_cast<uint64_t>(data[position + 1]) << 16) + (static_cast<uint64_t>(data[position + 2]) << 8) + static_cast<uint64_t>(data[position + 3]);
	position += 8;
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_float(const std::vector<uint8_t> data, int& position) {
	position++;
	tag<float> tag;
	tag.name = parse_string(data, position);
	uint32_t tmp = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	tag.value = static_cast<float>(tmp);
	position += 4;
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_double(const std::vector<uint8_t> data, int& position) {
	position++;
	tag<double> tag;
	tag.name = parse_string(data, position);
	uint64_t tmp = (static_cast<uint64_t>(data[position]) << 56) + (static_cast<uint64_t>(data[position + 1]) << 48) + (static_cast<uint64_t>(data[position + 2]) << 40) + (static_cast<uint64_t>(data[position + 3]) << 32)
		+ (static_cast<uint64_t>(data[position]) << 24) + (static_cast<uint64_t>(data[position + 1]) << 16) + (static_cast<uint64_t>(data[position + 2]) << 8) + static_cast<uint64_t>(data[position + 3]);
	tag.value = static_cast<double>(tmp);
	position += 8;
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_byte_array(const std::vector<uint8_t> data, int& position) {
	uint32_t length = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	position += 4;
	tag_byte_array tag;
	tag.name = parse_string(data, position);
	for (unsigned int i = 0; i < length; i++)
		tag.data.push_back(data[position++]);
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_string(const std::vector<uint8_t> data, int& position) {
	tag<std::string> tag;
	tag.name = parse_string(data, position);
	tag.value = parse_string(data, position);
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_list(const std::vector<uint8_t> data, int& position) {
	tag_list tag;
	tag.name = parse_string(data, position);
	uint8_t list_tag_type = data[position++];
	uint32_t length = (data[position] << 24) + (data[position + 1] << 16) + (data[position + 2] << 8) + data[position + 3];
	position += 4;
	tag.name = parse_string(data, position);
	for (unsigned int i = 0; i < length; i++)
		tag.items.push_back(dispatch_parse(data, position, list_tag_type));
	return std::make_shared<tag_base>(tag);
}

tag_ptr parse_tag_compound(const std::vector<uint8_t> data, int& position) {
	tag_compound tag;
	tag.name = parse_string(data, position);

	uint8_t next_tag_type = data[position++];
	if (next_tag_type != 0) // not at end of compound tag
		tag.items.push_back(dispatch_parse(data, position, next_tag_type));

	return std::make_shared<tag_base>(tag);
}

tag_ptr dispatch_parse(const std::vector<uint8_t> data, int& position, const uint8_t tag_type) {
	switch (tag_type) {
	case 0:
		return std::make_shared<tag_base>(tag_end{});
		break;
	case 1:
		return parse_tag_byte(data, position);
		break;
	case 2:
		return parse_tag_short(data, position);
		break;
	case 3:
		return parse_tag_int(data, position);
		break;
	case 4:
		return parse_tag_long(data, position);
		break;
	case 5:
		return parse_tag_float(data, position);
		break;
	case 6:
		return parse_tag_double(data, position);
		break;
	case 7:
		return parse_tag_byte_array(data, position);
		break;
	case 8:
		return parse_tag_string(data, position);
		break;
	case 9:
		return parse_tag_list(data, position);
		break;
	case 10:
		return parse_tag_compound(data, position);
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