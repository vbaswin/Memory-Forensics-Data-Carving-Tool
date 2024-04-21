#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "mainwindow.h"

void searchMainFun();

class Worker : public QObject {
	Q_OBJECT

public slots:
	void doWork() {
		// friend void MainWindow::searchSignatureFirst();
		searchMainFun();
		emit finished();
	}

signals:
	void finished();
};



#endif	  // WORKER_H
