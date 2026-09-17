#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "config.h"
#include "bitmap.h"
#include "wifi_scanner.h"
#include "menuView.h"
#include "passwordBook.h"



#define menu 0
#define wifi 1
#define passwd_book 2

/*

硬件连接方法:
5号引脚 -->  OLED的SDA
6号引脚 -->  OLED的SCL


0号引脚 --> 确认按钮
1号引脚 --> 右按钮
2号引脚 --> 左按钮
3号引脚 --> 下按钮
4号引脚 --> 上按钮
21号引脚--> 取消按钮

显示页面
		0: 表示不切换任何页面
        1: 切换至WIFI连接页面
        2: 切换至密码本页面(未完成)
        3: 切换至端口调用页面(未完成)
        4: 设置界面(未完成)
*/

static unsigned int modes = menu;		//当前显示的页面
//hw_timer_t * time_0 = timerBegin(0,80,true);	//硬件定时器(未使用)
//HTTPClient http; //http请求(未使用)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
passwordBook pw;


String http_return = "未连接";  	 //http请求返回的参数(未使用)
int connent_wifi_size = 0;			// 扫描的WIFI数量


int interrupt_time = 0;					// 当前中断时间
unsigned long last_interrupt_time = 0;  // 记录上次有效按键的时间

int submit_interrupt_time = 0;					// 确认/取消按钮的当前中断时间
unsigned long submit_last_interrupt_time = 0;	// 确认/取消按钮的上次有效按键的时间

//中断的按键号
int move_key = none;
int the_q_key = none;


//消抖时间(毫秒)
int delay_time = 150;


//等待连接热点方法(未使用)
void connection_WiFi(int time){
	int connection_time = 20;		//连接时间(20秒)
	while(WiFi.status() != WL_CONNECTED && connection_time >= 0){
		connection_time -= 1;
		Serial.print(".");
		delay(time);
		
	};
}



// ====================== 中断方法 =====================
void IRAM_ATTR fangxiang(void* arg) {
    int key_int = (int)arg;

	//添加最后中断时间
	interrupt_time = millis();
	// 中断处理内容
	switch (key_int)
	{
	case right:
		move_key = right;
		break;
	case left:
		move_key =left;
		break;
	case up:
		move_key = up;
		break;
	case down:
		move_key = down;		
		break;
	default:
		move_key = none;		
		break;
	}
}
void IRAM_ATTR queren(void* arg){
	int key_int = (int)arg;
	submit_interrupt_time = millis();
	switch (key_int)
	{
	case confirm:
		the_q_key = confirm;
		break;
	case cancel:
		the_q_key = cancel;
		break;
	default:
		the_q_key = none;
		break;
	}
}

void setup() {
	//串口打印
	Serial.begin(115200);
	//U8G2
	Wire.begin(PIN_SDA, PIN_SCL); //指定2个引脚为SDA数据,SCL时钟
	Wire.setClock(400000L);		//设置时钟频率400000L

	u8g2.begin();					//开始
	u8g2.enableUTF8Print();         // 启用 UTF-8 编码支持
	
	//添加点阵图
	u8g2.drawXBMP(0,5,70,64,bitmap_logo);
	
	//设置
	u8g2.setFont(u8g2_font_wqy12_t_gb2312);//设置字体
	u8g2.setCursor(72,5);//设置光标
	u8g2.drawUTF8(72,32,"加载中...");
	
	u8g2.sendBuffer();      //显示
	
	


	// 引脚定义
	pinMode(0,INPUT_PULLUP);
	pinMode(1,INPUT_PULLUP);
	pinMode(2,INPUT_PULLUP);
	pinMode(3,INPUT_PULLUP);
	pinMode(4,INPUT_PULLUP);
	pinMode(21,INPUT_PULLUP);
	
	//添加中断函数
	attachInterruptArg(digitalPinToInterrupt(up),fangxiang,(void*)up,FALLING);
	attachInterruptArg(digitalPinToInterrupt(down),fangxiang,(void*)down,FALLING);
	attachInterruptArg(digitalPinToInterrupt(left),fangxiang,(void*)left,FALLING);
	attachInterruptArg(digitalPinToInterrupt(right),fangxiang,(void*)right,FALLING);

	attachInterruptArg(digitalPinToInterrupt(confirm),queren,(void*)confirm,FALLING);
	attachInterruptArg(digitalPinToInterrupt(cancel),queren,(void*)cancel,FALLING);


	//设置WiFi类的参数
	WiFi.mode(WIFI_STA);//WiFi连接模式
	WiFi.setTxPower(WIFI_POWER_8_5dBm);//设置频率
	connent_wifi_size = WiFi.scanNetworks();//获取周围WIFI的数量
	
	
}

void loop() {
	//清除缓存
	u8g2.clearBuffer();
	

	//	Serial.print(".");
	
	 // ---- 备份按键，然后立即清空全局变量 ----
    int mv = none;      // 复制当前值
    int cf = none; 
	
	//移动键 消抖
    if(interrupt_time - last_interrupt_time > delay_time){
		last_interrupt_time = interrupt_time;
		mv = move_key;      // 复制当前值
	}else{
		move_key = none;
	}

	//确认键 消抖
	if(submit_interrupt_time - submit_last_interrupt_time > delay_time){
		submit_last_interrupt_time = submit_interrupt_time;
		cf = the_q_key;
	}else{
		the_q_key = none;
	}

	//显示的页面
	switch(modes){
		case 0:
			modes = show(u8g2,mv,cf);
			break;
		case 1:
			modes = wifi_connect(u8g2,connent_wifi_size,mv,cf);
			break;
		case 2:
			pw.show(u8g2,mv,cf);
			break;

	}




	//发送缓存
	u8g2.sendBuffer();
	
}
