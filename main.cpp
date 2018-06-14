#include <QCoreApplication>
#include "QSocketMqtt.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QSocketMqtt * mqtt;
    mqtt = QSocketMqtt::GetInstance();
    mqtt->setHostName("47.100.100.235",1884);
    mqtt->test();

    return a.exec();
}

