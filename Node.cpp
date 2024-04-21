#include "basic.h"
using namespace std;

class Node {
	unsigned char val_;
	bool endNode_;
	bool isRoot_;
	vector<unsigned char> pattern_;
	class Node *failureLink_;
	vector<class Node *> nextNodes_;
	int noOfChild_;
	// class Node *nextNodes_;
public:
	Node(unsigned char val, bool endNode, bool isRoot, Node *failureLink) : val_(val), endNode_(endNode), isRoot_(isRoot), failureLink_(failureLink), noOfChild_(0) {
		nextNodes_.reserve(1);
		pattern_.reserve(1);
	}

	bool checkRoot() {
		return isRoot_;
	}

	Node *getNextNode(int idx) {
		return nextNodes_[idx];
	}

			  // I returned unsigned int here ;)))
	unsigned char getValue() {
		return val_;
	}

	Node *getChild(int idx) {
		return nextNodes_[idx];
	}

	void addChildNode(Node *child) {
		nextNodes_.push_back(child);
		++noOfChild_;
	}

	int getNoOfChildNodes() {
		return noOfChild_;
	}

	void setEndNode(vector<unsigned char> v) {
		endNode_ = true;
		pattern_ = v;
	}

	bool isEnd() {
		return endNode_;
	}

	void setFailureNode(Node *failure) {
		failureLink_ = failure;
	}

	vector<unsigned char> getPattern() {
		return pattern_;
	}

	Node *getFailureNode() {
		return failureLink_;
	}
};
