#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableWidget>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QTimer>
#include <QSet>
#include <QGraphicsOpacityEffect>


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
    void mouseMoveEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;



private slots:
    //see mainwindow.cpp for more info on what each function does
    void on_pushButton_clicked();
    void TaskButton(int column, int row);
    void loadWeek();
    void on_Prev_clicked();
    void on_Next_clicked();
    void saveTaskInfo( QDialog *dialog, int column, int row,  QLineEdit *Name, QPlainTextEdit *Desc, QComboBox *StatusBox, QComboBox *PriorityBox, QComboBox *GroupBox);
    void deleteTaskInfo( QDialog *dialog, int column, int row);
    void on_MonthView_clicked();
    QString getMonthName(QDate date);
    void getWeekDayCycle(QDate date);
    void Unlink(int preday, int preslot, int reqday, int reqslot);
    void on_ListView_clicked();
    void CreatedPopUp();
    void DeletedPopUp();
    void LoadNextList(int start, QVBoxLayout *layout);
    void LoadPrevList(int start,  QVBoxLayout *layout);
    QString DateFormatMonthDay(int date);
    void loadfile();
    void savefile();
    void saveLink(QDialog *dialog, QString Prerequisite , QString Requisite);
    void on_Link_clicked();
    void GroupsDropDown(QPushButton * Button);
    void AddNewGroup();
    void DeleteAGroup();
    void saveGroup( QDialog *dialog, QString saveGroup);
    void SetActiveGroup(QString group);
    void saveRemovalOfGroup( QDialog *dialog, QString group);
    void movetask(int column, int row);
    void refreshList(QVBoxLayout *layout);
    void TaskPressedDuration(QPushButton *task, int column, int row);

    private:
    Ui::MainWindow *ui;
    QLabel *label = nullptr;

    //keeping the table available to various functions
    QTableWidget *table = nullptr;

    //tracking when the user is dragging something
    bool dragging = false;
    bool movingtask = false;

    //tracking when user is viewing calaender in month view
    bool monthViewActive = false;

    //tracking when month changes
    bool MonthChange = false;

    bool ListViewActive = false;
    bool ListLoadingState = true;
    int listDateStart = 0;
    int listDate = 0;
    int listDateEnd = 0;
    QVBoxLayout *taskLayout = nullptr;
    QScrollArea *area = nullptr;
    QWidget *GroupPopUp = nullptr;
    QVBoxLayout *groupLayout = nullptr;
    //sets this private variable at start up to current date and will be used to track the date as use naviagtes through calender
    QDate trackingdate = QDate::currentDate();


    //The info a task will hold
    struct Task {
        QString name;
        QString description;
        QString group;
        enum Priority { Low, Mid, High};
        Priority priority = Low;
        enum Status { Unstarted, Working, Completed};
        Status status = Unstarted;
        int prerequisiteDay = -1;
        int prerequisiteSlot = -1;
        int requisiteDay = -1;
        int requisiteSlot= -1;
    };
    Task tempMoving;
    void TaskInfoVisualEffect(QPushButton *TaskVisualButton, Task *taskToChange);

    //The stroage holding all the tasks
    Task TaskStorage[366][5];

    //The storgae to hold all groups
    QString Groups[10];
    int LastGroup = 0;

    //For keeping track of groups being filtered
    QSet<QString> ActiveGroups;
};

#endif // MAINWINDOW_H
