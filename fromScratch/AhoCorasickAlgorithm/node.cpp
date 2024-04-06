#include <iostream>
#include <vector>
using namespace std;

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

	void setFailureNode(node *failure) {
		failureLink_ = failure;
	}

	vector<char> getPattern() {
		return pattern_;
	}

	node *getFailureNode() {
		return failureLink_;
	}
};