#include "../inc/AhoCorasick.h"
#include "../inc/Node.h"
#include "../inc/basic.h"
#include "../inc/vectorHash.h"
#include <atomic>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>
using namespace std;

mutex debugFileMtx, outputFileMtx;
// gifFileMtx, pngFileMtx;

std::atomic<int> gifFileNo(1);
std::atomic<int> pngFileNo(1);
// std::atomic<int> jpegFileNo(1);
// std::atomic<int> pdfFileNo(1);
// std::atomic<int> zipFileNo(1);

const string outputFileName = "../output";

void createDirectoriesAndCleaning() {
	string parentFolder = "../output";

	filesystem::remove_all(parentFolder);

	string gifSubFolder = "gif";
	string pngSubFolder = "png";
	string jpegSubFolder = "jpeg";
	string pdfSubFolder = "pdf";
	string zipSubFolder = "zip";

	// Create the directories if they do not exist
	filesystem::create_directories(parentFolder + "/" + gifSubFolder);
	filesystem::create_directories(parentFolder + "/" + pngSubFolder);
	// filesystem::create_directories(parentFolder + "/" + jpegSubFolder);
	// filesystem::create_directories(parentFolder + "/" + pdfSubFolder);
	// filesystem::create_directories(parentFolder + "/" + zipSubFolder);
}

std::vector<std::vector<unsigned char>> headerSigs = {
	// GIF87a
	{0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
	// GIF89a
	{0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
	// PNG
	{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
	// JPEG
	// {0xFF, 0xD8},
	// PDF
	// {0x25, 0x50, 0x44, 0x46},

	// ZIP
	// {0x50, 0x4B, 0x03, 0x04}
	//
};

std::vector<std::vector<unsigned char>> footerSigs = {
	// GIF footer
	{0x00, 0x3B},
	// PNG footer
	{0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82},
	// JPEG
	// {0xFF, 0xD9},
	// PDF
	// {0x25, 0x25, 0x45, 0x4F, 0x46},
	// ZIP
	// {0x50, 0x4B, 0x05, 0x06},

	//
};

unordered_map<vector<unsigned char>, pair<string, vector<unsigned char>>, VectorHash> headerComp = {
	// GIF87a
	{{0x47, 0x49, 0x46, 0x38, 0x37, 0x61}, {"GIF Header", {0x00, 0x3B}}},
	// GIF89a
	{{0x47, 0x49, 0x46, 0x38, 0x39, 0x61}, {"GIF Header", {0x00, 0x3B}}},
	// PNG
	{{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, {"PNG Header", {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}}},

	// JPEG
	// {{0xFF, 0xD8}, {"JPEG Header", {0xFF, 0xD9}}},
	// PDF
	// {{0x25, 0x50, 0x44, 0x46}, {"PDF Header", {0x25, 0x25, 0x45, 0x4F, 0x46}}},
	// ZIP
	// {{0x50, 0x4B, 0x03, 0x04}, {"ZIP Header", {0x50, 0x4B, 0x05, 0x06}}}
	//
};

unordered_map<vector<unsigned char>, string, VectorHash> footerComp = {
	// GIF footer
	{{0x00, 0x3B}, "GIF Footer"},
	// PNG footer
	{{0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}, "PNG Footer"},

	// JPEG
	// {{0xFF, 0xD9}, "JPEG Footer"},
	//
	// {{0x25, 0x25, 0x45, 0x4F, 0x46}, "PDF Footer"},
	// ZIP
	// {{0x50, 0x4B, 0x05, 0x06}, "ZIP Footer"}

};


AhoCorasick AhoHeaderFirst(headerSigs), AhoFooterFirst(footerSigs);

ofstream debugFile;

void searchSignature(streampos start, streampos end, int threadNo) {
	int tempFileNo = -1;
	AhoCorasick AhoHeader(AhoHeaderFirst), AhoFooter(AhoFooterFirst);

	ifstream file("DESKTOP-AJ7FC09-20240207-184313.raw", ios::binary);

	if (!file) {
		cerr << "Thread " << threadNo << " could not open file\n";
		return;
	}

	file.seekg(start, ios::beg);

	int n = 20;
	unsigned char buffer[n];

	string filePath, fileType, fileExtension;
	ofstream outfile;

	bool fileStartCarving = false;
	vector<unsigned char> footerSig, temp;
	for (streampos cur = start; file.read(reinterpret_cast<char *>(buffer), n) && cur <= end; cur += n) {
		temp.clear();
		for (int i = 0; i < n; ++i) {
			unsigned char &val = buffer[i];
			// cout << val << " ";
			// if (i == (n - 1)) {
			// 	AhoHeader.setlastTrue();
			// 	AhoFooter.setlastTrue();
			// 	AhoHeader.inputTraversal(val);
			// 	AhoFooter.inputTraversal(val);
			// } else {
			if (AhoHeader.inputTraversal(val)) {
				if (fileStartCarving) {
					if (temp.size() != 0) {
						outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
						temp.clear();
					}
					outfile.write(reinterpret_cast<char *>(&footerSig[0]), footerSig.size());
					outfile.close();
				} else
					fileStartCarving = true;
				// for (const auto &c : AhoHeader.curPos_->getPattern())
				// debugFile << c << " ";
				streampos headerPos = file.tellg() - streamoff(n - i);

				vector<unsigned char> pattern = AhoHeader.foundPattern_->getPattern();
				string headerStr = headerComp[pattern].first;
				// debugFileMtx.lock();
				// debugFile << headerStr << " ";
				// debugFile << " at " << headerPos << " thread: " << threadNo << "\n";
				// debugFileMtx.unlock();


				if (headerStr == "GIF Header") {
					tempFileNo = gifFileNo++;
					fileType = "gif";
					fileExtension = ".gif";
				} else if (headerStr == "PNG Header") {
					tempFileNo = pngFileNo++;
					fileType = "png";
					fileExtension = ".png";
				}
				// else if (headerStr == "JPEG Header") {
				// 	tempFileNo = jpegFileNo++;
				// 	fileType = "jpeg";
				// 	fileExtension = ".jpeg";
				// } else if (headerStr == "PDF Header") {
				// 	tempFileNo = pdfFileNo++;
				// 	fileType = "pdf";
				// 	fileExtension = ".pdf";
				// } else if (headerStr == "ZIP Header") {
				// 	tempFileNo = zipFileNo++;
				// 	fileType = "zip";
				// 	fileExtension = ".zip";
				// }
				// if (fileStartCarving)
				footerSig = headerComp[pattern].second;

				filePath = outputFileName + "/" + fileType + "/" + to_string(tempFileNo) + fileExtension;
				outfile.open(filePath, ios::out | ios::binary);
				// if (!outfile)
				// cout << "Failed to open file: " << filePath << std::endl;
				// closing more important
				// outfile.close();

				outfile.write(reinterpret_cast<char *>(&pattern[0]), pattern.size());
			}
			if (fileStartCarving) {
				temp.push_back(val);
				if (AhoFooter.inputTraversal(val)) {
					// for (const auto &c : AhoHeader.curPos_->getPattern())
					// debugFile << c << " ";
					streampos headerPos = file.tellg() - streamoff(n - i);
					vector<unsigned char> footerPatternFound = AhoFooter.foundPattern_->getPattern();
					// debugFileMtx.lock();
					// debugFile << footerComp[footerPatternFound] << " ";
					// debugFile << " at " << headerPos << " thread: " << threadNo << "\n";
					// debugFileMtx.unlock();

					if (footerPatternFound == footerSig) {
						outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());

						outfile.close();
						temp.clear();
						fileStartCarving = false;
					}
				}
			}
			// AhoFooter.inputTraversal(val);
		}

		if (temp.size() != 0) {
			outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
			temp.clear();
		}
	}

	temp.clear();
	while (fileStartCarving) {
		if (file.read(reinterpret_cast<char *>(buffer), n)) {
			for (int i = 0; i < n; ++i) {
				unsigned char &val = buffer[i];
				temp.push_back(val);
				if (AhoFooter.inputTraversal(val)) {
					// for (const auto &c : AhoHeader.curPos_->getPattern())
					// debugFile << c << " ";
					streampos headerPos = file.tellg() - streamoff(n - i);
					vector<unsigned char> footerPatternFound = AhoFooter.foundPattern_->getPattern();
					debugFileMtx.lock();
					debugFile << footerComp[footerPatternFound] << " ";
					debugFile << " at " << headerPos << " thread: " << threadNo << "\n";
					debugFileMtx.unlock();

					if (footerPatternFound == footerSig) {
						outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());

						outfile.close();
						temp.clear();
						fileStartCarving = false;
					}
				}
			}
		}
		if (temp.size() != 0) {
			outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
			temp.clear();
		}
	}
}

int main() {
	AhoHeaderFirst.trieConstruction();
	AhoFooterFirst.trieConstruction();

	// AhoHeader.displayTreeParentFn();
	// AhoFooter.displayTreeParentFn();

	createDirectoriesAndCleaning();


	ifstream file("DESKTOP-AJ7FC09-20240207-184313.raw", ios::binary);

	if (!file) {
		cerr << "Could not open file\n";
		return 1;
	}

	// debugFile.open("debug.txt", ios::out);
	// debugFile << "Type+No\t\tOffset\t\tThread\n\n";

	file.seekg(0, ios::end);
	streampos fileSize = file.tellg();

	file.close();

	// streampos splitSize = 100000000;

	// cout << (size_t)fileSize << "\n";

	// int threadNo = 20;
	// searchSignature(0, fileSize, threadNo);

	streampos splitSize = 100000000;	// 100 MB

	vector<thread> threads;

	int threadNo = 0;
	streampos i = 0;
	for (streampos i = 0; i < fileSize; i += splitSize) {
		streampos end = min(i + splitSize, fileSize);
		threads.push_back(thread(searchSignature, i, end, threadNo++));
	}

	for (auto &t : threads) {
		t.join();
	}


	// AhoHeader.curPos_ = headerHead;
	// AhoFooter.curPos_ = footerHead;
	// int n = inputVector.size();
	// for (int i = 0; i < n; ++i) {
	// 	unsigned char &val = inputVector[i];
	// 	if (i == (n - 1)) {
	// 		AhoHeader.setlastTrue();
	// 		AhoFooter.setlastTrue();
	// 		AhoHeader.inputTraversal(val);
	// 		AhoFooter.inputTraversal(val);
	// 	} else {
	// 		AhoHeader.inputTraversal(val);
	// 		AhoFooter.inputTraversal(val);
	// 	}
	// }
	// cout << "\n\n";

	// for (unsigned char &val : inputVector) {
	// 	if (val == inputVector.back())
	// 		AhoFooter.inputTraversal(footerHead, val, true);
	// 	else
	// 		AhoFooter.inputTraversal(footerHead, val, false);
	// }


	// debugFile.close();

	return 0;
}

vector<unsigned char> inputVector = {
	// Some random "garbage" values
	0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
	// GIF87a header
	0x47, 0x49, 0x46, 0x38, 0x37, 0x61,
	0x00, 0x3b,
	0x47, 0x49, 0x46, 0x38, 0x37, 0x61,
	// More "garbage"
	0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78,
	0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
	// GIF89a header
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61,
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
	// Even more "garbage"
	0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0,
	// GIF footer
	0x00, 0x3B};
