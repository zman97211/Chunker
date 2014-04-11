#ifndef __NBT_DUMP_VISITOR_H__
#define __NBT_DUMP_VISITOR_H__

#include "nbt.h"

namespace nbt {

	class nbt_dump_visitor : public tag_visitor {
		void visit(tag_end* tag);
		void visit(tag<int8_t>* tag);
		void visit(tag<int16_t>* tag);
		void visit(tag<int32_t>* tag);
		void visit(tag<int64_t>* tag);
		void visit(tag<float>* tag);
		void visit(tag<double>* tag);
		void visit(tag_byte_array* tag);
		void visit(tag_int_array* tag);
		void visit(tag<std::string>* tag);
		void visit(tag_list* tag);
		void visit(tag_compound* tag);
	private:
		int depth = 0;
		void indent();
	};

}

#endif