#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QDialog>
#include <QLineEdit>
#include <QDrag>
#include <QScreen>
#include <QDateEdit>
#include <QComboBox>
#include <QDate>
#include <QVBoxLayout>

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
    //making the label follow the user cursor
    void mouseMoveEvent(QMouseEvent *event) override {
        if(label && (dragging == true)){
            label->move(mapFromGlobal(QCursor::pos()) - QPoint(label->width() / 2, label->height() / 2));
        }
    }

    //handles droping the task into the table
    void mousePressEvent(QMouseEvent *event) override
    {
        if((label) && (dragging == true) ){
            QPoint pos = table->mapFromGlobal(QCursor::pos());
            QModelIndex cords = table->indexAt(pos);
            if(cords.isValid()){
                int row = cords.row();
                int column = cords.column();
                int count = 0;
                int GridDay = trackingdate.addDays(-trackingdate.day()+ (column + (row * 7) + 1)).dayOfYear();
                Task newTask;
                QPushButton *TaskVisualButton = new QPushButton("New Task", this);
                if(!table->cellWidget(row, column) && (monthViewActive == false)){
                    table->setCellWidget(row, column, TaskVisualButton);
                    TaskStorage[trackingdate.addDays(column).dayOfYear()][row] = newTask;
                    connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { on_TaskButton_clicked(trackingdate.addDays(column).dayOfYear() ,row); });
                }else if(monthViewActive){
                    if((column + (row * 7) + 1) < trackingdate.daysInMonth() + 1){
                        while(!TaskStorage[GridDay][count].name.isEmpty()){
                            count++;
                        }
                        if(count != 5 && table->cellWidget(row,column)->layout()->count() != 6){
                            QWidget *container = table->cellWidget(row,column);
                            container->layout()->addWidget(TaskVisualButton);
                            TaskVisualButton->setStyleSheet("background-color: rgb(143,188,143)");
                            TaskStorage[GridDay][count] = newTask;
                            connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { on_TaskButton_clicked(GridDay ,count); });
                        }
                    }
                }
                delete label;
                label = nullptr;
                dragging = false;
            }
        }
    }

private slots:
    void on_pushButton_clicked();
    void on_TaskButton_clicked(int column, int row);
    void loadWeek();
    void on_Prev_clicked();
    void on_Next_clicked();
    void saveTaskInfo( QDialog *dialog, int column, int row,  QLineEdit *Name, QLineEdit *Desc, QDateEdit *Start, QDateEdit *End, QComboBox *StatusBox, QComboBox *PriorityBox);
    void deleteTaskInfo( QDialog *dialog, int column, int row);
    void on_MonthView_clicked();
    QString getMonthName(QDate date);
    void getWeekDayCycle(QDate date);


private:
    Ui::MainWindow *ui;
    QLabel *label = nullptr;

    //keeping the table available to various functions
    QTableWidget *table = nullptr;

    //tracking when the user is dragging something
    bool dragging = false;

    bool monthViewActive = false;

    bool MonthChange = false;


    //sets this private variable at start up to current date and will be used to track the date as use naviagtes through calender
    QDate trackingdate = QDate::currentDate();


    //The info a task will hold
    struct Task {
        QString name;
        QString description;
        QDate startDate;
        QDate endDate;
        enum Priority { Low, Mid, High};
        Priority priority = Low;
        enum Status { Unstarted, Working, Completed};
        Status status = Unstarted;
        QString prerequisite;
        QString requisite;
    };

    void TaskInfoVisualEffect(QPushButton *TaskVisualButton, Task *taskToChange);

    //The stroage holding all the tasks
    Task TaskStorage[366][5];

};

#endif // MAINWINDOW_H
