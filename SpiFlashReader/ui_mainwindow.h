/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QPushButton *SearshButton;
    QPushButton *OpenButton;
    QPushButton *CloseButton;
    QPushButton *SaveButton;
    QLabel *label_6;
    QPushButton *SendButton;
    QPlainTextEdit *console;
    QComboBox *NameBox;
    QComboBox *BaudRateBox;
    QComboBox *DataBitsBox;
    QComboBox *ParityBox;
    QComboBox *StopBitsBox;
    QComboBox *FlowControlBox;
    QProgressBar *progressBar;
    QLineEdit *StartAdr;
    QLabel *label_7;
    QLineEdit *StopAdr;
    QPushButton *SaveFile;
    QPushButton *LoadFile;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(583, 305);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 40, 47, 13));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 70, 47, 13));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 100, 47, 13));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 130, 47, 13));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 160, 61, 16));
        SearshButton = new QPushButton(centralWidget);
        SearshButton->setObjectName(QStringLiteral("SearshButton"));
        SearshButton->setGeometry(QRect(10, 10, 75, 23));
        OpenButton = new QPushButton(centralWidget);
        OpenButton->setObjectName(QStringLiteral("OpenButton"));
        OpenButton->setGeometry(QRect(10, 220, 75, 23));
        CloseButton = new QPushButton(centralWidget);
        CloseButton->setObjectName(QStringLiteral("CloseButton"));
        CloseButton->setGeometry(QRect(90, 220, 75, 23));
        SaveButton = new QPushButton(centralWidget);
        SaveButton->setObjectName(QStringLiteral("SaveButton"));
        SaveButton->setGeometry(QRect(10, 190, 161, 23));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(190, 10, 61, 16));
        SendButton = new QPushButton(centralWidget);
        SendButton->setObjectName(QStringLiteral("SendButton"));
        SendButton->setGeometry(QRect(490, 10, 75, 23));
        console = new QPlainTextEdit(centralWidget);
        console->setObjectName(QStringLiteral("console"));
        console->setGeometry(QRect(190, 40, 381, 141));
        console->setLayoutDirection(Qt::RightToLeft);
        NameBox = new QComboBox(centralWidget);
        NameBox->setObjectName(QStringLiteral("NameBox"));
        NameBox->setEnabled(true);
        NameBox->setGeometry(QRect(90, 10, 81, 22));
        NameBox->setMouseTracking(false);
        NameBox->setLayoutDirection(Qt::LeftToRight);
        NameBox->setEditable(false);
        NameBox->setInsertPolicy(QComboBox::InsertAtBottom);
        NameBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
        NameBox->setDuplicatesEnabled(false);
        NameBox->setFrame(true);
        BaudRateBox = new QComboBox(centralWidget);
        BaudRateBox->setObjectName(QStringLiteral("BaudRateBox"));
        BaudRateBox->setGeometry(QRect(90, 40, 81, 22));
        DataBitsBox = new QComboBox(centralWidget);
        DataBitsBox->setObjectName(QStringLiteral("DataBitsBox"));
        DataBitsBox->setGeometry(QRect(90, 70, 81, 22));
        ParityBox = new QComboBox(centralWidget);
        ParityBox->setObjectName(QStringLiteral("ParityBox"));
        ParityBox->setGeometry(QRect(90, 100, 81, 22));
        StopBitsBox = new QComboBox(centralWidget);
        StopBitsBox->setObjectName(QStringLiteral("StopBitsBox"));
        StopBitsBox->setGeometry(QRect(90, 130, 81, 22));
        FlowControlBox = new QComboBox(centralWidget);
        FlowControlBox->setObjectName(QStringLiteral("FlowControlBox"));
        FlowControlBox->setGeometry(QRect(90, 160, 81, 22));
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(190, 190, 381, 23));
        progressBar->setMaximum(100);
        progressBar->setValue(0);
        StartAdr = new QLineEdit(centralWidget);
        StartAdr->setObjectName(QStringLiteral("StartAdr"));
        StartAdr->setGeometry(QRect(250, 10, 61, 20));
        StartAdr->setMaxLength(8);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(320, 10, 61, 16));
        StopAdr = new QLineEdit(centralWidget);
        StopAdr->setObjectName(QStringLiteral("StopAdr"));
        StopAdr->setGeometry(QRect(380, 10, 61, 20));
        StopAdr->setMaxLength(8);
        SaveFile = new QPushButton(centralWidget);
        SaveFile->setObjectName(QStringLiteral("SaveFile"));
        SaveFile->setGeometry(QRect(190, 220, 75, 23));
        LoadFile = new QPushButton(centralWidget);
        LoadFile->setObjectName(QStringLiteral("LoadFile"));
        LoadFile->setGeometry(QRect(280, 220, 75, 23));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 583, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "BaudRate:", 0));
        label_2->setText(QApplication::translate("MainWindow", "Data bits:", 0));
        label_3->setText(QApplication::translate("MainWindow", "parity:", 0));
        label_4->setText(QApplication::translate("MainWindow", "stop bits:", 0));
        label_5->setText(QApplication::translate("MainWindow", "flow control:", 0));
        SearshButton->setText(QApplication::translate("MainWindow", "Search", 0));
        OpenButton->setText(QApplication::translate("MainWindow", "Open", 0));
        CloseButton->setText(QApplication::translate("MainWindow", "Close", 0));
        SaveButton->setText(QApplication::translate("MainWindow", "Save", 0));
        label_6->setText(QApplication::translate("MainWindow", "Start adres", 0));
        SendButton->setText(QApplication::translate("MainWindow", "Send", 0));
        StartAdr->setText(QApplication::translate("MainWindow", "0x000000", 0));
        label_7->setText(QApplication::translate("MainWindow", "Stopt adres", 0));
        StopAdr->setText(QApplication::translate("MainWindow", "0x000000", 0));
        SaveFile->setText(QApplication::translate("MainWindow", "Save File", 0));
        LoadFile->setText(QApplication::translate("MainWindow", "Load File", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
