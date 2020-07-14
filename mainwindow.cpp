#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fd = wiringPiI2CSetup(0x27);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnConfig_clicked()
{
    wiringPiI2CWriteReg8(fd, 0x00, 0x00);   //IODIRA, all output
    wiringPiI2CWriteReg8(fd, 0x01, 0x0F);   //IODIRB, all input

    /* set all on */
    wiringPiI2CWriteReg8(fd, 0x12, 0xF0);   //GPIOA, set all to high
    QThread::msleep(1000);
    wiringPiI2CWriteReg8(fd, 0x12, 0x00);   //GPIOA, set all to low
}
