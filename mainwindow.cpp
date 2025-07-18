#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    table = ui->tableWidget;
    QSize screenSize = screen()->availableGeometry().size();
    table->resize(screenSize.width() * 0.7 ,screenSize.height() * 0.7);
    resize(screenSize.width() * 0.9 ,screenSize.height() * 0.9);
    table->setGeometry((screenSize.width() * 0.9 * 0.15), ((screenSize.height() * 0.7) * 0.3), screenSize.width() * 0.7, screenSize.height() * 0.7 );
    setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    label = new QLabel("New Label", this);
    label->setStyleSheet("QLabel { background-color : gray; color : red; }");
    QPoint localPos = mapFromGlobal(QCursor::pos());
    label->setGeometry(localPos.x(),localPos.y(), 50, 40);
    label->setMouseTracking(true);
    label->show();
    dragging = true;
}


