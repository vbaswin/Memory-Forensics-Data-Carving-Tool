
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>
using namespace std;

int main() {
	char end[] = {0x00, 0x3b};
	for (int i = 0; i < sizeof(end); ++i) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(end[i]) << " ";
	}
	cout << std::dec;
	cout << "\nhello";
	cout << end << "\n";

	return 0;
}
