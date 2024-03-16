#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

void findJPEGHeaders(std::ifstream &file, std::streampos start, std::streampos end, int threadNum) {
	file.seekg(start);

	char buffer[2];
	while (file.tellg() < end && file.read(buffer, 2)) {
		if (buffer[0] == (char)0xFF && buffer[1] == (char)0xD8) {
			std::cout << "Thread " << threadNum << " found JPEG header at offset " << file.tellg() << '\n';
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

	int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(numThreads);

	for (int i = 0; i < numThreads; ++i) {
		std::streampos start = (fileSize / numThreads) * i;
		std::streampos end = (i == numThreads - 1) ? fileSize : (fileSize / numThreads) * (i + 1);
		threads[i] = std::thread(findJPEGHeaders, std::ref(file), start, end, i);
	}

	// for (auto &thread : threads) {
	// 	thread.join();
	// }

	std::cout << fileSize << "\n";

	return 0;
}
