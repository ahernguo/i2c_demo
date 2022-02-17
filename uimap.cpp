#include "uimap.h"

uimap::uimap(int group, int id, QObject *parent) : QObject(parent) {
	/* group and id */
	this->group = group;
	this->id = id;
	this->mask = 0x01 << id;
	/* name */
	this->name = new QLabel();
	this->name->setText(QString("GP%1%2").arg((group == 1 ? "A" : "B")).arg(id));
	this->name->setAlignment(Qt::AlignCenter);
	/* state */
	this->state = new QLabel();
	this->state->setScaledContents(true);
	/* turnOn */
	this->turnOn = new QPushButton("On");
	connect(this->turnOn, SIGNAL(clicked()), this, SLOT(turnOn_clicked()));
	this->turnOn->setStyleSheet("margin: 1px");
	/* turnOff */
	this->turnOff = new QPushButton("Off");
	connect(this->turnOff, SIGNAL(clicked()), this, SLOT(turnOff_clicked()));
	this->turnOff->setStyleSheet("margin: 1px");
	/* ComboBox */
	auto list = QStringList() << "IN" << "OUT";
	this->setting = new QComboBox();
	connect(this->setting, SIGNAL(currentIndexChanged(int)), this, SLOT(setting_indexChanged(int)));
	this->setting->addItems(list);
	this->setting->setCurrentIndex(1);
}

void uimap::turnOn_clicked() {
	emit changeStateClicked(this->group, this->id, true);
	changeState(this->mask);
	this->turnOn->setEnabled(false);
	this->turnOff->setEnabled(true);
}

void uimap::turnOff_clicked() {
	emit changeStateClicked(this->group, this->id, false);
	changeState(0);
	this->turnOn->setEnabled(true);
	this->turnOff->setEnabled(false);
}

void uimap::setting_indexChanged(int index) {
	if (index == 0) {
		this->turnOn->hide();
		this->turnOff->hide();
	} else {
		this->turnOn->show();
		this->turnOff->show();
	}
}

void uimap::bind(QTableWidget *table, int row) {
	/* if (GPA) >> put on right side >> [name, setting, on, off, state] */
	if (this->group == 1) {
		table->setCellWidget(row, 9, this->state);
		table->setCellWidget(row, 8, this->turnOff);
		table->setCellWidget(row, 7, this->turnOn);
		table->setCellWidget(row, 6, this->setting);
		table->setCellWidget(row, 5, this->name);
	} else {
		/* if (GPB) >> put on left side >> [state, off, on, setting, name] */
		table->setCellWidget(row, 4, this->name);
		table->setCellWidget(row, 3, this->setting);
		table->setCellWidget(row, 2, this->turnOn);
		table->setCellWidget(row, 1, this->turnOff);
		table->setCellWidget(row, 0, this->state);
	}
}

void uimap::settingMode(bool mode) {
	this->setting->setEnabled(mode);
	this->turnOn->setEnabled(!mode);
	this->turnOff->setEnabled(!mode);
	this->state->setText(" ");
	if (this->setting->currentIndex() == 0) {
		this->turnOn->hide();
		this->turnOff->hide();
	} else {
		this->turnOn->show();
		this->turnOff->show();
	}
}

void uimap::bitMask(int &value) {
	/* set bit to (0)output (1)input */
	if (this->setting->currentIndex() == 0) {
		/* this io sets to input */
		value |= this->mask;
	} else {
		/* this io sets to output */
		value &= ~this->mask;
	}
}

void uimap::changeState(int value) {
	QString path;
	if ((value & this->mask) == this->mask) {
		path = ":/images/images/GreenBall.png";
		this->turnOn->setEnabled(false);
		this->turnOff->setEnabled(true);
	} else {
		path = ":/images/images/GreyBall.png";
		this->turnOn->setEnabled(true);
		this->turnOff->setEnabled(false);
	}
	QPixmap img(path);
	this->state->setPixmap(img);
}
