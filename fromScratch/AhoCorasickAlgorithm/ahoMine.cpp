#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

class node {
	char val_;
	bool endNode_;
	class node *failureLink_;
	// vector<class node *> nextNodes_;
	class node *nextNodes_;
public:
	node(char val, bool endNode, node *failureLink) : val_(val), endNode_(endNode), failureLink_(failureLink), nextNodes_(NULL) {
		// cout << "inside node constructor\n";
	}

	node *getNextNode() {
		return nextNodes_;
	}

	char getNodeValue() {
		return val_;
	}

	void setNextNode(node *nextNode) {
		nextNodes_ = nextNode;
	}
};

void displayTree(node *head) {
	for (node *temp = head; temp != NULL; temp = temp->getNextNode())
		cout << temp->getNodeValue() << " -> ";
}

int main() {
	node *head = NULL, *prevNode = NULL, *newNode = NULL;

	vector<string> sig = {"abcde"};

	for (auto &val : sig[0]) {
		if (head == NULL) {
			head = new node(val, false, NULL);
			prevNode = head;
		} else {
			newNode = new node(val, false, NULL);
			prevNode->setNextNode(newNode);
			prevNode = newNode;
		}
	}

	displayTree(head);

	return 0;
}
