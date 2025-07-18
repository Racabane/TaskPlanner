#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    //making the lable follow the user cursor
    void mouseMoveEvent(QMouseEvent *event) override {
        if(label && (dragging == true)){
            QPoint localPos = mapFromGlobal(QCursor::pos());
            label->move(localPos - QPoint(label->width() / 2, label->height() / 2));
        }
    }

    // make the label drop
    void mousePressEvent(QMouseEvent *event) override
    {
        if(label && (dragging == true) ){
            QPoint pos = table->mapFromGlobal(QCursor::pos());
            QModelIndex cords = table->indexAt(pos);
            if(cords.isValid()){
                int row = cords.row();
                int column = cords.column();
                if(!table->cellWidget(row, column)){
                    table->setCellWidget(row, column, label);
                    dragging = false;
                }
            }
        }
    }

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QLabel *label = nullptr;
    QTableWidget *table = nullptr;
    bool dragging = false;

};

#endif // MAINWINDOW_H
