#include <fstream>
#include <iostream>

// using int, as streampos is really slow
// size_t largest int size, no problem of overflow
void searchSignature(std::ifstream &file, std::streampos start, std::streampos end, int thread) {
	int n = 20;
	char buffer[n];

	file.seekg(start, std::ios::beg);

	for (std::streampos cur = start; file.read(buffer, n) && cur <= end; cur += n) {
		// if ((buffer[0] == 'G' && buffer[1] == 'I' && buffer[2] == 'F' && buffer[3] == '8' &&
		// 	 (buffer[4] == '7' || buffer[4] == '9') && buffer[5] == 'a')) {
		// 	std::cout << "Found GIF header at offset " << file.tellg() << '\n';
		// }
		for (int i = 0; i < n - 1; ++i) {
			if (buffer[i] == (char)0xFF && buffer[i + 1] == (char)0xD8) {
				// calculates the correct position of the JPEG header by subtracting the number of bytes remaining in the buffer after the header from the current file position.
				std::streampos headerPos = file.tellg() - std::streamoff(n - i);
				std::cout << "Found JPEG header at offset " << headerPos << '\n';
			}
		}
	}
}

int main() {
	std::ifstream file("../DESKTOP-AJ7FC09-20240207-184313.raw", std::ios::binary);

	if (!file) {
		std::cerr << "Could not open file\n";
		return 1;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();

	int splitSize = 100000000;

	// for (int i = 0; i < fileSize;) {
	// 	int diff = fileSize - i;
	// 		if (i < splitSize) {

	// 		}
	searchSignature(file, 0, fileSize, 5);
	std::cout << fileSize << "\n";
	// }

	file.close();

	return 0;
}
