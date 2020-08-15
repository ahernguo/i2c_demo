#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QHeaderView>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>
#include <QWidget>
#include <wiringPiI2C.h>
#include "uimap.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_btnConfig_clicked();
		void on_io_clicked(int group, int id, bool onOff);

		void on_btnUnconfig_clicked();

		void on_btnRefresh_clicked();

	protected:
		void showEvent(QShowEvent *event);

    private:
        Ui::MainWindow *ui;
		QList<uimap*> *gpaList;
		QList<uimap*> *gpbList;

        int fd;
		int bus;
};

class Sleeper : public QThread {
	public:
		static void usleep(unsigned long usecs) {
			QThread::usleep(usecs);
		}
		static void msleep(unsigned long msecs) {
			QThread::msleep(msecs);
		}
		static void sleep(unsigned long secs) {
			QThread::sleep(secs);
		}
};

#endif // MAINWINDOW_H
