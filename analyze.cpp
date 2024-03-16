#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

int main() {
	// Input binary file and output hex text file paths
	std::string binaryFilePath = "dmpFile.raw";
	std::string hexTextFilePath = "output.hex";

	// Open binary file for reading in binary mode
	std::ifstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open()) {
		std::cerr << "Error opening binary file." << std::endl;
		return 1;
	}

	// Open hex text file for writing in text mode
	std::ofstream hexTextFile(hexTextFilePath);
	if (!hexTextFile.is_open()) {
		std::cerr << "Error opening hex text file." << std::endl;
		return 1;
	}

	// Set stream to output hexadecimal values
	hexTextFile << std::hex << std::setfill('0');

	// Read and convert each byte from binary file to hexadecimal format
	unsigned char byte;
	int i = 0, j = 10000;
	while (j >= 0) {
		binaryFile.read(reinterpret_cast<char *>(&byte), sizeof(byte));
		hexTextFile
			<< std::setw(2) << static_cast<int>(byte) << ' ';
		i += 2;
		if (i >= 80) {
			i = 0;
			hexTextFile << std::endl;
		}
		--j;
	}

	// Close files
	binaryFile.close();

	hexTextFile.close();
	return 0;
}
