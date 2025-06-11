#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include "serialport.h"
#include <stdint.h>

#define START_BYTE      0x55
#define SPEED_FORWARD   0x01
#define SPEED_BACK      0x02
#define CCR_FORWARD     0x03
#define CCR_BACK        0x04
#define DATA_READ       0x05
#define START_GRAPH     0x06
#define KP_BYTE         0x07
#define KI_BYTE         0x08
#define READ_DATA       0x0C
#define WRITE_BYTE      0x0F
#define WRITE_BUFFER    0x1F
#define READ_           0x10

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int startAdr;
    int stopAdr;
    uint32_t countInByte;
    QString str1;
    QByteArray saveBuf;
    QByteArray readBuf;

signals:

    void savesettings(Settings *settingsPort2);
    void writeData(QByteArray data, int *startAdr, int *stopAdr);
    void writeData1(QByteArray data);

private slots:
    void Print(QByteArray *data);
    void BtnSave_clicked();
    void Btn_Serch_clicked();
    void sendData();
    void valid_StartAdr();
    void valid_StopAdr();
    void saveToFile();
    void loadToFile();
};

#endif // MAINWINDOW_H
