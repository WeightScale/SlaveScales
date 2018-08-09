// scales.h

#ifndef _CORE_h
#define _CORE_h

#include "TaskController.h"
#include "Task.h"

/*
#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif*/
#include <ArduinoJson.h>
#include "Scale.h"
//using namespace ArduinoJson;

#define SETTINGS_FILE "/settings.json"

#define TIMEOUT_HTTP 3000
#define STABLE_NUM_MAX 10
//#define MAX_CHG 1013//980	//делитель U2=U*(R2/(R1+R2)) 0.25
//#define MIN_CHG 880			//ADC = (Vin * 1024)/Vref  Vref = 1V
#define MIN_CHG 670			//ADC = (Vin * 1024)/Vref  Vref = 1V

#define EN_NCP  12							/* сигнал включения питания  */
#define PWR_SW  13							/* сигнал от кнопки питания */
#define LED  2								/* индикатор работы */

#define SCALE_JSON		"scale"



extern TaskController taskController;		/*  */
extern Task taskBlink;								/*  */
extern Task taskBattery;							/*  */
//extern Task taskPower;
extern void connectWifi();

const char netIndex[] PROGMEM = R"(<html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/><body><form method='POST'><input name='ssid'><br/><input type='password' name='key'><br/><input type='submit' value='СОХРАНИТЬ'></form></body></html>)";

const char settings_html[] PROGMEM = R"(<!DOCTYPE html><html lang="en"><head> <meta charset="UTF-8"> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"/> <meta http-equiv="Pragma" content="no-cache"/> <title>Настройки</title> <link rel="stylesheet" type="text/css" href="global.css"> <style>input:focus{background: #FA6;outline: none;}table{width:100%;}input{width:100%;text-align:right;font-size:18px;}input[type=submit]{width:auto;}</style> <script>function GetValue(){var req=new XMLHttpRequest(); req.overrideMimeType('application/json'); req.onreadystatechange=function(){if (this.readyState===4 ){if(this.status===200){var json=JSON.parse(this.responseText); for (entry in json){try{document.getElementById(entry).innerHTML=json[entry];}catch (e){}}}}}; req.open("GET", "/sv", true); req.send(null);}function GetSettings(){var req=new XMLHttpRequest(); req.overrideMimeType('application/json'); req.onreadystatechange=function(){if (this.readyState===4){if(this.status===200){try{var json=JSON.parse(req.responseText); var scale=json.scale; for (entry in scale){try{document.getElementById(entry).value=scale[entry];}catch (e){}}var server=json.server; for (entry in server){try{document.getElementById(entry).value=server[entry];}catch (e){}}}catch(e){alert(e.toString());}finally{document.body.style.visibility='visible'; GetValue();}}}}; req.open("GET", "/settings.json", true); req.send(null);}window.onload=function(){GetSettings();}; </script></head><body style="visibility: hidden"><strong>Настройки</strong><hr><br/><fieldset style="width: auto"><details><summary>Общии настройки</summary> <br><form method='POST'> <h5>Доступ к настройкам</h5> <table> <tr> <td>ИМЯ:</td><td><input id="id_name_admin" name="name_admin" placeholder='имя админ'></td></tr><tr> <td>ПАРОЛЬ:</td><td><input type="password" id="id_pass_admin" name="pass_admin" placeholder='пароль админ'></td></tr><tr> <td></td><td><input type='submit' value='СОХРАНИТЬ'/></td></tr></table> </form></details></fieldset><br/><fieldset> <details><summary>Информация</summary> <br><table> <tr> <td><h5>Имя хоста:</h5></td><td align="right"><h5 id="id_local_host"></h5></td></tr></table> <hr> <h5 align='left'><b>Точка доступа весов</b></h5> <table> <tr> <td id="id_ap_ssid"></td><td align="right" id="id_ap_ip"></td></tr></table> <hr> <table> <tr> <td>Пломба:</td><td align="right"><div id="sl_id"></div></td></tr></table> <hr> <a href='/calibr.html'>калибровка</a> </details></fieldset><hr><footer align="center">2018 © Powered by www.scale.in.ua</footer></body></html>)";

typedef struct {
	//bool power_time_enable;
	String scaleName;
	String scalePass;
	//String scaleLanIp;
	//String scaleGateway;
	//String scaleSubnet;
	//String scaleWlanSSID;
	//String scaleWlanKey;
	int timeout;
	//int time_off;
	int bat_max;	
} settings_t;

class CoreClass : public AsyncWebHandler{
	private:
	settings_t _settings;
	
	String _username;
	String _password;
	bool _authenticated;
	
	bool saveAuth();
	bool loadAuth();		
	bool _downloadSettings();
			

	public:			
		CoreClass(const String& username, const String& password);
		~CoreClass();
		void begin();
		bool saveSettings();
		String& getNameAdmin(){return _settings.scaleName;};
		String& getPassAdmin(){return _settings.scalePass;};
		//String& getSSID(){return _settings.scaleWlanSSID;};
		//String& getLanIp(){return _settings.scaleLanIp;};
		//String& getGateway(){return _settings.scaleGateway;};
		//void setSSID(const String& ssid){_settings.scaleWlanSSID = ssid;};
		//void setPASS(const String& pass){_settings.scaleWlanKey = pass;};	
		//String& getPASS(){return _settings.scaleWlanKey;};
		/*#if! HTML_PROGMEM
			void saveValueSettingsHttp(AsyncWebServerRequest*);
		#endif*/
		//void handleSetAccessPoint(AsyncWebServerRequest*);
		

		virtual bool canHandle(AsyncWebServerRequest *request) override final;
		virtual void handleRequest(AsyncWebServerRequest *request) override final;
		virtual bool isRequestHandlerTrivial() override final {return false;}
		
		
};

class BatteryClass{	
	protected:
		unsigned int _charge;
		int _max;
		int _get_adc(byte times = 1);
	
	public:
		BatteryClass(){};
		~BatteryClass(){};
		int fetchCharge(int);
		bool callibrated();		
		void setCharge(unsigned int ch){_charge = ch;};
		unsigned int getCharge(){return _charge;};
		void setMax(int m){_max = m;};	
		int getMax(){return _max;};
};


//void powerOff();
void reconnectWifi(AsyncWebServerRequest*);
extern CoreClass * CORE;
extern BatteryClass BATTERY;
//extern Task POWER;

#endif //_CORE_h







