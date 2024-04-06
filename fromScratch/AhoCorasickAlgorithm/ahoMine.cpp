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
	// if (isRootChild)
	// cout << "\n";
	// cout << +head->getValue() << " -> ";
	std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((unsigned char)head->getValue()) << dec;
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

	// queue<node *> q;
	// for (node *temp = head; temp != NULL; temp = temp->getNextNode(0))
	// cout << temp->getValue() << " -> ";

	// cout << head->getNextNode(0)->getValue() << "\n";
	// cout << head->getNextNode(0)->getNextNode(0)->getValue() << "\n";
	// cout << head->getNextNode(1)->getValue() << "\n";
	// cout << head->getNextNode(1)->getNextNode(0)->getValue() << "\n";

	// q.push(head);
	// // int level = 0;
	// while (!q.empty()) {
	// 	node *curNode = q.front();
	// 	cout << curNode->getValue() << " ";
	// 	for (int i = 0; i < curNode->getNoOfChildNodes(); ++i) {
	// 		q.push(curNode->getChild(i));
	// 	}
	// 	q.pop();
	// }
}

int main() {
	node *head = new node('R', false, true, NULL), *prevNode = NULL, *newNode = NULL, *child = NULL;


	// ====> sort first lexicologicallly or by size

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
		{0x50, 0x89},
		// {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}
		//
	};


	sort(sigs.begin(), sigs.end(), compareInsideCharVector);

	for (auto &val : sigs) {
		displayInHex(val);
	}

	cout << "\n\n";

	node *failureNode = NULL;
	for (auto &sig : sigs) {
		prevNode = head;
		failureNode = head;
		// cout << "Sig: " << sig << "\n";
		for (int i = 0; i < (int)sig.size(); ++i) {
			int present = false;
			int noOfChild = prevNode->getNoOfChildNodes();
			for (int j = 0; j < noOfChild; ++j) {
				child = prevNode->getChild(j);
				// cout << "Child value: " << child->getValue() << " Sig value: " << sig[i] << "\n";
				if (child->getValue() == sig[i]) {
					cout << "present: ";
					displayCharInHex(sig[i]);
					cout << " Previous: ";
					displayCharInHex(prevNode->getValue());
					cout << "\n";
					if (child->isEnd()) {
						cout << "Hello\n";
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
				prevNode->setNextNode(newNode);
				prevNode = newNode;
			}
		}
		prevNode->setEndNode(sig);
		displayInHex(sig);
	}

	cout << "\n\n";

	displayTree(head);




	return 0;
}
