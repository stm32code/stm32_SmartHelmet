package com.example.smarthelmet.utils


import com.itfitness.mqttlibrary.MQTTHelper

object Common {
    var Sever =
        "tcp://iot-06z00axdhgfk24n.mqtt.iothub.aliyuncs.com:1883" //mqtt连接ip oneNET为183.230.40.39

//                var Sever = "tcp://192.168.1.20:1883" //mqtt连接ip oneNET为183.230.40.39
    var PORT = "6002"//oneNET为6002
    var ReceiveTopic = "/broadcast/h9sjncSJ2s1/test2"
    var PushTopic = "/broadcast/h9sjncSJ2s1/test1"
    var DriveID =
        "h9sjncSJ2s1.smart_helmet_app|securemode=2,signmethod=hmacsha256,timestamp=1703087545725|"  //mqtt连接id  oneNET为设备id
    var DriveName = "smart_helmet_app&h9sjncSJ2s1"  //设置用户名。跟Client ID不同。用户名可以看做权限等级"  oneNET为产品ID
    var DrivePassword = "60834c6cff7f1560f3e468592a0f1c93d124d5d0b094321f6c3e11e3b5b933dd"// //设置登录密码  oneNET为设备鉴权或apikey
    var Drive2ID = "1147239937"
    var api_key = "HxWbqoHYZN8WVYsBrdSVe=wcjTY="
    var DeviceOnline = false //底层是否在线
    var LatestOnlineDate = "离线" //最近在线时间
    var mqttHelper: MQTTHelper? = null
}


