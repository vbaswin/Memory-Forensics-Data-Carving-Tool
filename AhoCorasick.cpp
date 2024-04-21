#include "Node.h"
#include "basic.h"
#include "vectorHash.h"
using namespace std;

class AhoCorasick {
public:
	Node *head_ = NULL;
	Node *curPos_ = NULL;
	Node *foundPattern_ = NULL;
	bool last_ = false;
	vector<vector<unsigned char>> sigs_;

			  // unordered_map<vector<unsigned char>, pair<string, vector<unsigned char>>, VectorHash> compMap_;

			  // AhoCorasick(vector<vector<unsigned char>> sigs, unordered_map<vector<unsigned char>, pair<string, vector<unsigned char>>, VectorHash> compMap) : sigs_(sigs), compMap_(compMap) {
	AhoCorasick(vector<vector<unsigned char>> sigs) : sigs_(sigs) {
		sort(sigs_.begin(), sigs_.end(), compareInsideCharVector);
	}

	AhoCorasick(const AhoCorasick &other) {
		head_ = other.head_;
		curPos_ = other.curPos_;
		foundPattern_ = other.foundPattern_;
		last_ = other.last_;
		sigs_ = other.sigs_;
	}

	static bool compareInsideCharVector(vector<unsigned char> a, vector<unsigned char> b) {
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
		cout << "\n";
		cout << dec;
	}

	void displayCharInHex(unsigned char c) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
	}

	void displayTree(Node *head) {
		if (head == NULL)
			return;
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

	void displayTreeParentFn() {
		displayTree(head_);
	}

	void trieConstruction() {
		head_ = new Node('R', false, true, NULL);
		curPos_ = head_;
		Node *prevNode = NULL, *newNode = NULL, *child = NULL;
		Node *failureNode = NULL;
		for (auto &sig : sigs_) {
			prevNode = head_;
			failureNode = head_;
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
			prevNode->setFailureNode(prevNode);
			prevNode->setEndNode(sig);
		}
	}

	bool inputTraversal(unsigned char &val) {
		Node *child = NULL;
		bool present = false;
		int idx = -1;
		bool returnVal = false;
		// displayCharInHex(val);
		for (int i = 0; i < curPos_->getNoOfChildNodes(); ++i) {
			child = curPos_->getChild(i);
			if (val == child->getValue()) {
				present = true;
				idx = i;
				break;
			}
		}
		if (present) {
			curPos_ = curPos_->getChild(idx);
			if (last_) {
				curPos_ = curPos_->getFailureNode();
				if (curPos_ != head_) {
					// displayInHex(curPos_->getPattern());
					// compMap_[curPos_->getPattern()];
					foundPattern_ = curPos_;
					returnVal = true;
				}
				curPos_ = head_;
				// cout << "\n";
			}
		} else if ((!present || last_) && curPos_ != head_) {
			curPos_ = curPos_->getFailureNode();
			if (curPos_ != head_) {
				// displayInHex(curPos_->getPattern());
				// cout << compMap_[curPos_->getPattern()];
				foundPattern_ = curPos_;
				returnVal = true;
			}
			curPos_ = head_;
			inputTraversal(val);
		}
		return returnVal;
	}

	void setlastTrue() {
		last_ = true;
	}
};
