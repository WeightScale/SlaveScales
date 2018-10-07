#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "Core.h"
//#include "DateTime.h"
#include "BrowserServer.h"
#include "HttpUpdater.h"
#include "slave_config.h"

CoreClass * CORE;
BatteryClass BATTERY;
BlinkClass * BLINK;

CoreClass::CoreClass(const String& username, const String& password):
_username(username),
_password(password),
_authenticated(false){
	begin();
}

CoreClass::~CoreClass(){}

void CoreClass::begin(){		
	CoreMemory.init();
	_settings = &CoreMemory.eeprom.settings; //ссылка на переменную
	_hostname = String(_settings->apSSID);
	_hostname.toLowerCase();
	//_downloadSettings();
	//POWER.onRun(powerOff);
	//POWER.enabled = _settings->power_time_enable;	
	//POWER.setInterval(_settings->time_off);
	BATTERY.setMax(_settings->bat_max);
	if(BATTERY.callibrated()){		
		_settings->bat_max = BATTERY.getMax();
		CoreMemory.save();	
	};	
}

bool CoreClass::canHandle(AsyncWebServerRequest *request){	
	if(request->url().equalsIgnoreCase("/settings.html")){
		goto auth;
	}
#ifndef HTML_PROGMEM
	else if(request->url().equalsIgnoreCase("/sn")){
		goto auth;
	}
#endif
	else
		return false;
	auth:
		if (!request->authenticate(_settings->scaleName, _settings->scalePass)){
			if(!request->authenticate(_username.c_str(), _password.c_str())){
				request->requestAuthentication();
				return false;
			}
		}
		return true;
}

void CoreClass::handleRequest(AsyncWebServerRequest *request){
	if (request->args() > 0){		
		String message = " ";
		if (request->hasArg("ssid")){
			request->arg("ssid").toCharArray(_settings->wSSID,request->arg("ssid").length()+1);	
			if (String(_settings->wSSID).length()>0){
				taskConnectWiFi.resume();
				}else{
				taskConnectWiFi.pause();
			}
			goto save;
		}
		if (request->hasArg("assid")){
			request->arg("assid").toCharArray(_settings->apSSID,request->arg("assid").length()+1);
			goto save;
		}			
		if (request->hasArg("n_admin")){
			request->arg("n_admin").toCharArray(_settings->scaleName,request->arg("n_admin").length()+1);
			request->arg("p_admin").toCharArray(_settings->scalePass,request->arg("p_admin").length()+1);
			goto save;
		}		
		save:
		if (CoreMemory.save()){
			return request->send(200, TEXT_HTML, "OK");
		}		
		return request->send(400);
	}
	return request->send(200, TEXT_HTML, settings_html);
	//request->send(SPIFFS, request->url());
}


/*
void CoreClass::handleSetAccessPoint(AsyncWebServerRequest * request){	
	if (request->hasArg("ssids")){
		_settings->autoIp = true;
		_settings->scaleWlanSSID = request->arg("ssids");
		_settings->scaleWlanKey = request->arg("key");
	}
	AsyncWebServerResponse *response;	
	if (saveSettings()){
		response = request->beginResponse(200, TEXT_HTML, successResponse);
		response->addHeader("Connection", "close");
		request->onDisconnect([](){ESP.reset();});
	}else{
		response = request->beginResponse(400);
	}
	request->send(response);	
}*/

/*
#if! HTML_PROGMEM
void CoreClass::saveValueSettingsHttp(AsyncWebServerRequest *request) {	
	if (!browserServer.isAuthentified(request))
		return request->requestAuthentication();
	if (request->args() > 0){	// Save Settings
		if (request->hasArg("ssid")){
			_settings->autoIp = false;
			if (request->hasArg("auto"))
				_settings->autoIp = true;
			else
				_settings->autoIp = false;
			_settings->scaleLanIp = request->arg("lan_ip");			
			_settings->scaleGateway = request->arg("gateway");
			_settings->scaleSubnet = request->arg("subnet");
			_settings->scaleWlanSSID = request->arg("ssid");			
			_settings->scaleWlanKey = request->arg("key");	
			goto save;
		}
		
		if(request->hasArg("data")){
			DateTimeClass DateTime(request->arg("data"));
			Rtc.SetDateTime(DateTime.toRtcDateTime());
			request->send(200, TEXT_HTML, getDateTime());
			return;	
		}
		if (request->hasArg("host")){
			_settings->hostUrl = request->arg("host");
			_settings->hostPin = request->arg("pin").toInt();
			goto save;	
		}
		if (request->hasArg("n_admin")){
			_settings->scaleName = request->arg("n_admin");
			_settings->scalePass = request->arg("p_admin");
			goto save;
		}	
		if (request->hasArg("pt")){
			if (request->hasArg("pe"))
				POWER.enabled = _settings->power_time_enable = true;
			else
				POWER.enabled = _settings->power_time_enable = false;
			_settings->time_off = request->arg("pt").toInt();
			goto save;
		}	
		save:
		if (saveSettings()){
			goto url;
		}
		return request->send(400);	
	}
	url: 		
	request->send(SPIFFS, request->url());
}
#endif*/

/*
bool CoreClass::saveSettings() {	
	
	
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();

	if (!json.containsKey(SCALE_JSON)) {
		JsonObject& scale = json.createNestedObject(SCALE_JSON);
	}
	
	json[SCALE_JSON]["id_name_admin"] = _settings->scaleName;
	json[SCALE_JSON]["id_pass_admin"] = _settings->scalePass;
	//json[SCALE_JSON]["id_lan_ip"] = _settings->scaleLanIp;
	//json[SCALE_JSON]["id_gateway"] = _settings->scaleGateway;
	//json[SCALE_JSON]["id_subnet"] = _settings->scaleSubnet;
	//json[SCALE_JSON]["id_ssid"] = _settings->scaleWlanSSID;
	//json[SCALE_JSON]["id_key"] = _settings->scaleWlanKey;
	json[SCALE_JSON]["bat_max"] = _settings->bat_max;
	
	File serverFile = SPIFFS.open(SETTINGS_FILE, "w");
	if (!serverFile) {
		return false;
	}	
	
	json.printTo(serverFile);
	serverFile.flush();
	serverFile.close();
	return true;
}*/

/*
bool CoreClass::_downloadSettings() {
	_settings->scaleName = "admin";
	_settings->scalePass = "1234";
	//_settings->scaleLanIp = "192.168.1.100";
	//_settings->scaleGateway = "192.168.1.1";
	//_settings->scaleSubnet = "255.255.255.0";
	_settings->bat_max = MIN_CHG+1;
	File serverFile = SPIFFS.open(SETTINGS_FILE, "r");	
	
	if (!serverFile) {
		return false;
	}
	size_t size = serverFile.size();

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	
	serverFile.readBytes(buf.get(), size);
	serverFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) {
		return false;
	}
	if (json.containsKey(SCALE_JSON)){
		_settings->scaleName = json[SCALE_JSON]["id_name_admin"].as<String>();
		_settings->scalePass = json[SCALE_JSON]["id_pass_admin"].as<String>();
		//_settings->scaleLanIp = json[SCALE_JSON]["id_lan_ip"].as<String>();
		//_settings->scaleGateway = json[SCALE_JSON]["id_gateway"].as<String>();
		//_settings->scaleSubnet = json[SCALE_JSON]["id_subnet"].as<String>();
		//_settings->scaleWlanSSID = json[SCALE_JSON]["id_ssid"].as<String>();
		//_settings->scaleWlanKey = json[SCALE_JSON]["id_key"].as<String>();
		_settings->bat_max = json[SCALE_JSON]["bat_max"];	
	}
		
	_settings->bat_max = constrain(_settings->bat_max, MIN_CHG+1, 1024);
	return true;
}*/



/*
void powerOff(){
	browserServer.stop();
	SPIFFS.end();
	Scale.power_down(); /// Выключаем ацп
	digitalWrite(EN_NCP, LOW); /// Выключаем стабилизатор
	ESP.reset();
}*/

void reconnectWifi(AsyncWebServerRequest * request){
	AsyncWebServerResponse *response = request->beginResponse_P(200, PSTR(TEXT_HTML), "RECONNECT...");
	response->addHeader("Connection", "close");
	request->onDisconnect([](){
		SPIFFS.end();
		ESP.reset();});
	request->send(response);
}

int BatteryClass::fetchCharge(int times){
	_charge = _get_adc(times);
	_charge = constrain(_charge, MIN_CHG, _max);
	_charge = map(_charge, MIN_CHG, _max, 0, 100);
	return _charge;
}

int BatteryClass::_get_adc(byte times){
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += analogRead(A0);
	}
	return times == 0?sum :sum / times;	
}

bool BatteryClass::callibrated(){
	bool flag = false;
	int charge = _get_adc();	
	int t = _max;
	_max = constrain(t, MIN_CHG, 1024);
	if(t != _max){
		flag = true;	
	}
	charge = constrain(charge, MIN_CHG, 1024);
	if (_max < charge){
		_max = charge;	
		flag = true;
	}
	return flag;
}






