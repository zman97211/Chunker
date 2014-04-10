#ifndef __NBT_H__
#define __NBT_H__

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace nbt {

	struct tag_base {
		std::string name;
	};

	typedef std::shared_ptr<tag_base> tag_ptr;

	struct tag_end : public tag_base {};

	template <typename T>
	struct tag : public tag_base {
		T value;
	};

	struct tag_list : public tag_base {
		std::vector<tag_ptr> items;
	};

	struct tag_byte_array : public tag_base {
		std::vector<uint8_t> data;
	};

	struct tag_compound : public tag_base {
		std::vector<tag_ptr> items;
	};

	tag_ptr parse_nbt(const std::vector<uint8_t> data);
}

#endif