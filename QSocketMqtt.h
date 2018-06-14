#ifndef QSOCKETMQTT_H
#define QSOCKETMQTT_H

/*
 * 在此文件实现基于Qt socket的Mqtt的 open close  send receive 四大操作
 * 此四大操作被再次封装为基于Qt socket的Mqtt类
*/

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "MQTTPack/MQTTPacket.h"

int transport_getdata(unsigned char* buf, int count);

class QSocketMqtt : public QObject
{
    Q_OBJECT
private:
    static QSocketMqtt * m_pInstance;
    explicit QSocketMqtt(QObject *parent = 0);
private:
    QString serverip;
    quint16 port;
    QTcpSocket * m_socket;
    QTimer * m_timer;
public:

    void test();

    static QSocketMqtt *GetInstance();
    void setHostName(QString hostname,quint16 hostport);
    int  open();
    int  close();
    int  sendData(char * data,qint64 len);
    int  getData(char * data,qint64 len);
    int  publish(char *pTopic,char*pMessage);
    int  subscrib(char *pTopic, quint8 keepAliveInterval, char * pMessage);
signals:

public slots:
    void onTcpSocketConnectedSlots();
    void onTcpSocketDisconnectedSlots();//基于socket断开连接的slot函数
    void onTcpSocketReceiveDataSlots();//基于socket接收服务器的数据的slot函数

    void onTimerShow();
};

#endif // QSOCKETMQTT_H
