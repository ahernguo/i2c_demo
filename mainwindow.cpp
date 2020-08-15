#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	/*** initial list ***/
	this->gpaList = new QList<uimap*>();
	this->gpbList = new QList<uimap*>();

    /*** create configuration table ***/
	QTableWidget *table = new QTableWidget(8, 10);
    /* hide header */
	table->verticalHeader()->hide();
	table->horizontalHeader()->hide();
	table->setColumnWidth(0, 30);
	table->setColumnWidth(1, 30);
	table->setColumnWidth(2, 30);
	table->setColumnWidth(3, 60);
	table->setColumnWidth(4, 50);
	table->setColumnWidth(5, 50);
	table->setColumnWidth(6, 60);
	table->setColumnWidth(7, 30);
	table->setColumnWidth(8, 30);
	table->setColumnWidth(9, 30);
    /* loop for setting cells  */
    for (int idx = 0; idx < 8; idx++) {
		/* prepare tow rows */
		auto *gpa = new uimap(1, 7 - idx);
		auto *gpb = new uimap(2, idx);
		/* bind */
		gpa->bind(table, idx);
		gpb->bind(table, idx);
		/* connect signal and slots */
		connect(gpa, SIGNAL(changeStateClicked(int,int,bool)), this, SLOT(on_io_clicked(int,int,bool)));
		connect(gpb, SIGNAL(changeStateClicked(int,int,bool)), this, SLOT(on_io_clicked(int,int,bool)));
		/* add into list */
		this->gpaList->append(gpa);
		this->gpbList->append(gpb);
    }
    /* add to layout */
	ui->gridLayout_2->addWidget(table);
    table->show();

	/*** reset i2c, initial i2c when 'config' clicked  ***/
	this->fd = -1;
	this->bus = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::on_btnConfig_clicked()
{
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
