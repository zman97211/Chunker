#include "nbt_dump_visitor.h"
#include "nbt.h"
#include <iostream>
#include <boost/format.hpp>

using namespace nbt;

void nbt_dump_visitor::indent() {
	for (int i = 0; i < depth; i++)
		std::cout << "    ";
}

void nbt_dump_visitor::visit(tag_end* tag) {
	indent();
	std::cout << "tag_end" << std::endl;
}

void nbt_dump_visitor::visit(tag<int8_t>* tag) {
	indent();
	std::cout << "tag_byte: '" << tag->name << "' == " << tag->value << std::endl;
}

void nbt_dump_visitor::visit(tag<int16_t>* tag) {
	indent();
	std::cout << "tag_short: '" << tag->name << "' == " << tag->value << std::endl;
}

void nbt_dump_visitor::visit(tag<int32_t>* tag) {
	indent();
	std::cout << "tag_int: '" << tag->name << "' == " << tag->value << std::endl;
}

void nbt_dump_visitor::visit(tag<int64_t>* tag) {
	indent();
	std::cout << "tag_long: '" << tag->name << "' == " << tag->value << std::endl;
}

void nbt_dump_visitor::visit(tag<float>* tag) {
	indent();
	std::cout << "tag_float: '" << tag->name << "' == " << tag->value << std::endl;
}

void nbt_dump_visitor::visit(tag<double>* tag) {
	indent();
	std::cout << "tag_double: '" << tag->name << "' == " << tag->value << std::endl;
}

void nbt_dump_visitor::visit(tag_byte_array* tag) {
	indent();
	std::cout << "tag_byte_array: '" << tag->name << "' (size == " << tag->data.size() << "): ";
	for (auto b : tag->data)
		std::cout << (int)b << " ";
	std::cout << std::endl;
}

void nbt_dump_visitor::visit(tag<std::string>* tag) {
	indent();
	std::cout << "tag_string: '" << tag->name << "' == '" << tag->value << "'" << std::endl;
}

void nbt_dump_visitor::visit(tag_list* tag) {
	indent();
	std::cout << "tag_list: '" << tag->name << "' (" << tag->items.size() << " item(s) follow..." << std::endl;
	depth++;
	for (auto t : tag->items)
		t->accept(this);
	depth--;
}

void nbt_dump_visitor::visit(tag_compound* tag) {
	indent();
	std::cout << "tag_compound: '" << tag->name << "' (" << tag->items.size() << " item(s) follow..." << std::endl;
	depth++;
	for (auto t : tag->items)
		t->accept(this);
	depth--;
}