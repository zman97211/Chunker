#include <iostream>
#include "chunk_loader.h"
#include "nbt.h"

int main(int argc, char* argv[]) {
	try {
		auto chunk_data = chunk::load_chunk("D:\\bukkit\\server\\large", 0, 0);
		auto chunk = nbt::parse_nbt(chunk_data);
	}
	catch (const std::exception& ex) {
		std::cout << "Exception: " << ex.what() << std::endl;
	}

	return 0;
}