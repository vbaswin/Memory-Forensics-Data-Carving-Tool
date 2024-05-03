#ifndef SECTIONWORKER_H
#define SECTIONWORKER_H

#include <QObject>
#include <fstream>
#include "mainwindow.h"

void searchSignature(ifstream &, streampos, streampos, int);

class sectionWorker : public QObject {
	Q_OBJECT
public:
	sectionWorker(streampos start, streampos end, int threadNo, QObject *parent = nullptr)
		: QObject(parent), start(start), end(end), threadNo(threadNo) {}
	~sectionWorker() {
		if (file.is_open())
			file.close();
	}

public slots:
	void doWork() {
		// friend void MainWindow::searchSignatureFirst();
		// searchMainFun();
		searchSignature(file, start, end, threadNo);
		emit finished();
	}

signals:
	void finished();
	// void progressMade(int value);

private:
	streampos start;
	streampos end;
	int threadNo;
public:
	ifstream file;
};





#endif	  // SECTIONWORKER_H
