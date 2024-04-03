#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

class node {
	char val_;
	bool endNode_;
	bool isRoot_;
	class node *failureLink_;
	vector<class node *> nextNodes_;
	int noOfChild_;
	// class node *nextNodes_;
public:
	node(char val, bool endNode, bool isRoot, node *failureLink) : val_(val), endNode_(endNode), isRoot_(isRoot), failureLink_(failureLink), noOfChild_(0) {
		nextNodes_.reserve(1);
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
};

void displayTree(node *head) {
	queue<node *> q;
	// for (node *temp = head; temp != NULL; temp = temp->getNextNode(0))
	// cout << temp->getValue() << " -> ";

	// cout << head->getNextNode(0)->getValue() << "\n";
	// cout << head->getNextNode(0)->getNextNode(0)->getValue() << "\n";
	// cout << head->getNextNode(1)->getValue() << "\n";
	// cout << head->getNextNode(1)->getNextNode(0)->getValue() << "\n";

	q.push(head);
	int level = 0;
	while (!q.empty()) {
		node *curNode = q.front();
		cout << curNode->getValue() << " ";
		for (int i = 0; i < curNode->getNoOfChildNodes(); ++i) {
			q.push(curNode->getChild(i));
		}
		q.pop();
	}
}

int main() {
	node *head = new node('R', false, true, NULL), *prevNode = NULL, *newNode = NULL, *child = NULL;

	vector<string> sigs = {"abcde", "123"};
	for (auto &sig : sigs) {
		prevNode = head;
		for (auto &val : sig) {
			int present = false;
			for (int i = 0; i < prevNode->getNoOfChildNodes(); ++i) {
				child = prevNode->getChild(i);
				if (child->getValue() == val) {
					prevNode = child;
					present = true;
					break;
				}
			}
			if (!present) {
				newNode = new node(val, false, false, NULL);
				prevNode->setNextNode(newNode);
				prevNode = newNode;
			}
		}
	}

	displayTree(head);

	return 0;
}
