#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

// using int, as streampos is really slow
// size_t largest int size, no problem of overflow
void searchSignature(std::streampos start, std::streampos end, int threadNo) {
	std::ifstream file("../DESKTOP-AJ7FC09-20240207-184313.raw", std::ios::binary);

	if (!file) {
		std::cerr << "Thread " << threadNo << " could not open file\n";
		return;
	}

	file.seekg(start, std::ios::beg);

	int n = 20;
	char buffer[n];

	file.seekg(start, std::ios::beg);

	for (std::streampos cur = start; file.read(buffer, n) && cur <= end; cur += n) {
		// for (int i = 0; i < n - 1; ++i) {
		// 	if (buffer[i] == (char)0xFF && buffer[i + 1] == (char)0xD8) {
		// 		// calculates the correct position of the JPEG header by subtracting the number of bytes remaining in the buffer after the header from the current file position.
		// 		std::streampos headerPos = file.tellg() - std::streamoff(n - i);
		// 		std::cout << "Found JPEG header at offset: " << headerPos << " thread: " << threadNo << "\n";
		// 	}
		// }

		// for (int i = 0; i < n - 7; ++i) {
		// 	if ((buffer[i] == 'G' && buffer[i + 1] == 'I' && buffer[i + 2] == 'F' && buffer[i + 3] == '8' &&
		// 		 (buffer[i + 4] == '7' || buffer[i + 4] == '9') && buffer[i + 5] == 'a')) {
		// 		std::streampos headerPos = file.tellg() - std::streamoff(n - i);
		// 		std::cout << "Found GIF header at offset " << headerPos << '\n';
		// 	}
		// }

		for (int i = 0; i < n - 4; ++i) {
			if (buffer[i] == '%' && buffer[i + 1] == 'P' && buffer[i + 2] == 'D' && buffer[i + 3] == 'F') {
				std::streampos headerPos = file.tellg() - std::streamoff(n - i);
				std::cout << "Found PDF header at offset " << headerPos << '\n';
			}
		}
	}

	file.close();
}

int main() {
	std::ifstream file("../DESKTOP-AJ7FC09-20240207-184313.raw", std::ios::binary);

	if (!file) {
		std::cerr << "Could not open file\n";
		return 1;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();

	file.close();

	std::streampos splitSize = 100000000;	 // 100 MB

	std::vector<std::thread> threads;

	int threadNo = 0;
	std::streampos i = 0;
	for (std::streampos i = 0; i < fileSize; i += splitSize) {
		std::streampos end = std::min(i + splitSize, fileSize);
		threads.push_back(std::thread(searchSignature, i, end, threadNo++));
	}

	for (auto &t : threads) {
		t.join();
	}
	// searchSignature(file, 0, fileSize, 5);
	// std::cout << fileSize << "\n";


	return 0;
}
