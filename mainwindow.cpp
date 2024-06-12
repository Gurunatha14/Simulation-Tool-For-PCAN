#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(750,300);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, Qt::darkBlue);
//    QColor color =  QColor(RGB(20,0,102));
//    pal.setColor(QPalette::Background, color);
    this->setPalette(pal);

    this->setWindowTitle("SimulationToolSocketCAN");


    CreateActions();
    TableWidgetDisplay();
    TabsCreation();

    // Right Click menu for Transmit table
    table2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(transmitRightClickedMenu(QPoint)));

    // Right Click menu for Receive table
    table1->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table1,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(receiveRightClickedMenu(QPoint)));

    // Right Click menu for Trace tab  table4
    table4->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table4,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(traceTabTable4RightClickedMenu(QPoint)));


    // CanBus connect
    if(QCanBus::instance()->plugins().contains(QStringLiteral("peakcan"))) {
        // plugin available
        qDebug() << "Plugins are available";
    }

    QString errorString;
    device = QCanBus::instance()->createDevice(QStringLiteral("peakcan"),
                                               QStringLiteral("usb0"), &errorString);

    if(!device) {
        // Error handling goes here
        qDebug()<<"Device disconnected"<<device->errorString();
    }
    if(!device->connectDevice()){
     qDebug()<<"Failed to connect to device "<<device->errorString();
      }
    //status = device->connectDevice();
    qDebug() <<"device connected status : " << device->state();

    QCanBusDevice::Filter filter;
    QList<QCanBusDevice::Filter> filterList;

    //filter.frameId = 0x18FFC281;
    //filter.frameIdMask = 0x00FFFFFF;
    filter.format = QCanBusDevice::Filter::MatchBaseFormat;
    filter.type = QCanBusFrame::DataFrame;
    filterList.append(filter);
    // apply filter
    device->setConfigurationParameter(QCanBusDevice::RawFilterKey,QVariant::fromValue(filterList));
    //set baud rate
    device->setConfigurationParameter(QCanBusDevice::BitRateKey,QVariant(250000));
    connect(device,&QCanBusDevice::framesReceived,this,&MainWindow::receiveCanFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
    if(device){
        device->disconnectDevice();
        delete device;
    }
}

// Main Menubar and toolbar
void MainWindow::CreateActions()
{
    // Menu
    // File Menu
    QMenu *menu_file = new QMenu();
    menu_file->setTitle("File");
    menu_file->setVisible(true);
    menu_file->show();

    QAction *action_open_menu_file = new QAction();
    action_open_menu_file->setText("Open                Ctrl+O ");
    action_open_menu_file->setIcon(QIcon(":/Imgs/Icon/Open.png"));
    menu_file->addAction(action_open_menu_file);

    connect(action_open_menu_file,SIGNAL(triggered()),this,SLOT(action_open_triggered()));

    QAction *action_recent_files_menu_file = new QAction();
    action_recent_files_menu_file->setText("Recent Files");
    menu_file->addAction(action_recent_files_menu_file);

    QAction *action_save_menu_file = new QAction();
    action_save_menu_file->setText("Save                  Ctrl+S ");
    action_save_menu_file->setIcon(QIcon(":/Imgs/Icon/save.png"));
    menu_file->addAction(action_save_menu_file);

    connect(action_save_menu_file,SIGNAL(triggered()),this,SLOT(action_Save_triggered()));

    menu_file->addSeparator();

    QAction *action_settings_menu_file = new QAction();
    action_settings_menu_file->setText("Settings ");
    action_settings_menu_file->setIcon(QIcon(":/Imgs/Icon/Setting.jpg"));
    menu_file->addAction(action_settings_menu_file);

    menu_file->addSeparator();

    QAction *action_exit_menu_file = new QAction();
    action_exit_menu_file->setText("Exit");
    action_exit_menu_file->setIcon(QIcon(":/Imgs/Icon/Exit.png"));
    menu_file->addAction(action_exit_menu_file);

    connect(action_exit_menu_file,SIGNAL(triggered()),this,SLOT(action_Exit_Triggered()));

    ui->menubar->addMenu(menu_file); // end of file menu

    // Can Menu
    QMenu *menu_can = new QMenu();
    menu_can->setTitle("CAN");
    menu_can->setVisible(true);
    menu_can->show();

    QAction *action_connect_menu_can = new QAction();
    action_connect_menu_can->setText("Connect...         Ctrl+B ");
    action_connect_menu_can->setIcon(QIcon(":/Imgs/Icon/Connect.png"));
    menu_can->addAction(action_connect_menu_can);

    QAction *action_disconnect_menu_can = new QAction();
    action_disconnect_menu_can->setText("Disconnect...     Ctrl+D ");
    action_disconnect_menu_can->setIcon(QIcon(":/Imgs/Icon/Dis.png"));
    menu_can->addAction(action_disconnect_menu_can);

    QAction *action_reset_menu_can = new QAction();
    action_reset_menu_can->setText("Reset                        Esc ");
    action_reset_menu_can->setIcon(QIcon(":/Imgs/Icon/Reset.png"));
    menu_can->addAction(action_reset_menu_can);

    ui->menubar->addMenu(menu_can); // end of file menu

    // Edit Menu
    QMenu *menu_edit = new QMenu();
    menu_edit->setTitle("Edit");
    menu_edit->setVisible(true);
    menu_edit->show();

    QAction *action_cut_menu_edit = new QAction();
    action_cut_menu_edit->setText("Cut                                    Ctrl+X");
    action_cut_menu_edit->setIcon(QIcon(":/Imgs/Icon/Cut.png"));
    menu_edit->addAction(action_cut_menu_edit);

    connect(action_cut_menu_edit,SIGNAL(triggered()),this,SLOT(action_cut_triggered()));

    QAction *action_copy_menu_edit = new QAction();
    action_copy_menu_edit->setText("Copy                                 Ctrl+C");
    action_copy_menu_edit->setIcon(QIcon(":/Imgs/Icon/copy.png"));
    menu_edit->addAction(action_copy_menu_edit);

    connect(action_copy_menu_edit,SIGNAL(triggered()),this,SLOT(action_copy_triggered()));

    QAction *action_copy_as_csv_menu_edit = new QAction();
    action_copy_as_csv_menu_edit->setText("Copy As CSV         Shift+Ctrl+C");
    action_copy_as_csv_menu_edit->setIcon(QIcon(":/Imgs/Icon/copy_as_CSV.jpg"));
    menu_edit->addAction(action_copy_as_csv_menu_edit);

    QAction *action_paste_menu_edit = new QAction();
    action_paste_menu_edit->setText("Paste                                 Ctrl+V");
    action_paste_menu_edit->setIcon(QIcon(":/Imgs/Icon/paste.png"));
    menu_edit->addAction(action_paste_menu_edit);

    connect(action_paste_menu_edit,SIGNAL(triggered()),this,SLOT(action_paste_triggered()));

    QAction *action_delete_menu_edit = new QAction();
    action_delete_menu_edit->setText("Delete                                    Del ");
    action_delete_menu_edit->setIcon(QIcon(":/Imgs/Icon/Delete.png"));
    menu_edit->addAction(action_delete_menu_edit);

    QAction *action_select_all_menu_edit = new QAction();
    action_select_all_menu_edit->setText("Select All                          Ctrl+A ");
    action_select_all_menu_edit->setIcon(QIcon(":/Imgs/Icon/select_all.png"));
    menu_edit->addAction(action_select_all_menu_edit);

    QAction *action_clear_all_menu_edit = new QAction();
    action_clear_all_menu_edit->setText("Clear All                        Shift+Esc ");
    action_clear_all_menu_edit->setIcon(QIcon(":/Imgs/Icon/Clear_ALL.jpg"));
    menu_edit->addAction(action_clear_all_menu_edit);

    ui->menubar->addMenu(menu_edit); // end of Edit menu

    // Transmit menu
    QMenu *menu_transmit = new QMenu();
    menu_transmit->setTitle("Transmit");
    menu_transmit->setVisible(true);
    menu_transmit->show();

    QAction *action_new_message_menu_transmit = new QAction();
    action_new_message_menu_transmit->setText("New Message...                   Ins");
    action_new_message_menu_transmit->setIcon(QIcon(":/Imgs/Icon/New.png"));
    menu_transmit->addAction(action_new_message_menu_transmit);

    connect(action_new_message_menu_transmit,SIGNAL(triggered()),this,SLOT(action_new_message_triggered()));

    QAction *action_edit_message_menu_transmit = new QAction();
    action_edit_message_menu_transmit->setText("Edit Message...                 Enter");
    action_edit_message_menu_transmit->setIcon(QIcon(":/Imgs/Icon/Edit.png"));
    menu_transmit->addAction(action_edit_message_menu_transmit);

    QAction *action_send_menu_transmit = new QAction();
    action_send_menu_transmit->setText("Send                                 Space");
    action_send_menu_transmit->setIcon(QIcon(":/Imgs/Icon/Send.png"));
    menu_transmit->addAction(action_send_menu_transmit);

    QAction *action_pause_resume_menu_transmit = new QAction();
    action_pause_resume_menu_transmit->setText("Pause/Resume                Pause");
    //action_pause_resume_menu_transmit->setIcon(QIcon(":/Imgs/Icon/pause.jpg"));
    menu_transmit->addAction(action_pause_resume_menu_transmit);

    menu_transmit->addSeparator();

    QAction *action_process_remote_request_frames_menu_transmit = new QAction();
    action_process_remote_request_frames_menu_transmit->setText("Process Remote Request Frames");
    action_process_remote_request_frames_menu_transmit->setIcon(QIcon(":/Imgs/Icon/Check.png"));
    menu_transmit->addAction(action_process_remote_request_frames_menu_transmit);

    ui->menubar->addMenu(menu_transmit); // end of Transmit menu

    // View menu
    QMenu *menu_view = new QMenu();
    menu_view->setTitle("View");
    menu_view->setVisible(true);
    menu_view->show();

    QAction *action_receive_transmit_menu_view = new QAction();
    action_receive_transmit_menu_view->setText("Receive/Transmit     Alt+1");
    action_receive_transmit_menu_view->setIcon(QIcon(":/Imgs/Icon/Receive_Transmit.png"));
    menu_view->addAction(action_receive_transmit_menu_view);

    QAction *action_trace_menu_view = new QAction();
    action_trace_menu_view->setText("Trace                          Alt+2");
    action_trace_menu_view->setIcon(QIcon(":/Imgs/Icon/Trace.png"));
    menu_view->addAction(action_trace_menu_view);

    ui->menubar->addMenu(menu_view); //End of view menu

    // Trace menu
    QMenu *menu_trace = new QMenu();
    menu_trace->setTitle("Trace");
    menu_trace->setVisible(true);
    menu_trace->show();

    QAction *action_start_menu_trace = new QAction();
    action_start_menu_trace->setText("Start                                   Ctrl+T");
    action_start_menu_trace->setIcon(QIcon(":/Imgs/Icon/start.png"));
    menu_trace->addAction(action_start_menu_trace);

    QAction *action_pause_menu_trace = new QAction();
    action_pause_menu_trace->setText("Pause   ");
    action_pause_menu_trace->setIcon(QIcon(":/Imgs/Icon/pause.png"));
    menu_trace->addAction(action_pause_menu_trace);

    QAction *action_stop_menu_trace = new QAction();
    action_stop_menu_trace->setText("Stop                            Ctrl+Alt+T");
    action_stop_menu_trace->setIcon(QIcon(":/Imgs/Icon/Stop.png"));
    menu_trace->addAction(action_stop_menu_trace);

    QAction *action_save_menu_trace = new QAction();
    action_save_menu_trace->setText("Save...                      Shift+Ctrl+S");
    action_save_menu_trace->setIcon(QIcon(":/Imgs/Icon/save_Trace.jpg"));
    menu_trace->addAction(action_save_menu_trace);

    menu_trace->addSeparator();

    QAction *action_buffer_size_menu_trace = new QAction();
    action_buffer_size_menu_trace->setText("Buffer Size:                      ");
    menu_trace->addAction(action_buffer_size_menu_trace);

    QAction *action_linear_buffer_menu_trace = new QAction();
    action_linear_buffer_menu_trace->setText("Linear Buffer ");
    action_linear_buffer_menu_trace->setIcon(QIcon(":/Imgs/Icon/Linear_Buffer.jpg"));
    menu_trace->addAction(action_linear_buffer_menu_trace);

    QAction *action_ring_buffer_menu_trace = new QAction();
    action_ring_buffer_menu_trace->setText("ring Buffer  ");
    action_ring_buffer_menu_trace->setIcon(QIcon(":/Imgs/Icon/Ring_Buffer.jpg"));
    menu_trace->addAction(action_ring_buffer_menu_trace);

    menu_trace->addSeparator();

    QAction *action_log_data_frames_menu_trace = new QAction();
    action_log_data_frames_menu_trace->setText("Log Data Frames");
    action_log_data_frames_menu_trace->setIcon(QIcon(":/Imgs/Icon/Check.png"));
    menu_trace->addAction(action_log_data_frames_menu_trace);

    QAction *action_log_rtr_menu_trace = new QAction();
    action_log_rtr_menu_trace->setText("Log RTR");
    action_log_rtr_menu_trace->setIcon(QIcon(":/Imgs/Icon/Check.png"));
    menu_trace->addAction(action_log_rtr_menu_trace);

    QAction *action_log_status_changes_menu_trace = new QAction();
    action_log_status_changes_menu_trace->setText("Log Status Changes");
    action_log_status_changes_menu_trace->setIcon(QIcon(":/Imgs/Icon/Check.png"));
    menu_trace->addAction(action_log_status_changes_menu_trace);

    QAction *action_log_error_frames_menu_trace = new QAction();
    action_log_error_frames_menu_trace->setText("Log Error Frames");
    menu_trace->addAction(action_log_error_frames_menu_trace);

    QAction *action_log_error_counter_changes_menu_trace = new QAction();
    action_log_error_counter_changes_menu_trace->setText("Log Error Counter Changes");
    menu_trace->addAction(action_log_error_counter_changes_menu_trace);

    QAction *action_log_events_menu_trace = new QAction();
    action_log_events_menu_trace->setText("Log Events");
    menu_trace->addAction(action_log_events_menu_trace);

    ui->menubar->addMenu(menu_trace); // End of Trace menu

    // Window menu
    QMenu *menu_window = new QMenu();
    menu_window->setTitle("Window");
    menu_window->setVisible(true);
    menu_window->show();

    QAction *action_stay_on_top_menu_window = new QAction();
    action_stay_on_top_menu_window->setText("Stay On Top               Ctrl+I");
    action_stay_on_top_menu_window->setIcon(QIcon(":/Imgs/Icon/StayOnTop.png"));
    menu_window->addAction(action_stay_on_top_menu_window);

    QAction *action_make_floating_menu_window = new QAction();
    action_make_floating_menu_window->setText("Make Floating      ");
    action_make_floating_menu_window->setIcon(QIcon(":/Imgs/Icon/Floating.png"));
    menu_window->addAction(action_make_floating_menu_window);

    ui->menubar->addMenu(menu_window); // End of Window menu

    // Help menu
    QMenu *menu_help = new QMenu();
    menu_help->setTitle("Help");
    menu_help->setVisible(true);
    menu_help->show();

    QAction *action_pcan_view_help_menu_help = new QAction();
    action_pcan_view_help_menu_help->setText("PCAN-View Help ");
    action_pcan_view_help_menu_help->setIcon(QIcon(":/Imgs/Icon/Help.png"));
    menu_help->addAction(action_pcan_view_help_menu_help);

    QAction *action_file_format_menu_help = new QAction();
    action_file_format_menu_help->setText("File Format Specificationfor CAN Trace Files ");
    action_file_format_menu_help->setIcon(QIcon(":/Imgs/Icon/File Format.jpg"));
    menu_help->addAction(action_file_format_menu_help);

    menu_help->addSeparator();

    QAction *action_device_driver_menu_help = new QAction();
    action_device_driver_menu_help->setText("Device Driver Version...");
    action_device_driver_menu_help->setIcon(QIcon(":/Imgs/Icon/Device_Driver.jpg"));
    menu_help->addAction(action_device_driver_menu_help);

    QAction *action_end_user_menu_help = new QAction();
    action_end_user_menu_help->setText("End-User License Agreement... ");
    action_end_user_menu_help->setIcon(QIcon(":/Imgs/Icon/License_management.jpg"));
    menu_help->addAction(action_end_user_menu_help);

    QAction *action_about_menu_help = new QAction();
    action_about_menu_help->setText("About...");
    action_about_menu_help->setIcon(QIcon(":/Imgs/Icon/About.png"));
    menu_help->addAction(action_about_menu_help);

    ui->menubar->addMenu(menu_help); // End of Help menu
    ui->menubar->setStyleSheet("QMenuBar {background:rgb(204,204,255)}");


    //ToolBar
    ui->toolBar->addAction(action_open_menu_file);
    ui->toolBar->addAction(action_save_menu_file);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(action_connect_menu_can);
    ui->toolBar->addAction(action_disconnect_menu_can);
    ui->toolBar->addAction(action_reset_menu_can);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(action_new_message_menu_transmit);
    ui->toolBar->addAction(action_edit_message_menu_transmit);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(action_cut_menu_edit);
    ui->toolBar->addAction(action_copy_menu_edit);
    ui->toolBar->addAction(action_paste_menu_edit);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(action_start_menu_trace);
    ui->toolBar->addAction(action_pause_menu_trace);
    ui->toolBar->addAction(action_stop_menu_trace);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(action_pcan_view_help_menu_help);
    ui->toolBar->addAction(action_about_menu_help);
    ui->toolBar->setStyleSheet("QToolBar {background:rgb(204,204,255)}");
}
// Open action
void MainWindow::action_open_triggered()
{
 QString filter = " Transmit List Files (*.xmt)";
 QString file_name = QFileDialog::getOpenFileName(this,"Open File",QDir::homePath(),filter);
 //QMessageBox::information(this,"..",file_name);
 QFile file(file_name);
     if(!file.open(QFile::ReadOnly | QFile::Text)){
         QMessageBox::warning(this,"Status","File not opened");
     }
}

void MainWindow::action_Save_triggered()
{
     QString filter = " Transmit List Files (*.xmt)";
    QString file_name = QFileDialog::getSaveFileName(this,"To save File Choose location",QDir::homePath(),filter);
    QFile file(file_name);
    //file_path = file_name;
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","File not open");
        return;
    }
    QTextStream out(&file);
    //QString text = ;
    //out << text;
    file.flush();
    file.close();
}

// Action Exit
void MainWindow::action_Exit_Triggered()
{
    QApplication::quit();
}
// table Widgets
void MainWindow::TableWidgetDisplay()
{
    new_window = new QWidget;
    table1 = new QTableWidget(new_window);
    table1->setRowCount(0);
    table1->setColumnCount(6);

    QStringList hLabels;
    hLabels << "CAN-ID" <<"Type" << "Length"<<"Data"<<"Cycle Time"<<"Count";
    table1->setHorizontalHeaderLabels(hLabels);

    table1->verticalHeader()->hide();
//    QStringList vLabels;
//    vLabels << "Receive";
//    table1->setVerticalHeaderLabels(vLabels);

//    // Insert Data in the cell
//    for(int i=0;i<table1->rowCount();i++)
//    {
//        QTableWidgetItem *item;
//        for(int j =0; j<table1->columnCount();j++)
//        {
//            item = new QTableWidgetItem;
//            if(j==0)
//                item->setText("A");

//            table1->setItem(i,j,item);
//        }
//    }

    table2 = new QTableWidget(new_window);
    table2->setRowCount(0);
    table2->setColumnCount(8);

    QStringList hLabels2;
    hLabels2 << "CAN-ID" <<"Type" << "Length"<<"Data"<<"Cycle Time"<<"Count"<<"Trigger"<<"Comment";
    table2->setHorizontalHeaderLabels(hLabels2);

    table2->verticalHeader()->hide();
//    QStringList vLabels2;
//    vLabels2 << "Transmit";
//    table2->setVerticalHeaderLabels(vLabels2);

    // Cell Items Properties
    table1->setSelectionMode(QAbstractItemView::SingleSelection);
    table2->setSelectionMode(QAbstractItemView::SingleSelection);

    table1->setSelectionBehavior(QAbstractItemView::SelectRows);
    table2->setSelectionBehavior(QAbstractItemView::SelectRows);

    table1->setTextElideMode(Qt::ElideRight);
    table2->setTextElideMode(Qt::ElideRight);

    // Table Properties
    table1->setShowGrid(true);
    table1->setGridStyle(Qt::SolidLine);
    table1->setSortingEnabled(true);
    table1->setCornerButtonEnabled(true);

    table2->setShowGrid(true);
    table2->setGridStyle(Qt::SolidLine);
    table2->setSortingEnabled(true);
    table2->setCornerButtonEnabled(true);

    // Header Properties
    table1->horizontalHeader()->setVisible(true);
    table1->horizontalHeader()->setDefaultSectionSize(200);
    table1->horizontalHeader()->setStretchLastSection(true);

    table2->horizontalHeader()->setVisible(true);
    table2->horizontalHeader()->setDefaultSectionSize(200);
    table2->horizontalHeader()->setStretchLastSection(true);

    table1->setStyleSheet("QHeaderView::section { background: rgb(255,229,204)}");
    table2->setStyleSheet("QHeaderView::section { background: rgb(255,229,204)}");

    // Double Clickevent for Transmit(table2)
    connect(table2, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(table2DataItemDoubleClicked(int,int)));

    // PushButton
    button_1 = new QPushButton;
   // button_1->setVisible(true);
    button_1->setText("Receive");
    button_1->setFixedWidth(50);

    button_2 = new QPushButton;
   // button_2->setVisible(true);
    button_2->setText("Transmit");
    button_2->setFixedWidth(50);

//    // Layout horizontal
//    QHBoxLayout *h_layout1 = new QHBoxLayout;
//    h_layout1->addWidget(button_1);
//    h_layout1->addWidget(table1);


//    QHBoxLayout *h_layout2 = new QHBoxLayout;
//    h_layout2->addWidget(button_2);
//    h_layout2->addWidget(table2);

     //Layout vertical
    QVBoxLayout *v_layout = new QVBoxLayout(new_window);
    QSplitter *splitter1 = new QSplitter(new_window);
    splitter1->addWidget(button_1);
    splitter1->addWidget(table1);

    QSplitter *splitter2 = new QSplitter(new_window);
    splitter2->addWidget(button_2);
    splitter2->addWidget(table2);
    //splitter2->setOrientation(Qt::Vertical);
//    splitter2->setLayout(h_layout1);
//    splitter2->setLayout(h_layout2);

    QSplitter *splitter3 = new QSplitter(new_window);
    splitter3->addWidget(splitter1);
    splitter3->addWidget(splitter2);
    splitter3->setOrientation(Qt::Vertical);
    v_layout->addWidget(splitter3);

     //new_window->setLayout(v_layout);
    //this->centralWidget()->setLayout(v_layout);

}
// Right Click menu for Transmit table
void MainWindow::transmitRightClickedMenu(QPoint pos)
{
    //QModelIndex index =table1->indexAt(pos);
    QMenu *menu_transmit_rightclick = new QMenu(this);

    QAction *action_new_message_menu_transmit_rightclick  = new QAction();
    action_new_message_menu_transmit_rightclick ->setText("New Message...                   Ins");
    action_new_message_menu_transmit_rightclick ->setIcon(QIcon(":/Imgs/Icon/New.png"));
    menu_transmit_rightclick ->addAction(action_new_message_menu_transmit_rightclick );

    connect(action_new_message_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_new_message_triggered()));

    QAction *action_edit_message_menu_transmit_rightclick  = new QAction();
    action_edit_message_menu_transmit_rightclick ->setText("Edit Message...                 Enter");
    action_edit_message_menu_transmit_rightclick ->setIcon(QIcon(":/Imgs/Icon/Edit.png"));
    menu_transmit_rightclick ->addAction(action_edit_message_menu_transmit_rightclick );

    connect(action_edit_message_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_edit_message_triggered()));

    menu_transmit_rightclick->addSeparator();

    QAction *action_cut_menu_transmit_rightclick = new QAction();
    action_cut_menu_transmit_rightclick->setText("Cut                                    Ctrl+X");
    action_cut_menu_transmit_rightclick->setIcon(QIcon(":/Imgs/Icon/Cut.png"));
    menu_transmit_rightclick->addAction(action_cut_menu_transmit_rightclick);

    connect(action_cut_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_cut_triggered()));

    QAction *action_copy_menu_transmit_rightclick = new QAction();
    action_copy_menu_transmit_rightclick->setText("Copy                                 Ctrl+C");
    action_copy_menu_transmit_rightclick->setIcon(QIcon(":/Imgs/Icon/copy.png"));
    menu_transmit_rightclick->addAction(action_copy_menu_transmit_rightclick);

    connect(action_copy_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_copy_triggered()));

    QAction *action_paste_menu_transmit_rightclick = new QAction();
    action_paste_menu_transmit_rightclick->setText("Paste                                 Ctrl+V");
    action_paste_menu_transmit_rightclick->setIcon(QIcon(":/Imgs/Icon/paste.png"));
    menu_transmit_rightclick->addAction(action_paste_menu_transmit_rightclick);

    connect(action_paste_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_paste_triggered()));

    QAction *action_delete_menu_transmit_rightclick = new QAction();
    action_delete_menu_transmit_rightclick->setText("Delete                                    Del ");
    action_delete_menu_transmit_rightclick->setIcon(QIcon(":/Imgs/Icon/Delete.png"));
    menu_transmit_rightclick->addAction(action_delete_menu_transmit_rightclick);

    connect(action_delete_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_delete_triggered()));

    QAction *action_clear_all_menu_transmit_rightclick = new QAction();
    action_clear_all_menu_transmit_rightclick->setText("Clear All                        Shift+Esc ");
    action_clear_all_menu_transmit_rightclick->setIcon(QIcon(":/Imgs/Icon/Clear_ALL.jpg"));
    menu_transmit_rightclick->addAction(action_clear_all_menu_transmit_rightclick);

    connect(action_clear_all_menu_transmit_rightclick,SIGNAL(triggered()),this,SLOT(action_clear_all_triggered()));


    menu_transmit_rightclick->addSeparator();

    menu_transmit_rightclick->addAction(new QAction("CAN ID Format"));
    menu_transmit_rightclick->addAction(new QAction("Data Bytes Format"));

    menu_transmit_rightclick->popup(table2->viewport()->mapToGlobal(pos));

}

void MainWindow::table2DataItemDoubleClicked(int,int){
    QTableWidgetItem *itemc = new QTableWidgetItem;
    itemc = table2->item(table2->currentRow()+1,3);
    action_edit_message_triggered();
}
// Right Click menu for Receive table
void MainWindow::receiveRightClickedMenu(QPoint pos1)
{
    //QModelIndex index =table1->indexAt(pos);
    QMenu *menu_receive_rightclick = new QMenu(this);

    QAction *action_cut_menu_receive_rightclick = new QAction();
    action_cut_menu_receive_rightclick->setText("Cut                                    Ctrl+X");
    action_cut_menu_receive_rightclick->setIcon(QIcon(":/Imgs/Icon/Cut.png"));
    menu_receive_rightclick->addAction(action_cut_menu_receive_rightclick);

    connect(action_cut_menu_receive_rightclick,SIGNAL(triggered()),this,SLOT(action_cut_receivetable_triggered()));

    QAction *action_copy_menu_receive_rightclick = new QAction();
    action_copy_menu_receive_rightclick->setText("Copy                                 Ctrl+C");
    action_copy_menu_receive_rightclick->setIcon(QIcon(":/Imgs/Icon/copy.png"));
    menu_receive_rightclick->addAction(action_copy_menu_receive_rightclick);

    connect(action_copy_menu_receive_rightclick,SIGNAL(triggered()),this,SLOT(action_copy_receivetable_triggered()));

    QAction *action_delete_menu_receive_rightclick = new QAction();
    action_delete_menu_receive_rightclick->setText("Delete                                    Del ");
    action_delete_menu_receive_rightclick->setIcon(QIcon(":/Imgs/Icon/Delete.png"));
    menu_receive_rightclick->addAction(action_delete_menu_receive_rightclick);

    connect(action_delete_menu_receive_rightclick,SIGNAL(triggered()),this,SLOT(action_delete_receivetable_triggered()));

    menu_receive_rightclick->addSeparator();

    menu_receive_rightclick->addAction(new QAction("CAN ID Format"));
    menu_receive_rightclick->addAction(new QAction("Data Bytes Format"));

    menu_receive_rightclick->popup(table1->viewport()->mapToGlobal(pos1));

}

void MainWindow::action_cut_receivetable_triggered(){
    action_copy_receivetable_triggered();
    action_delete_receivetable_triggered();
}

void MainWindow::action_copy_receivetable_triggered(){
    QModelIndexList indexes = table1->selectionModel()->selectedRows();
    QTableWidgetItem *item1;
    QString str1;
    for(QModelIndex index:indexes){
        int row = index.row();
        for(int i=0; i<table1->columnCount(); i++){
            item1 = table1->item(row,i);
            if(item1){
                str1 += item1->text();
            }
            str1 += '\t';
        }
    }
    clipboard = QApplication::clipboard();
    clipboard->setText(str1);
}

void MainWindow::action_delete_receivetable_triggered(){
    QModelIndexList indexes = table1->selectionModel()->selectedRows();
    for (QModelIndex index:indexes){
           table1->removeRow(index.row());
    }
}
// Tabs
void MainWindow::TabsCreation()
{
    // Transmit/Receive tab
    tab = new QTabWidget(this);
    tab->addTab(new_window,"Receive/Transmit");
    tab->setTabIcon(0,QIcon(":/Imgs/Icon/Receive_Transmit.png"));
    this->setCentralWidget(tab);

    // Trace Tab
    trace_window = new QWidget;
    table3 = new QTableWidget(trace_window);
    table3->setColumnCount(9);


    QStringList hLabels3;
    hLabels3 << "Stopped" <<"0s" << "0%"<<"Ring Buffer"<<"Rx: 0"<<"Tx: 0"<<"Status: 0"<<"Errors: 0"<<"other: 0";
    table3->setHorizontalHeaderLabels(hLabels3);
    table3->setFixedHeight(25);

    table3->setShowGrid(true);
    table3->setGridStyle(Qt::SolidLine);
    //table3->setSortingEnabled(true);
    table3->setCornerButtonEnabled(true);

    table3->horizontalHeader()->setVisible(true);
    table3->horizontalHeader()->setDefaultSectionSize(200);

    //table3->horizontalHeader()->s
    table3->horizontalHeader()->setStretchLastSection(true);
    table3->setStyleSheet("QHeaderView::section { background: rgb(255,229,204)}");
    table3->verticalHeader()->hide();

    table4 = new QTableWidget(trace_window);
    table4->setRowCount(1);
    table4->setColumnCount(6);

    QStringList hLabels4;
    hLabels4 << "Time" <<"CAN-ID" << "Rx/Tx"<<"Type"<<"Length"<<"Data";
    table4->setHorizontalHeaderLabels(hLabels4);

    table4->setShowGrid(true);
    table4->setGridStyle(Qt::SolidLine);
    table4->setSortingEnabled(true);
    table4->setCornerButtonEnabled(true);

    table4->horizontalHeader()->setVisible(true);
    table4->horizontalHeader()->setDefaultSectionSize(200);
    table4->horizontalHeader()->setStretchLastSection(true);
    table4->verticalHeader()->hide();

    // Layout vertical
    QVBoxLayout *v_layout = new QVBoxLayout;
    v_layout->addWidget(table3);
    v_layout->addWidget(table4);

    trace_window->setLayout(v_layout);

    tab->addTab(trace_window,"Trace");
    tab->setTabIcon(1,QIcon(":/Imgs/Icon/Trace.png"));
}

// Right Click menu for Trace tab table4
void MainWindow::traceTabTable4RightClickedMenu(QPoint pos1)
{
   QMenu *menu_tracetable4_rightclick = new QMenu(this);

   QAction *action_start_menu_tracetable4_rightclick = new QAction();
   action_start_menu_tracetable4_rightclick->setText("Start                                   Ctrl+T");
   action_start_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Start.jpg"));
   menu_tracetable4_rightclick->addAction(action_start_menu_tracetable4_rightclick);

   QAction *action_pause_menu_tracetable4_rightclick = new QAction();
   action_pause_menu_tracetable4_rightclick->setText("Pause   ");
   action_pause_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/pause.jpg"));
   menu_tracetable4_rightclick->addAction(action_pause_menu_tracetable4_rightclick);

   QAction *action_stop_menu_tracetable4_rightclick = new QAction();
   action_stop_menu_tracetable4_rightclick->setText("Stop                            Ctrl+Alt+T");
   action_stop_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Stop.png"));
   menu_tracetable4_rightclick->addAction(action_stop_menu_tracetable4_rightclick);

   QAction *action_save_menu_tracetable4_rightclick = new QAction();
   action_save_menu_tracetable4_rightclick->setText("Save...                      Shift+Ctrl+S");
   action_save_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/save_Trace.jpg"));
   menu_tracetable4_rightclick->addAction(action_save_menu_tracetable4_rightclick);

   menu_tracetable4_rightclick->addSeparator();

   QAction *action_copy_menu_tracetable4_rightclick = new QAction();
   action_copy_menu_tracetable4_rightclick->setText("Copy                                 Ctrl+C");
   action_copy_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/copy.png"));
   menu_tracetable4_rightclick->addAction(action_copy_menu_tracetable4_rightclick);

   menu_tracetable4_rightclick->addSeparator();

   QAction *action_linear_buffer_menu_tracetable4_rightclick = new QAction();
   action_linear_buffer_menu_tracetable4_rightclick->setText("Linear Buffer ");
   action_linear_buffer_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Linear_Buffer.jpg"));
   menu_tracetable4_rightclick->addAction(action_linear_buffer_menu_tracetable4_rightclick);

   QAction *action_ring_buffer_menu_tracetable4_rightclick = new QAction();
   action_ring_buffer_menu_tracetable4_rightclick->setText("ring Buffer  ");
   action_ring_buffer_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Ring_Buffer.jpg"));
   menu_tracetable4_rightclick->addAction(action_ring_buffer_menu_tracetable4_rightclick);

   menu_tracetable4_rightclick->addSeparator();

   QAction *action_log_data_frames_menu_tracetable4_rightclick = new QAction();
   action_log_data_frames_menu_tracetable4_rightclick->setText("Log Data Frames");
   action_log_data_frames_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Check.png"));
   menu_tracetable4_rightclick->addAction(action_log_data_frames_menu_tracetable4_rightclick);

   QAction *action_log_rtr_menu_tracetable4_rightclick = new QAction();
   action_log_rtr_menu_tracetable4_rightclick->setText("Log RTR");
   action_log_rtr_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Check.png"));
   menu_tracetable4_rightclick->addAction(action_log_rtr_menu_tracetable4_rightclick);

   QAction *action_log_status_changes_menu_tracetable4_rightclick = new QAction();
   action_log_status_changes_menu_tracetable4_rightclick->setText("Log Status Changes");
   action_log_status_changes_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Check.png"));
   menu_tracetable4_rightclick->addAction(action_log_status_changes_menu_tracetable4_rightclick);

   QAction *action_log_error_frames_menu_tracetable4_rightclick = new QAction();
   action_log_error_frames_menu_tracetable4_rightclick->setText("Log Error Frames");
   menu_tracetable4_rightclick->addAction(action_log_error_frames_menu_tracetable4_rightclick);

   QAction *action_log_error_counter_changes_menu_tracetable4_rightclick = new QAction();
   action_log_error_counter_changes_menu_tracetable4_rightclick->setText("Log Error Counter Changes");
   menu_tracetable4_rightclick->addAction(action_log_error_counter_changes_menu_tracetable4_rightclick);

    menu_tracetable4_rightclick->addSeparator();

    QAction *action_make_floating_menu_tracetable4_rightclick = new QAction();
    action_make_floating_menu_tracetable4_rightclick->setText("Make Floating      ");
    action_make_floating_menu_tracetable4_rightclick->setIcon(QIcon(":/Imgs/Icon/Floating.png"));
    menu_tracetable4_rightclick->addAction(action_make_floating_menu_tracetable4_rightclick);

   menu_tracetable4_rightclick->popup(table4->viewport()->mapToGlobal(pos1));
}
// Action New Message
void MainWindow::action_new_message_triggered(){
    new_message_window = new QDialog();
    new_message_window->setWindowModality(Qt::WindowModality::ApplicationModal);
    new_message_window->setFixedSize(400,250);
    // line 1
    labelId = new QLabel();
    labelId->setText(tr("ID:(hex) "));
    labelId->setVisible(true);
    lineEditId = new QLineEdit();
    lineEditId->setVisible(true);
    //labelId->setAlignment(Qt::AlignTop);
    //labelId->setBuddy(lineEditId);
    lineEditId->setText("000");
    lineEditId->setMaxLength(8);
    lineEditId->setInputMask("HHHHHHHH");
    lineEditId->setFixedWidth(80);

    labelLength = new QLabel(new_message_window);
    labelLength->setText(tr("Length:"));
    labelLength->setVisible(true);
    comboBoxLength = new QComboBox(new_message_window);
    comboBoxLength->setVisible(true);
    comboBoxLength->setFixedWidth(60);
    //labelLength->setBuddy(comboBoxLength);
    for(int i=8; i>=0; i--){
        comboBoxLength->addItem(QString::number(i));
    }

    connect(comboBoxLength,SIGNAL(currentTextChanged(QString)),this,SLOT(dataLineEditHiding()));

    labelData = new QLabel(new_message_window);
    labelData->setText("Data: (hex) ");
    lineEditData0=new QLineEdit(new_message_window);
    lineEditData0->setText("00");
    lineEditData0->setMaxLength(2);
    lineEditData0->setInputMask("HH");
    lineEditData0->setFixedWidth(20);
    lineEditData1=new QLineEdit(new_message_window);
    lineEditData1->setText("00");
    lineEditData1->setMaxLength(2);
    lineEditData1->setInputMask("HH");
    lineEditData1->setFixedWidth(20);
    lineEditData2=new QLineEdit(new_message_window);
    lineEditData2->setText("00");
    lineEditData2->setMaxLength(2);
    lineEditData2->setInputMask("HH");
    lineEditData2->setFixedWidth(20);
    lineEditData3=new QLineEdit(new_message_window);
    lineEditData3->setText("00");
    lineEditData3->setMaxLength(2);
    lineEditData3->setInputMask("HH");
    lineEditData3->setFixedWidth(20);
    lineEditData4=new QLineEdit(new_message_window);
    lineEditData4->setText("00");
    lineEditData4->setMaxLength(2);
    lineEditData4->setInputMask("HH");
    lineEditData4->setFixedWidth(20);
    lineEditData5=new QLineEdit(new_message_window);
    lineEditData5->setText("00");
    lineEditData5->setMaxLength(2);
    lineEditData5->setInputMask("HH");
    lineEditData5->setFixedWidth(20);
    lineEditData6=new QLineEdit(new_message_window);
    lineEditData6->setText("00");
    lineEditData6->setMaxLength(2);
    lineEditData6->setInputMask("HH");
    lineEditData6->setFixedWidth(20);
    lineEditData7=new QLineEdit(new_message_window);
    lineEditData7->setText("00");
    lineEditData7->setMaxLength(2);
    lineEditData7->setInputMask("HH");
    lineEditData7->setFixedWidth(20);

    // retain size in hidden stae
    QSizePolicy sp_retain0 = lineEditData0->sizePolicy();
    sp_retain0.setRetainSizeWhenHidden(true);
    lineEditData0->setSizePolicy(sp_retain0);
    QSizePolicy sp_retain1 = lineEditData1->sizePolicy();
    sp_retain1.setRetainSizeWhenHidden(true);
    lineEditData1->setSizePolicy(sp_retain1);
    QSizePolicy sp_retain2 = lineEditData2->sizePolicy();
    sp_retain2.setRetainSizeWhenHidden(true);
    lineEditData2->setSizePolicy(sp_retain2);
    QSizePolicy sp_retain3 = lineEditData3->sizePolicy();
    sp_retain3.setRetainSizeWhenHidden(true);
    lineEditData3->setSizePolicy(sp_retain3);
    QSizePolicy sp_retain4 = lineEditData4->sizePolicy();
    sp_retain4.setRetainSizeWhenHidden(true);
    lineEditData4->setSizePolicy(sp_retain4);
    QSizePolicy sp_retain5 = lineEditData5->sizePolicy();
    sp_retain5.setRetainSizeWhenHidden(true);
    lineEditData5->setSizePolicy(sp_retain5);
    QSizePolicy sp_retain6 = lineEditData6->sizePolicy();
    sp_retain6.setRetainSizeWhenHidden(true);
    lineEditData6->setSizePolicy(sp_retain6);
    QSizePolicy sp_retain7 = lineEditData7->sizePolicy();
    sp_retain7.setRetainSizeWhenHidden(true);
    lineEditData7->setSizePolicy(sp_retain7);

    // Layouts
    QVBoxLayout *line1v1 = new QVBoxLayout;
    line1v1->addWidget(labelId);
    line1v1->addWidget(lineEditId);
    line1v1->setAlignment(Qt::AlignTop);

    QVBoxLayout *line1v2 = new QVBoxLayout;
    line1v2->addWidget(labelLength);
    line1v2->addWidget(comboBoxLength);
    line1v2->setAlignment(Qt::AlignTop);

    //line 1 data line edit
    QVBoxLayout *line1v3 = new QVBoxLayout;
    line1v3->addWidget(lineEditData0);
    dataLabel0 = new QLabel("   0");
    line1v3->addWidget(dataLabel0);
    QVBoxLayout *line1v4 = new QVBoxLayout;
    line1v4->addWidget(lineEditData1);
    dataLabel1 = new QLabel("   1");
    line1v4->addWidget(dataLabel1);
    QVBoxLayout *line1v5 = new QVBoxLayout;
    line1v5->addWidget(lineEditData2);
    dataLabel2 = new QLabel("   2");
    line1v5->addWidget(dataLabel2);
    QVBoxLayout *line1v6 = new QVBoxLayout;
    line1v6->addWidget(lineEditData3);
    dataLabel3 = new QLabel("   3");
    line1v6->addWidget(dataLabel3);
    QVBoxLayout *line1v7 = new QVBoxLayout;
    line1v7->addWidget(lineEditData4);
    dataLabel4 = new QLabel("   4");
    line1v7->addWidget(dataLabel4);
    QVBoxLayout *line1v8 = new QVBoxLayout;
    line1v8->addWidget(lineEditData5);
    dataLabel5 = new QLabel("   5");
    line1v8->addWidget(dataLabel5);
    QVBoxLayout *line1v9 = new QVBoxLayout;
    line1v9->addWidget(lineEditData6);
    dataLabel6 = new QLabel("   6");
    line1v9->addWidget(dataLabel6);
    QVBoxLayout *line1v10 = new QVBoxLayout;
    line1v10->addWidget(lineEditData7);
    dataLabel7 = new QLabel("   7");
    line1v10->addWidget(dataLabel7);

    QHBoxLayout *line1h1 = new QHBoxLayout;
    line1h1->addLayout(line1v3);
    line1h1->addLayout(line1v4);
    line1h1->addLayout(line1v5);
    line1h1->addLayout(line1v6);
    line1h1->addLayout(line1v7);
    line1h1->addLayout(line1v8);
    line1h1->addLayout(line1v9);
    line1h1->addLayout(line1v10);

    QVBoxLayout *line1v11 = new QVBoxLayout;
    line1v11->addWidget(labelData);
    line1v11->addLayout(line1h1);

    line1v11->setAlignment(Qt::AlignTop);

    line1 = new QHBoxLayout;
    line1->addLayout(line1v1);
    line1->addLayout(line1v2);
    line1->addLayout(line1v11);

    // Line 2
    labelCycleTime = new QLabel(new_message_window);
    labelCycleTime->setText("Cycle Time: ");
    lineEditCycleTime = new  QLineEdit(new_message_window);
    lineEditCycleTime->setText("0");
    lineEditCycleTime->setMaxLength(10);
    lineEditCycleTime->setInputMask("0000000009");
    lineEditCycleTime->setFixedWidth(80);
    QCheckBox *checkBoxPaused = new QCheckBox("&Paused");
    // Group Box for Message Type
    groupBoxMessageType = new QGroupBox(new_message_window);
    groupBoxMessageType->setTitle("Message Type");
    QCheckBox *checkBox1 = new QCheckBox("&Extended Frame");
    QCheckBox *checkBox2 = new QCheckBox("&CAN FD");
    QCheckBox *checkBox3 = new QCheckBox("&Remote Request");
    QCheckBox *checkBox4 = new QCheckBox("&Bit Rate Switch");

    QHBoxLayout *groupBoxLayouth1 = new QHBoxLayout;
    groupBoxLayouth1->addWidget(checkBox1);
    groupBoxLayouth1->addWidget(checkBox2);

    QHBoxLayout *groupBoxLayouth2 = new QHBoxLayout;
    groupBoxLayouth2->addWidget(checkBox3);
    groupBoxLayouth2->addWidget(checkBox4);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(groupBoxLayouth1);
    groupBoxLayout->addLayout(groupBoxLayouth2);

    groupBoxMessageType->setLayout(groupBoxLayout);

    QVBoxLayout *line2h1 = new QVBoxLayout;
    line2h1->addWidget(labelCycleTime);
    line2h1->addWidget(lineEditCycleTime);
    line2h1->addWidget(checkBoxPaused);

    QHBoxLayout *line2 = new QHBoxLayout;
    line2->addLayout(line2h1);
    line2->addWidget(new QLabel("ms"));
    line2->addStretch();
    line2->addWidget(groupBoxMessageType);

    //line 3
    QLabel *labelComment = new QLabel(new_message_window);
    labelComment->setText("Comment: ");

    lineEditComment = new QLineEdit(new_message_window);

    QHBoxLayout *line3 = new QHBoxLayout;
    line3->addWidget(labelComment);
    line3->addWidget(lineEditComment);

    //line 4
    buttonOk = new QPushButton;
    buttonOk->setText("Ok");
    //Signal for Ok Button Clicked
    connect(buttonOk,SIGNAL(clicked()),this,SLOT(on_buttonOk_clicked()));
    connect(new_message_window,SIGNAL(rejected()),this,SLOT(canSendFrame()));

    //QTableWidget
    buttonCancel = new QPushButton;
    buttonCancel->setText("Cancel");
    //Signal for Cancel Button Clicked
    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(on_buttonCancel_clicked()));

    buttonHelp = new QPushButton;
    buttonHelp->setText("Help");
    buttonHelp->setIcon(QIcon(":/Imgs/Icon/Help.png"));

    QHBoxLayout *line4 = new QHBoxLayout;
    line4->addSpacing(150);
    line4->addWidget(buttonOk);
    line4->addWidget(buttonCancel);
    line4->addWidget(buttonHelp);

    QVBoxLayout *mainl = new QVBoxLayout(new_message_window);
    mainl->addSpacing(10);
    mainl->addLayout(line1);
    mainl->addSpacing(10);
    mainl->addLayout(line2);
    mainl->addSpacing(10);
    mainl->addLayout(line3);
    mainl->addSpacing(10);
    mainl->addLayout(line4);

    new_message_window->setLayout(mainl);
    new_message_window->show();
}
// In new message window hiding data lineEdits
void MainWindow::dataLineEditHiding()
{
    qDebug()<<"Inside Line Edit hiding";
    if(comboBoxLength->currentText().toInt() == 0){
        // to hide
        labelData->hide();
        lineEditData0->hide();
        lineEditData0->setText(nullptr);
        lineEditData1->hide();
        lineEditData1->setText(nullptr);
        lineEditData2->hide();
        lineEditData2->setText(nullptr);
        lineEditData3->hide();
        lineEditData3->setText(nullptr);
        lineEditData4->hide();
        lineEditData4->setText(nullptr);
        lineEditData5->hide();
        lineEditData5->setText(nullptr);
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel0->hide();
        dataLabel1->hide();
        dataLabel2->hide();
        dataLabel3->hide();
        dataLabel4->hide();
        dataLabel5->hide();
        dataLabel6->hide();
        dataLabel7->hide();


    }
    else if(comboBoxLength->currentText().toInt() == 1){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData0->setText("00");
        // To hide
        lineEditData1->hide();
        lineEditData1->setText(nullptr);
        lineEditData2->hide();
        lineEditData2->setText(nullptr);
        lineEditData3->hide();
        lineEditData3->setText(nullptr);
        lineEditData4->hide();
        lineEditData4->setText(nullptr);
        lineEditData5->hide();
        lineEditData5->setText(nullptr);
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel1->hide();
        dataLabel2->hide();
        dataLabel3->hide();
        dataLabel4->hide();
        dataLabel5->hide();
        dataLabel6->hide();
        dataLabel7->hide();

    }
    else if(comboBoxLength->currentText().toInt() == 2){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        // To Hide
        lineEditData2->hide();
        lineEditData2->setText(nullptr);
        lineEditData3->hide();
        lineEditData3->setText(nullptr);
        lineEditData4->hide();
        lineEditData4->setText(nullptr);
        lineEditData5->hide();
        lineEditData5->setText(nullptr);
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel2->hide();
        dataLabel3->hide();
        dataLabel4->hide();
        dataLabel5->hide();
        dataLabel6->hide();
        dataLabel7->hide();
    }
    else if(comboBoxLength->currentText().toInt() == 3){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData2->setVisible(true);
        dataLabel2->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        lineEditData2->setText("00");
        // To Hide
        lineEditData3->hide();
        lineEditData3->setText(nullptr);
        lineEditData4->hide();
        lineEditData4->setText(nullptr);
        lineEditData5->hide();
        lineEditData5->setText(nullptr);
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel3->hide();
        dataLabel4->hide();
        dataLabel5->hide();
        dataLabel6->hide();
        dataLabel7->hide();
    }
    else if(comboBoxLength->currentText().toInt() == 4){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData2->setVisible(true);
        dataLabel2->setVisible(true);
        lineEditData3->setVisible(true);
        dataLabel3->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        lineEditData2->setText("00");
        lineEditData3->setText("00");
        // To Hide
        lineEditData4->hide();
        lineEditData4->setText(nullptr);
        lineEditData5->hide();
        lineEditData5->setText(nullptr);
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel4->hide();
        dataLabel5->hide();
        dataLabel6->hide();
        dataLabel7->hide();

    }
    else if(comboBoxLength->currentText().toInt() == 5){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData2->setVisible(true);
        dataLabel2->setVisible(true);
        lineEditData3->setVisible(true);
        dataLabel3->setVisible(true);
        lineEditData4->setVisible(true);
        dataLabel4->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        lineEditData2->setText("00");
        lineEditData3->setText("00");
        lineEditData4->setText("00");
        // To Hide
        lineEditData5->hide();
        lineEditData5->setText(nullptr);
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel5->hide();
        dataLabel6->hide();
        dataLabel7->hide();
    }
    else if(comboBoxLength->currentText().toInt() == 6){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData2->setVisible(true);
        dataLabel2->setVisible(true);
        lineEditData3->setVisible(true);
        dataLabel3->setVisible(true);
        lineEditData4->setVisible(true);
        dataLabel4->setVisible(true);
        lineEditData5->setVisible(true);
        dataLabel5->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        lineEditData2->setText("00");
        lineEditData3->setText("00");
        lineEditData4->setText("00");
        lineEditData5->setText("00");
        // To Hide
        lineEditData6->hide();
        lineEditData6->setText(nullptr);
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel6->hide();
        dataLabel7->hide();
    }
    else if(comboBoxLength->currentText().toInt() == 7){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData2->setVisible(true);
        dataLabel2->setVisible(true);
        lineEditData3->setVisible(true);
        dataLabel3->setVisible(true);
        lineEditData4->setVisible(true);
        dataLabel4->setVisible(true);
        lineEditData5->setVisible(true);
        dataLabel5->setVisible(true);
        lineEditData6->setVisible(true);
        dataLabel6->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        lineEditData2->setText("00");
        lineEditData3->setText("00");
        lineEditData4->setText("00");
        lineEditData5->setText("00");
        lineEditData6->setText("00");
        // To Hide
        lineEditData7->hide();
        lineEditData7->setText(nullptr);
        dataLabel7->hide();
    }
    else if(comboBoxLength->currentText().toInt() == 8){
        // To show
        labelData->setVisible(true);
        lineEditData0->setVisible(true);
        dataLabel0->setVisible(true);
        lineEditData1->setVisible(true);
        dataLabel1->setVisible(true);
        lineEditData2->setVisible(true);
        dataLabel2->setVisible(true);
        lineEditData3->setVisible(true);
        dataLabel3->setVisible(true);
        lineEditData4->setVisible(true);
        dataLabel4->setVisible(true);
        lineEditData5->setVisible(true);
        dataLabel5->setVisible(true);
        lineEditData6->setVisible(true);
        dataLabel6->setVisible(true);
        lineEditData7->setVisible(true);
        dataLabel7->setVisible(true);
        lineEditData0->setText("00");
        lineEditData1->setText("00");
        lineEditData2->setText("00");
        lineEditData3->setText("00");
        lineEditData4->setText("00");
        lineEditData5->setText("00");
        lineEditData6->setText("00");
        lineEditData7->setText("00");
    }
}
// Action Edit message
void MainWindow::action_edit_message_triggered(){
    new_message_window = new QDialog();
    new_message_window->setWindowModality(Qt::WindowModality::ApplicationModal);
    new_message_window->setFixedSize(400,250);
    // line 1
    labelId = new QLabel();
    labelId->setText(tr("ID:(hex) "));
    labelId->setVisible(true);
    lineEditId = new QLineEdit();
    lineEditId->setVisible(true);
    //labelId->setAlignment(Qt::AlignTop);
    //labelId->setBuddy(lineEditId);
    lineEditId->setText("000");
    lineEditId->setMaxLength(8);
    lineEditId->setInputMask("HHHHHHHH");
    lineEditId->setFixedWidth(80);

    labelLength = new QLabel(new_message_window);
    labelLength->setText(tr("Length:"));
    labelLength->setVisible(true);
    comboBoxLength = new QComboBox(new_message_window);
    comboBoxLength->setVisible(true);
    comboBoxLength->setFixedWidth(60);
    //labelLength->setBuddy(comboBoxLength);
    for(int i=8; i>=0; i--){
        comboBoxLength->addItem(QString::number(i));
    }

    labelData = new QLabel(new_message_window);
    labelData->setText("Data: (hex) ");
    lineEditData0=new QLineEdit(new_message_window);
    lineEditData0->setText("00");
    lineEditData0->setMaxLength(2);
    lineEditData0->setInputMask("HH");
    lineEditData0->setFixedWidth(20);
    lineEditData1=new QLineEdit(new_message_window);
    lineEditData1->setText("00");
    lineEditData1->setMaxLength(2);
    lineEditData1->setInputMask("HH");
    lineEditData1->setFixedWidth(20);
    lineEditData2=new QLineEdit(new_message_window);
    lineEditData2->setText("00");
    lineEditData2->setMaxLength(2);
    lineEditData2->setInputMask("HH");
    lineEditData2->setFixedWidth(20);
    lineEditData3=new QLineEdit(new_message_window);
    lineEditData3->setText("00");
    lineEditData3->setMaxLength(2);
    lineEditData3->setInputMask("HH");
    lineEditData3->setFixedWidth(20);
    lineEditData4=new QLineEdit(new_message_window);
    lineEditData4->setText("00");
    lineEditData4->setMaxLength(2);
    lineEditData4->setInputMask("HH");
    lineEditData4->setFixedWidth(20);
    lineEditData5=new QLineEdit(new_message_window);
    lineEditData5->setText("00");
    lineEditData5->setMaxLength(2);
    lineEditData5->setInputMask("HH");
    lineEditData5->setFixedWidth(20);
    lineEditData6=new QLineEdit(new_message_window);
    lineEditData6->setText("00");
    lineEditData6->setMaxLength(2);
    lineEditData6->setInputMask("HH");
    lineEditData6->setFixedWidth(20);
    lineEditData7=new QLineEdit(new_message_window);
    lineEditData7->setText("00");
    lineEditData7->setMaxLength(2);
    lineEditData7->setInputMask("HH");
    lineEditData7->setFixedWidth(20);
    QVBoxLayout *line1v1 = new QVBoxLayout;
    line1v1->addWidget(labelId);
    line1v1->addWidget(lineEditId);
    line1v1->setAlignment(Qt::AlignTop);

    QVBoxLayout *line1v2 = new QVBoxLayout;
    line1v2->addWidget(labelLength);
    line1v2->addWidget(comboBoxLength);
    line1v2->setAlignment(Qt::AlignTop);
    //line 1 data line edit
    QVBoxLayout *line1v3 = new QVBoxLayout;
    line1v3->addWidget(lineEditData0);
    line1v3->addWidget(new QLabel("   0"));
    QVBoxLayout *line1v4 = new QVBoxLayout;
    line1v4->addWidget(lineEditData1);
    line1v4->addWidget(new QLabel("   1"));
    QVBoxLayout *line1v5 = new QVBoxLayout;
    line1v5->addWidget(lineEditData2);
    line1v5->addWidget(new QLabel("   2"));
    QVBoxLayout *line1v6 = new QVBoxLayout;
    line1v6->addWidget(lineEditData3);
    line1v6->addWidget(new QLabel("   3"));
    QVBoxLayout *line1v7 = new QVBoxLayout;
    line1v7->addWidget(lineEditData4);
    line1v7->addWidget(new QLabel("   4"));
    QVBoxLayout *line1v8 = new QVBoxLayout;
    line1v8->addWidget(lineEditData5);
    line1v8->addWidget(new QLabel("   5"));
    QVBoxLayout *line1v9 = new QVBoxLayout;
    line1v9->addWidget(lineEditData6);
    line1v9->addWidget(new QLabel("   6"));
    QVBoxLayout *line1v10 = new QVBoxLayout;
    line1v10->addWidget(lineEditData7);
    line1v10->addWidget(new QLabel("   7"));

    QHBoxLayout *line1h1 = new QHBoxLayout;
    line1h1->addLayout(line1v3);
    line1h1->addLayout(line1v4);
    line1h1->addLayout(line1v5);
    line1h1->addLayout(line1v6);
    line1h1->addLayout(line1v7);
    line1h1->addLayout(line1v8);
    line1h1->addLayout(line1v9);
    line1h1->addLayout(line1v10);

    QVBoxLayout *line1v11 = new QVBoxLayout;
    line1v11->addWidget(labelData);
    line1v11->addLayout(line1h1);

    line1v11->setAlignment(Qt::AlignTop);

    QHBoxLayout *line1 = new QHBoxLayout;
    line1->addLayout(line1v1);
    line1->addLayout(line1v2);
    line1->addLayout(line1v11);

    // Line 2
    labelCycleTime = new QLabel(new_message_window);
    labelCycleTime->setText("Cycle Time: ");
    lineEditCycleTime = new  QLineEdit(new_message_window);
    lineEditCycleTime->setText("0");
    lineEditCycleTime->setMaxLength(10);
    lineEditCycleTime->setInputMask("0000000009");
    lineEditCycleTime->setFixedWidth(80);
    QCheckBox *checkBoxPaused = new QCheckBox("&Paused");
    // Group Box for Message Type
    groupBoxMessageType = new QGroupBox(new_message_window);
    groupBoxMessageType->setTitle("Message Type");
    QCheckBox *checkBox1 = new QCheckBox("&Extended Frame");
    QCheckBox *checkBox2 = new QCheckBox("&CAN FD");
    QCheckBox *checkBox3 = new QCheckBox("&Remote Request");
    QCheckBox *checkBox4 = new QCheckBox("&Bit Rate Switch");

    QHBoxLayout *groupBoxLayouth1 = new QHBoxLayout;
    groupBoxLayouth1->addWidget(checkBox1);
    groupBoxLayouth1->addWidget(checkBox2);

    QHBoxLayout *groupBoxLayouth2 = new QHBoxLayout;
    groupBoxLayouth2->addWidget(checkBox3);
    groupBoxLayouth2->addWidget(checkBox4);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addLayout(groupBoxLayouth1);
    groupBoxLayout->addLayout(groupBoxLayouth2);

    groupBoxMessageType->setLayout(groupBoxLayout);

    QVBoxLayout *line2h1 = new QVBoxLayout;
    line2h1->addWidget(labelCycleTime);
    line2h1->addWidget(lineEditCycleTime);
    line2h1->addWidget(checkBoxPaused);

    QHBoxLayout *line2 = new QHBoxLayout;
    line2->addLayout(line2h1);
    line2->addWidget(new QLabel("ms"));
    line2->addStretch();
    line2->addWidget(groupBoxMessageType);

    //line 3
    QLabel *labelComment = new QLabel(new_message_window);
    labelComment->setText("Comment: ");

    lineEditComment = new QLineEdit(new_message_window);

    QHBoxLayout *line3 = new QHBoxLayout;
    line3->addWidget(labelComment);
    line3->addWidget(lineEditComment);

    //line 4
    buttonOkEditMessage = new QPushButton;
    buttonOkEditMessage->setText("Ok");
    //Signal for Ok Button Clicked
    connect(buttonOkEditMessage,SIGNAL(clicked()),this,SLOT(on_buttonOkEditMessage_clicked()));

    buttonCancel = new QPushButton;
    buttonCancel->setText("Cancel");
    //Signal for Cancel Button Clicked
    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(on_buttonCancel_clicked()));

    buttonHelp = new QPushButton;
    buttonHelp->setText("Help");
    buttonHelp->setIcon(QIcon(":/Imgs/Icon/Help.png"));

    QHBoxLayout *line4 = new QHBoxLayout;
    line4->addSpacing(150);
    line4->addWidget(buttonOkEditMessage);
    line4->addWidget(buttonCancel);
    line4->addWidget(buttonHelp);

    QVBoxLayout *mainl = new QVBoxLayout(new_message_window);
    mainl->addSpacing(10);
    mainl->addLayout(line1);
    mainl->addSpacing(10);
    mainl->addLayout(line2);
    mainl->addSpacing(10);
    mainl->addLayout(line3);
    mainl->addSpacing(10);
    mainl->addLayout(line4);

    new_message_window->setLayout(mainl);
    new_message_window->show();
}
// Ok Button Clicked in Edit Message Window
void MainWindow::on_buttonOkEditMessage_clicked(){
    int newRow = table2->currentRow();
    QString CAN_Id = lineEditId->text() + 'h';
    //qDebug() <<"CAN Id is " << CAN_Id;
    QString Data = lineEditData0->text() +" "+ lineEditData1->text() +" "+  lineEditData2->text() +" "+  lineEditData3->text()
                   +" "+ lineEditData4->text() +" "+  lineEditData5->text() +" "+  lineEditData6->text() +" "+  lineEditData7->text();
    //qDebug() <<"Data is " << Data;
    QString Length = comboBoxLength->currentText();

    QString Comment = lineEditComment->text();

    QString CycleTime = lineEditCycleTime->text();


    // Displaying data Received from new message popup window
    QTableWidgetItem *itemCanId;
    itemCanId = new QTableWidgetItem;
    itemCanId->setText(CAN_Id);
    table2->setItem(newRow,0,itemCanId);

    QTableWidgetItem *itemLength;
    itemLength = new QTableWidgetItem;
    itemLength->setText(Length);
    table2->setItem(newRow,2,itemLength);

    QTableWidgetItem *itemData;
    itemData = new QTableWidgetItem;
    itemData->setText(Data);
    table2->setItem(newRow,3,itemData);

    QTableWidgetItem *itemCycleTime;
    itemCycleTime = new QTableWidgetItem;
    itemCycleTime->setCheckState(Qt::Checked);
    itemCycleTime->setText(CycleTime);
    table2->setItem(newRow,4,itemCycleTime);

    QTableWidgetItem *itemComment;
    itemComment = new QTableWidgetItem;
    itemComment->setText(Comment);
    table2->setItem(newRow,7,itemComment);

    new_message_window->close();
}
// Action Cut for TransmitTable (table2)
void MainWindow::action_cut_triggered(){

    action_copy_triggered();
    action_delete_triggered();
}
// Action Copy for TransmitTable (table2)
void MainWindow::action_copy_triggered(){
    QModelIndexList indexes = table2->selectionModel()->selectedRows();
    QTableWidgetItem *item1;
    QString str1;
    for(QModelIndex index:indexes){
        int row = index.row();
        for(int i=0; i<table2->columnCount(); i++){
            item1 = table2->item(row,i);
            if(item1){
                str1 += item1->text();
            }
            str1 += '\t';
        }
    }
    clipboard = QApplication::clipboard();
    clipboard->setText(str1);
}

// Action Paste for TransmitTable (table2)
void MainWindow::action_paste_triggered(){
  QString str2 = QApplication::clipboard()->text();
  //qDebug()<< QApplication::clipboard()->text();
   QStringList items =str2.split('\t');
   //QTableWidgetItem *currentItem = table2->currentItem();
   int newRow = table2->rowCount();
   table2->insertRow(newRow);
   for(int col=0;col<items.size();++col){
       if(col<table2->columnCount())
       {
           QTableWidgetItem *newItem = nullptr;
           if(col == 4){
             newItem = new QTableWidgetItem();
             newItem->setCheckState((items.at(col) == true) ? Qt::Unchecked : Qt::Checked);
             newItem->setText(items.at(col));
           }
           else{
               newItem = new QTableWidgetItem(items.at(col));
           }
            table2->setItem(newRow,col,newItem);
       }
   }
}
// Action Delete for TransmitTable (table2)
void MainWindow::action_delete_triggered(){
   QModelIndexList indexes = table2->selectionModel()->selectedRows();
   for (QModelIndex index:indexes){
          table2->removeRow(index.row());
   }
}
// Action Clear All for TransmitTable (table2)
void MainWindow::action_clear_all_triggered(){
    table2->setRowCount(0);
    table2->clearContents();
}

// Ok Button Clicked in New Message Window
void MainWindow::on_buttonOk_clicked(){
    int newRow = table2->rowCount()+1;   //Dynamic row creation for transmit table
    table2->setRowCount(newRow);

    QString CAN_Id = lineEditId->text() + 'h';
    //QString CAN_Id = lineEditId->text();
    //qDebug() <<"CAN Id is " << CAN_Id;
    QString Data = lineEditData0->text() +" "+ lineEditData1->text() +" "+  lineEditData2->text() +" "+  lineEditData3->text()
                   +" "+ lineEditData4->text() +" "+  lineEditData5->text() +" "+  lineEditData6->text() +" "+  lineEditData7->text();
    //qDebug() <<"Data is " << Data;
    QString Length = comboBoxLength->currentText();

    QString Comment = lineEditComment->text();

    QString CycleTime = lineEditCycleTime->text();


    // Displaying data Received from new message popup window
    QTableWidgetItem *itemCanId;
    itemCanId = new QTableWidgetItem;
    itemCanId->setText(CAN_Id);
    table2->setItem(newRow-1,0,itemCanId);

    QTableWidgetItem *itemLength;
    itemLength = new QTableWidgetItem;
    itemLength->setText(Length);
    table2->setItem(newRow-1,2,itemLength);

    QTableWidgetItem *itemData;
    itemData = new QTableWidgetItem;
    itemData->setText(Data);
    table2->setItem(newRow-1,3,itemData);

    QTableWidgetItem *itemCycleTime;
    itemCycleTime = new QTableWidgetItem;
    itemCycleTime->setCheckState(Qt::Checked);
    itemCycleTime->setText(CycleTime);
    table2->setItem(newRow-1,4,itemCycleTime);

    QTableWidgetItem *itemComment;
    itemComment = new QTableWidgetItem;
    itemComment->setText(Comment);
    table2->setItem(newRow-1,7,itemComment);

    new_message_window->close();

//    if (QCanBus::instance()->plugins().contains(QStringLiteral("peakcan"))) {
//        // plugin available
//        qDebug() << "Plugins are available";
//    }

//    QString errorString;
//    device = QCanBus::instance()->createDevice(
//        QStringLiteral("peakcan"), QStringLiteral("usb0"), &errorString);
//    if (!device) {
//        // Error handling goes here
//        qDebug() << errorString;
//    } else {
//        device->connectDevice();
//    }
//   // QCanBusFrame frame;
//    QString CAN_Id1 = lineEditId->text();
//    bool ok1;
//    quint32 CAN_Id_uint = CAN_Id1.toUShort(&ok1,16);
//    frame.setFrameId(CAN_Id_uint);
//    //frame.setFrameType(QCanBusFrame::DataFrame);
//     //unsigned char ch;
//     unsigned int size = Length.toInt();
//    QStringList dataList = Data.split(" ");
//    //QByteArray payload(size,);
////    QByteArray payload;
//    //payload.append(size);
//    bool ok;
//    for(const QString & value : dataList){
//        uchar byteValue = value.toUShort(&ok,16);
//        if(!ok){
//            qDebug() << "Invalid hex value: "<< value;
//            return;
//        }
//        payload.append(byteValue);
//    }

////    while(payload.size() < size){
////        payload.append("0x00");
////    }
//    frame.setPayload(payload);
//    qDebug()<<"Payload "<<payload;
//    //frame.setTimeStamp(300);
//    bool written = device->writeFrame(frame);
//    qDebug() << "written status "<< written;


//    timerCycleTime = new QTimer;
//    connect(timerCycleTime,SIGNAL(timeout()),this,SLOT(cycleTimerTimeOut()));
//    timerCycleTime->setInterval(lineEditCycleTime->text().toInt());
//    timerCycleTime->start();
}

// For sending Frames to CanBus
void MainWindow::canSendFrame(){

    int currentRowis = table2->rowCount()-1;
    //qDebug()<<"current Row is : "<<currentRowis;

    QString CAN_Id = table2->item(currentRowis,0)->text().chopped(1);
    //qDebug() <<"CAN ID Present :"<<CAN_Id;

    bool ok;
    quint32 CAN_Id_unit = CAN_Id.toUInt(&ok,16);
    //qDebug() <<"CAN ID Present :"<<CAN_Id_unit;
    if(!ok){
        qDebug() << "Invalid CAN Id: "<< CAN_Id;
    }
    QString Data = table2->item(currentRowis,3)->text();
    int length = table2->item(currentRowis,2)->text().toInt();
    qDebug()<<"Data Length is "<<length;

    for(int i=1,j=2;i<8;i++,j+=3){
        if(length == i)
            Data.remove(j,(Data.length()-length));
    }

    QStringList dataList = Data.split(" ");
    bool ok1;
    for(const QString & value : dataList){
        uchar byteValue = value.toUShort(&ok1,16);
        if(!ok){
            qDebug() << "Invalid hex value: "<< value;
            continue;
        }
        payload.append(byteValue);
    }
    frame.setFrameId(CAN_Id_unit);
    //frame.setBitrateSwitch(QCanBusFrame::DataFrame);
    frame.setPayload(payload);
    //qDebug()<<"Payload "<<payload;
    bool written = device->writeFrame(frame);
    //device->writeFrame(frame);
    qDebug() << "written status "<< written;


//    timerCycleTime = new QTimer;
//    connect(timerCycleTime,SIGNAL(timeout()),this,SLOT(cycleTimerTimeOut()));
//    int time = table2->item(currentRowis,4)->text().toInt();
//    timerCycleTime->setInterval(time);
//    timerCycleTime->start();
 // device->waitForFramesWritten(0);
    payload.clear();
}

// For Receive Frames from CanBus
void MainWindow::receiveCanFrame()
{
    qDebug()<<"Inside Recceive Can Frame :";
    QCanBusFrame frame = device->readFrame();

    quint16 can_id = frame.frameId();

    QString can_id_str = QString::number(can_id,16) + 'h';

    // Finding if exsists
    int existingRow = -1;
    for(int row = 0; row < table1->rowCount(); ++row){
        QTableWidgetItem *item = table1->item(row,0);
        if(item && item->text() == can_id_str){
            existingRow = row;
            break;
        }
    }
    if(existingRow != -1){
       // Update existing row
        QTableWidgetItem *can_id_receive = new QTableWidgetItem;
        can_id_receive->setText(can_id_str);
        qDebug()<<"Frame id is: "<<can_id_str;
        table1->setItem(existingRow,0,can_id_receive);

        int length = frame.payload().size();
        QString length_str = QString::number(length);
        QTableWidgetItem *length_receive = new QTableWidgetItem;
        length_receive->setText(length_str);
        table1->setItem(existingRow,2,length_receive);
        qDebug()<<"Length is "<<length;

        QByteArray data = frame.payload();
//        QString data_str = QString(data.toHex());
        //QString data_str = QString::number(data.at(0),16) + " " + QString::number(data.at(1),16) + " " + QString::number(data.at(2),16) + " " + QString::number(data.at(3),16);
        QString data_str;
        for(int i =0;i<data.size();i++){
            data_str.append(QString::number(data.at(i),16) + " ");
        }
        QTableWidgetItem *data_receive = new QTableWidgetItem;
        data_receive->setText(data_str);
        table1->setItem(existingRow,3,data_receive);

    }
    else{
        int newRow = table1->rowCount()+1;
        qDebug()<<"row count is "<<newRow;
        table1->setRowCount(newRow);

        QTableWidgetItem *can_id_receive = new QTableWidgetItem;
        can_id_receive->setText(can_id_str);
        qDebug()<<"Frame id is: "<<can_id_str;
        table1->setItem(newRow-1,0,can_id_receive);

        int length = frame.payload().size();
        QString length_str = QString::number(length);
        QTableWidgetItem *length_receive = new QTableWidgetItem;
        length_receive->setText(length_str);
        table1->setItem(newRow-1,2,length_receive);
        qDebug()<<"Length is "<<length;

        QByteArray data = frame.payload();
        //QString data_str = QString(data.toHex());
        //QString data_str = QString::number(data.at(0),16) + " " + QString::number(data.at(1),16) + " " + QString::number(data.at(2),16) + " " + QString::number(data.at(3),16);
        QString data_str;
        for(int i =0;i<data.size();i++){
            data_str.append(QString::number(data.at(i),16) + " ");
        }
        QTableWidgetItem *data_receive = new QTableWidgetItem;
        data_receive->setText(data_str);
        table1->setItem(newRow-1,3,data_receive);
        //qDebug()<<"Received Payload is : "<<data_receive;

    }

}
// cycle Timer Time out
void MainWindow::cycleTimerTimeOut(){
//    frame.setTimeStamp();
//    frame.setPayload(payload);
    device->writeFrame(frame);

}
// Cancel buttton new message window
void MainWindow::on_buttonCancel_clicked(){
    new_message_window->close();

}
