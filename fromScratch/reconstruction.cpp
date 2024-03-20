#include <fstream>
#include <iostream>

using namespace std;

int main() {
	// Define chunk size
	const int chunkSize = 1024;	   // Adjust as needed

	// Open the input file
	ifstream infile("../File07.gif", ios::binary);

	if (!infile.is_open()) {
		cerr << "Error opening input file!" << endl;
		return 1;
	}

	// Create a buffer for the chunk
	char buffer[chunkSize];

	// Open the output file
	ofstream outfile("output.gif", ios::out | ios::binary);

	if (!outfile.is_open()) {
		cerr << "Error opening output file!" << endl;
		return 1;
	}

	// Read in chunks and write to output
	while (infile.read(buffer, chunkSize)) {
		// Get the number of bytes actually read (might be less than chunkSize)
		streamsize bytesRead = infile.gcount();

		// Write the read bytes to the output file
		outfile.write(buffer, bytesRead);
	}

	infile.close();
	outfile.close();

	cout << "File processed in chunks and written to output.gif successfully." << endl;

	return 0;
}
