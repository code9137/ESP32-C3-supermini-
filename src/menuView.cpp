/*
用于显示菜单
    返回值:
        0: 表示不切换任何页面
        1: 切换至WIFI连接页面
        2: 切换至密码本页面(未完成)
        3: 切换至端口调用页面(未完成)
        4: 设置界面(未完成)

    形参:
        (u8g2实例,方向键,确认键)    
*/
#include <Arduino.h>
#include <U8g2lib.h>

#include "config.h"

static int key_index = 0;

int show(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2,int move,int con){
    u8g2.drawUTF8(13,13," === 选择功能 === ");
    u8g2.drawUTF8(0,30,"WIFI扫描器");
    u8g2.drawUTF8(0,45,"密码本");
    u8g2.drawUTF8(0,60,"功能3");

    Serial.print(".");


    //判断按键
    if(move == up && key_index >= 1)key_index --;
    if(move == down && key_index <= 4)key_index ++;

    //显示指针
    int station = 15 * (key_index+2);
    u8g2.drawUTF8(111,station,"<==");

    //点击确认
    if(con == confirm){
        return key_index + 1;
    }

    //点击取消,重启单片机
    if(con == cancel){
        ESP.restart();
    }

    return 0;
}