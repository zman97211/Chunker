#ifndef __NBT_H__
#define __NBT_H__

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace nbt {
	class tag_visitor;

	struct tag_base {
		virtual ~tag_base() {};
		std::string name;
		virtual void accept(tag_visitor* v) = 0;
	};

	typedef std::shared_ptr<tag_base> tag_ptr;

	struct tag_end : public tag_base {
		void accept(tag_visitor* v);
	};

	template <typename T>
	struct tag : public tag_base {
		T value;
		void accept(tag_visitor* v);
	};

	struct tag_list : public tag_base {
		std::vector<tag_ptr> items;
		void accept(tag_visitor* v);
	};

	struct tag_byte_array : public tag_base {
		std::vector<uint8_t> data;
		void accept(tag_visitor* v);
	};

	struct tag_compound : public tag_base {
		std::vector<tag_ptr> items;
		void accept(tag_visitor* v);
		
	};

	tag_ptr parse_nbt(const std::vector<uint8_t> data);

	class tag_visitor {
	public:
		virtual void visit(tag_end* tag) = 0;
		virtual void visit(tag<uint8_t>* tag) = 0;
		virtual void visit(tag<uint16_t>* tag) = 0;
		virtual void visit(tag<uint32_t>* tag) = 0;
		virtual void visit(tag<uint64_t>* tag) = 0;
		virtual void visit(tag<float>* tag) = 0;
		virtual void visit(tag<double>* tag) = 0;
		virtual void visit(tag_byte_array* tag) = 0;
		virtual void visit(tag<std::string>* tag) = 0;
		virtual void visit(tag_list* tag) = 0;
		virtual void visit(tag_compound* tag) = 0;
	};
}

#endif