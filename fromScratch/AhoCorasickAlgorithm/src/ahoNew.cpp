#include "../inc/AhoCorasick.h"
#include "../inc/Node.h"
#include "../inc/basic.h"
using namespace std;

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

bool compareInsideCharVector(vector<unsigned char> a, vector<unsigned char> b) {
	for (int i = 0; i < (int)min(a.size(), b.size()); ++i) {
		if (a[i] < b[i]) {
			return true;
		} else if (a[i] > b[i]) {
			return false;
		}
	}
	return a.size() < b.size();
}

int main() {
	std::vector<std::vector<unsigned char>> headerSigs = {
		// GIF87a
		{0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
		// GIF89a
		{0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
		// PNG
		{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

	std::vector<std::vector<unsigned char>> footerSigs = {
		// GIF footer
		{0x00, 0x3B},
		// PNG footer
		{0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}};


	AhoCorasick AhoHeader(headerSigs), AhoFooter(footerSigs);

	AhoHeader.trieConstruction();
	AhoFooter.trieConstruction();


	// AhoHeader.displayTreeParentFn();
	// AhoFooter.displayTreeParentFn();

	// AhoHeader.curPos_ = headerHead;
	// AhoFooter.curPos_ = footerHead;
	int n = inputVector.size();
	for (int i = 0; i < n; ++i) {
		unsigned char &val = inputVector[i];
		if (i == (n - 1)) {
			AhoHeader.setlastTrue();
			AhoFooter.setlastTrue();
			AhoHeader.inputTraversal(val);
			AhoFooter.inputTraversal(val);
		} else {
			AhoHeader.inputTraversal(val);
			AhoFooter.inputTraversal(val);
		}
	}
	// cout << "\n\n";

	// for (unsigned char &val : inputVector) {
	// 	if (val == inputVector.back())
	// 		AhoFooter.inputTraversal(footerHead, val, true);
	// 	else
	// 		AhoFooter.inputTraversal(footerHead, val, false);
	// }

	return 0;
}
