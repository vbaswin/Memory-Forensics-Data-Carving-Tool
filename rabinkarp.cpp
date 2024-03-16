/*
	PROGRAM : Rabin Karp for multiple pattern search in the dump file.
*/
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const int prime = 101;
const int d = 256;

class RabinKarpMultiplePatterns {
public:
	// constructor to initialize the class with a vector of patterns.
	RabinKarpMultiplePatterns(const std::vector<std::string> &patterns) : patterns(patterns) {}

	//
	std::vector<int> rabinKarpMultiple(const std::string &text) {
		std::vector<int> results;
		for (const auto &pattern : patterns) {
			auto patternMatches = rabinKarpSingle(text, pattern);
			results.insert(results.end(), patternMatches.begin(), patternMatches.end());
		}
		return results;
	}
private:
	std::vector<int> rabinKarpSingle(const std::string &text, const std::string &pattern) {
		int m = pattern.size();
		int n = text.size();
		std::vector<int> patternsFound;

		// calculate hash for pattern and first window of text
		int patternHash = createHash(pattern, m);
		int textHash = createHash(text.substr(0, m), m);

		for (int i = 0; i <= n - m; ++i) {
			// check if the hash values match
			if (patternHash == textHash) {
				// check character by character since there is a match
				if (text.substr(i, m) == pattern) {
					patternsFound.push_back(i);
				}
			}

			// calculate hash for the next window of text
			if (i < n - m) {
				textHash = recalculateHash(text, i, i + m, textHash, m);
			}
		}

		return patternsFound;
	}

	// calculate hash value
	int createHash(const std::string &s, int length) {
		int hashValue = 0;
		for (int i = 0; i < length; ++i) {
			hashValue = (hashValue * d + s[i]) % prime;
		}
		return hashValue;
	}

	// recalculate hash using rolling hash for new window
	int recalculateHash(const std::string &s, int oldIndex, int newIndex, int oldHash, int length) {
		int oldCharCode = s[oldIndex];
		int newCharCode = s[newIndex];

		// remove the contribution of the old character
		int hashValue = (oldHash - oldCharCode * static_cast<int>(std::pow(d, length - 1))) % prime;

		// add the contribution of the new character
		hashValue = (hashValue * d + newCharCode) % prime;
		return hashValue;
	}
private:
	std::vector<std::string> patterns;
};

int main() {
	// add necessary file signatures
	std::vector<std::string> patternsToSearch = {"47494637", "FFD8", "25504446"};

	// add path of the dump file
	std::string memoryDumpFilePath = "File07gif.raw";

	// file opeing
	std::ifstream file(memoryDumpFilePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cerr << "Unable to open the file." << std::endl;
		return 1;
	}

	// finding file size
	std::streamsize fileSize = file.tellg();

	file.seekg(0, std::ios::beg);

	std::vector<char> binaryData(fileSize);

	char byte;
	while (file.get(byte)) {
		// Output each byte in hexadecimal format
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)byte << " ";
	}

	if (file.read(binaryData.data(), fileSize)) {
		std::string binaryDataStr(binaryData.data(), fileSize);

		// object
		RabinKarpMultiplePatterns rk(patternsToSearch);
		std::vector<int> matches = rk.rabinKarpMultiple(binaryDataStr);

		if (!matches.empty()) {
			std::cout << "Patterns found at positions:";
			for (int position : matches) {
				std::cout << " " << position;
			}
			std::cout << std::endl;
		} else {
			std::cout << "No patterns found." << std::endl;
		}
	} else {
		std::cerr << "Error reading the file." << std::endl;
		return 1;
	}

	return 0;
}
