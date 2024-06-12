#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void CreateActions();
    void TableWidgetDisplay();
    void TabsCreation();

    QTableWidget *table1;
    QTableWidget *table2;
    QTableWidget *table3;
    QTableWidget *table4;
    QPushButton *button_1;
    QPushButton *button_2;

    QTabWidget *tab;
    QWidget *new_window;
    QWidget *trace_window;

    //NewMessageDialog *newmessagedialog;
    QDialog *new_message_window;
    QLabel *labelId;
    QLineEdit *lineEditId;
    QLabel *labelLength;
    QComboBox *comboBoxLength;
    QLabel *labelData;
    QLineEdit *lineEditData0;
    QLineEdit *lineEditData1;
    QLineEdit *lineEditData2;
    QLineEdit *lineEditData3;
    QLineEdit *lineEditData4;
    QLineEdit *lineEditData5;
    QLineEdit *lineEditData6;
    QLineEdit *lineEditData7;
    QLabel *dataLabel0;
    QLabel *dataLabel1;
    QLabel *dataLabel2;
    QLabel *dataLabel3;
    QLabel *dataLabel4;
    QLabel *dataLabel5;
    QLabel *dataLabel6;
    QLabel *dataLabel7;
    QLabel *labelCycleTime;
    QLineEdit *lineEditCycleTime;
    QGroupBox *groupBoxMessageType;
    QLineEdit *lineEditComment;
    QPushButton *buttonOk;
    QPushButton *buttonOkEditMessage;
    QPushButton *buttonCancel;
    QPushButton *buttonHelp;

    // New Message dialog layouts
    QHBoxLayout *line1;

    QClipboard *clipboard;

    //Timer
    QTimer *timerCycleTime;

    // Frame
    QCanBusFrame frame;
    QByteArray payload;
    QCanBusDevice *device;

public slots:
    void transmitRightClickedMenu(QPoint pos);
    void receiveRightClickedMenu(QPoint pos);
    void traceTabTable4RightClickedMenu(QPoint pos);

    // table 2
    void table2DataItemDoubleClicked(int,int);

    // Timer
    void cycleTimerTimeOut();
    void canSendFrame();
    // Receive Frame
    void receiveCanFrame();
    // Receive table Actions
    void action_cut_receivetable_triggered();
    void action_copy_receivetable_triggered();
    void action_delete_receivetable_triggered();

    // action open
    void action_open_triggered();
    void action_Save_triggered();
    void action_Exit_Triggered();
    void action_new_message_triggered();
    void dataLineEditHiding(); // new window
    void action_edit_message_triggered();
    void action_clear_all_triggered();
    void action_delete_triggered();
    void action_cut_triggered();
    void action_copy_triggered();
    void action_paste_triggered();
    void on_buttonOk_clicked();
    void on_buttonOkEditMessage_clicked();
    void on_buttonCancel_clicked();
};
#endif // MAINWINDOW_H
