#include <iostream>
#include <fstream>
#include "chunk_loader.h"
#include "nbt.h"
#include "nbt_dump_visitor.h"

nbt::tag_ptr get_a_real_chunk() {
	auto chunk_data = chunk::load_chunk("D:\\bukkit\\server\\large", 0, 0);
	return nbt::parse_nbt(chunk_data);
}

nbt::tag_ptr get_test_nbt() {
	std::ifstream region{ "D:\\bukkit\\bigtest.nbt", std::ios_base::binary | std::ios_base::in };
	region.seekg(0, std::ifstream::beg);
	auto begin = region.tellg();
	region.seekg(0, std::ifstream::end);
	auto end = region.tellg();
	region.seekg(0, std::ifstream::beg);
	auto length = end - begin;
	std::vector<uint8_t> data(static_cast<unsigned int>(length));
	region.read(reinterpret_cast<char*>(&data[0]), length);
	return nbt::parse_nbt(data);
}

int main(int argc, char* argv[]) {
	try {
		auto nbt_data = get_test_nbt();
		nbt::nbt_dump_visitor v;
		nbt_data->accept(&v);
	}
	catch (const std::exception& ex) {
		std::cout << "Exception: " << ex.what() << std::endl;
	}

	system("pause");
	return 0;
}