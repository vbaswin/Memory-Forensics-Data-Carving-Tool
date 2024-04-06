#include "node.cpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

bool compareInsideCharVector(vector<char> a, vector<char> b) {
	return a.size() < b.size();
}

void displayInHex(vector<char> str, int sz) {
	cout << "sz: " << sz << "\t";
	for (char c : str) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
	}
	cout << "\n";
	cout << dec;
}

void displayTree(node *head) {
	// if (isRootChild)
	// cout << "\n";
	cout << std::hex;
	// cout << +head->getValue() << " -> ";
	std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(head->getValue()) << dec;
	if (head->getFailureNode() != NULL)
		cout << " Failure: " << head->getFailureNode()->getValue();
	cout << " -> ";
	if (head->isEnd()) {
		cout << "\nSig: ";
		displayInHex(head->getPattern(), head->getPattern().size());
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

	std::vector<std::vector<char>> sigs = {
		{'A', 'B', 'C', 'D'},
		{'A', 'B', 'C', 'D', 'E'},
		{'A', 'B'},
		{'G', 'I', 'F', '8', '7', 'a'},
		{'G', 'I', 'F', '8', '9', 'a'},
		{'J', 'P', 'E', 'G'},
		{'J', 'P', 'A', 'M'},
		{'J', 'P', 'E', 'A', 'M', 'M', 'X'},
		// {'\x00', '\x3B'},
		{'\x89', '\x50', '\x4E', '\x47', '\x0D', '\x0A', '\x1A', '\x0A'}};

	sort(sigs.begin(), sigs.end(), compareInsideCharVector);

	for (auto &val : sigs) {
		displayInHex(val, val.size());
	}

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
					// cout << "present: " << sig[i] << "\n";
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
	}

	displayTree(head);

	return 0;
}
