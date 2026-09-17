#include <WiFi.h>
#include <WString.h>
#include <U8g2lib.h>
#include <Arduino.h>

#pragma once
int wifi_connect(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2,int connent_wifi_size,int move,int con);