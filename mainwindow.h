#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "work.h"
#include <QThread>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    
private slots:
    void on_btn_Connect_clicked();
    void on_btn_Exit_clicked();
    void On_doWork(QString);

private:
    Ui::MainWindow *ui;
    Work *mwork;
    QThread *mthread;
};

#endif // MAINWINDOW_H
