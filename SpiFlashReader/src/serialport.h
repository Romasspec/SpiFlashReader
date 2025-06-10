#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <stdint.h>

struct Settings {   //Структура с настройками порта
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class serialPort : public QObject
{
    Q_OBJECT
public:
    explicit serialPort(QObject *parent = 0);
    ~serialPort();
    QSerialPort port1;
    Settings settingsPort1;
    int progres;
    int startAdr;
    int stopAdr;
private:
    QByteArray dataRX;

signals:
    void finished_Port(); //Сигнал закрытия класса
    void error_(QString err);//Сигнал ошибок порта
    void outPort(QByteArray dataout);
    void outProgress(int progres1);

public slots:
    void disconnectPort1();
    void connectPort1();
    void startPort1();
    void writeSettingsPort1(Settings *settingsPortPtr);
    void writeToPort1(QByteArray data, int *startAdr, int *stopAdr);

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void readInPort1();

public:


};

#endif // SERIALPORT_H
