#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "mainwindow.h"

void searchMainFun();

class Worker : public QObject {
	Q_OBJECT

public:
	Worker(MainWindow* mainWindow) : mainWindow(mainWindow) {}

public slots:
	void doWork() {
		// MainWindow::searchSignatureFirst();
		mainWindow->searchMainFun();
		// searchMainFun();
		emit finished();
	}

signals:
	void finished();


public:
	MainWindow* mainWindow;
};



#endif	  // WORKER_H
