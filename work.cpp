/*  Author: Tai Phung
 *  Date created: 2-3-2016
 *  Embbeded Networking Laboratory - Lab411, C9
 *  Hanoi University of Science and Technology
 *  email: taiphungdinh@gmail.com
 *  License: GNU 2.0
 */

#include "work.h"
#include <QThread>
#include <QDebug>
#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <errno.h>   /* ERROR Number Definitions           */
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <QString>
#include <QTime>
#include <QDate>
#include <QFile>

#define DEBUG
#define BAUDRATE B19200

void SetPortAttribute(int);
int CheckCode(QString);
void StrCpy(char*, char*, int, int);
void WriteImage(FILE*, char*, int);

Work::Work(QObject *parent) :
    QObject(parent)
{
    fd = -1;
    port = "/dev/ttyUSB0";
}

void Work::doWork(){
    fd = open( port.toLatin1().data(),O_RDWR | O_NOCTTY);
    if(fd == -1){
        emit receiveData("Cannot open Device!\n");
        return;
    }

    SetPortAttribute(fd);
    char read_buffer[1024];     /* Buffer to store the data received              */
    int bytes_read;             /* Number of bytes read by the read() system call */
    int i;
    int j = 0;

    // Processing
    char line[1024];
    QString FileName;

    QString Start, End, Line, Code,Image;

    while(1){
        tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */
        bytes_read = read(fd, &read_buffer, 1024); /* Read the data                   */
        if(bytes_read < 1){
            continue;
        }
        for(i=0;i<bytes_read;i++) {
            if(read_buffer[i] == '\n'){
                line[j] = '\0';
                Line = QString::fromLocal8Bit(line);/*convert from char[] to QString to use funtion "mid"*/
                if(j > 1){
                    emit receiveData(Line);
                }
                /*Code xu ly thong tin nhan duoc tu ttyusb*/
               /* Code = Line.mid(0, 3);
                switch(CheckCode(Code)){
                    case 1:
                    {
                        Start = Line.mid(16, 4);
                        End = Line.mid(j-4, 4);
                        if(!Start.compare("FFD8")) {
                            QTime time;
                            QString t = time.currentTime().toString("hh_mm_ss");
                            QDate date;
                            QString d = date.currentDate().toString("_dd_MM_yyyy");
                            FileName = "/home/hunght/Images/" + t + d + ".jpg";
                        }
                        Image = Line.mid(16, Line.length() - 16);
                        WriteAppend(FileName, Image);
                        if(!End.compare("FFD9"))
                        {
                            emit receiveData("Image Received Completely!");
                            //emit ImageReceived(FileName); //chuyen file anh sang mainwindow
                        }
                        break;
                    }
                    case 2:
                    {
                        QString mac = Line.mid(11,2);
                        bool ok;
                        int tmp = mac.toInt(&ok, 10);
                        QString address = Line.mid(4,4);
                        emit nodeJoin(tmp, address);
                    }
                    default: break;
                }*/
                j = 0;
            } else{
                line[j] = read_buffer[i];
                j++;
            }
        }
    }
    close(fd);
}

void SetPortAttribute(int fd){
    /*---------- Setting the Attributes of the serial port using termios structure --------- */
    struct termios SerialPortSettings;  /* Create the structure                          */
    tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

    /* Setting the Baud rate */
    cfsetispeed(&SerialPortSettings, BAUDRATE); /* Set Read  Speed                       */
    cfsetospeed(&SerialPortSettings, BAUDRATE); /* Set Write Speed                       */

    /* 8N1 Mode */
    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;    /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;  //Enable receiver,Ignore Modem Control lines
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */
    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    /* Setting Time outs */
    SerialPortSettings.c_cc[VMIN] = 1; /* Read at least 10 characters */
    SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0){           /* Set the attributes to the termios structure*/
        qDebug()<<"ERROR in Setting attributes!";
        exit(1);
    }
}

int CheckCode(QString code){
    if(!code.compare("#RC")) return 1;
    if(!code.compare("#JN")) return 2;
    return 0;
}

void Work::WriteHex(QString str)
{
    int hex_val;
    int hex_len = str.length()/2;
    char hex[hex_len];
    int i;
    QString tmp;
    for(i = 0; i < hex_len; i++){
        tmp = str.mid(i*2, 2);
        bool ok;
        hex_val = tmp.toInt(&ok, 16);
        hex[i] = (char)hex_val;
        emit receiveData(QString::number(hex_val));
    }
    write(fd, &hex, hex_len);
}

void Work::set_Device(QString m)
{
    port="/dev/"+m;
    qDebug()<<port;
}
void Work::WriteAppend(QString FileName, QString Image)
{
    QString str, image;
    int hex;
    int len = Image.length()/2;
    for(int i=0; i < len; i++)
    {
        str = Image.mid(i*2, 2);
        bool ok;
        hex = str.toInt(&ok, 16);
        image += (char) hex;
    }
    QFile file(FileName);
    if(!file.open(QIODevice::Append|QIODevice::Truncate)) return;
    file.write(image.toLatin1());
    file.close();
}

