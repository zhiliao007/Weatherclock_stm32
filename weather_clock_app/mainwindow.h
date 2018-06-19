#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
//#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initList();
    void addIpItem(QByteArray data);
    void BroadcastGetIpCommand();
    void initBroadcast();
private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QString message;
    // 用来存放数据的大小信息
    quint16 blockSize;
     //QTcpServer *tcpServer;
    QUdpSocket * receiver;
    QUdpSocket * sender;
private slots:
    QString getLocalHostIP();//查询本机IP
    void newConnect();//新连接
    void readMessage();//读取数据
    void displayError(QAbstractSocket::SocketError);
    void sendMessage();//发送数据
    void processPengingDatagram();
};

#endif // MAINWINDOW_H
