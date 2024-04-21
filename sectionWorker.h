#ifndef SECTIONWORKER_H
#define SECTIONWORKER_H

#include <QObject>
#include "mainwindow.h"

void searchSignature(streampos, streampos, int);

class sectionWorker : public QObject {
	Q_OBJECT
public:
	sectionWorker(streampos start, streampos end, int threadNo, QObject *parent = nullptr)
		: QObject(parent), start(start), end(end), threadNo(threadNo) {}

public slots:
	void doWork() {
		// friend void MainWindow::searchSignatureFirst();
		// searchMainFun();
		searchSignature(start, end, threadNo);
		emit finished();
	}

signals:
	void finished();
	void progressMade(int value);

private:
	streampos start;
	streampos end;
	int threadNo;
};





#endif	  // SECTIONWORKER_H
