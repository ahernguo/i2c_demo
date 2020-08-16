#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

	/* start MainWindow at center of current screen */
	auto curPos = QCursor::pos();								//get mouse position
	auto scr = QGuiApplication::screenAt(curPos)->geometry();	//get geometry of screen which mouse located

	/* calculate screen center */
	int x = (scr.width() - w.width()) / 2.0;
	int y = (scr.height() - w.height()) / 2.0;

	/* show at center */
	w.setGeometry(x, y, w.width(), w.height());
    w.show();

    return a.exec();
}
