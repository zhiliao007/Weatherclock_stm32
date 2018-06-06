#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QString message;
    // 用来存放数据的大小信息
    quint16 blockSize;
     //QTcpServer *tcpServer;
private slots:
    void newConnect();//新连接
    void readMessage();//读取数据
    void displayError(QAbstractSocket::SocketError);
   void sendMessage();//发送数据
};

#endif // MAINWINDOW_H
