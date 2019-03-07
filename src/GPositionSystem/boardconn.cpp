#include "boardconn.h"
#include  "ui_boardconn.h"

BoardConn::BoardConn(QWidget *parent)
    :QWidget(parent),m_socket(new QTcpSocket(this))
    ,ui(new Ui::BoardConn)
{
    ui->setupUi(this);

}

void BoardConn::createWg()
{

}
