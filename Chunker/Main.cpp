#include <iostream>
#include <fstream>
#include "chunk_loader.h"
#include "nbt.h"

void print(nbt::tag_base* t) {
	std::cout << t->to_string() << std::endl;
}

int main(int argc, char* argv[]) {
	try {
		//auto chunk_data = chunk::load_chunk("D:\\bukkit\\server\\large", 0, 0);
		//auto chunk = nbt::parse_nbt(chunk_data);

		std::ifstream region{ "D:\\bukkit\\bigtest.nbt", std::ios_base::binary | std::ios_base::in };
		region.seekg(0, std::ifstream::beg);
		auto begin = region.tellg();
		region.seekg(0, std::ifstream::end);
		auto end = region.tellg();
		region.seekg(0, std::ifstream::beg);
		auto length = end - begin;
		std::vector<uint8_t> data(length);
		region.read(reinterpret_cast<char*>(&data[0]), length);
		auto parsed = nbt::parse_nbt(data);
		auto tag_comp1 = dynamic_cast<nbt::tag_compound*>(parsed.get());
		std::cout << "tag_comp1->items.size() == " << tag_comp1->items.size() << std::endl;
		std::cout << "tag_comp1->items[0] is " << tag_comp1->items[0]->to_string() << std::endl;
		auto tag_long1 = dynamic_cast<nbt::tag<uint64_t>*>(tag_comp1->items[0].get());
	}
	catch (const std::exception& ex) {
		std::cout << "Exception: " << ex.what() << std::endl;
	}

	system("pause");
	return 0;
}