QT += core
QT -= gui
QT += network  #加入网络模块


TARGET = QEmbeddedMqttTest 
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    QSocketMqtt.cpp \
    MQTTPack/MQTTConnectClient.c \
    MQTTPack/MQTTConnectServer.c \
    MQTTPack/MQTTDeserializePublish.c \
    MQTTPack/MQTTFormat.c \
    MQTTPack/MQTTPacket.c \
    MQTTPack/MQTTSerializePublish.c \
    MQTTPack/MQTTSubscribeClient.c \
    MQTTPack/MQTTSubscribeServer.c \
    MQTTPack/MQTTUnsubscribeClient.c \
    MQTTPack/MQTTUnsubscribeServer.c

HEADERS += \
    QSocketMqtt.h \
    MQTTPack/MQTTConnect.h \
    MQTTPack/MQTTFormat.h \
    MQTTPack/MQTTPacket.h \
    MQTTPack/MQTTPublish.h \
    MQTTPack/MQTTSubscribe.h \
    MQTTPack/MQTTUnsubscribe.h \
    MQTTPack/StackTrace.h

DISTFILES += \
    README.md

