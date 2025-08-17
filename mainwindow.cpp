#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Loads the ui of the application
    ui->setupUi(this);
    this->setStyleSheet("background-color: rgb(143,188,143); color: black;");


    //sizes the main window based on the users screen
    QSize screenSize = screen()->availableGeometry().size();
    resize(screenSize.width() * 0.9 ,screenSize.height() * 0.9);

    //sets the table size in the main window
    table = ui->tableWidget;
    QPushButton *CreateButton = ui->pushButton;
    QPushButton *MonthButton = ui->MonthView;

    QToolButton *GroupsDrop = ui->toolButton;
    QToolButton *LeftArrow = ui->Prev;
    QToolButton *RightArrow = ui->Next;
    table->setStyleSheet("QHeaderView::section{background-color: rgb(90,170,90); border: 0px; margin: 0px; padding: 0px;} QTableWidget{background-color:  rgb(210,240,210); }");

    QString baseButtonDesign = "QPushButton {background-color: rgb(130,230,160); border-color: rgb(75,120,75); border-style: solid; border-radius: 5px; border-width: 5px;} "
        " QPushButton:hover {background-color: rgb(110,200,110); border-color: rgb(75,110,75);} QPushButton:pressed {background-color: rgb(150,230,150); border-color: rgb(95,130,95);}";
    QString baseToolButtonDesign =  "QToolButton {background-color: rgb(130,230,160); border-color: rgb(75,120,75); border-style: solid; border-radius: 5px; border-width: 5px;} "
        " QToolButton:hover {background-color: rgb(110,200,110); border-color: rgb(75,110,75);} QToolButton:pressed {background-color: rgb(130,220,130); border-color: rgb(110,130,110);}";
    CreateButton->setStyleSheet(baseButtonDesign );
    MonthButton->setStyleSheet(baseButtonDesign);
    GroupsDrop->setStyleSheet(baseToolButtonDesign);
    LeftArrow->setStyleSheet(baseToolButtonDesign);
    RightArrow->setStyleSheet(baseToolButtonDesign);

    //add buttons
    table->setGeometry(20, 100, screenSize.width() * 0.9 - 50, screenSize.height() * 0.8 );
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //loads the current week into the table on start of application
    loadWeek();

    setMouseTracking(true);
}

//function closes main window
MainWindow::~MainWindow()
{
    delete ui;
}

//this function is for loading the content in the table for the viewed week
void MainWindow::loadWeek(){
    //clears all the old content of the table so it does not get in the way
    table->clearContents();

    //Updates main window to show the month thats being viewed
    ui->MonthDisplay->setText(getMonthName(trackingdate));
    getWeekDayCycle(trackingdate);

    //loops through the columns and rows of the table to fill in any tasks set for thise days
    for(int column = 0; column < 7; column++){
        //sets the header of the columns to have the numerical date of the month and day of week
        table->setHorizontalHeaderItem(column, new QTableWidgetItem(table->horizontalHeaderItem(column)->text() + " " + QString::number(trackingdate.addDays(column).day())));

        for(int row = 0; row < 5; row++){
            //if a task is found in task storage the matches the date that the table is currently veiwing
            //places it as a button and connects the button to the releavent task
            if(!TaskStorage[trackingdate.addDays(column).dayOfYear()][row].name.isEmpty()){
                QPushButton *TaskVisualButton = new QPushButton(TaskStorage[trackingdate.addDays(column).dayOfYear()][row].name);
                table->setCellWidget(row, column, TaskVisualButton);
                TaskInfoVisualEffect(TaskVisualButton, &TaskStorage[trackingdate.addDays(column).dayOfYear()][row]);
                connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { on_TaskButton_clicked(trackingdate.addDays(column).dayOfYear(), row); });
            }
        }
    }
}

void MainWindow::on_MonthView_clicked()
{
    if(monthViewActive == false || MonthChange == true){
        monthViewActive = true;
        table->clearContents();
        ui->MonthView->setText("Week View");

        ui->MonthDisplay->setText(getMonthName(trackingdate));

        int startofMonth = trackingdate.dayOfYear() - trackingdate.day() + 1;

        getWeekDayCycle(trackingdate.addDays(-trackingdate.day() + 1));
        int week = 0;
        int day = 0;

        for(int i = 0; i < trackingdate.daysInMonth(); i++){
            QWidget *container = new QWidget();
            QVBoxLayout *layoutOfContainer = new QVBoxLayout(container);
            QLabel *Daynumber = new QLabel();
            Daynumber->setText(QString::number(i + 1));
            Daynumber->setFixedSize(50,20);
            container->setStyleSheet("background-color:  rgb(210,240,210); color: black;");

            layoutOfContainer->addWidget(Daynumber);

            for(int k = 0; k < 5; k++){
                if(!TaskStorage[startofMonth + i][k].name.isEmpty()){
                    QPushButton *TaskVisualButton = new QPushButton(TaskStorage[(startofMonth + i)][k].name);
                    layoutOfContainer->addWidget(TaskVisualButton);
                    Task *taskToChange = &TaskStorage[(startofMonth + i)][k];
                    TaskInfoVisualEffect(TaskVisualButton, taskToChange);
                    connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { on_TaskButton_clicked(startofMonth + i, k); });
                }
            }
            table->setCellWidget(week, day, container);
            day++;
            if(day == 7){
                day = 0;
                week++;
            }
        }
    }else{
        monthViewActive = false;
        ui->MonthView->setText("Month View");
        loadWeek();
    }
}

//this function is to load the previous week/month depending on view
void MainWindow::on_Prev_clicked()
{
    //period of time it should stick to is the first day of the current year and the last day of the year we want to store ie if i want two years the current and next should not go past end of second year december 31
    int days = trackingdate.dayOfYear() + (365 * (trackingdate.year() - QDate::currentDate().year()));

    if(monthViewActive == false){
        if(days <=  8){
            trackingdate = trackingdate.addDays( 8 - days);
        }
        trackingdate = trackingdate.addDays( -7);
        loadWeek();
    }else{
        if(days >= 32){
            trackingdate = trackingdate.addDays( - trackingdate.daysInMonth() - trackingdate.day() + 1);
            MonthChange = true;
            on_MonthView_clicked();
            MonthChange = false;
        }
    }
}

//this function is to load the next week/month depending on view
void MainWindow::on_Next_clicked()
{
    //period of time it should stick to is the first day of the current year and the last day of the year we want to store ie if i want two years the current and next should not go past end of second year december 31
    int days = trackingdate.dayOfYear() + (365 * (trackingdate.year() - QDate::currentDate().year()));

    if(monthViewActive == false){
        if(days >=  351){
            trackingdate = trackingdate.addDays(  (352 - days));
        }
        trackingdate = trackingdate.addDays(7);
        loadWeek();
    }else{
        if(days <= 365 - trackingdate.daysInMonth()){
            trackingdate = trackingdate.addDays( trackingdate.daysInMonth() - trackingdate.day() + 1);
            MonthChange = true;
            on_MonthView_clicked();
            MonthChange = false;
        }

    }
}


//this function is for when the create task button is clicked to create a label to place in the table
void MainWindow::on_pushButton_clicked()
{
    //Name of label while dragging
    label = new QLabel("New Task", this);

    //first color is for background second for text
    label->setStyleSheet("QLabel { background-color : gray; color : black; }");

    //Places the label once created where the users cursor is at
    QPoint localPos = mapFromGlobal(QCursor::pos());
    label->setGeometry(localPos.x(),localPos.y(), 50, 40);
    label->setMouseTracking(true);

    //makes the label visable
    label->show();

    //sets the private variable dragging to true for other fuctions to use
    dragging = true;
}

//This fuction is to open the edit menu for editing tasks
void MainWindow::on_TaskButton_clicked(int column, int row)
{
    //gets the releavent task for editing from the task storage
    Task *selectedTask = &TaskStorage[column][row];

    //opens the edit page
    QDialog *dialog = new QDialog();
    dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialog->setMinimumHeight(320);
    dialog->setMinimumWidth(480);
    dialog->setStyleSheet("QDialog {background-color: rgb(143,188,143)}");

    //Tells the user to enter task name in the text field
    QLabel *LabelName = new QLabel();
    LabelName->setText("Enter Task Name: ");
    LabelName->setParent(dialog);
    LabelName->show();
    LabelName->setGeometry(25, 10, 100, 20);

    //The field where users should enter task name
    QLineEdit *Name = new QLineEdit(selectedTask->name);
    Name->setText(selectedTask->name);
    Name->setGeometry(150, 10, 100, 20);
    Name->setParent(dialog);
    Name->show();

    //Tells the user to enter task description in the text field
    QLabel *DescName = new QLabel();
    DescName->setText("Enter Task Description: ");
    DescName->setParent(dialog);
    DescName->show();
    DescName->setGeometry(25, 50, 120, 20);

    //The field where users should enter task description
    QLineEdit *Desc = new QLineEdit();
    Desc->setText(selectedTask->description);
    Desc->setToolTip("Enter Task Description: ");
    Desc->setGeometry(150, 50, 100, 30);
    Desc->setParent(dialog);
    Desc->show();

    //Tells the user to enter task start date in the date field
    QLabel *StartLabel = new QLabel();
    StartLabel->setText("Start Date: ");
    StartLabel->setParent(dialog);
    StartLabel->show();
    StartLabel->setGeometry(25, 125, 100, 20);

    //The field where users should enter the start date
    QDateEdit *Start = new QDateEdit();
    Start->setDate(selectedTask->startDate);
    Start->setParent(dialog);
    Start->show();
    Start->setGeometry(100, 125, 100, 20);

    //Tells the user to enter task end date in the date field
    QLabel *EndLabel = new QLabel();
    EndLabel->setText("End Date: ");
    EndLabel->setParent(dialog);
    EndLabel->show();
    EndLabel->setGeometry(225, 125, 100, 20);

    //The field where users should enter the end date
    QDateEdit *End = new QDateEdit();
    End->setDate(selectedTask->endDate);
    End->setParent(dialog);
    End->show();
    End->setGeometry(300, 125, 100, 20);

    //Tells the user to enter task priority in the dropdown field
    QLabel *PriorityLabel = new QLabel();
    PriorityLabel->setText("Task Priority: ");
    PriorityLabel->setParent(dialog);
    PriorityLabel->show();
    PriorityLabel->setGeometry(225, 175, 100, 20);

    //The field where users should enter the priority
    QComboBox *PriorityBox = new QComboBox();
    PriorityBox->setParent(dialog);
    PriorityBox->show();
    PriorityBox->setGeometry(300, 175, 100, 20);
    PriorityBox->addItem("Low");
    PriorityBox->addItem("Mid");
    PriorityBox->addItem("High");
    PriorityBox->setCurrentIndex(selectedTask->priority);


    //Tells the user to enter task status in the dropdown field
    QLabel *StatusLabel = new QLabel();
    StatusLabel->setText("Task Status: ");
    StatusLabel->setParent(dialog);
    StatusLabel->show();
    StatusLabel->setGeometry(25, 175, 100, 20);

    //The field where users should enter the status
    QComboBox *StatusBox = new QComboBox();
    StatusBox->setParent(dialog);
    StatusBox->show();
    StatusBox->setGeometry(100, 175, 100, 20);
    StatusBox->addItem("Unstarted");
    StatusBox->addItem("Working");
    StatusBox->addItem("Done");
    StatusBox->setCurrentIndex(selectedTask->status);


    //The submit button in the edit menu that will handle saving the info
    QPushButton *submitButton = new QPushButton("submit");
    submitButton->setParent(dialog);
    submitButton->show();
    submitButton->setGeometry(200,200, 100,20);
    connect(submitButton,&QPushButton::clicked, this,  [=]() { saveTaskInfo(dialog, column, row, Name, Desc, Start, End, StatusBox, PriorityBox); });

    //The delete button in the edit menu that will handle deleting the info
    QPushButton *deleteButton = new QPushButton("delete");
    deleteButton->setParent(dialog);
    deleteButton->show();
    deleteButton->setGeometry(25,200, 100,20);
    connect(deleteButton,&QPushButton::clicked, this,  [=]() { deleteTaskInfo(dialog, column, row); });

    //shows the edit menu
    dialog->show();
}

//This function is for when the user clicks the submit button within the edit menu that is used for editing tasks
void MainWindow::saveTaskInfo( QDialog *dialog, int column, int row,  QLineEdit *Name, QLineEdit *Desc, QDateEdit *Start, QDateEdit *End, QComboBox *StatusBox, QComboBox *PriorityBox ){
    //Gets the relavent task within the task storage that users is editing
    Task *selectedTask = &TaskStorage[column][row];

    //Gathers the info currently in the edit menu and saves it to the task in the task storage
    selectedTask->name = Name->text();
    if(Name->text().isEmpty()){
        selectedTask->name = "Blank";
    }
    selectedTask->description = Desc->text();
    selectedTask->endDate = End->date();
    selectedTask->startDate = Start->date();

    // selectedTask->priority = PriorityBox->currentText();//maybe optimize, https://stackoverflow.com/questions/16955918/qt-using-enums-with-qcombobox
    if(PriorityBox->currentText() == "Low"){
        selectedTask->priority = Task::Low;
    } else if(PriorityBox->currentText() == "Mid"){
        selectedTask->priority = Task::Mid;
    } else{
        selectedTask->priority = Task::High;
    }

    if(StatusBox->currentText() == "Unstarted"){
        selectedTask->status = Task::Unstarted;
    } else if(StatusBox->currentText() == "Working"){
        selectedTask->status = Task::Working;
    } else{
        selectedTask->status = Task::Completed;
    }

    //closes the edit menu of the task
    dialog->close();

    //refreshes the tasks in the table to show the reflected changes to the task
    if(monthViewActive == false){
        loadWeek();
    }else{
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
}

//This function is for when the user clicks the delete button within the edit menu that is used for editing tasks
void MainWindow::deleteTaskInfo(QDialog *dialog, int column, int row){

    //The Data Type Task is created with no information a blank
    Task blank;

    //The blank is used to overwirte any saved infomartion of the task that is saved in order to return to how it was before to create the effect of deletion
    TaskStorage[column][row] = blank;

    //Closes the task edit menu
    dialog->close();

    //refreshes the tasks in the table to show the visual deletion of the task
    loadWeek();
}


QString MainWindow::getMonthName(QDate date){
    switch(date.month()){
    case 1:
        return "January";
    case 2:
        return "February";
    case 3:
        return "March";
    case 4:
        return "April";
    case 5:
        return "May";
    case 6:
        return "June";
    case 7:
        return "July";
    case 8:
        return "August";
    case 9:
        return "September";
    case 10:
        return "October";
    case 11:
        return "November";
    case 12:
        return "December";
    default:
        return "Month";
    }
}

//helper fuction to get the right order of the week being viewed
void MainWindow::getWeekDayCycle(QDate date){

    switch(date.dayOfWeek()){
        case 7:
            table->setHorizontalHeaderLabels(QStringList({"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"}));
            break;
        case 1:
            table->setHorizontalHeaderLabels(QStringList({"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday", "Sunday"}));
            break;
        case 2:
            table->setHorizontalHeaderLabels(QStringList({"Tuesday","Wednesday","Thursday","Friday","Saturday", "Sunday", "Monday"}));
            break;
        case 3:
            table->setHorizontalHeaderLabels(QStringList({"Wednesday","Thursday","Friday","Saturday", "Sunday", "Monday", "Tuesday"}));
            break;
        case 4:
            table->setHorizontalHeaderLabels(QStringList({"Thursday","Friday","Saturday", "Sunday", "Monday", "Tuesday", "Wednesday"}));
            break;
        case 5:
            table->setHorizontalHeaderLabels(QStringList({"Friday", "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday"}));
            break;
        case 6:
            table->setHorizontalHeaderLabels(QStringList({"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"}));
            break;
        default:
            break;
    }
}

void MainWindow::TaskInfoVisualEffect(QPushButton *TaskVisualButton, Task *taskToChange){
    QString base ="QPushButton{ color: black; } ";

    if(taskToChange->priority == Task::Low){
        base += "QPushButton{background-color: rgb(70,220,50);} QPushButton:hover {background-color: rgb(90,180,90); }";
    } else if(taskToChange->priority == Task::Mid){
        base += "QPushButton{background-color: rgb(230,250,0);} QPushButton:hover {background-color: rgb(200,220,0); }";
    } else{
        base += "QPushButton{background-color: rgb(220,70,50);} QPushButton:hover {background-color: rgb(190,90,90); }";
    }

    if(taskToChange->status == Task::Unstarted){
        base  += "QPushButton{border-color: rgb(170,170,160);;}";
    } else if(taskToChange->status == Task::Working){
        base += "QPushButton{border-color: rgb(110,110,105);;}";
    } else{
        base += "QPushButton{border-color: rgb(60,60,60);;}";
    }
    TaskVisualButton->setStyleSheet(base);
}
