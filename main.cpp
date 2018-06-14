#include <QCoreApplication>
#include "QSocketMqtt.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QSocketMqtt * mqtt;
    mqtt = QSocketMqtt::GetInstance();
    mqtt->setHostName("192.168.9.75",31688);
    mqtt->test();

    return a.exec();
}

