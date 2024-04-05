#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

void displayInHex(vector<char> str, int sz) {
	cout << "sz: " << sz << "\t";
	for (char c : str) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
	}
	cout << "\n";
	cout << dec;
}

class node {
	char val_;
	bool endNode_;
	bool isRoot_;
	vector<char> pattern_;
	class node *failureLink_;
	vector<class node *> nextNodes_;
	int noOfChild_;
	// class node *nextNodes_;
public:
	node(char val, bool endNode, bool isRoot, node *failureLink) : val_(val), endNode_(endNode), isRoot_(isRoot), failureLink_(failureLink), noOfChild_(0) {
		nextNodes_.reserve(1);
		pattern_.reserve(1);
	}

	bool checkRoot() {
		return isRoot_;
	}

	node *getNextNode(int idx) {
		return nextNodes_[idx];
	}

	char getValue() {
		return val_;
	}

	node *getChild(int idx) {
		return nextNodes_[idx];
	}

	void setNextNode(node *nextNode) {
		nextNodes_[noOfChild_++] = nextNode;
	}

	void addChildNode(node *child) {
		nextNodes_[noOfChild_++] = child;
	}

	int getNoOfChildNodes() {
		return noOfChild_;
	}

	void setEndNode(vector<char> v) {
		endNode_ = true;
		pattern_ = v;
	}

	bool isEnd() {
		return endNode_;
	}

	vector<char> getPattern() {
		return pattern_;
	}
};

void displayTree(node *head) {
	// if (isRootChild)
	// cout << "\n";
	cout << std::hex;
	cout << +head->getValue() << " -> ";
	if (head->isEnd()) {
		cout << "\nSig: ";
		displayInHex(head->getPattern(), head->getPattern().size());
		cout << "\n";
	}
	cout << std::dec;
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

	// vector<string> sigs = {
	// 	"GIF87a",
	// 	"GIF89a",
	// 	"JPEG",
	// 	"JPAM",
	// 	"JPEAMMX",
	// 	"\x00\x3B"};
	// // "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"};

	std::vector<std::vector<char>> sigs = {
		{'G', 'I', 'F', '8', '7', 'a'},
		{'G', 'I', 'F', '8', '9', 'a'},
		{'J', 'P', 'E', 'G'},
		{'J', 'P', 'A', 'M'},
		{'J', 'P', 'E', 'A', 'M', 'M', 'X'},
		{'\x00', '\x3B'},
		{'\x89', '\x50', '\x4E', '\x47', '\x0D', '\x0A', '\x1A', '\x0A'}};

	for (auto &sig : sigs) {
		prevNode = head;
		// cout << "Sig: " << sig << "\n";
		for (int i = 0; i < (int)sig.size(); ++i) {
			int present = false;
			int noOfChild = prevNode->getNoOfChildNodes();
			for (int j = 0; j < noOfChild; ++j) {
				child = prevNode->getChild(j);
				// cout << "Child value: " << child->getValue() << " Sig value: " << sig[i] << "\n";
				if (child->getValue() == sig[i]) {
					// cout << "present: " << sig[i] << "\n";
					prevNode = child;
					present = true;
					break;
				}
			}
			if (!present) {
				newNode = new node(sig[i], false, false, NULL);
				prevNode->setNextNode(newNode);
				prevNode = newNode;
			}
		}
		prevNode->setEndNode(sig);
	}

	displayTree(head);

	return 0;
}
