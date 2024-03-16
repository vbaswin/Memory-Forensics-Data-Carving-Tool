#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QStackedWidget>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <fstream>
#include <sstream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	connect(ui->chooseFile, &QPushButton::clicked, this, &MainWindow::chooseFileButtonClicked);

	connect(ui->analyseButton, &QPushButton::clicked, this, &MainWindow::goToLoadingPage);
	connect(ui->goBackToMainPageButton, &QPushButton::clicked, this, &MainWindow::goBackToMainPage);


	connect(ui->selectCategoryComboBox, &QComboBox::currentTextChanged, this, &MainWindow::categorySelect);

}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::goToLoadingPage() {
	// _02Loading *loadPage = new _02Loading();

	// ui->stackedWidget->addWidget(loadPage);
	ui->stackedWidget->setCurrentWidget(ui->LoadPage);

	string categoryType = ui->selectCategoryComboBox->currentText().toStdString();
	string fileType = ui->categorySpecificSignatureComboBox->currentText().toStdString();
	ui->outputDiplayTextBrowser->clear();

	// ==> using C++ specific functions to avoid modifications to the original file - if we use
	// Qt specific ones <==

	map<string, map<string, vector<pair<int, string>>>> SigDB = {
		{
		 "Image", {
				{"JPG", {{0,"FF D8 FF"}}},
				{"PNG", {{0, "89 50 4E 47 0D 0A 1A 0A"}}},
				{"GIF", {{0, "47 49 46 38"}, {0, "47 49 46 37"}}}
			}
		},
		{
		 "Video", {
				{"MP4", {{4, "00 00 00 20 66 74 79 70 69 73 6F 6D 00 00 00 00 69 73 6F 6D"}}},
				{"MKV", {{0, "1A 45 DF A3"}}}
			}
		},
		{
		 "Audio", {
				{"MP3", {{0, "FF FB"}, {0, "FF F3"}, {0,"FF F2"}}}
			}
		},
		{
		 "Doc", {
				{"PDF", {{0, "25 50 44 46"}}}
			}
		}
	};


	if (categoryType == "All") {
		for (auto &categories: SigDB) {
			for (auto &types: categories.second) {
				for (auto &signatures: types.second) {
					// qInfo() << "Hello\n";
					// qInfo() << signatures.second << "\n";
					searchSignature(inputFileContent, signatures.second, types.first);
				}
			}
		}
	} else {

		if (fileType == "All") {
			for (auto &types: SigDB[categoryType]) {
				for (auto &signatures: types.second) {
					// qInfo() << "Hello\n";
					// qInfo() << signatures.second << "\n";
					searchSignature(inputFileContent, signatures.second, types.first);
				}
			}
		} else {
				for (auto &signatures: SigDB[categoryType][fileType]) {
					// qInfo() << "Hello\n";
					// qInfo() << signatures.second << "\n";
					searchSignature(inputFileContent, signatures.second, fileType);
				}
		}


	}
	inputFile.close();

}

void MainWindow::goBackToMainPage() {
	ui->stackedWidget->setCurrentWidget(ui->MainPage);
}

void MainWindow::categorySelect() {
	QString categoryString = ui->selectCategoryComboBox->currentText();
	// qInfo() << categoryString << "\n";
	// qInfo() << ui->selectCategoryComboBox->currentIndex();

	QMap<QString, QStringList> CategoryDB = {
		{ "Image", { "JPG","PNG", "GIF"} },
		{ "Video", { "MP4", "MKV" }},
		{ "Audio",  { "MP3" }},
		{ "Doc",{ "PDF"}}
	};

	ui->categorySpecificSignatureComboBox->clear();
	ui->categorySpecificSignatureComboBox->addItem("All");
	ui->categorySpecificSignatureComboBox->addItems(CategoryDB[categoryString]);

}

void MainWindow::chooseFileButtonClicked() {
	// be default use home dir for any OS, so QDir::homepath
	QString filePath = QFileDialog::getOpenFileName(this, "Open Dump File", QDir::homePath());


	inputFile.open(filePath.toStdString(), ios::binary);

	if (!inputFile.is_open()) {
		QMessageBox::warning(this, "Error", "Cannot open file");
	}

	string currentDir = QDir::currentPath().toStdString();
	string hexTextFilePath = currentDir + "/output_hex.txt";

	// // Open hex text file for writing in text mode
	std::ofstream hexTextFile(hexTextFilePath);
	if (!hexTextFile.is_open()) {
		qInfo() << "Error opening hex text file.\n";
	}

	// // Set stream to output hexadecimal values
	hexTextFile << std::hex << std::setfill('0');

			  // Read and convert each byte from binary file to hexadecimal format
	// unsigned char byte;
	// int i = 0, j = 10000;
	// while (j >= 0) {
	// 	inputFile.read(reinterpret_cast<char *>(&byte), sizeof(byte));
	// 	hexTextFile << std::setw(2) << static_cast<int>(byte) << ' ';

	// 	i += 2;
	// 	if (i >= 80) {
	// 		i = 0;
	// 		hexTextFile << std::endl;
	// 	}
	// 	--j;
	// }

	std::ostringstream hexStringStream;
	unsigned char byte;

	while (inputFile.read(reinterpret_cast<char *>(&byte), sizeof(byte))) {
		hexStringStream << std::setw(2) << std::hex << static_cast<int>(byte) << ' ';
	}

	inputFileContent = hexStringStream.str();
	hexTextFile << inputFileContent;

	// string chumma = "hello";
	// hexTextFile << chumma << "\n";
	// qInfo() << hexTextFilePath;
	// qInfo() << QCoreApplication::applicationDirPath();

	// qInfo() << inputFileContent;

	hexTextFile.close();

	// string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	// inputFileContent = fileContent;


	// string hexFilePath = "output.hex";

	// ofstream hexFile(hexFilePath);

	// if (!hexFile.is_open()) {
	// 	qInfo() << "Error opening HEX file.\n";
	// 	// return 1;
	// }

	// // Set stream to output hexadecimal values
	// hexFile << std::hex << std::setfill('0');

	// 		  // Read and convert each byte from DMP file to HEX format
	// char byte;
	// while (inputFile.get(byte)) {
	// 	hexFile << std::setw(2) << static_cast<int>(static_cast<unsigned char>(byte)) << ' ';
	// }

	// hexFile.close();


	// qInfo() << searchSignature(fileContent, categoryType, fileType, 0, SigDB);

			  // Search for the substring in the file content
	// qInfo() << filePath << "\n";
}

int MainWindow::searchSignature(string fileContent, string signature, string fileType) {

	size_t position = fileContent.find(signature);
	QString msg;

	if (position != std::string::npos) {
		msg = QString::fromStdString(fileType) + " found at position: " + QString::number(position) + "\n";
		ui->outputDiplayTextBrowser->append(msg);
		return position;
	}
		// qInfo() << "Substring not found." << "\n";

	return -1;
}
