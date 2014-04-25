#include "load_save_png.hpp"

#include <iostream>

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage:\n\t ./pngs_to_ramps <in1.png> [in2.png] [...]" << std::endl;
		return 1;
	}
	std::vector< std::string > names;
	for (int arg = 1; arg < argc; ++arg) {
		std::string file = argv[arg];
		uint32_t width, height;
		std::vector< uint32_t > data;
		if (!load_png(file, width, height, data)) {
			std::cerr << "Error loading '" << file << "'" << std::endl;
			return 1;
		}
		if (width != 256) {
			std::cerr << "Warning, width of '" << file << "' is not 256; there may be artifacts." << std::endl;
		}

		std::string name = file.substr(0, file.size() - 4) + "_ramp";
		for (uint32_t i = name.size() - 1; i < name.size(); --i) {
			if (name[i] == '/') {
				name = name.substr(i+1);
				break;
			}
		}
		std::cout << "\nuint8_t " << name << "[256*3] = {";
		uint32_t y = height / 2;
		for (uint32_t i = 0; i < 256; ++i) {
			if (i % 16 == 0) std::cout << "\n\t";
			uint32_t c = data[y * width + i * width / 256];
			uint32_t r = c & 0xff;
			uint32_t g = (c >> 8) & 0xff;
			uint32_t b = (c >> 16) & 0xff;
			std::cout << r << ',' << g << ',' << b << ", ";
		}
		std::cout << "};\n";
		names.push_back(name);
	}
	std::cout << "\n";
	std::cout << "#define PNG_RAMPS \\\n";
	for (std::vector< std::string >::iterator n = names.begin(); n != names.end(); ++n) {
		std::cout << "\t" << *n << ", \\\n";
	}
	std::cout.flush();
	return 0;
}
