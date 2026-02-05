#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <fstream>
#include <string>
#include <iostream>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Loads the ui of the application
    ui->setupUi(this);

    //sizes the main window based on the users screen
    QSize screenSize = screen()->availableGeometry().size();
    resize(screenSize.width() ,screenSize.height());

    //sets the table size in the main window
    table = ui->tableWidget;

    //Sets up the group button dropdown with the basic add group and deletegroup options
    QPushButton *GroupsDrop = ui->toolButton;
    GroupPopUp = new QWidget(nullptr, Qt::Popup);
    groupLayout = new QVBoxLayout(GroupPopUp);
    connect(GroupsDrop, &QPushButton::clicked, this, [=]() { GroupsDropDown(GroupsDrop);});
    QPushButton *AddGroup = new QPushButton("Add Group");
    QPushButton *DeleteGroup = new QPushButton("Delete Group");
    AddGroup->setStyleSheet("QPushButton {background-color: green;} QPushButton:hover {background-color: rgb(90,180,90); }");
    DeleteGroup->setStyleSheet("QPushButton {background-color: red;} QPushButton:hover {background-color: rgb(210,90,90); }");
    groupLayout->addWidget(AddGroup);
    groupLayout->addWidget(DeleteGroup);
    connect(AddGroup, &QPushButton::clicked, this, [=]() { AddNewGroup();});
    connect(DeleteGroup, &QPushButton::clicked, this, [=]() { DeleteAGroup();});

    //sets color of mainwindow and text that is inherited to all children
    this->setStyleSheet("background-color: rgb(143,188,143); color: black;");

    //styles the table
    table->setStyleSheet("QHeaderView::section{background-color: rgb(90,170,90); border: 0px; margin: 0px; padding: 0px;} QTableWidget{background-color:  rgb(210,240,210); }");

    //styles the buttons in the tool bar
    QString baseButtonDesign = "QPushButton {background-color: rgb(130,230,160); border-color: rgb(75,120,75); border-style: solid; border-radius: 5px; border-width: 5px;} "
        " QPushButton:hover {background-color: rgb(110,200,110); border-color: rgb(75,110,75);} QPushButton:pressed {background-color: rgb(150,230,150); border-color: rgb(95,130,95);}";
    ui->pushButton->setStyleSheet(baseButtonDesign );
    ui->ListView->setStyleSheet(baseButtonDesign );
    ui->MonthView->setStyleSheet(baseButtonDesign);
    GroupsDrop->setStyleSheet(baseButtonDesign);
    ui->Prev->setStyleSheet(baseButtonDesign);
    ui->Next->setStyleSheet(baseButtonDesign);
    ui->Link->setStyleSheet(baseButtonDesign );

    //sizes the table on the mainwindow and strech columns and rows to match table size
    table->setGeometry(screenSize.width() * 0.1, 0, screenSize.width() * 0.875, screenSize.height() * 0.95);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    loadfile();

    //loads the current week into the table on start of application
    loadWeek();
}

//function closes main window
MainWindow::~MainWindow()
{
    delete ui;
}

//function to add groups but saves with saveGroup
void MainWindow::AddNewGroup(){

    //pop up for when the user click on the add group in the group button dropdown
    QDialog *dialog = new QDialog();
    dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialog->setMinimumHeight(100);
    dialog->setMinimumWidth(350);
    dialog->setMaximumHeight(120);
    dialog->setMaximumWidth(375);
    dialog->setStyleSheet("QDialog {background-color: rgb(143,188,143)}");

    //The place for the user to enter the name of the new Group
    QLabel *Label = new QLabel("New Group Name: ", dialog);
    Label->setGeometry(25, 10, 100, 20);
    QLineEdit *Group = new QLineEdit(dialog);
    Group->setGeometry(135, 10, 150, 20);

    //The user must actually click on the submit button to save the new group with the saveGroup function
    QPushButton *submitButton = new QPushButton("submit", dialog);
    submitButton->setGeometry(250,75, 100,20);
    connect(submitButton,&QPushButton::clicked, this,  [=]() { saveGroup(dialog, Group->text()); });
    dialog->show();
}

//saves the group added using the addNewgroup function
void MainWindow::saveGroup( QDialog *dialog, QString group){
    //adds the new group to the group array, incrments the amount of groups, and adds the group as a button in the groups dropdown
    Groups[LastGroup] = group;
    LastGroup++;
    QPushButton *groupbutton = new QPushButton(group);
    groupbutton->setStyleSheet("QPushButton {background-color: blue;} QPushButton:hover {background-color: rgb(90,90,180); }");
    groupLayout->addWidget(groupbutton);
    connect(groupbutton, &QPushButton::clicked, this, [=]() { SetActiveGroup(group);});
    dialog->close();
}

//function that is activated by buttons in the group dropdown toggled buttosn are the filter tags to view speciifc groups only in the table
void MainWindow::SetActiveGroup(QString group){
    //acts like a togle for the group buttons in the group dropdown add or removes groups from the applied filter to tasks shown in the table
    if(ActiveGroups.contains(group)){
        ActiveGroups.remove(group);
    }else{
        ActiveGroups.insert(group);
    }

    //styles the buttons in colors in order for the user to know weather or not the group buttons in the griup dropdown are toggled or not
    for(int i = 0; i < groupLayout->count(); i++){
        QPushButton *buttonToStyle = static_cast<QPushButton*>(groupLayout->itemAt(i)->widget());
        if(buttonToStyle->text() == group){
            if(ActiveGroups.contains(group)){
                buttonToStyle->setStyleSheet("QPushButton {background-color: yellow; color: black;} QPushButton:hover {background-color: rgb(180,180,90); }");
            }else{
                buttonToStyle->setStyleSheet("QPushButton {background-color: blue;} QPushButton:hover {background-color: rgb(90,90,180); }");
            }
            break;
        }
    }

    //refreshes the  table to show the reflected changes
    if(monthViewActive == false){
        loadWeek();
    }else{
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
}

//a function to delete a group but saves with saveRemovalOfGroup function
void MainWindow::DeleteAGroup(){
    //creates the pop up for when the user click delet group in the group dropdown
    QDialog *dialog = new QDialog();
    dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialog->setMinimumHeight(100);
    dialog->setMinimumWidth(350);
    dialog->setMaximumHeight(120);
    dialog->setMaximumWidth(375);
    dialog->setStyleSheet("QDialog {background-color: rgb(143,188,143)}");

    //a place for the user to select the group to remove
    QLabel *Label = new QLabel("Select Group to Remove: ", dialog);
    Label->setGeometry(25, 20, 400, 20);
    QComboBox *GroupBox = new QComboBox(dialog);
    GroupBox->setGeometry(175, 20, 100, 20);
    for(int i = 0; i < LastGroup; i++){
        if(!Groups[i].isEmpty()){
            GroupBox->addItem(Groups[i]);
        }
    }

    //if the user wishes to save the removal of the group they must clcik sumbit in order to call the saveremovalofgroup function
    QPushButton *submitButton = new QPushButton("submit", dialog);
    submitButton->setGeometry(250,75, 100,20);
    connect(submitButton,&QPushButton::clicked, this,  [=]() { saveRemovalOfGroup(dialog, GroupBox->currentText()); });
    dialog->show();
}

//saves the actual removal of group
void MainWindow::saveRemovalOfGroup( QDialog *dialog, QString group){
    //finds the group in the group array and removes it then shifts everything over to remove gap and decrements group count
    int arrayIndex = -1;
    for(int l = 0; l < LastGroup; l++){
        if(Groups[l] == group){
            arrayIndex = l;
            break;
        }
    }
    for(int k = arrayIndex; k < LastGroup - 1; k++){
        Groups[k] = Groups[k + 1];
    }
    LastGroup--;
    Groups[LastGroup].clear();

    //finds the button coresponding to the removed group in the group dropdown and removes it
    for(int i = 0; i < groupLayout->count(); i++){
        QPushButton *buttonToRemove = static_cast<QPushButton*>(groupLayout->itemAt(i)->widget());
        if(buttonToRemove->text() == group){
            groupLayout->removeWidget(buttonToRemove);
            buttonToRemove->deleteLater();
            break;
        }
    }

    //all tasks within that group have their group changed back to none
    for(int i = 0; i < 366; i++){
        for(int k = 0; k < 5; k++){
            Task *task = &TaskStorage[i][k];
            if(!task->name.isEmpty() && task->group == group){
                task->group.clear();
            }
        }
    }
    //removes the group from the filter in case it was in it
    ActiveGroups.remove(group);

    //refreshes the  table to show the reflected changes
    if(monthViewActive == false){
        loadWeek();
    }else{
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
    dialog->close();
}

//Dropdown menu functioan;lity for groups button in mainwindow
void MainWindow::GroupsDropDown(QPushButton *Button){
    //find postion of button then move popo up below it and show it aslo add buttons to create and dlete groups and connect and then go addd a plcae for groups,
    GroupPopUp->move(Button->mapToGlobal(QPoint(0, Button->height())));
    GroupPopUp->show();
}

//open lauch of application will load data from txt file into task storage
void MainWindow::loadfile(){
    //loads the tasks from save file if there has been a save before
    std::ifstream infile("tasksforqtapp.txt");
    std::string line;
    if(!std::getline(infile, line)){
        return;
    }

    //the first line of the save file is the groups they are added to group array and group dropdown
    QStringList values =  QString::fromStdString(line).split('|');
    LastGroup = 0;
    for(int i = 1; i < values.size(); i++){
        Groups[i - 1] = values[i];
        QPushButton *groupbutton = new QPushButton(values[i]);
        groupbutton->setStyleSheet("QPushButton {background-color: blue;} QPushButton:hover {background-color: rgb(90,90,180); }");
        groupLayout->addWidget(groupbutton);
        connect(groupbutton, &QPushButton::clicked, this, [=]() { SetActiveGroup(values[i]);});
        LastGroup++;
    }

    //the rest of the save file is lines each one being a task loaded into the task storage
    while(std::getline(infile, line)){
        QStringList values = QString::fromStdString(line).split('|');
        Task *task = &TaskStorage[values[0].toInt()][values[1].toInt()];
        task->name = values[2];
        values[3].replace("\\*n", "\n");
        task->description = values[3];
        task->priority = static_cast<Task::Priority>(values[4].toInt());
        task->status = static_cast<Task::Status>(values[5].toInt());
        task->prerequisiteDay = values[6].toInt();
        task->prerequisiteSlot = values[7].toInt();
        task->requisiteDay = values[8].toInt();
        task->requisiteSlot = values[9].toInt();
        task->group = values[10];
    }
}

//everything in taskstorage will be saved onto the txt file
void MainWindow::savefile(){
    std::ofstream outfile("tasksforqtapp.txt");
    std::string line;

    //saves the groups as the first line in the txt file
    outfile << "Groups |";
    for(int k = 0; k < LastGroup; k++){
        outfile << Groups[k].toStdString();
        if(k < LastGroup - 1){
            outfile << "|";
        }
    }
    outfile << "\n";

    //saves each task as a line in the txt file
    for(int i = 0; i < 366; i++){
        for(int k = 0; k < 5; k++){
            Task *task = &TaskStorage[i][k];
            if(!task->name.isEmpty()){
                task->description.replace("\n", "\\*n");
                outfile << i << "|" << k << "|" << task->name.toStdString() << "|" << task->description.toStdString() << "|" << task->priority << "|" << task->status << "|" << task->prerequisiteDay << "|" <<
                    task->prerequisiteSlot << "|" << task->requisiteDay << "|" << task->requisiteSlot << "|" << task->group.toStdString() << "\n";
            }
        }
    }
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
            //places it as a button and connects the button to the releavent task but if filter is in palce only shows tasks belonging to the toggled groups
            if(!TaskStorage[trackingdate.addDays(column).dayOfYear()][row].name.isEmpty()  && (ActiveGroups.isEmpty() || ActiveGroups.contains(TaskStorage[trackingdate.addDays(column).dayOfYear()][row].group))){
                QPushButton *TaskVisualButton = new QPushButton(TaskStorage[trackingdate.addDays(column).dayOfYear()][row].name);
                table->setCellWidget(row, column, TaskVisualButton);
                TaskInfoVisualEffect(TaskVisualButton, &TaskStorage[trackingdate.addDays(column).dayOfYear()][row]);
                TaskPressedDuration(TaskVisualButton, trackingdate.addDays(column).dayOfYear(), row);
            }
        }
    }
}

//this function is for loading the content in the table for the viewed month
void MainWindow::on_MonthView_clicked()
{
    //If user click on the month view button and it is not active will load the month or if a month change has occured
    if(monthViewActive == false || MonthChange == true){
        //sets the month view to be active
        monthViewActive = true;
        //clears the table from past view too allow it to be filled with the new info
        table->clearContents();
        //sets the month view button to say week view so that the user is aware cliking on it again wil take them back to week view
        ui->MonthView->setText("Week View");

        //gets the month that is being view and displays it on mainwindow for user to be aware of it
        ui->MonthDisplay->setText(getMonthName(trackingdate));

        //uses helper fuction to determine the week cycle based on the first day of month ie  if first day is wednsday then it will go wednsday, thursday, friday...
        getWeekDayCycle(trackingdate.addDays(-trackingdate.day() + 1));

        //gets the first day of the month as a int value fror the array to use to select realted task
        int startofMonth = trackingdate.dayOfYear() - trackingdate.day() + 1;
        //variables to keep track of the row and column in the table to place the contianer holding the days tasks
        int week = 0, day = 0;

        //a loop to go through all the days of the month being viewed
        for(int i = 0; i < trackingdate.daysInMonth(); i++){
            //adds a container with a layout to each day since qtable boxes can only hold one widget so this one widget will be a container that can hold more widgets
            //to contain all tasks since each task is represent by a qbutton widget
            QWidget *container = new QWidget();
            QVBoxLayout *layoutOfContainer = new QVBoxLayout(container);
            container->setStyleSheet("background-color:  rgb(210,240,210); color: black;");

            //creates a label to hold the day number and displays it in the corresponding box in the table
            QLabel *Daynumber = new QLabel(QString::number(i + 1));
            Daynumber->setFixedSize(50,20);
            layoutOfContainer->addWidget(Daynumber);

            //for each day it will cycle through all five slots meant for tasks asscoiated with that day
            for(int k = 0; k < 5; k++){
                //if it finds a task in the slot and it match the groups toggled creates a visual button and places in the container to be seen in the box and connects it to
                //releated task along with visuallly changing the buttons look to match its stored information
                if(!TaskStorage[startofMonth + i][k].name.isEmpty() && (ActiveGroups.isEmpty() || ActiveGroups.contains(TaskStorage[startofMonth + i][k].group))){
                    QPushButton *TaskVisualButton = new QPushButton(TaskStorage[(startofMonth + i)][k].name);
                    layoutOfContainer->addWidget(TaskVisualButton);
                    TaskInfoVisualEffect(TaskVisualButton, &TaskStorage[(startofMonth + i)][k]);
                    TaskPressedDuration(TaskVisualButton, startofMonth + i, k);
                }
            }
            //adds the container to the table in the repective box
            table->setCellWidget(week, day, container);
            //updates the row and column of the table being viewed
            day++;
            if(day == 7){
                day = 0;
                week++;
            }
        }
    }else{
        //if it is month view clciking on it again when is says weeks view will just change it back to week view
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

    //adjusts the date and loads the table based on the date
    if(monthViewActive == false){
        if(days <=  8){
            trackingdate = trackingdate.addDays( 8 - days);
        }
        trackingdate = trackingdate.addDays( -7);
        loadWeek();
    }else{
        if(days >= 32){
            trackingdate = trackingdate.addDays( - trackingdate.day() + 1);
            trackingdate = trackingdate.addMonths(-1);
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

    //adjusts the date and loads the table based on the date
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
    label = new QLabel("  Task", this);
    label->setStyleSheet("QLabel { background-color : gray; color : black; }");

    //Places the label once created where the users cursor is at
    label->setGeometry( mapFromGlobal(QCursor::pos()).x(), mapFromGlobal(QCursor::pos()).y(), 50, 40);
    label->show();

    //sets the private variable dragging to true for other fuctions to use
    dragging = true;
    grabMouse();
}

//making the label follow the user cursor
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(label && (dragging == true)){
        label->move(mapFromGlobal(QCursor::pos()) - QPoint(label->width() / 2, label->height() / 2));
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    savefile();
    event->accept();
}

//handles droping the task into the table
void MainWindow::mousePressEvent(QMouseEvent *event)
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
            if(movingtask){
                newTask = tempMoving;
            }
            QPushButton *TaskVisualButton = new QPushButton("New Task", this);
            bool placed = false;

            //checks if the box on the table clicked is empty in week view to ensure user is not attempting to creaste a task where one alreay resides
            if(!table->cellWidget(row, column) && (monthViewActive == false)){
                //places the visual button onto the table at specified location
                table->setCellWidget(row, column, TaskVisualButton);
                //places the datatype into the table at appropite date that the table and array line up on
                TaskStorage[trackingdate.addDays(column).dayOfYear()][row] = newTask;
                //connects the visual button to the right task in the array so that when the button is click on the table it will change the correct task
                connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { TaskButton(trackingdate.addDays(column).dayOfYear() ,row); });
                placed = true;
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
                        placed = true;
                    }
                }
            }
            if(placed){
                if(movingtask){
                    if(monthViewActive){
                        MonthChange = true;
                        on_MonthView_clicked();
                        MonthChange = false;
                    }else{
                        loadWeek();
                    }
                }
                //cleans up the label for user visual feedback since no longer needed
                movingtask = false;
                delete label;
                label = nullptr;
                //make sure it know that nothing is being dragged anymore
                dragging = false;
                releaseMouse();

            }
        }
    }
}

//this fuction determines if the task on table is being clicked for opening edit menu or attempting to drag task to new date
void MainWindow::TaskPressedDuration(QPushButton *task, int column, int row){
    //creates a timer for each button that starts once pressed
    QTimer *pressedDuration = new QTimer(task);
    //starts timer when pressed
    connect(task, &QPushButton::pressed, this, [=]() { pressedDuration->start(400); });
    //if short click opend edit menu
    connect(task, &QPushButton::released, this, [=]() {
        if(pressedDuration->isActive()){
            pressedDuration->stop();
            TaskButton(column, row);
        }
    });
    //if time runs out it means user is attempting to drag task
    connect(pressedDuration, &QTimer::timeout, this, [=]() { movetask(column, row); });
}

void MainWindow::movetask(int column, int row){
    movingtask = true;
    tempMoving = TaskStorage[column][row];

    //The Data Type Task is created with no information a blank
    Task blank;

    //The blank is used to overwirte any saved infomartion of the task that is saved in order to return to how it was before to create the effect of deletion
    TaskStorage[column][row] = blank;
    if(monthViewActive){
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }else{
        loadWeek();
    }
    on_pushButton_clicked();
}

//This fuction is to open the edit menu for editing tasks
void MainWindow::TaskButton(int column, int row)
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
    QLabel *LabelName = new QLabel("Enter Task Name: ", dialog);
    LabelName->setGeometry(25, 10, 100, 20);

    //The field where users should enter task name
    QLineEdit *Name = new QLineEdit(selectedTask->name, dialog);
    Name->setGeometry(150, 10, 100, 20);

    //Tells the user to enter task description in the text field
    QLabel *DescName = new QLabel("Enter Task Description: ", dialog);
    DescName->setGeometry(25, 50, 120, 20);

    //The field where users should enter task description
    QPlainTextEdit *Desc = new QPlainTextEdit(selectedTask->description, dialog);
    Desc->setGeometry(150, 50, 300, 100);

    //Tells the user to enter task priority in the dropdown field
    QLabel *PriorityLabel = new QLabel("Task Priority: ", dialog);
    PriorityLabel->setGeometry(225, 175, 100, 20);

    //The field where users should enter the priority
    QComboBox *PriorityBox = new QComboBox(dialog);
    PriorityBox->setGeometry(300, 175, 100, 20);
    PriorityBox->addItems({"Low", "Mid", "High"});
    PriorityBox->setCurrentIndex(selectedTask->priority);

    //Tells the user to enter task status in the dropdown field
    QLabel *StatusLabel = new QLabel("Task Status: ", dialog);
    StatusLabel->setGeometry(25, 175, 100, 20);

    //The field where users should enter the status
    QComboBox *StatusBox = new QComboBox(dialog);
    StatusBox->setGeometry(100, 175, 100, 20);
    StatusBox->addItems({"Unstarted" ,"Working", "Done"});
    StatusBox->setCurrentIndex(selectedTask->status);

    QLabel *PreReq = new QLabel( "Prerequiste Task: ", dialog);
    PreReq->setGeometry(25, 225, 150, 20);

    QPushButton *unlinkpre = new QPushButton("None", dialog);
    unlinkpre->setGeometry(125, 225, 150, 20);

    QPushButton *unlinkreq = new QPushButton("None", dialog);
    unlinkreq->setGeometry(125, 250, 150, 20);

    QLabel *Req = new QLabel("Requiste Task: ", dialog);
    Req->setGeometry(25, 250, 150, 20);

    if(selectedTask->prerequisiteDay > -1  && selectedTask->prerequisiteSlot > -1 && !TaskStorage[selectedTask->prerequisiteDay][selectedTask->prerequisiteSlot].name.isEmpty()){
        unlinkpre->setText(TaskStorage[selectedTask->prerequisiteDay][selectedTask->prerequisiteSlot].name);
        connect(unlinkpre, &QPushButton::clicked, this, [=]() { Unlink(selectedTask->prerequisiteDay, selectedTask->prerequisiteSlot, column, row); });
    }
    if(selectedTask->requisiteDay > -1 && selectedTask->requisiteSlot > -1 && !TaskStorage[selectedTask->requisiteDay][selectedTask->requisiteSlot].name.isEmpty()){
        unlinkreq->setText( TaskStorage[selectedTask->requisiteDay][selectedTask->requisiteSlot].name);
        connect(unlinkreq, &QPushButton::clicked, this, [=]() { Unlink(column, row, selectedTask->requisiteDay, selectedTask->requisiteSlot ); });

    }

    //Tells the user to enter task status in the dropdown field
    QLabel *GroupLabel = new QLabel("Group: ", dialog);
    GroupLabel->setGeometry(25, 280, 100, 20);

    //The field where users select a group
    QComboBox *GroupBox = new QComboBox(dialog);
    GroupBox->setGeometry(100, 280, 100, 20);
    GroupBox->addItem("None");
    for(int i = 0; i < LastGroup; i++){
        if(!Groups[i].isEmpty()){
            GroupBox->addItem(Groups[i]);
        }
    }
    if(GroupBox->findText(selectedTask->group) != -1){
        GroupBox->setCurrentIndex(GroupBox->findText(selectedTask->group));
    }else{
        GroupBox->setCurrentIndex(0);
    }

    //The submit button in the edit menu that will handle saving the info
    QPushButton *submitButton = new QPushButton("submit", dialog);
    submitButton->setGeometry(350,320, 100,30);
    connect(submitButton,&QPushButton::clicked, this,  [=]() { saveTaskInfo(dialog, column, row, Name, Desc, StatusBox, PriorityBox, GroupBox); });

    //The delete button in the edit menu that will handle deleting the info
    QPushButton *deleteButton = new QPushButton("delete", dialog);
    deleteButton->setGeometry(25,320, 100,30);
    connect(deleteButton,&QPushButton::clicked, this,  [=]() { deleteTaskInfo(dialog, column, row); });

    //shows the edit menu
    dialog->show();
}

void MainWindow::Unlink(int preday, int preslot, int reqday, int reqslot){
    TaskStorage[preday][preslot].requisiteDay = -1;
    TaskStorage[preday][preslot].requisiteSlot = -1;
    TaskStorage[reqday][reqslot].prerequisiteDay = -1;
    TaskStorage[reqday][reqslot].prerequisiteSlot = -1;
    //refreshes the tasks in the table to show the reflected changes to the task
    if(ListViewActive){
        refreshList(taskLayout);
    }else if(monthViewActive == false){
        loadWeek();
    }else{
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
}

//This function is for when the user clicks the submit button within the edit menu that is used for editing tasks
void MainWindow::saveTaskInfo( QDialog *dialog, int column, int row,  QLineEdit *Name, QPlainTextEdit *Desc, QComboBox *StatusBox, QComboBox *PriorityBox, QComboBox *GroupBox ){
    //Gets the relavent task within the task storage that users is editing
    Task *selectedTask = &TaskStorage[column][row];

    //Gathers the info currently in the edit menu and saves it to the task in the task storage
    selectedTask->name = Name->text();
    if(Name->text().isEmpty()){
        selectedTask->name = "Blank";
    }
    selectedTask->description = Desc->toPlainText();

    if(GroupBox->currentText() != "None"){
        selectedTask->group = GroupBox->currentText();
    }else{
        selectedTask->group.clear();
    }

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

    //refreshes the tasks in the table to show the reflected changes to the task
    if(ListViewActive){
        refreshList(taskLayout);
    }else if(monthViewActive == false){
        loadWeek();
    }else{
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }

    //closes the edit menu of the task
    dialog->close();
    CreatedPopUp();
}

//This function is for when the user clicks the delete button within the edit menu that is used for editing tasks
void MainWindow::deleteTaskInfo(QDialog *dialog, int column, int row){

    //The Data Type Task is created with no information a blank
    Task blank;

    //The blank is used to overwirte any saved infomartion of the task that is saved in order to return to how it was before to create the effect of deletion
    TaskStorage[column][row] = blank;

    //Closes the task edit menu
    dialog->close();
    DeletedPopUp();

    //refreshes the tasks in the table to show the visual deletion of the task
    if(monthViewActive){
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }else if(ListViewActive){
        refreshList(taskLayout);
    }else{
        loadWeek();
    }
}

//helper function to get dates month as string
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

//fuction to style task buttons in table
void MainWindow::TaskInfoVisualEffect(QPushButton *TaskVisualButton, Task *taskToChange){
    QString base ="QPushButton{ color: black; border: solid; border-radius: 5px;";
    QString color;
    QGraphicsOpacityEffect *progress = new QGraphicsOpacityEffect(TaskVisualButton);
    if(taskToChange->priority == Task::Low){
        color = "background-color: rgb(150,240,240);";
    } else if(taskToChange->priority == Task::Mid){
        color = "background-color: rgb(230,250,100);";
    } else{
        color = "background-color: rgb(220,100,70);";
    }

    if(taskToChange->prerequisiteDay != -1){
        base += " border-left: 6px solid rgb(30,100,30);";
    }

    if(taskToChange->requisiteDay != -1){
        base += " border-right: 6px solid rgb(30,100,30);";
    }

    if(taskToChange->status == Task::Unstarted){
        progress->setOpacity(1.0);
    } else if(taskToChange->status == Task::Working){
        progress->setOpacity(0.65);
    } else{
        progress->setOpacity(0.3);
        color = "background-color: rgb(163,200,163);";
    }
    base += color;
    base += "}";

    if(taskToChange->priority == Task::Low){
        base += "QPushButton:hover {background-color: rgb(130,220,220); } QPushButton:pressed {background-color: rgb(170,255,255); }";
    } else if(taskToChange->priority == Task::Mid){
        base += "QPushButton:hover {background-color:  rgb(210,230,80); } QPushButton:pressed {background-color: rgb(250,255,120); }";
    } else{
        base += "QPushButton:hover {background-color: rgb(200,80,50); } QPushButton:pressed {background-color: rgb(240,120,90); }";
    }
    TaskVisualButton->setGraphicsEffect(progress);
    TaskVisualButton->setStyleSheet(base);
}

void MainWindow::on_ListView_clicked()
{
    if(ListViewActive){
        ListViewActive = false;
        delete area;
        area = nullptr;
        QString baseButtonDesign = "QPushButton {background-color: rgb(130,230,160); border-color: rgb(75,120,75); border-style: solid; border-radius: 5px; border-width: 5px;} "
                                   " QPushButton:hover {background-color: rgb(110,200,110); border-color: rgb(75,110,75);} QPushButton:pressed {background-color: rgb(150,230,150); border-color: rgb(95,130,95);}";
        ui->MonthView->setStyleSheet(baseButtonDesign);
        ui->MonthView->setEnabled(true);
        ui->Link->setStyleSheet(baseButtonDesign);
        ui->Link->setEnabled(true);
        ui->pushButton->setStyleSheet(baseButtonDesign);
        ui->pushButton->setEnabled(true);
        ui->toolButton->setStyleSheet(baseButtonDesign);
        ui->toolButton->setEnabled(true);
        ui->MonthDisplay->setStyleSheet(baseButtonDesign);
        ui->MonthDisplay->setEnabled(true);
        ui->Prev->setStyleSheet(baseButtonDesign);
        ui->Prev->setEnabled(true);
        ui->Next->setStyleSheet(baseButtonDesign);
        ui->Next->setEnabled(true);
        table->show();
        loadWeek();
    }else{
        //clears screen to set up list view
        QString baseButtonDesign = "QPushButton {background-color: rgba(180,210,180,50%); border-color: rgba(140,150,140,50%); border-style: solid; border-radius: 5px; border-width: 5px;} ";
        ListViewActive = true;
        monthViewActive = false;
        table->clearContents();
        table->hide();
        ui->MonthView->setStyleSheet(baseButtonDesign);
        ui->MonthView->setEnabled(false);
        ui->Link->setStyleSheet(baseButtonDesign);
        ui->Link->setEnabled(false);
        ui->pushButton->setStyleSheet(baseButtonDesign);
        ui->pushButton->setEnabled(false);
        ui->toolButton->setStyleSheet(baseButtonDesign);
        ui->toolButton->setEnabled(false);
        ui->MonthDisplay->setStyleSheet(baseButtonDesign);
        ui->MonthDisplay->setEnabled(false);
        ui->Prev->setStyleSheet(baseButtonDesign);
        ui->Prev->setEnabled(false);
        ui->Next->setStyleSheet(baseButtonDesign);
        ui->Next->setEnabled(false);

        //making of the main lsit view that will show 20 task in scrollable area but can load a different 20 base on firsat and last button for performance
        listDateStart = QDate::currentDate().dayOfYear();
        QWidget *container = new QWidget();
        QWidget *taskContainer = new QWidget();

        //fix styling of list view also get buttons to show up
        area = new QScrollArea(this);
        area->setGeometry( 175, 0, screen()->availableGeometry().size().width() * 0.9, screen()->availableGeometry().size().height() * 0.975);
        area->show();
        area->setWidgetResizable(true);
        area->setStyleSheet("background-color: rgb(210,240,210);");
        area->setWidget(container);
        // container->setStyleSheet("background-color: rgb(100,0,0);");

        QVBoxLayout *mainlayout = new QVBoxLayout(container);
        taskLayout = new QVBoxLayout(taskContainer);

        QPushButton *LoadPrevTasks = new QPushButton("Load Previous Tasks");
        QPushButton *LoadNextTasks = new QPushButton("Load Next Tasks");
        LoadNextTasks->setFixedSize(screen()->availableGeometry().size().width() * 0.825,200);
        mainlayout->setAlignment(Qt::AlignCenter);
        LoadNextTasks->setStyleSheet("QPushButton{background-color: rgb(130,220,130);} QPushButton:hover {background-color: rgb(110,200,110); }");
        LoadPrevTasks->setStyleSheet("QPushButton{background-color: rgb(130,220,130);} QPushButton:hover {background-color: rgb(110,200,110); }");
        LoadPrevTasks->setFixedSize(screen()->availableGeometry().size().width() * 0.825,200);

        connect(LoadPrevTasks, &QPushButton::clicked, this, [=]() { ListLoadingState = false; LoadPrevList(listDateStart - 1, taskLayout); });
        connect(LoadNextTasks, &QPushButton::clicked, this, [=]() { ListLoadingState = true; LoadNextList(listDateEnd + 1, taskLayout); });


        mainlayout->addWidget(LoadPrevTasks);
        container->setLayout(mainlayout);
        mainlayout->addWidget(taskContainer);
        //fuction that will find 20 tasks
        LoadNextList(listDateStart, taskLayout);
        mainlayout->addWidget(LoadNextTasks);
    }

}

void MainWindow::LoadNextList(int start,  QVBoxLayout *layout){

    while(layout->count() > 0){
        QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
    int tasksfound = 0;
    layout->setAlignment(Qt::AlignCenter);
    int recordFirstDay = -1;
    int recordLastDay = -1;

    while(tasksfound != 20 && start <= 365){
        for(int i = 0; i < 5; i++){
            if(!TaskStorage[start][i].name.isEmpty()){
                if(recordFirstDay == -1){
                    recordFirstDay = start;
                }
                recordLastDay = start;
                tasksfound++;
                QWidget *taskcard = new QWidget();
                QLabel *carddate = new QLabel(DateFormatMonthDay(start));
                carddate->setFixedSize(screen()->availableGeometry().size().width() * 0.3,200);
                carddate->setAlignment(Qt::AlignCenter);
                QHBoxLayout *innerlayout = new QHBoxLayout();
                taskcard->setObjectName("Card");
                taskcard->setStyleSheet("QWidget{border: 2px solid black;} QWidget#Card:hover {background-color: rgb(100,100,100);}");
                innerlayout->addWidget(carddate);
                taskcard->setLayout(innerlayout);
                QPushButton *TaskVisualButton = new QPushButton(TaskStorage[start][i].name);
                TaskVisualButton->setFixedSize(screen()->availableGeometry().size().width() * 0.5,200);
                innerlayout->addWidget(TaskVisualButton);
                Task *taskToChange = &TaskStorage[start][i];
                TaskInfoVisualEffect(TaskVisualButton, taskToChange);
                connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { TaskButton(start, i); });
                layout->addWidget(taskcard);
            }
        }
        start++;
    }
    if(recordFirstDay != -1){
        listDateStart = recordFirstDay;
        listDateEnd =  recordLastDay;
    }else{
        listDateStart = 365;
        listDateEnd = 365;
    }
}

//
QString MainWindow::DateFormatMonthDay(int date){
    return QDate::currentDate().addDays(- QDate::currentDate().dayOfYear() + 1).addDays(date).toString("MMMM d");
}

void MainWindow::LoadPrevList(int start, QVBoxLayout *layout){
    while(layout->count() > 0){
        QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
    int tasksfound = 0;
    int recordFirstDay = -1;
    int recordLastDay = -1;

    while(tasksfound != 20 && start >= 0){
        for(int i = 0; i < 5; i++){
            if(!TaskStorage[start][i].name.isEmpty()){
                if(recordLastDay == -1){
                    recordLastDay = start;
                }
                recordFirstDay = start;
                tasksfound++;
                QWidget *taskcard = new QWidget();
                QLabel *carddate = new QLabel(DateFormatMonthDay(start));
                carddate->setFixedSize(screen()->availableGeometry().size().width() * 0.2,200);
                carddate->setAlignment(Qt::AlignCenter);
                QHBoxLayout *innerlayout = new QHBoxLayout();
                taskcard->setObjectName("Card");
                taskcard->setStyleSheet("QWidget{border: 2px solid black;} QWidget#Card:hover {background-color: rgb(100,100,100);}");
                innerlayout->addWidget(carddate);
                taskcard->setLayout(innerlayout);
                QPushButton *TaskVisualButton = new QPushButton(TaskStorage[start][i].name);
                TaskVisualButton->setFixedSize(screen()->availableGeometry().size().width() * 0.5,200);
                innerlayout->addWidget(TaskVisualButton);
                Task *taskToChange = &TaskStorage[start][i];
                TaskInfoVisualEffect(TaskVisualButton, taskToChange);
                connect(TaskVisualButton, &QPushButton::clicked, this, [=]() { TaskButton(start, i);});
                layout->insertWidget(0, taskcard);
            }
        }
        start--;
    }
    if(recordFirstDay != -1){
        listDateStart = recordFirstDay;
        listDateEnd =  recordLastDay;
    }else{
        listDateEnd = 0;
        listDateStart = 0;
    }
}

void MainWindow::refreshList(QVBoxLayout *layout){
    if(ListLoadingState){
        LoadPrevList(listDateStart - 1, layout);
        LoadNextList(listDateEnd + 1, layout);
    }else{
        LoadNextList(listDateEnd + 1, layout);
        LoadPrevList(listDateStart - 1, layout);
    }
}

//
void MainWindow::on_Link_clicked()
{
    QDialog *dialog = new QDialog();
    dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    dialog->setMinimumHeight(120);
    dialog->setMinimumWidth(480);
    dialog->setStyleSheet("QDialog {background-color: rgb(143,188,143)}");

    //
    QLabel *LabelName = new QLabel("Enter  Prerequisite:                                                       Enter  Requisite: ", dialog);
    LabelName->setGeometry(25, 10, 400, 20);
    QComboBox *Prerequisite = new QComboBox(dialog);
    Prerequisite->setEditable(true);
    Prerequisite->setGeometry(300, 50, 200, 30);
    QComboBox *Requisite = new QComboBox(dialog);
    Requisite->setEditable(true);
    Requisite->setGeometry(25, 50, 200, 30);

    //
    for(int i = 0; i < 366; i++){
        for(int j = 0; j < 5; j++){
            Task &task = TaskStorage[i][j];
            if(!task.name.isEmpty()){
                Prerequisite->addItem(task.name ,  QString::number(i) + "|" +  QString::number(j));
                Requisite->addItem(task.name ,  QString::number(i) + "|" +  QString::number(j));
            }
        }
    }

    //
    QPushButton *submitButton = new QPushButton("submit", dialog);
    submitButton->setGeometry(375,100, 100,20);
    connect(submitButton,&QPushButton::clicked, this,  [=]() { saveLink(dialog, Prerequisite->currentData().toString(), Requisite->currentData().toString() ); });
    dialog->show();
}

//adds infomration to the respective tasks on there linked counterpart
void MainWindow::saveLink(QDialog *dialog, QString Prerequisite , QString Requisite){
    QStringList prePos = Prerequisite.split('|');
    QStringList reqPos = Requisite.split('|');
    Task* PreTask = &TaskStorage[reqPos[0].toInt()][reqPos[1].toInt()];
    Task* reqTask = &TaskStorage[prePos[0].toInt()][prePos[1].toInt()];
    reqTask->prerequisiteDay = reqPos[0].toInt();
    reqTask->prerequisiteSlot = reqPos[1].toInt();
    PreTask->requisiteDay =   prePos[0].toInt();
    PreTask->requisiteSlot = prePos[1].toInt();
    dialog->close();
    if(monthViewActive == false){
        loadWeek();
    }else{
        MonthChange = true;
        on_MonthView_clicked();
        MonthChange = false;
    }
}

//a pop up that appers when a task is changed for a short time
void MainWindow::CreatedPopUp(){
    QLabel *Popup = new QLabel("Task successfully updated!", this);
    Popup->setStyleSheet("background-color: rgb(210,240,210); border-radius: 5px");
    Popup->setGeometry(15, 575, 140, 75);
    Popup->show();
    QTimer::singleShot(2000, Popup, &QObject::deleteLater);
}

//a pop up that appers when a task is removed for a short time
void MainWindow::DeletedPopUp(){
    QLabel *Popup = new QLabel("Task successfully removed!", this);
    Popup->setGeometry(15, 575, 140, 75);
    Popup->setStyleSheet("background-color: rgb(210,240,210); border-radius: 5px");
    Popup->show();
    QTimer::singleShot(2000, Popup, &QObject::deleteLater);
}
