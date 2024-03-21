#include <filesystem>
#include <fstream>
#include <string>

int main() {
	int fileNo = 20;

	std::string folder = "output";
	std::string subfolder = "gif";

	// Create the directories if they do not exist
	std::filesystem::create_directories(folder + "/" + subfolder);

	std::string filename = std::to_string(fileNo++) + ".gif";
	std::ofstream outfile;

	// Open the file in the created directory
	outfile.open(folder + "/" + subfolder + "/" + filename, std::ios::out | std::ios::binary);


	outfile.close();
}
