#include "node.cpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

class AhoCorasick {
public:
	std::vector<std::vector<unsigned char>> headerSigs;
	std::vector<std::vector<unsigned char>> footerSigs;

	AhoCorasick() {
		headerSigs = {
			// GIF87a
			{0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
			// GIF89a
			{0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
			// PNG
			{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

		footerSigs = {
			// GIF footer
			{0x00, 0x3B},
			// PNG footer
			{0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}};

		sort(headerSigs.begin(), headerSigs.end(), compareInsideCharVector);
		sort(footerSigs.begin(), footerSigs.end(), compareInsideCharVector);
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
			prevNode->setEndNode(sig);
		}
	}

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
};