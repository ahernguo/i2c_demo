#ifndef UIMAP_H
#define UIMAP_H

#include <QPushButton>
#include <QComboBox>
#include <QGraphicsEllipseItem>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QObject>

class uimap : public QObject
{
	Q_OBJECT

	public:
		explicit uimap(int group, int id, QObject *parent = nullptr);
		void bind(QTableWidget *table, int row);
		void bitMask(int &value);
		void changeState(int value);
		void settingMode(bool mode);

	signals:
		void changeStateClicked(int group, int id, bool onOff);

	private slots:
		void turnOn_clicked();
		void turnOff_clicked();
		void setting_indexChanged(int index);

	private:
		int group;
		int id;
		int mask;
		QComboBox *setting;
		QLabel *name;
		QPushButton *turnOn;
		QPushButton *turnOff;
		QLabel *state;
};

#endif // UIMAP_H
