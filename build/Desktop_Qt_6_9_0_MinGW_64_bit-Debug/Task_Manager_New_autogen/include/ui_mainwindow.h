/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTableWidget *tableWidget;
    QPushButton *pushButton;
    QPushButton *MonthView;
    QToolButton *toolButton;
    QToolButton *Prev;
    QToolButton *Next;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        tableWidget = new QTableWidget(centralwidget);
        if (tableWidget->columnCount() < 7)
            tableWidget->setColumnCount(7);
        if (tableWidget->rowCount() < 5)
            tableWidget->setRowCount(5);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setGeometry(QRect(30, 70, 541, 131));
        tableWidget->setMinimumSize(QSize(541, 131));
        tableWidget->setMouseTracking(true);
        tableWidget->setAcceptDrops(true);
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setRowCount(5);
        tableWidget->setColumnCount(7);
        tableWidget->horizontalHeader()->setStretchLastSection(false);
        tableWidget->verticalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setCascadingSectionResizes(false);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(25, 25, 150, 50));
        pushButton->setMouseTracking(true);
        MonthView = new QPushButton(centralwidget);
        MonthView->setObjectName("MonthView");
        MonthView->setGeometry(QRect(375, 25, 150, 50));
        toolButton = new QToolButton(centralwidget);
        toolButton->setObjectName("toolButton");
        toolButton->setGeometry(QRect(200, 25, 150, 50));
        toolButton->setPopupMode(QToolButton::ToolButtonPopupMode::DelayedPopup);
        Prev = new QToolButton(centralwidget);
        Prev->setObjectName("Prev");
        Prev->setGeometry(QRect(550, 25, 50, 50));
        Prev->setArrowType(Qt::ArrowType::LeftArrow);
        Next = new QToolButton(centralwidget);
        Next->setObjectName("Next");
        Next->setGeometry(QRect(600, 25, 50, 50));
        Next->setArrowType(Qt::ArrowType::RightArrow);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 18));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Create Task", nullptr));
        MonthView->setText(QCoreApplication::translate("MainWindow", "Month View", nullptr));
        toolButton->setText(QCoreApplication::translate("MainWindow", "Groups", nullptr));
        Prev->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        Next->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
