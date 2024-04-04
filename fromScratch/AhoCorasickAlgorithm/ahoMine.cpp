#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

class node {
	char val_;
	bool endNode_;
	bool isRoot_;
	string pattern_;
	class node *failureLink_;
	vector<class node *> nextNodes_;
	int noOfChild_;
	// class node *nextNodes_;
public:
	node(char val, bool endNode, bool isRoot, node *failureLink) : val_(val), endNode_(endNode), isRoot_(isRoot), pattern_(""), failureLink_(failureLink), noOfChild_(0) {
		nextNodes_.reserve(1);
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

	void setEndNode(string str) {
		endNode_ = true;
		pattern_ = str;
	}

	bool isEnd() {
		return endNode_;
	}

	string getPattern() {
		return pattern_;
	}
};

void displayTree(node *head, bool isRootChild) {
	if (isRootChild)
		cout << "\n";
	cout << head->getValue() << " -> ";
	if (head->isEnd())
		cout << "\nSig: " << head->getPattern() << "\n";
	for (int i = 0; i < head->getNoOfChildNodes(); ++i)
		displayTree(head->getChild(i), head->checkRoot());

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

	vector<string> sigs = {"ab", "abc", "ac"};
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

	displayTree(head, false);

	return 0;
}
