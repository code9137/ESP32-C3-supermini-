#pragma once 


#include<Arduino.h>
#include <Preferences.h>
#include <U8g2lib.h>

class passwordBook{
    public:
        //初始化
        passwordBook();

        //读取串口数据
        String readData();

        // //串口输出密码
        // bool outputPassword();

        //判定是否为密码
        bool isPassword(String word);

        //判断是否为密码源(name)
        bool isName(String word);

        //保存密码与名称
        bool save(String name,String password);

        //输出密码
        String out(String name);

        //模拟键盘输入
        bool keyborad(String word);

        //显示
        void show(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2,int mv,int con);
};