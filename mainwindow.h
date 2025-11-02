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
#include <QHBoxLayout>
#include <QScrollArea>
#include <QCloseEvent>
#include <QPlainTextEdit>
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

    void closeEvent(QCloseEvent *event) override{
        savefile();
        event->accept();
    }

    //handles droping the task into the table
    void mousePressEvent(QMouseEvent *event) override
    {
        //only takes action after clicking on create task button
        if((label) && (dragging == true) ){
            //Gets the cursors postion and uses it to determine which box on the table they are clicking on
            QPoint pos = table->mapFromGlobal(QCursor::pos());
            QModelIndex cords = table->indexAt(pos);

            //if the coordinates of the click when choosing place to drop are valid  procceds to drop
            if(cords.isValid()){
                //since we will be using row and column often made vaiables to reduce calls to cords variable
                int row = cords.row();
                int column = cords.column();

                //The data type that will store the new task info as well as the visual button that will represent it on the table
                Task newTask;
                QPushButton *TaskVisualButton = new QPushButton("New Task", this);

                //checks if the box on the table clicked is empty in week view to ensure user is not attempting to creaste a task where one alreay resides
                if(!table->cellWidget(row, column) && (monthViewActive == false)){
                    //places the visual button onto the table at specified location
                    table->setCellWidget(row, column, TaskVisualButton);
                    //places the datatype into the table at appropite date that the table and array line up on
                    TaskStorage[trackingdate.addDays(column).dayOfYear()][row] = newTask;
                    //connects the visual button to the right task in the array so that when the button is click on the table it will change the correct task
                    connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { TaskButton(trackingdate.addDays(column).dayOfYear() ,row); });
                }
                //when creating a new task and dropping in during month view is handled differently then week view since each box in the table now represents a day instead a individual task
                else if(monthViewActive){
                    //only allows placing the tasks in the first 28 to 31 days boxes in the table depending on month to ensure user does not place task in boxes that are blank and not associated with any day
                    if((column + (row * 7) + 1) < trackingdate.daysInMonth() + 1){
                        //To write and read to the right place in the array the day int value is calculated ie jan 1 = 1 and dec 31 = 365
                        int GridDay = trackingdate.addDays(-trackingdate.day()+ (column + (row * 7) + 1)).dayOfYear();
                        //finds an empty spot in the array for that day to store the task in since each day has five tasks asscoiated being the second demension of the array
                        int count = 0;
                        while(!TaskStorage[GridDay][count].name.isEmpty()){
                            count++;
                        }
                        //Since count didnt go past 5 that means it found a valid spot for the task and the table box only holds less then 5 visual buttons a task can be placed
                        if(count != 5 && table->cellWidget(row,column)->layout()->count() != 6){
                            //Grabs the container that holds the buttons in the box of the table
                            QWidget *container = table->cellWidget(row,column);
                            //adds a new visual button to the container
                            container->layout()->addWidget(TaskVisualButton);
                            //sets the color of the button
                            TaskVisualButton->setStyleSheet("background-color: rgb(143,188,143)");
                            //creates a datatype in the array for that date
                            TaskStorage[GridDay][count] = newTask;
                            //connect the button to that datatype
                            connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { TaskButton(GridDay ,count); });
                        }
                    }
                }
                //cleans up the label for user visual feedback since no longer needed
                delete label;
                label = nullptr;
                //make sure it know that nothing is being dragged anymore
                dragging = false;
            }
        }
    }

private slots:
    //see mainwindow.cpp for more info on what each function does
    void on_pushButton_clicked();
    void TaskButton(int column, int row);
    void loadWeek();
    void on_Prev_clicked();
    void on_Next_clicked();
    void saveTaskInfo( QDialog *dialog, int column, int row,  QLineEdit *Name, QPlainTextEdit *Desc, QComboBox *StatusBox, QComboBox *PriorityBox);
    void deleteTaskInfo( QDialog *dialog, int column, int row);
    void on_MonthView_clicked();
    QString getMonthName(QDate date);
    void getWeekDayCycle(QDate date);


    void on_ListView_clicked();


    void LoadNextList(int start, QVBoxLayout *layout);
    void LoadPrevList(int start,  QVBoxLayout *layout);
    QString DateFormatMonthDay(int date);
    void loadfile();
    void savefile();

    void saveLink(QDialog *dialog, QString Prerequisite , QString Requisite);
    void on_Link_clicked();

private:
    Ui::MainWindow *ui;
    QLabel *label = nullptr;

    //keeping the table available to various functions
    QTableWidget *table = nullptr;

    //tracking when the user is dragging something
    bool dragging = false;

    //tracking when user is viewing calaender in month view
    bool monthViewActive = false;

    //tracking when month changes
    bool MonthChange = false;

    bool ListViewActive = false;
    int listDateStart = 0;
    int listDate = 0;
    int listDateEnd = 0;
    QScrollArea *area = nullptr;


    //sets this private variable at start up to current date and will be used to track the date as use naviagtes through calender
    QDate trackingdate = QDate::currentDate();


    //The info a task will hold
    struct Task {
        QString name;
        QString description;
        enum Priority { Low, Mid, High};
        Priority priority = Low;
        enum Status { Unstarted, Working, Completed};
        Status status = Unstarted;
        int prerequisiteDay = -1;
        int prerequisiteSlot = -1;
        int requisiteDay = -1;
        int requisiteSlot= -1;
    };

    void TaskInfoVisualEffect(QPushButton *TaskVisualButton, Task *taskToChange);

    //The stroage holding all the tasks
    Task TaskStorage[366][5];

};

#endif // MAINWINDOW_H
