#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#

//#define GET_HOST_COMMAND "GetCYHost"
#define GET_HOST_COMMAND "GetIPAddr"
#define LOCAL_PORT 11121
#define DEST_PORT 12811
#define TRY_TIMES 1

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

    initList();
    initBroadcast();
    qDebug()<<getLocalHostIP()<<endl;
    qDebug()<<"=============！"<<endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getLocalHostIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
        foreach (QHostAddress address, list)
        {
           if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
           {
               if(address.toString().contains( "192.168."))
               {
                   qDebug()<<address.toString();
                   return address.toString();
               }
           }
        }
           return 0;
}

void MainWindow::newConnect()
{
    qDebug()<<5;
  // 初始化数据大小信息为0
    blockSize = 0;
    // 取消已有的连接
    tcpSocket->abort();//终止当前的连接
/*  tcpSocket->connectToHost(ui->lineEdit_ip->text(),
                             ui->lineEdit_duankou->text().toInt());//与服务器连接，在对话框中输入ip和端口号 */
 //   tcpSocket->connectToHost(,1234);
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

void MainWindow::initBroadcast()
{
    receiver = new QUdpSocket(this);
    /////绑定，第一个参数为端口号，第二儿表示允许其它地址链接该广播
    receiver->bind(LOCAL_PORT,QUdpSocket::ShareAddress);

    //readyRead:每当有数据报来时发送这个信号
    connect(receiver,SIGNAL(readyRead()),this,SLOT(processPengingDatagram()));

    BroadcastGetIpCommand();
}

void MainWindow::BroadcastGetIpCommand()
{
    //QByteArray datagram = "Hello World!";
    QByteArray datagram = GET_HOST_COMMAND;
    int times = TRY_TIMES;
    while(times--)
    {
        //sender->writeDatagram(datagram.data(),datagram.size(),QHostAddress::Broadcast,1066);
        receiver->writeDatagram(datagram.data(),datagram.size(),QHostAddress::Broadcast,DEST_PORT);
    }
}

void MainWindow::initList()
{
    ui->label_test->setFixedWidth(70);
    ui->label_test->setText("广播IP地址：");

    QListWidgetItem* lst1 = new QListWidgetItem("ip list:", ui->listWidget);

    ui->listWidget->insertItem(1, lst1);

    //ui->listWidget->show();

    //setCentralWidget(ui->listWidget);
}

void MainWindow::processPengingDatagram()
{
    //数据报不为空
    while( receiver->hasPendingDatagrams() )
    {
        QByteArray datagram;
        //datagram大小为等待处理数据报的大小才能就收数据;
        datagram.resize( receiver->pendingDatagramSize() );
        //接收数据报
        receiver->readDatagram(datagram.data(),datagram.size());
        //ui->label_test->setText(datagram);
        addIpItem(datagram);
    }
}

void MainWindow::addIpItem(QByteArray data)
{

    QListWidgetItem* lst1 = new QListWidgetItem(data, ui->listWidget);
    ui->listWidget->insertItem(1, lst1);
}
