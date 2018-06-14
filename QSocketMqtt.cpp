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
    //测试
    this->publish("test","starthlo");
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

int QSocketMqtt::publish(char *pTopic, char *pMessage)
{
    int32_t len,rc;
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    unsigned char buf[200];
    MQTTString topicString = MQTTString_initializer;
    int msglen = strlen(pMessage);
    int buflen = sizeof(buf);

    data.clientID.cstring = "me";
    data.keepAliveInterval = 5;
    data.cleansession = 1;
    len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */

    topicString.cstring = pTopic;
    len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, (unsigned char*)pMessage, msglen); /* 2 */

    len += MQTTSerialize_disconnect(buf + len, buflen - len); /* 3 */
    this->open();
    rc = this->sendData((char*)buf,len);
    this->close();
    if (rc == len)
            printf("Successfully published\n\r");
        else
            printf("Publish failed\n\r");
      return 0;

}


int transport_getdata(unsigned char* buf, int count)
{
    QSocketMqtt * mqtt;
    mqtt = QSocketMqtt::GetInstance();

  return mqtt->getData((char*)buf,count);
}

int QSocketMqtt::subscrib(char *pTopic, quint8 keepAliveInterval, char *pMessage)
{
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        int rc = 0;
        unsigned char buf[200];
        int buflen = sizeof(buf);
        int msgid = 1;
        MQTTString topicString = MQTTString_initializer;
        int req_qos = 0;

        int len = 0;
        rc = this->open();
        if(rc < 0){
            printf("transport_open error\n\r");
            return rc;
       }


        data.clientID.cstring = "";
        data.keepAliveInterval = keepAliveInterval;//服务器保持连接时间，超过该时间后，服务器会主动断开连接，单位为秒
        data.cleansession = 1;
        data.username.cstring = "";
        data.password.cstring = "";

        len = MQTTSerialize_connect(buf, buflen, &data);
        rc = this->sendData((char*)buf, len);
        if(rc != len){
            printf("connect transport_sendPacketBuffer error\n\r");
            goto exit;
        }

        /* wait for connack */
        this->m_socket->waitForReadyRead(3000);
        if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
        {
            unsigned char sessionPresent, connack_rc;

            if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
            {
                printf("Unable to connect, return code %d\n\r", connack_rc);
                goto exit;
            }
        }else{
            printf("MQTTPacket_read error\n\r");
                goto exit;
       }

        /* subscribe */
        topicString.cstring = pTopic;
        len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

        rc = this->sendData((char*)buf, len);
      if(rc != len){
        printf("connect transport_sendPacketBuffer error\n\r");
        goto exit;
      }
      this->m_socket->waitForReadyRead(3000);
        if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
        {
            unsigned short submsgid;
            int subcount;
            int granted_qos;

            rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
            if (granted_qos != 0)
            {
                printf("granted qos != 0, %d\n\r", granted_qos);
                goto exit;
            }
        }
        else
            goto exit;

        /* loop getting msgs on subscribed topic */
        topicString.cstring = pTopic;
      memset(buf,0,buflen);

      if( !this->m_socket->waitForReadyRead(keepAliveInterval*1000)){
          rc = -1;
          goto exit;
      }
      if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH){
          unsigned char dup;
          int qos;
          unsigned char retained;
          unsigned short msgid;
          int payloadlen_in;
          unsigned char* payload_in;
          MQTTString receivedTopic;

          rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
              &payload_in, &payloadlen_in, buf, buflen);
         // printf("message arrived %d: %s\n\r", payloadlen_in, payload_in);
          strcpy(pMessage,(const char *)payload_in);
        }
       // printf("disconnecting\n\r");
        len = MQTTSerialize_disconnect(buf, buflen);
        rc = this->sendData((char*)buf, len);
      exit:
          this->close();
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
    char data[50];
    int rc = this->subscrib("qt",15,data);
    qDebug() << "rc =  "<< rc;
    if(rc < 0){
        qDebug() << " no mqtt rece data";
        this->publish("test","loop");
    }else{
        qDebug() << "data = " << data;
    }

    m_timer->start(500);

}

