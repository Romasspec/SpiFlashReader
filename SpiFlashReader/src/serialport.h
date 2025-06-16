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
    int progresWr;
    int startAdr;
    int stopAdr;
private:
    QByteArray dataRX;
    QByteArray dataTX;
    QByteArray tempdataRX;
    bool read_fl;
    int ncykl;
    int last;
    int nextbyte;

signals:
    void finished_Port(); //Сигнал закрытия класса
    void error_(QString err);//Сигнал ошибок порта
    void outPort(QByteArray *dataout);
    void outProgress(int progres1);
    void outProgressWrite(int progres1);
    void nextData();

public slots:
    void disconnectPort1();
    void connectPort1();
    void startPort1();
    void writeSettingsPort1(Settings *settingsPortPtr);
    void writeToPort1(QByteArray data, int *startAdr, int *stopAdr);
    void writedataToPort1(QByteArray data);


private slots:
    void handleError(QSerialPort::SerialPortError error);
    void readInPort1();
    void writedatanext();

public:


};

#endif // SERIALPORT_H
