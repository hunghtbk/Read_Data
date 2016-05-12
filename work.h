#ifndef WORK_H
#define WORK_H

#include <QObject>

class Work : public QObject
{
    Q_OBJECT
public:
    explicit Work(QObject *parent = 0);
    void WriteByte(char*, int);
    void WriteHex(QString);
    void set_Device(QString);
signals:
    void requestWork();
    void receiveData(QString);
    void nodeJoin(int, QString);
public slots:
    void doWork();


public:
    int fd; //File Description

private:
    QString port;
private:
    void WriteAppend(QString, QString);
};

#endif // WORK_H
