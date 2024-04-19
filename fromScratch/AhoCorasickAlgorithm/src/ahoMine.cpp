#include "Node.cpp"
// #include "allClasses.h"
#include "../inc/basic.h"
using namespace std;

vector<unsigned char> inputVector = {
	// Some random "garbage" values
	0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
	// GIF87a header
	0x47, 0x49, 0x46, 0x38, 0x37, 0x61,
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

void displayInHex(vector<unsigned char> str) {
	cout << "\nsz: " << str.size() << "\t";
	for (unsigned char c : str) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
	}
	cout << "\n\n";
	cout << dec;
}

void displayCharInHex(unsigned char c) {
	std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
}

void displayTree(Node *head) {
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

void trieConstruction(Node **head, vector<vector<unsigned char>> &sigs) {
	*head = new Node('R', false, true, NULL);
	Node *prevNode = NULL, *newNode = NULL, *child = NULL;
	Node *failureNode = NULL;
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
				newNode = new Node(sig[i], false, false, NULL);
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

Node *curPos = NULL;

void inputTraversal(Node *head, unsigned char &val, bool last) {
	Node *child = NULL;
	bool present = false;
	int idx = -1;
	displayCharInHex(val);
	// cout << ": Children\n";
	for (int i = 0; i < curPos->getNoOfChildNodes(); ++i) {
		child = curPos->getChild(i);
		// displayCharInHex(val);
		// cout << " ";
		// displayCharInHex(child->getValue());
		// cout << "\n";
		if (val == child->getValue()) {
			// cout << "\ninside";
			present = true;
			idx = i;
			break;
		}
	}
	// cout << "\n";
	if (present) {
		curPos = curPos->getChild(idx);
		// cout << "\nfirst: ";
		// displayCharInHex(val);
		// cout << "\n";
		if (last) {
			curPos = curPos->getFailureNode();
			if (curPos != head)
				displayInHex(curPos->getPattern());
			curPos = head;
			cout << "\n";
		}
	} else if ((!present || last) && curPos != head) {
		// cout << "\nSecond: ";
		// displayCharInHex(val);
		// cout << "\n";
		curPos = curPos->getFailureNode();
		if (curPos != head)
			displayInHex(curPos->getPattern());
		curPos = head;
		inputTraversal(head, val, last);
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


	Node *headerHead = NULL, *footerHead = NULL;

	sort(headerSigs.begin(), headerSigs.end(), compareInsideCharVector);
	sort(footerSigs.begin(), footerSigs.end(), compareInsideCharVector);

	// AhoCorasick aho;

	// for (auto &val : aho.headerSigs) {
	// 	displayInHex(val);
	// }

	// cout << "\n\n";

	trieConstruction(&headerHead, headerSigs);
	trieConstruction(&footerHead, footerSigs);


	// cout << "\n\n";

	// displayTree(headerHead);
	// displayTree(footerHead);

	curPos = headerHead;
	for (unsigned char &val : inputVector) {
		if (val == inputVector.back())
			inputTraversal(headerHead, val, true);
		else
			inputTraversal(headerHead, val, false);
	}
	cout << "\n\n";

	curPos = footerHead;
	for (unsigned char &val : inputVector) {
		if (val == inputVector.back())
			inputTraversal(footerHead, val, true);
		else
			inputTraversal(footerHead, val, false);
	}

	return 0;
}
