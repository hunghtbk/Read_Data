/*  Author: Hung Hoang Trong
 *  Date Created: 9/5/2016
 *  Embedded Networking Research Group - Lab411
 *  Hanoi University of Science and Technology
 *  Contact: hunght.thaibinh@gmail.com
 *  Description: This software use for receiving data from device ttyUSB...
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hled.h"
#include "work.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->led1->turnOff();
    mthread=new QThread();
    mwork=new Work();
    mwork->moveToThread(mthread);

    connect(mwork,SIGNAL(requestWork()),mthread,SLOT(start()));
    connect(mthread,SIGNAL(started()),mwork,SLOT(doWork()));
    connect(mwork,SIGNAL(receiveData(QString)),this,SLOT(On_doWork(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::On_doWork(QString m)
{
    ui->pledit_Receive->insertPlainText("\n");
    ui->pledit_Receive->insertPlainText(m);
    ui->pledit_Receive->insertPlainText("\n");
    ui->pledit_Receive->moveCursor(QTextCursor::End);
}

void MainWindow::on_btn_Connect_clicked()
{
    ui->led1->turnOn();
    ui->pledit_Receive->insertPlainText("_______________________________________Receive Data______________________________________");
    mwork->requestWork();
    mwork->set_Device(ui->cbx_Device->currentText());
}

void MainWindow::on_btn_Exit_clicked()
{
    qApp->quit();
}
