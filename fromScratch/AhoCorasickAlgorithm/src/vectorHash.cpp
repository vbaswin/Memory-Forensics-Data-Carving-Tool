#include "../inc/basic.h"

struct VectorHash {
	size_t operator()(const std::vector<unsigned char> &v) const {
		size_t hash = 2166136261u;
		for (unsigned char i : v) {
			hash ^= i;
			hash *= 16777619;
		}
		return hash;
	}
};