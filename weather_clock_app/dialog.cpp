#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    tcpSocket = new QTcpSocket(this);
    w = new MainWindow;
    ui->setupUi(this);
    this->showFullScreen();

    connect(ui->ok,&QPushButton::clicked,this,&Dialog::on_ok_clicked);
    connect(ui->skip,&QPushButton::clicked,this,&Dialog::on_skip_clicked);
}

Dialog::~Dialog()
{
    delete ui;
    delete w;
}

void Dialog::usleep(unsigned int msec)
{
  /* 方法1 */
    QElapsedTimer t;

    t.start();
    while (t.elapsed() < msec) {
    QCoreApplication::processEvents();
    }
}

/*void Dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok)  == button)
    {
        if(ui->name->text() == tr("qqq") && ui->pwd->text() == tr("qqq"))
        {

            accept();
        }
        else
        {
            QMessageBox::warning(this,tr("警告"),tr("用户名或密码错误"));
            ui->name->clear();
            ui->pwd->clear();
          //  ui->name->setFocus();
        }
    }
     else if(button == ui->buttonBox->button((QDialogButtonBox::Cancel)))
        //显示弹窗是否确定退出
        ;
}
*/

/*void QSleepTimeSet::Delay_MSec_Suspend(unsigned int msec)
{

    QTime _Timer = QTime::currentTime();

    QTime _NowTimer;
    do{
              _NowTimer=QTime::currentTime();
    }while (_Timer.msecsTo(_NowTimer)<=msec);

}*/

void Dialog::on_ok_clicked()
{

    tcpSocket->abort();//终止当前的连接
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
        {
            qDebug()<<address.toString();
            QString str = address.toString().section('.',0,2);
            QString num;
            qDebug()<<num.number(1)<<endl;
            tcpSocket->connectToHost(str + "." +num.number(1),1234);
            usleep(500);
            if(tcpSocket->waitForConnected(200))
            {
                qDebug()<<"连接成功！"<<endl;
                QString msg = ui->name->text() + ui->pwd->text();
                qDebug()<<msg<<endl;
 //               tcpSocket->write(msg.toLatin1());
                qDebug()<<"发送成功"<<endl;
            }
            w->show();
            return;
        }
    }
}

void Dialog::on_skip_clicked()
{
    w->show();
}
