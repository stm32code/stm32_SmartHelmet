package com.example.smarthelmet.utils

import java.util.Locale

object PduConvert {
    fun toPdt(phone: String, data: String): PduData? {
        val headers = "0011000D9168"
        val pduPhone = twoBitInversion(phone) ?: return null
        val unicodeData = stringToUnicode(data)
        val dataLength = (unicodeData.length / 2).toString(16).padStart(2, '0')
        val message =
            (headers + pduPhone + "0008FF" + dataLength + unicodeData).uppercase(Locale.getDefault())
        return PduData(data.length * 2 + 15, message)
    }

    private fun twoBitInversion(data: String): String? {
        var phone = data
        return if (isNumeric(phone)) {
            if (phone.length % 2 == 1) phone += "F"
            phone.chunked(2) // 每两位一组进行分割
                .joinToString("") { it.reversed() } // 每组内的字符翻转后拼接到一起
        } else
            null
    }

    private fun isNumeric(input: String): Boolean {
        val regex = Regex("""^\d+$""")
        return regex.matches(input)
    }

    fun stringToUnicode(input: String): String {
        val stringBuilder = StringBuilder()
        for (char in input) {
            stringBuilder.append("\\u").append(char.toInt().toString(16).padStart(4, '0'))
        }
        return stringBuilder.toString().replace("\\u", "")
    }

    fun unicodeToString(input: String): String {
        val stringBuilder = StringBuilder()
        input.chunked(4).forEach {
            val charCode = Integer.parseInt(it, 16)
            stringBuilder.append(charCode.toChar())
        }
        return stringBuilder.toString()
    }
}