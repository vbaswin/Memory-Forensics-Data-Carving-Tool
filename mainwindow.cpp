#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QStackedWidget>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <Qstring>
#include <fstream>
#include <sstream>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <QThread>
#include <thread>
#include "basic.h"
#include "AhoCorasick.h"
#include "Node.h"
#include <QMutex>
#include <Qtimer>
#include "vectorHash.h"
#include "worker.h"
#include "sectionWorker.h"
using namespace std;

void searchMainFun();
const int MAX_SIZE = 500000000;
int noOfThreads = 1;
int curThreadNo = 0;

mutex debugFileMtx, outputFileMtx;
// gifFileMtx, pngFileMtx;

std::atomic<int> gifFileNo(1);
std::atomic<int> pngFileNo(1);
std::atomic<int> jpegFileNo(1);
std::atomic<int> pdfFileNo(1);
std::atomic<int> zipFileNo(1);

string outputFolderPath = "";

void createDirectoriesAndCleaning() {

filesystem::remove_all(outputFolderPath);

string gifSubFolder = "gif";
string pngSubFolder = "png";
string jpegSubFolder = "jpeg";
string pdfSubFolder = "pdf";
string zipSubFolder = "zip";

		  // Create the directories if they do not exist
filesystem::create_directories(outputFolderPath + "/" + gifSubFolder);
filesystem::create_directories(outputFolderPath + "/" + pngSubFolder);
filesystem::create_directories(outputFolderPath + "/" + jpegSubFolder);
filesystem::create_directories(outputFolderPath + "/" + pdfSubFolder);
filesystem::create_directories(outputFolderPath + "/" + zipSubFolder);
}



std::vector<std::vector<unsigned char>> headerSigs = {
	// GIF87a
	{0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
	// GIF89a
	{0x47, 0x49, 0x46, 0x38, 0x39, 0x61},
	// PNG
	{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
	// JPEG
	{0xFF, 0xD8},
	// PDF
	{0x25, 0x50, 0x44, 0x46},

	// ZIP
	{0x50, 0x4B, 0x03, 0x04}
	//
};

std::vector<std::vector<unsigned char>> footerSigs = {
	// GIF footer
	{0x00, 0x3B},
	// PNG footer
	{0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82},
	// JPEG
	{0xFF, 0xD9},
	// PDF
	{0x25, 0x25, 0x45, 0x4F, 0x46},
	// ZIP
	{0x50, 0x4B, 0x05, 0x06},

	//
};

unordered_map<vector<unsigned char>, pair<string, vector<unsigned char>>, VectorHash> headerComp = {
	// GIF87a
	{{0x47, 0x49, 0x46, 0x38, 0x37, 0x61}, {"GIF Header", {0x00, 0x3B}}},
	// GIF89a
	{{0x47, 0x49, 0x46, 0x38, 0x39, 0x61}, {"GIF Header", {0x00, 0x3B}}},
	// PNG
	{{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, {"PNG Header", {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}}},

	// JPEG
	{{0xFF, 0xD8}, {"JPEG Header", {0xFF, 0xD9}}},
	// PDF
	{{0x25, 0x50, 0x44, 0x46}, {"PDF Header", {0x25, 0x25, 0x45, 0x4F, 0x46}}},
	// ZIP
	{{0x50, 0x4B, 0x03, 0x04}, {"ZIP Header", {0x50, 0x4B, 0x05, 0x06}}}
	//
};

unordered_map<vector<unsigned char>, string, VectorHash> footerComp = {
	// GIF footer
	{{0x00, 0x3B}, "GIF Footer"},
	// PNG footer
	{{0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}, "PNG Footer"},

	// JPEG
	{{0xFF, 0xD9}, "JPEG Footer"},
	// PDF
	{{0x25, 0x25, 0x45, 0x4F, 0x46}, "PDF Footer"},
	// ZIP
	{{0x50, 0x4B, 0x05, 0x06}, "ZIP Footer"}

};

AhoCorasick AhoHeaderFirst(headerSigs), AhoFooterFirst(footerSigs);

// ofstream debugFile;
string inputFilePath = "";

void searchSignature(streampos start, streampos end, int threadNo) {
	int tempFileNo = -1;
	AhoCorasick AhoHeader(AhoHeaderFirst), AhoFooter(AhoFooterFirst);

	ifstream file(inputFilePath, ios::binary);

	if (!file) {
		cerr << "Thread " << threadNo << " could not open file\n";
		return;
	}

	file.seekg(start, ios::beg);

	int n = 100;
	unsigned char buffer[n];

	string filePath, fileType, fileExtension;
	ofstream outfile;

	size_t curFileSize = 0;

	bool fileStartCarving = false;
	int startBuffer = 0, endBuffer = n;
	vector<unsigned char> footerSig;
	for (streampos cur = start; file.read(reinterpret_cast<char *>(buffer), n) && cur <= end; cur += n) {
		// temp.clear();
		startBuffer = 0;
		int i;
		for (i = 0; i < n; ++i) {
			unsigned char &val = buffer[i];
			// cout << val << " ";
			// if (i == (n - 1)) {
			// 	AhoHeader.setlastTrue();
			// 	AhoFooter.setlastTrue();
			// 	AhoHeader.inputTraversal(val);
			// 	AhoFooter.inputTraversal(val);
			// } else {
			if (AhoHeader.inputTraversal(val)) {
				if (fileStartCarving) {
					// if (temp.size() != 0) {
					// 	outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
					// 	temp.clear();
					// }
					endBuffer = i;
					outfile.write(reinterpret_cast<char*>(buffer + startBuffer), endBuffer - startBuffer);
					outfile.write(reinterpret_cast<char *>(&footerSig[0]), footerSig.size());
					outfile.close();
					curFileSize = 0;
					startBuffer = i + 1;
					endBuffer = n;
				} else
					fileStartCarving = true;
				// for (const auto &c : AhoHeader.curPos_->getPattern())
				// debugFile << c << " ";
				streampos headerPos = file.tellg() - streamoff(n - i);

				vector<unsigned char> pattern = AhoHeader.foundPattern_->getPattern();
				string headerStr = headerComp[pattern].first;
				curFileSize += pattern.size();
				// debugFileMtx.lock();
				// debugFile << headerStr << " ";
				// debugFile << " at " << headerPos << " thread: " << threadNo << "\n";
				// debugFileMtx.unlock();


				if (headerStr == "GIF Header") {
					tempFileNo = gifFileNo++;
					fileType = "gif";
					fileExtension = ".gif";
				} else if (headerStr == "PNG Header") {
					tempFileNo = pngFileNo++;
					fileType = "png";
					fileExtension = ".png";
				}
				else if (headerStr == "JPEG Header") {
					tempFileNo = jpegFileNo++;
					fileType = "jpeg";
					fileExtension = ".jpeg";
				} else if (headerStr == "PDF Header") {
					tempFileNo = pdfFileNo++;
					fileType = "pdf";
					fileExtension = ".pdf";
				} else if (headerStr == "ZIP Header") {
					tempFileNo = zipFileNo++;
					fileType = "zip";
					fileExtension = ".zip";
				}
				// if (fileStartCarving)
				footerSig = headerComp[pattern].second;

				filePath = outputFolderPath + "/" + fileType + "/" + to_string(tempFileNo) + fileExtension;
				outfile.open(filePath, ios::out | ios::binary);
				// if (!outfile)
				// cout << "Failed to open file: " << filePath << std::endl;
				// closing more important
				// outfile.close();

				outfile.write(reinterpret_cast<char *>(&pattern[0]), pattern.size());
			}
			if (fileStartCarving) {
				// temp.push_back(val);
				++curFileSize;
				if (AhoFooter.inputTraversal(val) || curFileSize > MAX_SIZE) {
					// for (const auto &c : AhoHeader.curPos_->getPattern())
					// debugFile << c << " ";
					streampos headerPos = file.tellg() - streamoff(n - i);
					vector<unsigned char> footerPatternFound = AhoFooter.foundPattern_->getPattern();
					// debugFileMtx.lock();
					// debugFile << footerComp[footerPatternFound] << " ";
					// debugFile << " at " << headerPos << " thread: " << threadNo << "\n";
					// debugFileMtx.unlock();

					if (footerPatternFound == footerSig) {
						// outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
						// outfile.write((char *)buffer, i);
						endBuffer = i;
						outfile.write(reinterpret_cast<char*>(buffer + startBuffer), endBuffer - startBuffer);

						startBuffer = i + 1;
						endBuffer = n;
						outfile.close();
						// temp.clear();
						fileStartCarving = false;
						curFileSize = 0;
					}
				}
			}
			// AhoFooter.inputTraversal(val);
		}

		// if (temp.size() != 0) {
		// 	outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
		// 	curFileSize += temp.size();
		// 	temp.clear();
		// }
		if (startBuffer < i) {
			endBuffer = i;
			outfile.write(reinterpret_cast<char*>(buffer + startBuffer), endBuffer - startBuffer);
			curFileSize += (endBuffer - startBuffer);
			endBuffer = n;
		}
	}

	// temp.clear();

	while (fileStartCarving) {
		startBuffer = 0;
		int i;
		if (file.read(reinterpret_cast<char *>(buffer), n)) {
			for (i = 0; i < n; ++i) {
				unsigned char &val = buffer[i];
				// temp.push_back(val);
				if (AhoFooter.inputTraversal(val) || curFileSize > MAX_SIZE) {
					// for (const auto &c : AhoHeader.curPos_->getPattern())
					// debugFile << c << " ";
					streampos headerPos = file.tellg() - streamoff(n - i);
					vector<unsigned char> footerPatternFound = AhoFooter.foundPattern_->getPattern();
					// debugFileMtx.lock();
					// debugFile << footerComp[footerPatternFound] << " ";
					// debugFile << " at " << headerPos << " thread: " << threadNo << "\n";
					// debugFileMtx.unlock();

					if (footerPatternFound == footerSig) {
						// outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
						endBuffer = i;
						outfile.write(reinterpret_cast<char*>(buffer + startBuffer), endBuffer - startBuffer);

						outfile.close();
						// temp.clear();
						fileStartCarving = false;
						curFileSize = 0;
						startBuffer = i+1;
						endBuffer = n;
					}
				}
			}
		}
		// if (temp.size() != 0) {
		// 	outfile.write(reinterpret_cast<char *>(&temp[0]), temp.size());
		// 	curFileSize += temp.size();
		// 	temp.clear();
		// }
		if (startBuffer < i) {
			endBuffer = i;
			outfile.write(reinterpret_cast<char*>(buffer + startBuffer), endBuffer - startBuffer);
			curFileSize += (endBuffer - startBuffer);
			endBuffer = n;
		}
	}
}


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	timer = new QTimer(this);
	elapsedTimer = new QElapsedTimer();

	connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);

	connect(ui->chooseFile, &QPushButton::clicked, this, &MainWindow::chooseFileButtonClicked);
	connect(ui->chooseOutputFolder, &QPushButton::clicked, this, &MainWindow::chooseOutputFolderButtonClicked);

	connect(ui->analyseButton, &QPushButton::clicked, this, &MainWindow::goToLoadingPage);
	connect(ui->goBackToMainPageButton, &QPushButton::clicked, this, &MainWindow::goBackToMainPage);
	connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::cancelProgress);

	// connect(ui->gifCheckBox, &QCheckBox::stateChanged, this, &MainWindow::gifCheckBoxStateChanged);
	// connect(ui->selectCategoryComboBox, &QComboBox::currentTextChanged, this, &MainWindow::categorySelect);

}

MainWindow::~MainWindow() {
	delete ui;
}

MainWindow *prevWindowPtr;

void MainWindow::searchMainFun() {

	AhoHeaderFirst.trieConstruction();
	AhoFooterFirst.trieConstruction();

	createDirectoriesAndCleaning();

	ifstream file;

	file.open(inputFilePath, ios::binary);

	if (!file) {
		cerr << "Could not open file\n";
		return;
	}

	file.seekg(0, ios::end);
	streampos fileSize = file.tellg();

	file.close();

	QList<QThread*> threads;

	streampos splitSize = 100000000;
	noOfThreads = fileSize / splitSize;
	prevWindowPtr->ui->progressBar->setRange(0, noOfThreads);

	// cerr << noOfThreads << " " << curThreadNo;

	int threadNo = 0;
	for (streampos i = 0; i < fileSize; i += splitSize) {
		streampos end = std::min(i + splitSize, fileSize);

		QThread* thread = new QThread;
		sectionWorker* sectionWorker = new class sectionWorker(i, end, threadNo++);

		sectionWorker->moveToThread(thread);
		connect(thread, &QThread::started, sectionWorker, &sectionWorker::doWork);
		connect(sectionWorker, &sectionWorker::finished, prevWindowPtr , &MainWindow::updateProgressBar);
		connect(sectionWorker, &sectionWorker::finished, thread, &QThread::quit);
		connect(sectionWorker, &sectionWorker::finished, sectionWorker, &sectionWorker::deleteLater);
		connect(thread, &QThread::finished, thread, &QThread::deleteLater);

		thread->start();
		threads.append(thread);
	}

	// cerr << threads.size() << "\n";
	// int threadNumber = 1;
	// for (QThread* thread : threads) {
		// thread->wait();
		// cerr << threadNumber++ << " completed\n"
	// }
	// cerr << "Out of this function\n";

}

void MainWindow::updateProgressBar() {
	// cerr << "Iside update proress bar\n";
	mutex.lock();
	ui->progressBar->setValue(ui->progressBar->value() + 1);

	QString displayFileNoData = QString("No of Files Carved\n"
										"------------------------\n"
										"GIF\t: %1\n"
										"PNG\t: %2\n"
										"JPEG\t: %3\n"
										"PDF\t: %4\n"
										"ZIP\t: %5\n")
									.arg(gifFileNo - 1)
									.arg(pngFileNo - 1)
									.arg(jpegFileNo - 1)
									.arg(pdfFileNo - 1)
									.arg(zipFileNo - 1);


	ui->outputDiplayTextBrowser->setText(displayFileNoData);
	QString threadText = QString("%1 threads completed execution").arg(++curThreadNo);
	ui->outputDiplayTextBrowser->append(threadText);

	if (curThreadNo == noOfThreads) {
		ui->outputDiplayTextBrowser->append("\nFile carving completed\n");
		timer->stop();

		ui->outputDiplayTextBrowser->append(QString("Output Folder Path: %1").arg(QString::fromStdString(outputFolderPath)));
	}

	mutex.unlock();
}

void MainWindow::goToLoadingPage() {

	// addSignatures();

	// _02Loading *loadPage = new _02Loading();

	// ui->stackedWidget->addWidget(loadPage);
	ui->stackedWidget->setCurrentWidget(ui->LoadPage);

	// string categoryType = ui->selectCategoryComboBox->currentText().toStdString();
	// string fileType = ui->categorySpecificSignatureComboBox->currentText().toStdString();
	// ui->outputDiplayTextBrowser->clear();
	clearGlobalVarsAndOutput();

	elapsedTimer->start();
	timer->start(1000); // update every second

	prevWindowPtr = this;

	QThread* thread = new QThread;
	MainWindow* mainWindow = new MainWindow;
	Worker* worker = new Worker(mainWindow);

	worker->moveToThread(thread);
	connect(thread, &QThread::started, worker, &Worker::doWork);
	connect(worker, &Worker::finished, this, &MainWindow::displayCompletedMsg);
	connect(worker, &Worker::finished, thread, &QThread::quit);
	connect(worker, &Worker::finished, worker, &Worker::deleteLater);
	connect(thread, &QThread::finished, thread, &QThread::deleteLater);

	thread->start();
}

void MainWindow::updateTime() {
		int elapsed = elapsedTimer->elapsed() / 1000; // get elapsed seconds
		int seconds = elapsed % 60;
		int minutes = (elapsed / 60) % 60;
		int hours = (elapsed / 3600) % 24;

		QString timeText = QString("%1 : %2 : %3")
							   .arg(hours, 2, 10, QChar('0'))
							   .arg(minutes, 2, 10, QChar('0'))
							   .arg(seconds, 2, 10, QChar('0'));

		ui->timeLabel->setText(timeText);
}

void MainWindow::searchSignatureFirst() {
	cerr << "Inside search signature first\n";
}

void MainWindow::clearGlobalVarsAndOutput() {
	noOfThreads = 1;
	curThreadNo = 0;


	gifFileNo = 1;
	pngFileNo = 1;
	jpegFileNo = 1;
	pdfFileNo = 1;
	zipFileNo = 1;

	mutex.lock();
	QString displayFileNoData = QString("No of Files Carved\n"
										"------------------------\n"
										"GIF\t: 0\n"
										"PNG\t: 0\n"
										"JPEG\t: 0\n"
										"PDF\t: 0\n"
										"ZIP\t: 0\n");

	ui->outputDiplayTextBrowser->setText(displayFileNoData);

	QString threadText = QString("0 theads completed execution");
	ui->outputDiplayTextBrowser->append(threadText);

	mutex.unlock();

}

void MainWindow::goBackToMainPage() {
	clearGlobalVarsAndOutput();
	timer->stop();

	ui->stackedWidget->setCurrentWidget(ui->MainPage);
}

void MainWindow::cancelProgress() {
	clearGlobalVarsAndOutput();
	timer->stop();

}

void MainWindow::categorySelect() {
	// QString categoryString = ui->selectCategoryComboBox->currentText();
	// qInfo() << categoryString << "\n";
	// qInfo() << ui->selectCategoryComboBox->currentIndex();

	// QMap<QString, QStringList> CategoryDB = {
	// 	{ "Image", { "JPG","PNG", "GIF"} },
	// 	{ "Video", { "MP4", "MKV" }},
	// 	{ "Audio",  { "MP3" }},
	// 	{ "Doc",{ "PDF"}}
	// };

	// ui->categorySpecificSignatureComboBox->clear();
	// ui->categorySpecificSignatureComboBox->addItem("All");
	// ui->categorySpecificSignatureComboBox->addItems(CategoryDB[categoryString]);

}


void MainWindow::chooseFileButtonClicked() {
	// be default use home dir for any OS, so QDir::homepath
	QString filePath = QFileDialog::getOpenFileName(this, "Open Dump File", QDir::homePath());

	inputFilePath = filePath.toStdString();

	// cout << inputFilePath << "\n";
	// inputFile.open(filePath.toStdString(), ios::binary);

	// if (!inputFile.is_open()) {
	// 	QMessageBox::warning(this, "Error", "Cannot open file");
	// }

	// string currentDir = QDir::currentPath().toStdString();
	// string hexTextFilePath = currentDir + "/output_hex.txt";

	// cerr << inputFilePath << "\n";
	ui->inputFilePathShow->setText(QString::fromStdString(inputFilePath));
}

void MainWindow::chooseOutputFolderButtonClicked() {
	// be default use home dir for any OS, so QDir::homepath
	QString dirPath = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::homePath());

	outputFolderPath = dirPath.toStdString();

	outputFolderPath.append("/output");

	// cerr << outputFolderPath << "\n";
	ui->outputFolderPathShow->setText(QString::fromStdString(outputFolderPath));
}

void MainWindow::displayCompletedMsg() {
	// ui->progressBar->setValue(100);

	// ui->outputDiplayTextBrowser->append("\nCompleted\n");
	// cerr << "Hello\n";
}

// int MainWindow::searchSignature(string fileContent, string signature, string fileType) {

// 	return -1;
// }
