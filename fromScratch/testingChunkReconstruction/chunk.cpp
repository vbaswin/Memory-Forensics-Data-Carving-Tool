#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

mutex mtx;

// mutex debugFileMutex;

const int maxGifSize = 4000;
const int checkFileNo = 1;

const string outputFileName = "output";
ofstream debugFile;

int fileNo = 1;

void displayInHex(char str[], int sz) {
	cout << "sz: " << sz << "\t";
	for (int i = 0; i < sz; ++i) {
		cout << hex << setw(2) << setfill('0') << (0xFF & static_cast<int>(str[i])) << " ";
	}
	cout << "\n";
	cout << dec;
}

void createDirectoriesAndCleaning() {
	string parentFolder = "output";

	filesystem::remove_all(parentFolder);

	string gifSubFolder = "gif";

	// Create the directories if they do not exist
	filesystem::create_directories(parentFolder + "/" + gifSubFolder);
}

// using int, as streampos is really slow
// size_t largest int size, no problem of overflow
void searchSignature(streampos start, streampos end, int threadNo) {
	int tempFileNo = -1;
	ifstream file("test1.gif", ios::binary);

	if (!file) {
		cerr << "Thread " << threadNo << " could not open file\n";
		return;
	}

	file.seekg(start, ios::beg);

	int n = 20;
	char buffer[n];

	file.seekg(start, ios::beg);
	int startedReconstruction = 0;

	string filePath;
	ofstream outfile;

	int curGifFileSize = 0;
	for (streampos cur = start; file.read(buffer, n) && cur <= end; cur += n) {
		displayInHex(buffer, n);
		int endPos = 0;
		int remainingStart = 0;
		if (startedReconstruction) {
			if (curGifFileSize > maxGifSize) {
				char end[] = {0x00, 0x3B};
				if (tempFileNo == checkFileNo) {
					cout << "Reading till sig end or thread limit end, end reached\n";
					displayInHex(end, 2);
				}
				outfile.write(end, 2);
				outfile.close();
				tempFileNo = -1;
				curGifFileSize = 0;
				startedReconstruction = 0;
			}
			for (int i = 0; i < n - 1; ++i) {
				if (buffer[i] == (char)0x00 && buffer[i + 1] == (char)0x3B) {
					cout << "Inside reconstuction\n";
					startedReconstruction = 0;
					endPos += 2;
					remainingStart = endPos + 1;
					break;
				}
				++curGifFileSize;
				++endPos;
			}
			if (!startedReconstruction) {
				// outfile.close();
				tempFileNo = -1;
				curGifFileSize = 0;
			} else
				++endPos;
			outfile.write(buffer, endPos);
			if (tempFileNo == checkFileNo) {
				cout << "Reading till sig end or thread limit end\n";
				displayInHex(buffer, endPos);
				cout << "\n";
			}
			if (startedReconstruction)
				continue;
		}

		// for (int i = 0; i < n - 1; ++i) {
		// 	if (buffer[i] == (char)0xFF && buffer[i + 1] == (char)0xD8) {
		// 		// calculates the correct position of the JPEG header by subtracting the number of bytes remaining in the buffer after the header from the current file position.
		// 		streampos headerPos = file.tellg() - streamoff(n - i);
		// 		cout << "Found JPEG header at offset: " << headerPos << " thread: " << threadNo << "\n";
		// 	}
		// }

		char temp[n];
		int sz = 0;
		int curBufPos = 0;

		for (int i = remainingStart; i < n - 6; ++i) {
			if ((buffer[i] == 'G' && buffer[i + 1] == 'I' && buffer[i + 2] == 'F' && buffer[i + 3] == '8' &&
				 (buffer[i + 4] == '7' || buffer[i + 4] == '9') && buffer[i + 5] == 'a')) {
				streampos headerPos = file.tellg() - streamoff(n - i);

				mtx.lock();
				tempFileNo = fileNo++;
				mtx.unlock();

				debugFile << "Gif" << tempFileNo << " \t\t" << headerPos << " \t\t" << threadNo << "\n";
				// i += 6;
				// continue;
				startedReconstruction = 1;

				filePath = to_string(tempFileNo) + ".gif";
				outfile.open(filePath, ios::out | ios::binary);
				// cout << "Current Gif fileNo: " << fileNo++ << "\n";

				mtx.unlock();

				temp[0] = buffer[i];
				temp[1] = buffer[i + 1];
				temp[2] = buffer[i + 2];
				temp[3] = buffer[i + 3];
				temp[4] = buffer[i + 4];
				temp[5] = buffer[i + 5];
				sz = 5;
				curGifFileSize = 6;
				curBufPos = i + 6;
				break;
			}
		}

		if (startedReconstruction) {
			int j;
			for (j = curBufPos; j < n - 1; ++j) {
				if (buffer[j] == (char)0x00 && buffer[j + 1] == (char)0x3B) {
					startedReconstruction = 0;
					temp[++sz] = 0x00;
					temp[++sz] = 0x3B;
					break;
				}
				temp[++sz] = buffer[j];
				// cout << "curr buff: ";
				// displayInHex(buffer, 20);
				// cout << "curr temp: ";
				// displayInHex(temp, sz);
				++curGifFileSize;
			}
			if (!startedReconstruction) {
				outfile.close();
				tempFileNo = -1;
				curGifFileSize = 0;
			} else {
				temp[++sz] = buffer[j + 1];
				++curGifFileSize;
			}
			outfile.write(temp, sz + 1);
			if (tempFileNo == checkFileNo) {
				cout << "found start startPos: " << curBufPos - 6 << " sz: " << sz + 1 << "\n";
				displayInHex(temp, sz + 1);
				cout << "\n";
			}
		}

		// for (int i = 0; i < n - 4; ++i) {
		// 	if (buffer[i] == '%' && buffer[i + 1] == 'P' && buffer[i + 2] == 'D' && buffer[i + 3] == 'F') {
		// 		streampos headerPos = file.tellg() - streamoff(n - i);
		// 		cout << "Found PDF header at offset " << headerPos << '\n';
		// 	}
		// }
	}

	while (startedReconstruction) {
		if (file.read(buffer, n) || curGifFileSize > maxGifSize) {
			char end[] = {0x00, 0x3B};
			if (tempFileNo == checkFileNo) {
				cout << "Reading till sig end or thread limit end, end reached\n";
				// cout << end << "\n";
				displayInHex(end, 2);
			}
			outfile.write(end, 2);
			tempFileNo = -1;
			outfile.close();
			curGifFileSize = 0;
			startedReconstruction = 0;
		}

		int endPos = 0;
		for (int i = 0; i < n - 1; ++i) {
			if (buffer[i] == (char)0x00 && buffer[i + 1] == (char)0x3B) {
				startedReconstruction = 0;
				endPos += 2;
				break;
			}
			++curGifFileSize;
			++endPos;
		}
		if (!startedReconstruction) {
			outfile.close();
			tempFileNo = -1;
			curGifFileSize = 0;
		} else
			++endPos;
		outfile.write(buffer, endPos);
		if (tempFileNo == checkFileNo) {
			cout << "Reading beyond thread limit\n";
			// cout.write(buffer, endPos);
			displayInHex(buffer, endPos);
			cout << "\n";
		}
	}

	file.close();
}

int main() {
	ifstream file("test1.gif", ios::binary);

	if (!file) {
		cerr << "Could not open file\n";
		return 1;
	}

	debugFile.open("debug.txt", ios::out);
	debugFile << "Type+No\t\tOffset\t\tThread\n\n";

	// createDirectoriesAndCleaning();

	file.seekg(0, ios::end);
	streampos fileSize = file.tellg();

	file.close();

	searchSignature(0, fileSize, 5);
	// cout << fileSize << "\n";


	debugFile.close();
	return 0;
}
