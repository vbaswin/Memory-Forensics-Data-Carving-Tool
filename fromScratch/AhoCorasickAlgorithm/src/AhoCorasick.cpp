#include "../inc/Node.h"
#include "../inc/basic.h"
using namespace std;

class AhoCorasick {
public:
	Node *curPos_ = NULL;
	Node *head_ = NULL;
	vector<vector<unsigned char>> sigs_;

	// AhoCorasick(vector<vector<unsigned char>> sigs): sigs_(sigs) {}

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

	void inputTraversal(Node *head, unsigned char &val, bool last) {
		Node *child = NULL;
		bool present = false;
		int idx = -1;
		displayCharInHex(val);
		// cout << ": Children\n";
		for (int i = 0; i < curPos_->getNoOfChildNodes(); ++i) {
			child = curPos_->getChild(i);
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
			curPos_ = curPos_->getChild(idx);
			// cout << "\nfirst: ";
			// displayCharInHex(val);
			// cout << "\n";
			if (last) {
				curPos_ = curPos_->getFailureNode();
				if (curPos_ != head)
					displayInHex(curPos_->getPattern());
				curPos_ = head;
				cout << "\n";
			}
		} else if ((!present || last) && curPos_ != head) {
			// cout << "\nSecond: ";
			// displayCharInHex(val);
			// cout << "\n";
			curPos_ = curPos_->getFailureNode();
			if (curPos_ != head)
				displayInHex(curPos_->getPattern());
			curPos_ = head;
			inputTraversal(head, val, last);
		}
	}
};
