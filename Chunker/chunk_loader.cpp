#include "chunk_loader.h"
#include "zlib.h"
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <stdexcept>

namespace chunk {
	namespace imp {
		uint32_t swap_endian(uint32_t i) {
			uint8_t* raw = reinterpret_cast<uint8_t*>(&i);
			return (raw[3] << 0) | (raw[2] << 8) | (raw[1] << 16) | (raw[0] << 24);
		}

		boost::filesystem::path construct_path(std::string level_path, int x, int y) {
			boost::filesystem::path base{ level_path };
			boost::filesystem::path dir{ "region" };
			boost::filesystem::path region{ (boost::format("r.%1%.%2%.mca") % x % y).str() };
			return base / dir / region;
		}

		std::vector<uint8_t> decompress (std::vector<uint8_t> comp) {
			uint32_t buffer_size = 1024 * 1024; // 1 meg, I think this should be sufficient? Kludge.
			std::vector<uint8_t> buffer(buffer_size);
			int z_result = uncompress(reinterpret_cast<Bytef*>(&buffer[0]), reinterpret_cast<uLongf*>(&buffer_size), reinterpret_cast<Bytef*>(&comp[0]), comp.size());

			switch (z_result) {
			case Z_OK:
				break;
			case Z_MEM_ERROR:
				throw std::runtime_error{ "Out of memory trying to decompress chunk." };
				break;
			case Z_BUF_ERROR:
				throw std::runtime_error{ "Uncompressed chunk data was over 1 meg limit." };
				break;
			default:
				throw std::runtime_error{ "Unknown error while decompressing chunk data." };
				break;
			}

			buffer.shrink_to_fit();
			return buffer;
		}
	}

	// Returns a vector of uncompressed chunk data, or an empty vector if the chunk doesn't exist.
	std::vector<uint8_t> chunk::load_chunk(std::string level_path, int chunk_x, int chunk_y) {
		int region_x = chunk_x >> 5;
		int region_y = chunk_y >> 5;

		auto region_file = imp::construct_path(level_path, region_x, region_y);
		if (!boost::filesystem::exists(region_file))
			throw std::invalid_argument{ "Could not locate region file: " + region_file.string() };

		int local_chunk_x = chunk_x % 32;
		if (local_chunk_x < 0)
			local_chunk_x += 32;
		int local_chunk_y = chunk_y % 32;
		if (local_chunk_y < 0)
			local_chunk_y += 32;

		std::ifstream region{ region_file.string(), std::ios_base::binary | std::ios_base::in };
		int chunk_location_offset = 4 * (local_chunk_y * 32 + local_chunk_x);
		region.seekg(chunk_location_offset);
		uint32_t location_data;
		region.read(reinterpret_cast<char*>(&location_data), sizeof(location_data));
		location_data = imp::swap_endian(location_data);

		// If the location is 0, there is no chunk data, so return an empty vector.
		if (location_data == 0)
			return std::vector<uint8_t>{};

		int offset = 4096 * (location_data >> 8);

		region.seekg(offset);
		uint32_t length;
		region.read(reinterpret_cast<char*>(&length), sizeof(length));
		length = imp::swap_endian(length);
		length -= 1; // The length includes the single-byte compression type field plus the length of data, but we just want data.

		uint8_t compression_type;
		region.read(reinterpret_cast<char*>(&compression_type), sizeof(compression_type));
		if (compression_type != 2)
			throw std::runtime_error{ "The chunk being loaded used a compression type that is not supported." };

		std::vector<uint8_t> result(length); // Initialize with length number of elements.
		region.read(reinterpret_cast<char*>(&result[0]), length);

		std::vector<uint8_t> uncompressed = imp::decompress(result);

		return uncompressed;
	}
}