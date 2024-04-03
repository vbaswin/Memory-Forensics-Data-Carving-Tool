#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

struct Node {
	std::unordered_map<char, Node *> children;
	bool is_terminal = false;
	int pattern_id;	   // Index of the pattern ending at this node (if any)
	Node *failure;	   // Failure function (suffix link)
};

class AhoCorasick {
private:
	Node *root;

	void buildFailureLinks(Node *node) {
		std::queue<Node *> q;
		q.push(root);

		while (!q.empty()) {
			Node *current = q.front();
			q.pop();

			for (auto &[ch, child] : current->children) {
				Node *failure = root;
				while (failure != nullptr && !failure->children.count(ch)) {
					failure = failure->failure;
				}
				child->failure = failure ? failure->children[ch] : root;
				if (failure && failure->is_terminal) {
					child->is_terminal = true;
				}
				q.push(child);
			}
		}
	}
public:
	AhoCorasick() {
		root = new Node();
	}

	~AhoCorasick() {
		delete root;	// Recursive deletion of nodes
	}

	void insertPattern(const std::string &pattern, int pattern_id) {
		Node *current = root;
		for (char ch : pattern) {
			if (!current->children.count(ch)) {
				current->children[ch] = new Node();
			}
			current = current->children[ch];
		}
		current->is_terminal = true;
		current->pattern_id = pattern_id;
	}

	void search(const std::string &text, std::vector<int> &matches) {
		Node *current = root;
		for (char ch : text) {
			while (current != nullptr && !current->children.count(ch)) {
				current = current->failure;
			}
			current = current ? current->children[ch] : root;
			if (current->is_terminal) {
				matches.push_back(current->pattern_id);
			}
			// Check output links of current node for any matching patterns
			for (Node *output = current; output != root; output = output->failure) {
				if (output->is_terminal) {
					matches.push_back(output->pattern_id);
				}
			}
		}
	}
};

int main() {
	std::vector<std::string> patterns = {"find", "fine", "fined"};
	AhoCorasick ac;
	for (int i = 0; i < patterns.size(); ++i) {
		ac.insertPattern(patterns[i], i);
	}

	std::string text = "This is a fine text to find something.";
	std::vector<int> matches;
	ac.search(text, matches);

	for (int match : matches) {
		std::cout << "Found pattern " << patterns[match] << std::endl;
	}

	return 0;
}
