#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
using namespace std;

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private:
	Ui::MainWindow *ui;
	ifstream inputFile;
	string inputFileContent;
	// ofstream hexFile
public slots:
	void goToLoadingPage();
	void goBackToMainPage();
	void categorySelect();
	void chooseFileButtonClicked();
	void chooseOutputFolderButtonClicked();
	void displayNoOfFiles();
	void searchSignatureFirst();
	// int searchSignature(string, string, string);
};
#endif	  // MAINWINDOW_H
