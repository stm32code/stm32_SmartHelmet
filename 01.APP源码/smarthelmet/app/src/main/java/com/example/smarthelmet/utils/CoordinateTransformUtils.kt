package com.example.smarthelmet.utils

import com.baidu.mapapi.model.LatLng

/**
 * Description: 各坐标系之间的转换工具类
 *
 * @author JourWon
 * @date Created on 2018年6月19日
 */
object CoordinateTransformUtils {
    // 圆周率π
    private const val PI = 3.1415926535897932384626

    // 火星坐标系与百度坐标系转换的中间量
    private const val X_PI = 3.14159265358979324 * 3000.0 / 180.0

    // Krasovsky 1940
    // 长半轴a = 6378245.0, 1/f = 298.3
    // b = a * (1 - f)
    // 扁率ee = (a^2 - b^2) / a^2;
    // 长半轴
    private const val SEMI_MAJOR = 6378245.0

    // 扁率
    private const val FLATTENING = 0.00669342162296594323

    // WGS84=>GCJ02 地球坐标系=>火星坐标系
    fun wgs84ToGcj02(lng: Double, lat: Double): LatLng {

        val offset = offset(lng, lat)
        val mglng = lng + offset[0]
        val mglat = lat + offset[1]
        return LatLng(mglat, mglng)
    }

    // GCJ-02=>BD09 火星坐标系=>百度坐标系
    fun gcj02ToBd09(lng: Double, lat: Double): LatLng {
        val z = Math.sqrt(lng * lng + lat * lat) + 0.00002 * Math.sin(lat * X_PI)
        val theta = Math.atan2(lat, lng) + 0.000003 * Math.cos(lng * X_PI)
        val bd_lng = z * Math.cos(theta) + 0.0065
        val bd_lat = z * Math.sin(theta) + 0.006
        return LatLng(bd_lng, bd_lat)
    }

    // WGS84=>BD09 地球坐标系=>百度坐标系
    fun wgs84ToBd09(lng: Double, lat: Double): LatLng {
        val point: LatLng = wgs84ToGcj02(lat ,lng )
        return gcj02ToBd09(point.latitude, point.longitude)
    }


    // 经度偏移量
    private fun transformLng(lng: Double, lat: Double): Double {
        var ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + 0.1 * lng * lat + 0.1 * Math.sqrt(
            Math.abs(lng)
        )
        ret += (20.0 * Math.sin(6.0 * lng * PI) + 20.0 * Math.sin(2.0 * lng * PI)) * 2.0 / 3.0
        ret += (20.0 * Math.sin(lng * PI) + 40.0 * Math.sin(lng / 3.0 * PI)) * 2.0 / 3.0
        ret += (150.0 * Math.sin(lng / 12.0 * PI) + 300.0 * Math.sin(lng / 30.0 * PI)) * 2.0 / 3.0
        return ret
    }

    // 纬度偏移量
    private fun transformLat(lng: Double, lat: Double): Double {
        var ret =
            -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + 0.1 * lng * lat + 0.2 * Math.sqrt(
                Math.abs(lng)
            )
        ret += (20.0 * Math.sin(6.0 * lng * PI) + 20.0 * Math.sin(2.0 * lng * PI)) * 2.0 / 3.0
        ret += (20.0 * Math.sin(lat * PI) + 40.0 * Math.sin(lat / 3.0 * PI)) * 2.0 / 3.0
        ret += (160.0 * Math.sin(lat / 12.0 * PI) + 320 * Math.sin(lat * PI / 30.0)) * 2.0 / 3.0
        return ret
    }

    // 偏移量
    fun offset(lng: Double, lat: Double): DoubleArray {
        val lngLat = DoubleArray(2)
        var dlng = transformLng(lng - 105.0, lat - 35.0)
        var dlat = transformLat(lng - 105.0, lat - 35.0)
        val radlat = lat / 180.0 * PI
        var magic = Math.sin(radlat)
        magic = 1 - FLATTENING * magic * magic
        val sqrtmagic = Math.sqrt(magic)
        dlng = dlng * 180.0 / (SEMI_MAJOR / sqrtmagic * Math.cos(radlat) * PI)
        dlat = dlat * 180.0 / (SEMI_MAJOR * (1 - FLATTENING) / (magic * sqrtmagic) * PI)
        lngLat[0] = dlng
        lngLat[1] = dlat
        return lngLat
    }
}
