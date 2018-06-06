#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket(this);
    connect(ui->pushButton_send, SIGNAL(clicked()), this, SLOT(sendMessage()));//当点击发送的时候发送数据出去
    connect(this->tcpSocket,SIGNAL(readyRead()),this,SLOT(readMessage()));//当有消息接受时会触发接收
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(ui->pushButton_login,SIGNAL(clicked()),this,SLOT(newConnect())); //新建一个连接

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newConnect()
{
    qDebug()<<5;
  // 初始化数据大小信息为0
    blockSize = 0;
    // 取消已有的连接
    tcpSocket->abort();//终止当前的连接
    tcpSocket->connectToHost(ui->lineEdit_ip->text(),
                             ui->lineEdit_duankou->text().toInt());//与服务器连接，在对话框中输入ip和端口号
 if(tcpSocket->waitForConnected(1000))
    {
       qDebug()<<"连接成功！"<<endl;
       ui->messageLabel->setText("成功！");
   }
 else
     ui->messageLabel->setText("超时");
}

void MainWindow::readMessage()
{
   QByteArray arr=this->tcpSocket->readAll();
   qDebug() << arr;
   QString string = arr;
   this->ui->messageLabel->setText(string);
   //this->ui->textBrowser->setText("hgdiyg");
}

void MainWindow::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}

void MainWindow::sendMessage()
{
  QString username;
    username=ui->lineEdit_username->text();
    qDebug()<<username<<endl;

    // 用于暂存我们要发送的数据
    //QByteArray block;
    //QDataStream out(&block, QIODevice::WriteOnly);
    // 设置数据流的版本，客户端和服务器端使用的版本要相同
    //out.setVersion(QDataStream::Qt_4_0);
    //out << (quint16)0;
    //out<<tr("helloworld");
    //out << username;
    //out.device()->seek(0);
    //out << (quint16)(block.size() - sizeof(quint16));
    // 发送数据成功后，显示提示
   tcpSocket->write(username.toLatin1());
   qDebug()<<"发送成功"<<endl;
}
