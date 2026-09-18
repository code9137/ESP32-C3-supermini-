/*
	密码本: 用于记录从串口中输入的密码和账号
		原理:
			使用Preferences (Arduino自带)
			以键值对的方式存储  账号:密码

		串口输入应按照以下格式进行输入:
			name用户名
			password密码

			例如:
				nameamy    			// 表明输入的用户名:amy,并指定下次输入为密码
				password123456      // 表示密码: 123456

		注意: 

*/
#include<Arduino.h>
#include <Preferences.h>
#include <BleKeyboard.h>
#include <U8g2lib.h>


#include "config.h"
#include "passwordBook.h"

Preferences preferences;//非易失性存储（NVS）方案
String input(""); //串口输入的字符串


//指针
static int key_index = 0;

//判断是否点击确定
static bool isConfirm = false;

		//初始化
		passwordBook::passwordBook(){
			preferences.begin("password", false);//开启空间命名password,false表示可修改
			
		}

		//读取串口数据
		String passwordBook::readData(){

			if(Serial.available()>0){		//
				Serial.setTimeout(5000);	//超时时间: 5000ms
				input = Serial.readStringUntil('\n'); //阻塞方法 等到 \n 或超时
			}
			Serial.print(input);
			return input;
		}

		// //根据输入的账号名,串口输出密码
		// bool passwordBook::outputPassword(){
		// 	password = preferences.getString(name.c_str());
		// 	Serial.print(password);
		// 	return true;
		// }

		//判定串口输入的是否为密码
		bool passwordBook::isPassword(String word){
			if(word.startsWith("password")){				
				return true;
			}
			return false;
		}

		//判断是否为账户名(name)
		bool passwordBook::isName(String word){
			if(word.startsWith("name")){
				return true;
			}
			return false;
		}

		//串口输入后,保存密码与名称
		bool passwordBook::save(String name,String password){
			//判断
			
			if(!this->isName(name) || !this->isPassword(password)){
				return false;
			}

			//保存
			preferences.putString(name.c_str(),password.c_str());
			Serial.println("保存："+name+"密码："+password);
			
			return true;
			
		}

		//串口输出指定name的密码
		String passwordBook::out(String name){
			String passwd = "";
			passwd = preferences.getString(name.c_str());
			Serial.println(passwd);
			return passwd;
		}

		//模拟键盘输入
		bool passwordBook::keyborad(String word){
			return 1;
		}

		void passwordBook::show(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &u8g2,int mv,int con){
			u8g2.drawUTF8(24,13,"=== 密码本 ===");
			u8g2.drawUTF8(0,30,"输出密码");
			u8g2.drawUTF8(0,45,"保存密码");
			u8g2.drawUTF8(0,60,"删除账号");

			//设置指针
			if(mv == up && key_index >= 1){
				key_index --;
			}else if(mv == down && key_index < 2){
				key_index ++;
			}

			//显示指针
			u8g2.drawUTF8(0,30 + 15*(key_index),"|||||||||||||||||");


			// 确定/取消
			if(con == confirm){
				isConfirm = true;
			}

			if(isConfirm){
				switch (key_index)
				{
				case 0:{ //输入用户名,输出密码
					u8g2.clearBuffer();
					u8g2.drawUTF8(0,30,"串口读取中...");
					u8g2.sendBuffer();
					String input_data = this->readData();
					
					//输出
					String draw_word = "读取成功: "+input_data;
					u8g2.drawUTF8(0,30,draw_word.c_str());

					//串口输出
					this->out(input_data);

					break;
				}


				case 1:{//保存密码
					u8g2.clearBuffer();
					u8g2.drawUTF8(0,30,"串口读取用户名...");
					u8g2.sendBuffer();
					String input_name = this->readData();
					String input_password = this->readData();

					if( this->save(input_name,input_password))
					{
						u8g2.drawUTF8(0,45,"保存成功");
						u8g2.sendBuffer();
					}else {
						u8g2.drawUTF8(0,45,"读取失败");
						u8g2.sendBuffer();
					}
					break;
				}
						
				
				case 2:
					break;
				default:
					break;
				}
			}

		}







/***
 	string与Java的String非常相似
 			
	substring()：         截取字符串
	endWith(string):	  结尾是否为指定的字符串
	startsWith(string)    开头是否为指定的字符串
	concat(string/long)	  追加长度


	Serial
		available()		        输出串口读取的长度
		readStringUntil(string) 读取串口数据，直到读取到指定值



 */