#include <WiFi.h>
#include <WString.h>
#include <U8g2lib.h>
#include <Arduino.h>

#include "config.h"
#include "wifi_scanner.h"

static int key = 1; 				        //指针(用于指向需要的WIFI名)
static int page = 0;					    //当前页数

static boolean isConfirm = false;//是否确认
static int wifi_index = 0;//选择的wifi下标
/**
 * 注意:  u8g2,page,key必须要 引用&
 *      因为page,key的值在方法体中有修改,必须映射到外部的page,key变量
 *      u8g2是一个大型对象,不使用&,就要将对象拷贝一份给方法体,这样会浪费很多内存,所以应该使用 引用&
 * 
 */
int wifi_connect(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2,int connent_wifi_size,int move,int con){

	if(!isConfirm){
		//遍历WIFI,并显示在屏幕
		for(int i = 1+(page*4);i<connent_wifi_size;i++){
			String text = String(i) + ": " + WiFi.SSID(i-1);
			u8g2.drawUTF8(0,(i-page*4)*15,text.c_str());

			if(i-(page*4) == key)u8g2.drawUTF8(111,(key)*15,"<==");   //设置指针
		}

		//移动判定
		if (move == up && !( key == 1 && page == 0)) {
			key--;
		} else if (move == down && !(page*4+key == connent_wifi_size-1)) {
			key++;
		}

		
		if(con == confirm){
			isConfirm = !isConfirm;
				wifi_index = key-1 + page*4;
		}else if(con == cancel){
			return 0;//返回到主菜单
		}
		
		//判断指针是否超过当前页面所显示的最大数量
		if(key > 4){
			page ++;
			key = 1;
		}
		if(key < 1){
			page --;
			key = 1;
		}


	}else{
		u8g2.clearBuffer();

			//是否确认
			u8g2.drawUTF8(0,30,"是否对该网络进行连接?");
			u8g2.drawUTF8(0,15,WiFi.SSID(wifi_index).c_str());
			u8g2.drawUTF8(0,45,"(是/否)");
			
			//确认
			if(con == confirm){
				u8g2.clearBuffer();
				u8g2.drawUTF8(0,30,("正在连接网络："+WiFi.SSID(wifi_index)).c_str());
				//连接WIFI
				WiFi.begin(WiFi.SSID(wifi_index),"52134568");
				u8g2.sendBuffer();
				delay(3000);

				//连接状态
				if(WiFi.status() == WL_CONNECTED){
					u8g2.clearBuffer();
					u8g2.drawUTF8(0,15,"WIFI连接成功");
					u8g2.drawUTF8(0,30,(WiFi.SSID(wifi_index)).c_str());
					u8g2.sendBuffer();
					delay(3000);
				}else{
					u8g2.clearBuffer();
					u8g2.drawUTF8(0,15,"WIFI连接失败");
					u8g2.drawUTF8(0,30,(WiFi.SSID(wifi_index)).c_str());
					u8g2.sendBuffer();
					delay(3000);
				}
			}else if(con == cancel){
				isConfirm = !isConfirm;
			}
	}
	return 1;//保持当前页面
}