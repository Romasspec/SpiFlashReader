#include "serialport.h"
//#include <qdebug.h>
#include <QDebug>

serialPort::serialPort(QObject *parent) :
    QObject(parent)
{

}

serialPort::~serialPort()
{
    qDebug("By in Thread!");
    emit finished_Port();
}

void serialPort::startPort1()
{
    qDebug("Hello World in Thread!");
    startAdr = 0;
    stopAdr = 1;
    connect(&port1,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&port1,SIGNAL(readyRead()),this, SLOT(readInPort1()));
}

void serialPort::connectPort1()
{
    qDebug() << settingsPort1.name << settingsPort1.baudRate;
    port1.setPortName(settingsPort1.name);
    if(port1.open(QIODevice::ReadWrite)) {
        if(port1.setBaudRate(settingsPort1.baudRate)
                &&port1.setDataBits(settingsPort1.dataBits)
                &&port1.setParity(settingsPort1.parity)
                &&port1.setStopBits(settingsPort1.stopBits)
                &&port1.setFlowControl(settingsPort1.flowControl)) {
            if(port1.isOpen()) {
                error_(settingsPort1.name.toLocal8Bit() + " >> Открыт!\r");
                qDebug (settingsPort1.name.toLocal8Bit() + " >> Открыт!\r");
            } else {
                port1.close();
                error_(port1.errorString().toLocal8Bit());
            }
        }
    } else {
        port1.close();
        error_(port1.errorString().toLocal8Bit());
    }

}

void serialPort::disconnectPort1()
{
    if(port1.isOpen())
    {
        port1.close();
        error_(settingsPort1.name.toLocal8Bit() + " >> Закрыт!\r");
        qDebug (settingsPort1.name.toLocal8Bit() + " >> Закрыт!\r");
    }

}

void serialPort::writeSettingsPort1(Settings *settingsPortPtr)
{
    settingsPort1.name = settingsPortPtr->name;
    settingsPort1.baudRate = settingsPortPtr->baudRate;
    settingsPort1.dataBits = settingsPortPtr->dataBits;
    settingsPort1.parity = settingsPortPtr->parity;
    settingsPort1.stopBits = settingsPortPtr->stopBits;
    settingsPort1.flowControl = settingsPortPtr->flowControl;
    qDebug("Настройки приняты");
//    qDebug () << settingsPort1.name;
//    qDebug () << settingsPort1.baudRate;
//    qDebug () << settingsPort1.dataBits;
//    qDebug () << settingsPort1.parity;
//    qDebug () << settingsPort1.stopBits;
//    qDebug () << settingsPort1.flowControl;
}

void serialPort::handleError(QSerialPort::SerialPortError error)
{
    if(port1.isOpen() && (error == QSerialPort::ResourceError))
    {
        error_(port1.errorString().toLocal8Bit());
        disconnectPort1();
    }

}

void serialPort::readInPort1()
{
//    QByteArray data;
    dataRX.append(port1.readAll());
//    outPort(data);
    progres = (dataRX.size()* 100 )/ (stopAdr - startAdr) ;
    if (progres >= 100) {
        outPort(dataRX);
    }
    outProgress(progres);
}

void serialPort::writeToPort1(QByteArray data, int *startAdr1, int *stopAdr1)
{
    progres = 0;
    dataRX.clear();
    startAdr = *startAdr1;
    stopAdr = *stopAdr1;

    if (startAdr > stopAdr) {
        stopAdr = startAdr + 1;
    }

    if(port1.isOpen()) {
        port1.write(data);
    }

}
