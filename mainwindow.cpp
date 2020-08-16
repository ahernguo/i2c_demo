#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	this->ui->setupUi(this);

	/*** initial list ***/
	this->gpaList = new QList<uimap*>();
	this->gpbList = new QList<uimap*>();

	/*** create a pointer that point to configuration table ***/
	auto *table = this->ui->tbMain;
	/* change style */
	table->setColumnCount(10);
	table->setRowCount(8);
	table->setStyleSheet("QTableWidget { background-color:transparent; } QTableWidget::item { padding: 2px }");	//replace white to transparent
	table->setShowGrid(false);								//hide grid border
	table->setFrameStyle(QFrame::NoFrame);					//hide table border
    /* hide header */
	table->verticalHeader()->hide();
	table->horizontalHeader()->hide();
	/* set grid width */
	table->setColumnWidth(0, 30);	//GPB - State
	table->setColumnWidth(1, 35);	//GPB - Off
	table->setColumnWidth(2, 35);	//GPB - On
	table->setColumnWidth(3, 65);	//GPB - Setting
	table->setColumnWidth(4, 50);	//GPB - Name
	table->setColumnWidth(5, 50);	//GPA - Name
	table->setColumnWidth(6, 65);	//GPA - Setting
	table->setColumnWidth(7, 35);	//GPA - On
	table->setColumnWidth(8, 35);	//GPA - Off
	table->setColumnWidth(9, 30);	//GPA - State
    /* loop for setting cells  */
    for (int idx = 0; idx < 8; idx++) {
		/* prepare two rows */
		auto *gpa = new uimap(1, 7 - idx);	//the layout of MCP23017 of GPA was reversed. [7..0] from up to bottom
		auto *gpb = new uimap(2, idx);		//the layout of MCP23017 of GPB was forward. [0..7] from up to bottom
		/* bind widget to table cell. the direction will decides by group value */
		gpa->bind(table, idx);
		gpb->bind(table, idx);
		/* connect signal and slots */
		connect(gpa, SIGNAL(changeStateClicked(int,int,bool)), this, SLOT(on_io_clicked(int,int,bool)));
		connect(gpb, SIGNAL(changeStateClicked(int,int,bool)), this, SLOT(on_io_clicked(int,int,bool)));
		/* add into list */
		this->gpaList->append(gpa);
		this->gpbList->append(gpb);
    }

	/*** reset i2c, initial i2c when 'config' clicked  ***/
	this->fd = -1;
	this->bus = -1;
}

MainWindow::~MainWindow() {
	delete this->ui;
}

void MainWindow::on_io_clicked(int group, int id, bool onOff) {
	/* GPIOA = 0x12, GPIOB = 0x13 (bank = 0) */
	int sw = (group == 1) ? 0x12 : 0x13;
	/* read current value */
	int cur = wiringPiI2CReadReg8(this->fd, sw);
	/* prepare bit and mask */
	int mask = 0x01 << id;
	/* change to the value that user click */
	if (onOff) {
		cur |= mask;
	} else {
		cur &= ~mask;
	}
	/* write value to i2c */
	wiringPiI2CWriteReg8(this->fd, sw, cur);
}

void MainWindow::showEvent(QShowEvent *event) {
	QWidget::showEvent(event);
	this->ui->btnUnconfig->click();
}

void MainWindow::on_btnConfig_clicked() {
	int gpa = 0, gpb = 0;

	/* disable spinBox */
	this->ui->sbBus->setEnabled(false);
	/* change button */
	this->ui->btnConfig->hide();
	this->ui->btnUnconfig->show();
	this->ui->btnRefresh->show();
	/* change UI and mask bit */
	foreach (uimap *map, *this->gpaList) {
		map->bitMask(gpa);
		map->settingMode(false);	//change UI
	}
	foreach (uimap *map, *this->gpbList) {
		map->bitMask(gpb);
		map->settingMode(false);	//change UI
	}
	/* open and initial i2c */
	if (this->ui->sbBus->value() != this->bus) {
		this->bus = this->ui->sbBus->value();	//store value
		this->fd = wiringPiI2CSetup(this->bus);	//setup i2c
	}
	/* setting i2c */
	wiringPiI2CWriteReg8(this->fd, 0x00, gpa);	//IODIRA
	wiringPiI2CWriteReg8(this->fd, 0x01, gpb);	//IODIRB
}

void MainWindow::on_btnUnconfig_clicked() {
	/* disable spinBox */
	this->ui->sbBus->setEnabled(true);
	/* change button */
	this->ui->btnConfig->show();
	this->ui->btnUnconfig->hide();
	this->ui->btnRefresh->hide();
	/* change ui */
	foreach (uimap *map, *this->gpaList) {
		map->settingMode(true);
	}
	foreach (uimap *map, *this->gpbList) {
		map->settingMode(true);
	}
}

void MainWindow::on_btnRefresh_clicked() {
	/* read value from i2c */
	int gpa = wiringPiI2CReadReg8(this->fd, 0x12);
	int gpb = wiringPiI2CReadReg8(this->fd, 0x13);
	/* change ui */
	foreach (uimap *map, *this->gpaList) {
		map->changeState(gpa);
	}
	foreach (uimap *map, *this->gpbList) {
		map->changeState(gpb);
	}
}
