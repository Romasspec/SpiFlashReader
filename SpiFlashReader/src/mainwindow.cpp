#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSpinBox>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->BaudRateBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->BaudRateBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->BaudRateBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->BaudRateBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui->BaudRateBox->addItem(QLatin1String("Custom"));
    ui->BaudRateBox->setCurrentIndex(3);
    // fill data bits
    ui->DataBitsBox->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->DataBitsBox->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->DataBitsBox->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->DataBitsBox->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->DataBitsBox->setCurrentIndex(3);
    // fill parity
    ui->ParityBox->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->ParityBox->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->ParityBox->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->ParityBox->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->ParityBox->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);
    // fill stop bits
    ui->StopBitsBox->addItem(QLatin1String("1"), QSerialPort::OneStop);
    #ifdef Q_OS_WIN
    ui->StopBitsBox->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
    #endif
    ui->StopBitsBox->addItem(QLatin1String("2"), QSerialPort::TwoStop);
    // fill flow control
    ui->FlowControlBox->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui->FlowControlBox->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->FlowControlBox->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);
    ui->StartAdr->setText("0x000000");
    ui->StopAdr->setText("0x000064");
    startAdr = ui->StartAdr->text().toInt(0,16);
    stopAdr = ui->StopAdr->text().toInt(0,16);
    QThread *thread1 = new QThread();
    serialPort *serialPort1 = new serialPort();
    serialPort1->progres = 0;
    ui->progressBar->setValue(serialPort1->progres);
    serialPort1->moveToThread(thread1);
    serialPort1->port1.moveToThread(thread1);
    connect(serialPort1,SIGNAL(error_(QString)),this, SLOT(Print(QString)));
    connect(thread1,SIGNAL(started()),serialPort1,SLOT(startPort1()),Qt::QueuedConnection);
    connect(this, SIGNAL(savesettings(Settings*)),serialPort1,SLOT(writeSettingsPort1(Settings*)));
    connect(ui->SaveButton, SIGNAL(clicked(bool)),this, SLOT(BtnSave_clicked()));
    connect(ui->SearshButton, SIGNAL(clicked(bool)),this, SLOT(Btn_Serch_clicked()),Qt::UniqueConnection);
    connect(ui->OpenButton, SIGNAL(clicked(bool)), serialPort1, SLOT(connectPort1()), Qt::UniqueConnection);
    connect(ui->CloseButton, SIGNAL(clicked(bool)), serialPort1, SLOT(disconnectPort1()), Qt::UniqueConnection);
    connect(serialPort1,SIGNAL(finished_Port()),thread1, SLOT(quit()));
    connect(thread1, SIGNAL(finished()), serialPort1, SLOT(deleteLater()));
    connect(serialPort1, SIGNAL(finished_Port()), thread1, SLOT(deleteLater()));
    connect(serialPort1, SIGNAL(outPort(QByteArray)),this, SLOT(Print(QByteArray)));
    connect(this, SIGNAL(writeData(QByteArray, int*, int*)), serialPort1, SLOT(writeToPort1(QByteArray, int*, int*)));
    connect(this, SIGNAL(writeData1(QByteArray)),serialPort1, SLOT(writedataToPort1(QByteArray)));
    connect(ui->SendButton, SIGNAL(clicked(bool)), this, SLOT(sendData()));
    connect(serialPort1, SIGNAL(outProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(ui->StartAdr, SIGNAL(editingFinished()), this, SLOT(valid_StartAdr()), Qt::UniqueConnection);
    connect(ui->StopAdr, SIGNAL(editingFinished()), this, SLOT(valid_StopAdr()), Qt::UniqueConnection);
    connect(ui->SaveFile,SIGNAL(clicked(bool)), this, SLOT(saveToFile()), Qt::UniqueConnection);
     connect(ui->LoadFile,SIGNAL(clicked(bool)), this, SLOT(loadToFile()), Qt::UniqueConnection);
    thread1->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Print(QByteArray data)
{
//    ui->console->textCursor().insertText(data+'\r');
//    ui->console->moveCursor(QTextCursor::End);//Scroll
    uint8_t byte;
    int size1;

    //ui->console->textCursor().insertText(data.toHex());
    //qDebug () << data.toHex();

    QByteArray rxBuf;
    rxBuf += data;
    saveBuf += rxBuf;
    size1 = rxBuf.size();
//     qDebug () << size1;
    for(int i=0; i<size1;i++){
        byte = (uint8_t)rxBuf[0] ;
        if (byte < 10) {
            str1.append('0');
        }
            str1.append(QString::number(byte,16));

        rxBuf.remove(0,1);
        countInByte ++;
//        ui->Nbyte->setValue(countInByte);
        }
//     ui->console->textCursor().insertText(str1);
//     qDebug(str1);
     qDebug() << countInByte;
}

void MainWindow::BtnSave_clicked()
{
    Settings *settingsPortPtr = new Settings;
    settingsPortPtr->name = ui->NameBox->currentText();
    settingsPortPtr->baudRate = ui->BaudRateBox->currentText().toInt();
    settingsPortPtr->dataBits = static_cast<QSerialPort::DataBits> (ui->DataBitsBox->currentText().toInt());
    settingsPortPtr->parity = static_cast<QSerialPort::Parity> (ui->ParityBox->currentText().toInt());
    settingsPortPtr->stopBits = static_cast<QSerialPort::StopBits> (ui->StopBitsBox->currentText().toInt());
    settingsPortPtr->flowControl = static_cast<QSerialPort::FlowControl> (ui->FlowControlBox->currentText().toInt());

    savesettings(settingsPortPtr);
}

void MainWindow::Btn_Serch_clicked()
{
    ui->NameBox->clear();
    foreach (const QSerialPortInfo &information, QSerialPortInfo::availablePorts()) {
        ui->NameBox->addItem(information.portName());
    }
}

void MainWindow::sendData()
{
    QByteArray BUF_TX;

    countInByte = 0;
    str1.clear();
    saveBuf.clear();

    BUF_TX[0] = START_BYTE;
    BUF_TX[1] = READ_DATA;
    BUF_TX[2] = (uint8_t)((startAdr)&0xFF);
    BUF_TX[3] = (uint8_t)((startAdr>>8)&0xFF);
    BUF_TX[4] = (uint8_t)((startAdr>>16)&0xFF);
    BUF_TX[5] = (uint8_t)((stopAdr)&0xFF);
    BUF_TX[6] = (uint8_t)((stopAdr>>8)&0xFF);
    BUF_TX[7] = (uint8_t)((stopAdr>>16)&0xFF);

    writeData(BUF_TX, &startAdr, &stopAdr);
}

void MainWindow::valid_StartAdr()
{
    //QByteArray data_TX;
    int data;
    QString string = ui->StartAdr->text();

    while (string.length() < 8) {
        string.append("0");
    }
       ui->StartAdr->setText(string);

    string.resize(2);
    if(string.compare("0x",Qt::CaseSensitive) == 0) {
        data = ui->StartAdr->text().toInt(0,16);
    } else {
        string = ui->StartAdr->text();
        string.replace(0,2,"0x");
        ui->StartAdr->setText(string);
    }
    startAdr = data;
    if (stopAdr < startAdr) {
        stopAdr = startAdr;
        QString string1 = QString::number(stopAdr, 16);
        while (string1.length() < 6) {
            string1.prepend("0");
        }
        string1.prepend("0x");
        ui->StopAdr->setText(string1);
        qDebug () << startAdr;
        qDebug () << stopAdr;
    }
}

void MainWindow::valid_StopAdr()
{
    //QByteArray data_TX;
    int data;
    QString string = ui->StopAdr->text();

    while (string.length() < 8) {
        string.append("0");
    }
       ui->StopAdr->setText(string);

    string.resize(2);
    if(string.compare("0x",Qt::CaseSensitive) == 0) {
        data = ui->StopAdr->text().toInt(0,16);
    } else {
        string = ui->StopAdr->text();
        string.replace(0,2,"0x");
        ui->StopAdr->setText(string);
    }
    stopAdr = data;
    if (stopAdr < startAdr) {
        stopAdr = startAdr;
        QString string1 = QString::number(stopAdr, 16);
        while (string1.length() < 6) {
            string1.prepend("0");
        }
        string1.prepend("0x");
        ui->StopAdr->setText(string1);
        qDebug () << startAdr;
        qDebug () << stopAdr;
    }
}

void MainWindow::saveToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file flash"), "", tr("flash (*.bin);;All Files (*)"));

    if(fileName.isEmpty()) {
        return;
    } else {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this,tr("Unable to open file"),file.errorString());
            return;
        }
//
//        int size = saveBuf.toHex().size();
//        qDebug () << size;
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_5);

        out.writeRawData(saveBuf, saveBuf.size());
//        qDebug () << QDataStream::writeRawData(saveBuf, saveBuf.size());
    }
    ui->progressBar->setValue(0);
}

void MainWindow::loadToFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file flash"), "", tr("flash (*.bin);;All Files (*)"));
    if(fileName.isEmpty()) {
        return;
    } else {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this,tr("Unable to open file"),file.errorString());
            return;
        }
//
//        int size = saveBuf.toHex().size();
//        qDebug () << size;
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_5);
//        qDebug () << file.size();
        readBuf.clear();
        int fileSize = file.size();
        char *buf = new char [fileSize];
        in.readRawData(buf, fileSize);
//        qDebug () << QDataStream::writeRawData(saveBuf, saveBuf.size());

        QByteArray BUF_TX;
        countInByte = 0;
        str1.clear();
        saveBuf.clear();

        BUF_TX[0] = START_BYTE;
        BUF_TX[1] = WRITE_BUFFER;
        BUF_TX[2] = (uint8_t)((startAdr)&0xFF);
        BUF_TX[3] = (uint8_t)((startAdr>>8)&0xFF);
        BUF_TX[4] = (uint8_t)((startAdr>>16)&0xFF);
        BUF_TX[5] = (uint8_t)((fileSize)&0xFF);              //количесво байт
        BUF_TX[6] = (uint8_t)((fileSize>>8)&0xFF);
        BUF_TX[7] = (uint8_t)((fileSize>>16)&0xFF);
        stopAdr = startAdr + fileSize;
        writeData(BUF_TX, &startAdr, &stopAdr);

        for(int i = 0; i < file.size(); i++) {
            readBuf.append(buf[i]);
        }

        writeData1(readBuf);

    }
}
