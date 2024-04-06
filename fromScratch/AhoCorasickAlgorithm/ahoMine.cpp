#include "node.cpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<unsigned char> inputVector = {
	// Some random "garbage" values
	0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
	// GIF87a header
	0x47, 0x49, 0x46, 0x38, 0x37, 0x61,
	// More "garbage"
	0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78,
	// GIF89a header
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61,
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

void displayInHex(vector<unsigned char> str) {
	cout << "sz: " << str.size() << "\t";
	for (unsigned char c : str) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
	}
	cout << "\n";
	cout << dec;
}

void displayCharInHex(unsigned char c) {
	std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
}

void displayTree(node *head) {
	displayCharInHex(head->getValue());
	if (head->getFailureNode() != NULL) {
		cout << " Failure: ";
		displayCharInHex(head->getFailureNode()->getValue());
	}
	cout << " -> ";
	if (head->isEnd()) {
		cout << "\nSig: ";
		displayInHex(head->getPattern());
		cout << "\n";
	}
	for (int i = 0; i < head->getNoOfChildNodes(); ++i)
		displayTree(head->getChild(i));
}

void trieConstruction(node **head, vector<vector<unsigned char>> &sigs) {
	*head = new node('R', false, true, NULL);
	node *prevNode = NULL, *newNode = NULL, *child = NULL;
	node *failureNode = NULL;
	for (auto &sig : sigs) {
		prevNode = *head;
		failureNode = *head;
		for (int i = 0; i < (int)sig.size(); ++i) {
			int present = false;
			int noOfChild = prevNode->getNoOfChildNodes();
			for (int j = 0; j < noOfChild; ++j) {
				child = prevNode->getChild(j);
				if (child->getValue() == sig[i]) {
					if (child->isEnd()) {
						failureNode = child;
					}
					prevNode = child;
					present = true;
					break;
				}
			}
			if (!present) {
				newNode = new node(sig[i], false, false, NULL);
				newNode->setFailureNode(failureNode);
				prevNode->addChildNode(newNode);
				prevNode = newNode;
			}
		}
		// failureNode = prevNode;
		prevNode->setFailureNode(prevNode);
		prevNode->setEndNode(sig);
	}
}

void inputTraversal(node *head) {
	// displayCharInHex(head->getValue());
	node *curPos = head, *child = NULL;
	for (unsigned char &val : inputVector) {
		// cout << val;
		displayCharInHex(val);
		bool present = false;
		int idx = -1;
		for (int i = 0; i < curPos->getNoOfChildNodes(); ++i) {
			child = curPos->getChild(i);
			if (val == child->getValue()) {
				present = true;
				idx = i;
				break;
			}
		}
		if (present) {
			cout << "Hello\n";
			curPos = curPos->getChild(idx);
		} else if (!present && curPos != head) {
			cout << "\n";
			curPos = curPos->getFailureNode();
			// node *failure = curPos->getFailureNode();
			// if (!failure->checkRoot() && failure->getPattern() != NULL)
			// displayInHex(failure->getPattern());
			// cout << "Match value: ";
			// displayCharInHex(failure->getValue());
			// cout << "\n";
		}
	}
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


	node *headerHead = NULL, *footerHead = NULL;

	sort(headerSigs.begin(), headerSigs.end(), compareInsideCharVector);
	sort(footerSigs.begin(), footerSigs.end(), compareInsideCharVector);

	// AhoCorasick aho;

	// for (auto &val : aho.headerSigs) {
	// 	displayInHex(val);
	// }

	// cout << "\n\n";

	trieConstruction(&headerHead, headerSigs);
	// trieConstruction(&footerHead, footerSigs);


	// cout << "\n\n";

	displayTree(headerHead);
	// displayTree(footerHead);

	inputTraversal(headerHead);

	return 0;
}
