#ifndef __CHUNK_LOADER_H__
#define __CHUNK_LOADER_H__

#include <cstdint>
#include <vector>

namespace chunk {

	std::vector<uint8_t> load_chunk(std::string level_path, int chunk_x, int chunk_y);

}

#endif
