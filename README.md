#   QEmbeddedMqttTest
####  在Qt中使用嵌入式Mqtt
#### 联系邮箱:yexiaopeng1992@126.com
#### 开发环境：Ubuntu16 64bit + Qt5.5
#### 实现功能：启动后，先Mqtt服务器发送“Start'字符串。然后订阅服务器消息。规定时间内未接收到服务器消息，上传"loop"，若接收到消息则打印消息，此后再次循环等待服务器消息。
---
#### 本身Qt提供了QMTT的模块，但为集成到QTsdk中，需要用户自己下载源码编译。但官方的Mqtt对Qt的版本有一定要求，经测试Qt5.5无法正常编译，而Qt5.8开始可以编译，实际情况大家科技自己测试。附上说明博客  https://www.cnblogs.com/yexiaopeng/p/8542894.html
---
#### 本次使用eclipse提供的嵌入式版本的Mqtt源码库配合Qt的TcpSocket进行演示。下载地址：https://github.com/eclipse/paho.mqtt.embedded-c
---
#### 使用方法为将paho.mqtt.embedded-c下的MQTTPacket/src的代码加入到Qt工程中，然后基于Qt的socket编写 open/close/sendData/getData函数，并在自定义的publish和subscrib调用这四个函数，配合MQTTPacket.h中的函数实现Mqtt的订阅和推送功能。
---
#### 注意： 
- open/close/sendData函数基本没问题，需要注意的是getData需要使用read()函数，因为Mqtt的通讯需要比对头部数据，会有移位操作。
- 在subscrib函数中，调用transport_getdata函数前，需要进行数据接收完成判断
- MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH 为接收服务器发送的订阅的数据，因为要将超时时间设置长一些。



