#include "QSocketMqtt.h"

QSocketMqtt * QSocketMqtt::m_pInstance = NULL;

QSocketMqtt::QSocketMqtt(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket();
    m_timer = new QTimer();
    connect(m_socket,SIGNAL(connected()),this,SLOT(onTcpSocketConnectedSlots()));
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(onTcpSocketDisconnectedSlots()));
    connect(m_socket,SIGNAL(readyRead()),this,SLOT(onTcpSocketReceiveDataSlots()));

    connect(m_timer,SIGNAL(timeout())  ,this,  SLOT(onTimerShow())   );
}

void QSocketMqtt::test()
{
    this->open();
    this->sendData("start",5);
    m_timer->start(1000);
}

QSocketMqtt *QSocketMqtt::GetInstance()
{
    if(m_pInstance == NULL){
        m_pInstance = new QSocketMqtt();
    }
    return m_pInstance;
}

void QSocketMqtt::setHostName(QString hostname, quint16 hostport)
{
    serverip = hostname;
    port = hostport;
}

int QSocketMqtt::open()
{
    m_socket->abort();
    m_socket->connectToHost(serverip,port);
    if(!m_socket->waitForConnected(10000)){
        return -1;
    }
    return 1;
}

int QSocketMqtt::close()
{
    m_socket->close();
    if(!m_socket->waitForDisconnected(3000)){
        return -1;
    }
    return 1;
}

int QSocketMqtt::sendData(char *data, qint64 len)
{
    int rc =  m_socket->write(data,len);
    if(!m_socket->waitForBytesWritten()){
        return -1;
    }
    return rc;
}

int QSocketMqtt::getData(char *data, qint64 len)
{
    int rc =  m_socket->read(data,len);
    return rc;
}

void QSocketMqtt::onTcpSocketConnectedSlots()
{

}

void QSocketMqtt::onTcpSocketDisconnectedSlots()
{

}

void QSocketMqtt::onTcpSocketReceiveDataSlots()
{

}

void QSocketMqtt::onTimerShow()
{
    m_timer->stop();

    if( !m_socket->waitForReadyRead(60000) ){
        sendData("loop",4);
    }else{
        char data[50];
        memset(data,0,50);
        int rc = getData(data,50);
        qDebug() << "rc = " << rc;
        qDebug() << "data = " << data;
    }

    m_timer->start(500);


}

