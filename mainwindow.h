#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <Windows.h>
#include <QDesktopServices>
#include <QProcess>
#include <QtSql>
#include <QSqlDatabase>
#include <QEventLoop>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QProgressBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void AddRasp();
    void DelRasp();
    void Process();
    void ChangeDelBtn();
    void A_Propos();
    void create_window();
    void update_windows(QString text, int progress = -1);

private:
    void Find_Local_Ip();
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
