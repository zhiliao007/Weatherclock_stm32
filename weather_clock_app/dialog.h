#ifndef DIALOG_H
#define DIALOG_H
#include "mainwindow.h"
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QElapsedTimer>
#include <QNetworkInterface>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_skip_clicked();

    void on_ok_clicked();

private:
    Ui::Dialog *ui;
    MainWindow *w;
    QTcpSocket *tcpSocket;
    void usleep(unsigned int msec);
};

#endif // DIALOG_H
