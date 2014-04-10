#ifndef __NBT_H__
#define __NBT_H__

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace nbt {

	struct tag_base {
		virtual ~tag_base() {};
		virtual std::string to_string() { return "tag_base"; };
		std::string name;
	};

	typedef std::shared_ptr<tag_base> tag_ptr;

	struct tag_end : public tag_base {
		std::string to_string() { return "tag_end"; };
	};

	template <typename T>
	struct tag : public tag_base {
		T value;
		std::string to_string() { return "tag<T>"; };
	};

	struct tag_list : public tag_base {
		std::vector<tag_ptr> items;
		std::string to_string() { return "tag_list"; };
	};

	struct tag_byte_array : public tag_base {
		std::vector<uint8_t> data;
		std::string to_string() { return "tag_byte_array"; };
	};

	struct tag_compound : public tag_base {
		std::vector<tag_ptr> items;
		std::string to_string() { return "tag_compound"; };
	};

	tag_ptr parse_nbt(const std::vector<uint8_t> data);
}

#endif