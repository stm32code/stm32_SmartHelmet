package com.example.smarthelmet

import android.Manifest
import android.content.SharedPreferences
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.ArrayAdapter
import com.baidu.location.LocationClient
import com.baidu.location.LocationClientOption
import com.baidu.mapapi.SDKInitializer
import com.baidu.mapapi.common.BaiduMapSDKException
import com.baidu.mapapi.map.BaiduMap
import com.baidu.mapapi.map.MapStatus
import com.baidu.mapapi.map.MapStatusUpdateFactory
import com.baidu.mapapi.map.MyLocationData
import com.baidu.mapapi.model.LatLng
import com.baidu.mapapi.utils.SpatialRelationUtil
import com.blankj.utilcode.util.LogUtils
import com.example.smarthelmet.bean.EquipmentBean
import com.example.smarthelmet.bean.Receive
import com.example.smarthelmet.bean.Send
import com.example.smarthelmet.bean.SendData
import com.example.smarthelmet.databinding.ActivityMainBinding
import com.example.smarthelmet.utils.BeatingAnimation
import com.example.smarthelmet.utils.Common
import com.example.smarthelmet.utils.Common.PushTopic
import com.example.smarthelmet.utils.CoordinateTransformUtils
import com.example.smarthelmet.utils.MToast
import com.example.smarthelmet.utils.TimeCycle
import com.google.gson.Gson
import com.gyf.immersionbar.ImmersionBar
import com.itfitness.mqttlibrary.MQTTHelper
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken
import org.eclipse.paho.client.mqttv3.MqttCallback
import org.eclipse.paho.client.mqttv3.MqttMessage
import pub.devrel.easypermissions.EasyPermissions
import retrofit2.Call
import retrofit2.Retrofit

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private var isDebugView = false //是否显示debug界面
    private var arrayList = ArrayList<String>()// debug消息列表
    private var adapter: ArrayAdapter<String>? = null// debug适配器
    private var onlineFlag = false //是否在线标识
    private lateinit var myMap: BaiduMap
    private lateinit var retrofit: Retrofit
    private var isGetHttp = false //是否成功获取了坐标
    private lateinit var dataEquipmentCall: Call<EquipmentBean>
    private var isFirst = true //第一次定位
    private lateinit var sharedPreferences: SharedPreferences
    private lateinit var editor: SharedPreferences.Editor
    private var nowLocation: LatLng = LatLng(39.923411, 116.403236)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        sharedPreferences = getSharedPreferences("Location", MODE_PRIVATE)
        editor = sharedPreferences.edit()
        getPermission()
        mqttServer()
        initView()
        isOnline()
    }

    /**
     * 界面的初始化
     */
    private fun initView() {
        setSupportActionBar(binding.toolbar)
        ImmersionBar.with(this).init()
        debugView()
        initMap() //初始化地图
        warringLayout(false, "")
        eventManager()
    }

    /***
     * 初始化地图
     */
    private fun initMap() {
        myMap = binding.baiduMap.map
        myMap.isMyLocationEnabled = true // 开启定位图层

    }

    private fun eventManager() {
        binding.warringLayout.setOnClickListener {
            binding.warringLayout.visibility = View.GONE
        }
    }

    /**
     * @brief debug界面的初始化
     */
    private fun debugView() {
        adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, arrayList)
        binding.debugView.adapter = adapter
    }

    /**
     * mqtt服务
     */
    private fun mqttServer() {
        Common.mqttHelper = MQTTHelper(
            this, Common.Sever, Common.DriveID, Common.DriveName, Common.DrivePassword, true, 30, 60
        )
        Common.mqttHelper!!.connect(Common.ReceiveTopic, 1, true, object : MqttCallback {
            override fun connectionLost(cause: Throwable?) {
                MToast.mToast(this@MainActivity, "连接已经断开")
            }

            override fun messageArrived(topic: String?, message: MqttMessage?) {
                //收到消息
                val data = Gson().fromJson(message.toString(), Receive::class.java)
                LogUtils.eTag("接收到消息", message?.payload?.let { String(it) })
                onlineFlag = true
                binding.online.text = "在线"
                debugViewData(2, message?.payload?.let { String(it) }!!)
                println(data)
                if (data != null) {
                    analysisOfData(data)
                } else {
                    MToast.mToast(this@MainActivity, "数据异常")
                }
            }

            override fun deliveryComplete(token: IMqttDeliveryToken?) {

            }
        })

    }
    /**
     * @brief 解析接受到的数据
     */
    private fun dataAnalysis(data: Receive?) {
        if (data != null) {
            try {
                val temp = CoordinateTransformUtils.wgs84ToBd09(
                    data.lat!!.toDouble(),
                    data.lon!!.toDouble()
                )
                setMap(temp.latitude, temp.longitude)
            } catch (e: Exception) {
                Log.e("ReceiveDataError", e.toString())
                MToast.mToast(this, "接收数据异常")
            }
        }
    }
    /***
     * 数据处理
     */
    private fun analysisOfData(data: Receive) {
        try {
            var isWarning = false
            if (data.lon != null && data.lat != null) {
                dataAnalysis(data)
                binding.longitudeText.text = data.lon
                binding.latitudeText.text = data.lat
//                nowLocation = LatLng(data.lat!!.toDouble(), data.lon!!.toDouble())
//                setMap(nowLocation.latitude, nowLocation.longitude)
            }
            if (data.somg != null) {
                binding.somgText.text = String.format("%d%%", data.somg!!.toInt() / 10)
            }
            if (data.voice != null) {
                binding.voiceText.text = if (data.voice == "1") "是" else "否"
                //binding.voiceText.text = String.format("%d%%", data.voice!!.toInt() / 4)
            }
            if (data.light != null) {
                binding.lightText.text = String.format("%d%%", data.light!!.toInt() / 4)
            }

            if (data.put_cap != null) {
                binding.putCapText.text = if (data.put_cap == "1") "佩戴" else "未佩戴"
            }
            if (data.heard != null) {
                if (data.heard == "1") {
                    isWarning = true
                    binding.heardText.text = "撞击"
                    warringLayout(true, "头部受到撞击")
                } else {
                    binding.heardText.text = "正常"
                    warringLayout(false, "头部受到撞击")
                }
            }
            if (data.warning != null && !isWarning) {
                if (data.warning == "1") {
                    warringLayout(true, "手动报警")
                } else {
                    warringLayout(false, "手动报警")
                }
            }

        } catch (e: Exception) {
            e.printStackTrace()
            MToast.mToast(this, "数据解析失败:$e")
        }
    }

    /***
     * 设置地图根据经纬度定位
     *  lati：经度
     *  longi：维度
     */
    private fun setMap(
        lati: Double,
        longi: Double,
    ) {
        myMap.setMyLocationData(
            MyLocationData.Builder()
                .direction(-1F).latitude(lati)
                .longitude(longi).build()
        )
        Log.e("定位经纬度", "纬度:${lati},经度:${longi}")
        // 第一次定位视角会直接跳转到定位的位
        val builder = MapStatus.Builder()
        builder.target(LatLng(lati, longi))
        myMap.animateMapStatus(MapStatusUpdateFactory.newMapStatus(builder.build()))


    }

    /**
     * @brief 再次封装MQTT发送
     * @param message 需要发送的消息
     */
    private fun sendMessage(cmd: Int, message: String) {
//        if (Common.mqttHelper != null && Common.mqttHelper!!.subscription) {
//            var str = ""
//            when (cmd) {
//
//            }
//            Thread {
//                Common.mqttHelper!!.publish(
//                    PushTopic, str, 1
//                )
//            }.start()
//            debugViewData(1, str)
//        }
    }

    /***
     * 判断硬件是否在线
     */
    private fun isOnline() {
        Thread {
            var i = 0
            while (true) {//开启轮询，每18s重置标识
                if (i > 3) {
                    i = 0
                    runOnUiThread {
                        binding.online.text = if (onlineFlag) "在线" else "离线"
                    }
                    onlineFlag = false
                }
                i++
                Thread.sleep(6000)
//                Thread.sleep(2500)
            }
        }.start()
    }

    /**
     * @brief 动态获取权限
     */
    private fun getPermission() {
        val perms = arrayOf(
            Manifest.permission.ACCESS_WIFI_STATE,
            Manifest.permission.CHANGE_WIFI_STATE,
            Manifest.permission.WRITE_SETTINGS,
            Manifest.permission.INTERNET,
            Manifest.permission.ACCESS_NETWORK_STATE,
            Manifest.permission.READ_PHONE_STATE,
            Manifest.permission.WAKE_LOCK,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.SCHEDULE_EXACT_ALARM,
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_LOCATION_EXTRA_COMMANDS
        )
        if (!EasyPermissions.hasPermissions(this, *perms)) EasyPermissions.requestPermissions(
            this, "这是必要的权限", 100, *perms
        ) else {
            // 是否同意隐私政策，默认为false
            SDKInitializer.setAgreePrivacy(this, true)
            try {
                // 在使用 SDK 各组间之前初始化 context 信息，传入 ApplicationContext
                SDKInitializer.initialize(this)
            } catch (_: BaiduMapSDKException) {
            }
        }

    }

    /**
     * @brief debug界面数据添加
     * @param str 如果为 1 添加发送数据到界面   为 2 添加接受消息到界面
     */
    private fun debugViewData(str: Int, data: String) {
        if (arrayList.size >= 255) arrayList.clear()
        runOnUiThread {
            when (str) {
                1 -> { //发送的消息
                    arrayList.add("目标主题:${Common.ReceiveTopic} \n时间:${TimeCycle.getDateTime()}\n发送消息:$data ")
                }

                2 -> {
                    arrayList.add("来自主题:${Common.ReceiveTopic} \n时间:${TimeCycle.getDateTime()}\n接到消息:$data ")
                }
            }
            // 在添加新数据之后调用以下方法，滚动到列表底部
            binding.debugView.post {
                binding.debugView.setSelection(adapter?.count?.minus(1)!!)
            }
            adapter?.notifyDataSetChanged()
        }

    }

    /**
     * @brief 显示警告弹窗和设置弹窗内容
     * @param visibility 是否显示
     * @param str 显示内容
     */
    private fun warringLayout(visibility: Boolean, str: String) {
        if (visibility) {
            binding.warringLayout.visibility = View.VISIBLE
            binding.warringText.text = str
            BeatingAnimation().onAnimation(binding.warringImage)
        } else {
            binding.warringLayout.visibility = View.GONE
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        //在activity执行onDestroy时执行binding.map.onDestroy()，销毁地图
        binding.baiduMap.onDestroy()
    }

    override fun onResume() {
        super.onResume()
        //在activity执行onResume时执行binding.map.onResume ()，重新绘制加载地图
        binding.baiduMap.onResume()
    }

    override fun onPause() {
        super.onPause()
        //在activity执行onPause时执行binding.map.onPause ()，暂停地图的绘制
        binding.baiduMap.onPause()
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_scrolling, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.setDebugView -> { // set debug view is enabled or disabled
                isDebugView = !isDebugView
                binding.debugView.visibility = if (isDebugView) View.VISIBLE else View.GONE
                true
            }

            else -> super.onOptionsItemSelected(item)
        }
    }
}