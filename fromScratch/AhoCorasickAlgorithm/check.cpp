#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
	char ch = 0x80;
	cout << std::hex << +ch << "\n";


	std::string str = "Hello";
	for (char c : str) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
	}
	std::cout << "\n";
	return 0;
}
