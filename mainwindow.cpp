#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Loads the ui of the application
    ui->setupUi(this);
    this->setStyleSheet("background-color: #8fbc8f");


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
    table->setStyleSheet("QHeaderView::section{background-color: rgb(90,170,90); border: 0px; margin: 0px; padding: 0px;} QTableWidget{background-color: rgb(220,240,230) }");

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
    table->setGeometry(20, 100, screenSize.width() * 0.9 - 50, screenSize.height() * 0.7 );
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

//this function is for loading the content in the table for the viewed week
void MainWindow::loadWeek(){
    //clears all the old content of the table so it does not get in the way
    table->clearContents();


    QDate date = trackingdate;

    //
    int dayofyear = date.dayOfYear();
    int dayofweek = date.dayOfWeek();
    int startofweek = dayofyear - dayofweek + 1;

    //loops through the columns of the table
    for(int i = 0; i < 7; i++){
        //sets the header of the columns to have the numerical date of the month
        QTableWidgetItem *headerItem = new QTableWidgetItem(QString::number(date.addDays( - dayofweek + 1 + i).day()));
        table->setHorizontalHeaderItem(i, headerItem);

        //loops through the rows of the table
        for(int j = 0; j < 5; j++){
            //if a task is found in task storage the matches the date that the table is currently veiwing
            //places it as a button and connects the button to the releavent task
            if(!TaskStorage[startofweek + i][j].name.isEmpty()){
                QPushButton *TaskVisualButton = new QPushButton(TaskStorage[(startofweek + i)][j].name);
                table->setCellWidget(j, i, TaskVisualButton);
                Task *taskToChange = &TaskStorage[(startofweek + i)][j];
                TaskInfoVisualEffect(TaskVisualButton, taskToChange);
                connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { on_TaskButton_clicked(startofweek + i, j); });
            }
        }
    }
}

void MainWindow::TaskInfoVisualEffect(QPushButton *TaskVisualButton, Task *taskToChange){
   QString base =" "
        "QPushButton{"
        "color: black; "
        "border-style: solid;"
        "border-radius: 10px;"
        "padding: 10px;"
        "border-width: 10px;}"
        "QPushButton:pressed {"
        "color: black;"
        "background-color: red;}";

    if(taskToChange->priority == Task::Low){
        base += "QPushButton{background-color: rgb(70,220,50);}";
    } else if(taskToChange->priority == Task::Mid){
        base += "QPushButton{background-color: rgb(230,250,0);}";
    } else{
        base += "QPushButton{background-color: rgb(220,70,50);}";
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

//this function is to load the previous week/month depending on view
void MainWindow::on_Prev_clicked()
{
    if(monthViewActive == false){
        trackingdate = trackingdate.addDays(-(trackingdate.dayOfWeek() - 1) -7);
        loadWeek();
    }else{
        trackingdate = trackingdate.addDays( - trackingdate.daysInMonth() - trackingdate.day() + 1);
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
}

//this function is to load the next week/month depending on view
void MainWindow::on_Next_clicked()
{
    if(monthViewActive == false){
        trackingdate = trackingdate.addDays(-(trackingdate.dayOfWeek() - 1) + 7);
        loadWeek();
    }else{
        trackingdate = trackingdate.addDays( trackingdate.daysInMonth() - trackingdate.day() + 1);
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
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
    } else if(PriorityBox->currentText() == "Working"){
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


void MainWindow::on_MonthView_clicked()
{
    if(monthViewActive == false || MonthChange == true){
        monthViewActive = true;
        table->clearContents();
        ui->MonthView->setText("Week View");

        QDate date = trackingdate;

        //
        int dayofyear = date.dayOfYear();
        int dayofweek = date.dayOfWeek();
        int startofMonth = dayofyear - date.day() + 1;
        int startofweek = dayofyear - dayofweek + 1;
        int week = 0;
        int day = 0;

        for(int i = 0; i < date.daysInMonth(); i++){
            QWidget *container = new QWidget();
            QVBoxLayout *layoutOfContainer = new QVBoxLayout(container);
            container->setStyleSheet("background-color:  rgb(220,240,230)");
            QTableWidgetItem *headerItem = new QTableWidgetItem(QString::number(i + 1));
            table->setHorizontalHeaderItem(i, headerItem);
            day++;
            if(day == 7){
                day = 0;
                week++;
            }
            QLabel *Daynumber = new QLabel();
            Daynumber->setText(QString::number(i + 1));
            layoutOfContainer->addWidget(Daynumber);

            for(int k = 0; k < 5; k++){
                if(!TaskStorage[startofMonth + i][k].name.isEmpty()){
                    QPushButton *TaskVisualButton = new QPushButton(TaskStorage[(startofMonth + i)][k].name);
                    layoutOfContainer->addWidget(TaskVisualButton);
                    Task *taskToChange = &TaskStorage[(startofMonth + i)][k];
                    // TaskInfoVisualEffect(TaskVisualButton, taskToChange);
                    connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { on_TaskButton_clicked(startofMonth + i, k); });
                }
            }
            table->setCellWidget(week, day, container);
        }
    }else{
        monthViewActive = false;
        ui->MonthView->setText("Month View");
        loadWeek();
    }
}

