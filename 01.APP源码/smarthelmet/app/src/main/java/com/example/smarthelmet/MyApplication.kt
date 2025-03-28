package com.example.smarthelmet

import android.app.Application
import com.baidu.location.LocationClient
import com.baidu.mapapi.CoordType
import com.baidu.mapapi.SDKInitializer

class MyApplication : Application() {
    override fun onCreate(){
        super.onCreate()
        LocationClient.setAgreePrivacy(true)
        SDKInitializer.setAgreePrivacy(applicationContext,true)
        //在使用SDK各组件之前初始化context信息，传入ApplicationContext
        SDKInitializer.initialize(this)
        //包括BD09LL和GCJ02两种坐标，默认是BD09LL坐标。
        SDKInitializer.setCoordType(CoordType.BD09LL)
    }
}