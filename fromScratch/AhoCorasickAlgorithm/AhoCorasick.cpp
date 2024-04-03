#include <iostream>
#include <queue>
#include <string>
#include <vector>

using namespace std;

const int ALPHABET_SIZE = 26;

struct TrieNode {
	struct TrieNode *children[ALPHABET_SIZE];
	bool isEndOfWord;
	struct TrieNode *failure;
	vector<int> occurances;
};

struct TrieNode *getNode(void) {
	struct TrieNode *pNode = new TrieNode;
	pNode->isEndOfWord = false;
	for (int i = 0; i < ALPHABET_SIZE; i++)
		pNode->children[i] = NULL;
	pNode->failure = NULL;
	return pNode;
}

void insert(struct TrieNode *root, string key, int index) {
	struct TrieNode *pCrawl = root;
	for (int i = 0; i < key.length(); i++) {
		int index = key[i] - 'a';
		if (!pCrawl->children[index])
			pCrawl->children[index] = getNode();
		pCrawl = pCrawl->children[index];
	}
	pCrawl->isEndOfWord = true;
	pCrawl->occurances.push_back(index);
}

void computeFailureFunction(struct TrieNode *root) {
	queue<TrieNode *> q;
	if (root) {
		for (int i = 0; i < ALPHABET_SIZE; i++) {
			if (root->children[i]) {
				root->children[i]->failure = root;
				q.push(root->children[i]);
			}
		}
	}
	while (!q.empty()) {
		TrieNode *node = q.front();
		q.pop();
		for (int i = 0; i < ALPHABET_SIZE; i++) {
			TrieNode *child = node->children[i];
			if (child) {
				TrieNode *failure = node->failure;
				while (failure && !failure->children[i])
					failure = failure->failure;
				if (failure)
					child->failure = failure->children[i];
				else
					child->failure = root;
				q.push(child);
			}
		}
	}
}

void searchWords(struct TrieNode *root, string text) {
	TrieNode *node = root;
	for (int i = 0; i < text.size(); i++) {
		while (node && !node->children[text[i] - 'a'])
			node = node->failure;
		if (!node)
			node = root;
		else
			node = node->children[text[i] - 'a'];
		TrieNode *temp = node;
		while (temp != root) {
			if (temp->isEndOfWord) {
				for (int j = 0; j < temp->occurances.size(); j++)
					cout << "Pattern found at index " << i - temp->occurances[j] << endl;
			}
			temp = temp->failure;
		}
	}
}

int main() {
	string text = "abcde";
	vector<string> patterns = {"abc", "bcd", "cde"};
	struct TrieNode *root = getNode();
	for (int i = 0; i < patterns.size(); i++)
		insert(root, patterns[i], patterns[i].size());
	computeFailureFunction(root);
	searchWords(root, text);
	return 0;
}
