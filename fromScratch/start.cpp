#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mtx;

// std::mutex debugFileMutex;

const int maxGifSize = 4000;
const std::string outputFileName = "output";
std::ofstream debugFile;

int fileNo = 0;

void createDirectoriesAndCleaning() {
	std::string parentFolder = "output";

	std::filesystem::remove_all(parentFolder);

	std::string gifSubFolder = "gif";

	// Create the directories if they do not exist
	std::filesystem::create_directories(parentFolder + "/" + gifSubFolder);
}

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
	int startedReconstruction = 0;

	std::string filePath;
	std::ofstream outfile;

	int curGifFileSize = 0;
	for (std::streampos cur = start; file.read(buffer, n) && cur <= end; cur += n) {
		int endPos = 0;
		if (startedReconstruction) {
			if (curGifFileSize > maxGifSize) {
				char end[] = {0x00, 0x3B};
				outfile.write(end, 2);
				outfile.close();
				curGifFileSize = 0;
			}
			for (int i = 0; i < n - 2; ++i) {
				if (buffer[i] == (char)0x00 && buffer[i + 1] == (char)0x3B) {
					startedReconstruction = 0;
					endPos += 2;
					break;
				}
				++curGifFileSize;
				++endPos;
			}
			outfile.write(buffer, n);
			if (!startedReconstruction) {
				outfile.write(buffer, endPos);
				outfile.close();
				curGifFileSize = 0;
			}
		}

		// for (int i = 0; i < n - 1; ++i) {
		// 	if (buffer[i] == (char)0xFF && buffer[i + 1] == (char)0xD8) {
		// 		// calculates the correct position of the JPEG header by subtracting the number of bytes remaining in the buffer after the header from the current file position.
		// 		std::streampos headerPos = file.tellg() - std::streamoff(n - i);
		// 		std::cout << "Found JPEG header at offset: " << headerPos << " thread: " << threadNo << "\n";
		// 	}
		// }

		char temp[n];
		int sz = 0;
		int curBufPos = 0;

		for (int i = 0; i < n - 7; ++i) {
			if ((buffer[i] == 'G' && buffer[i + 1] == 'I' && buffer[i + 2] == 'F' && buffer[i + 3] == '8' &&
				 (buffer[i + 4] == '7' || buffer[i + 4] == '9') && buffer[i + 5] == 'a')) {
				std::streampos headerPos = file.tellg() - std::streamoff(n - i);

				mtx.lock();

				debugFile << "Gif" << fileNo << " \t\t" << headerPos << " \t\t" << threadNo << "\n";
				// i += 6;
				// continue;
				startedReconstruction = 1;

				filePath = outputFileName + "/gif/" + std::to_string(fileNo++) + ".gif";
				outfile.open(filePath, std::ios::out | std::ios::binary);
				// std::cout << "Current Gif fileNo: " << fileNo++ << "\n";

				mtx.unlock();

				temp[0] = buffer[i];
				temp[1] = buffer[i + 1];
				temp[2] = buffer[i + 2];
				temp[3] = buffer[i + 3];
				temp[4] = buffer[i + 4];
				temp[5] = buffer[i + 5];
				sz = 6;
				curGifFileSize = 6;
				curBufPos = i + 6;
				break;
			}
		}

		if (startedReconstruction) {
			for (int i = curBufPos; i < n - 2; ++i) {
				if (buffer[i] == (char)0x00 && buffer[i + 1] == (char)0x3B) {
					startedReconstruction = 0;
					sz += 2;
					break;
				}
				++sz;
				++curGifFileSize;
			}
			outfile.write(temp, sz);
			if (!startedReconstruction) {
				outfile.close();
				curGifFileSize = 0;
			}
		}

		// for (int i = 0; i < n - 4; ++i) {
		// 	if (buffer[i] == '%' && buffer[i + 1] == 'P' && buffer[i + 2] == 'D' && buffer[i + 3] == 'F') {
		// 		std::streampos headerPos = file.tellg() - std::streamoff(n - i);
		// 		std::cout << "Found PDF header at offset " << headerPos << '\n';
		// 	}
		// }
	}

	file.close();
}

int main() {
	std::ifstream file("../DESKTOP-AJ7FC09-20240207-184313.raw", std::ios::binary);

	if (!file) {
		std::cerr << "Could not open file\n";
		return 1;
	}

	debugFile.open("debug.txt", std::ios::out);
	debugFile << "Type+No\t\tOffset\t\tThread\n\n";

	createDirectoriesAndCleaning();

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


	debugFile.close();
	return 0;
}
