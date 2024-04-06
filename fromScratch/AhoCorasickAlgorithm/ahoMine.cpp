#include "node.cpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

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
	if (head->getFailureNode() != NULL)
		cout << " Failure: " << head->getFailureNode()->getValue();
	cout << " -> ";
	if (head->isEnd()) {
		cout << "\nSig: ";
		displayInHex(head->getPattern());
		cout << "\n";
	}
	for (int i = 0; i < head->getNoOfChildNodes(); ++i)
		displayTree(head->getChild(i));
}

int main() {
	node *head = new node('R', false, true, NULL), *prevNode = NULL, *newNode = NULL, *child = NULL;

	std::vector<std::vector<unsigned char>> sigs = {
		{0x41, 0x42, 0x43, 0x44},
		{0x41, 0x42, 0x43, 0x44, 0x45},
		{0x41, 0x42},
		{0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
		{0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
		{0x4A, 0x51, 0x45, 0x48},
		{0x4A, 0x50, 0x41, 0x4D},
		{0x4A, 0x50, 0x45, 0x41, 0x4D, 0x4D, 0x58},
		{0x00, 0x3B},
		{0x90, 0x50, 0x89},
		{0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};


	sort(sigs.begin(), sigs.end(), compareInsideCharVector);

	for (auto &val : sigs) {
		displayInHex(val);
	}

	cout << "\n\n";

	node *failureNode = NULL;
	for (auto &sig : sigs) {
		prevNode = head;
		failureNode = head;
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

	cout << "\n\n";

	displayTree(head);

	for (int i = 0; i < head->getNoOfChildNodes(); ++i)
		displayCharInHex(head->getChild(i)->getValue());
	cout << "\n\n";

	for (int i = 0; i < head->getChild(0)->getNoOfChildNodes(); ++i)
		displayCharInHex(head->getChild(0)->getChild(i)->getValue());

	return 0;
}
