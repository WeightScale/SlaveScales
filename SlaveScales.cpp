#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include "BrowserServer.h"
#include "Core.h"
#include "Task.h"
#include "HttpUpdater.h"

/*
 * This example serves a "hello world" on a WLAN and a SoftAP at the same time.
 * The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.
 * 
 * Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
 * Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.
 * 
 * Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.
 * 
 * This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
 */

void onSTAGotIP(const WiFiEventStationModeGotIP& evt);
void onSTADisconnected(const WiFiEventStationModeDisconnected& evt);
void takeBlink();
void takeBattery();
void takeWeight();
//void powerSwitchInterrupt();
void connectWifi();
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

WebSocketsClient webSocket;
//
TaskController taskController = TaskController();		/*  */
Task taskBlink(takeBlink, 500);							/*  */
Task taskBattery(takeBattery, 20000);					/* 20 Обновляем заряд батареи */
//WiFi.channel(i);Task taskPower(powerOff, 2400000);						/* 10 минут бездействия и выключаем */
Task taskConnectWiFi(connectWifi, 5000);				/* Пытаемся соедениться с точкой доступа каждые 60 секунд */
Task taskWeight(takeWeight,200);
WiFiEventHandler STAGotIP;
WiFiEventHandler STADisconnected;

unsigned int COUNT_FLASH = 500;
unsigned int COUNT_BLINK = 500;
#define USE_SERIAL Serial
//HTTPClient http;

void connectWifi();

void setup() {
	pinMode(EN_NCP, OUTPUT);
	digitalWrite(EN_NCP, HIGH);
	pinMode(LED, OUTPUT);	
	//Serial.begin(115200);
	/*while (digitalRead(PWR_SW) == HIGH){
		delay(100);
	};*/
	
	SPIFFS.begin();
	browserServer.begin();
	Scale.setup(&browserServer);	
	takeBattery();
  
	taskController.add(&taskBlink);
	taskController.add(&taskBattery);
	taskController.add(&taskConnectWiFi);
	taskController.add(&taskWeight);
	//taskController.add(&taskPower);	

	STAGotIP = WiFi.onStationModeGotIP(&onSTAGotIP);	
	STADisconnected = WiFi.onStationModeDisconnected(&onSTADisconnected);
	/*http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	http.setTimeout(3000);*/
	//ESP.eraseConfig();
	WiFi.persistent(false);
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	//WiFi.smartConfigDone();
	WiFi.mode(WIFI_AP_STA);
	WiFi.hostname(MY_HOST_NAME);
	WiFi.softAPConfig(apIP, apIP, netMsk);
	WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASSWORD);
	delay(500); 
	
	connectWifi();
	
	 
	
	// event handler
	webSocket.onEvent(webSocketEvent);
	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");
	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(2000);
}

/*********************************/
/* */
/*********************************/
void takeBlink() {
	bool led = !digitalRead(LED);
	digitalWrite(LED, led);	
	taskBlink.setInterval(led ? COUNT_BLINK : COUNT_FLASH);
}

/**/
void takeBattery(){
	BATTERY.fetchCharge(1);		
}

void takeWeight(){
	Scale.fetchWeight();
	//webSocket.sendTXT("slave");	
	webSocket.sendTXT("{\"slave\":{\"w\":\""+String(Scale.getBuffer())+"\",\"c\":"+String(BATTERY.getCharge())+",\"s\":"+String(Scale.getStableWeight())+"}}");
	taskWeight.updateCache();
}

/*
void powerSwitchInterrupt(){
	if(digitalRead(PWR_SW)==HIGH){
		unsigned long t = millis();
		digitalWrite(LED, HIGH);
		while(digitalRead(PWR_SW)==HIGH){ // 
			delay(100);	
			if(t + 4000 < millis()){ // 
				digitalWrite(LED, HIGH);
				while(digitalRead(PWR_SW) == HIGH){delay(10);};// 
				powerOff();			
				break;
			}
			digitalWrite(LED, !digitalRead(LED));
		}
	}
}*/

void connectWifi() {
	//USE_SERIAL.println("Connecting...");
	WiFi.disconnect(false);
	/*!  */
	int n = WiFi.scanComplete();
	if(n == -2){
		n = WiFi.scanNetworks();
		if (n <= 0){
			goto scan;
		}
	}else if (n > 0){
		goto connect;
	}else{
		goto scan;
	}
	connect:
		for (int i = 0; i < n; ++i)	{
			if(WiFi.SSID(i) == MASTER_SSID/*CORE->getSSID().c_str()*/){
				//USE_SERIAL.println(CORE.getSSID());
				String ssid_scan;
				int32_t rssi_scan;
				uint8_t sec_scan;
				uint8_t* BSSID_scan;
				int32_t chan_scan;
				bool hidden_scan;

				WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
				WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASSWORD, chan_scan); //Устанавливаем канал как роутера
				WiFi.begin ( MASTER_SSID/*CORE->getSSID().c_str()*/, MASTER_PASSWORD/*CORE->getPASS().c_str()*/,chan_scan,BSSID_scan);
				//WiFi.begin ( CORE.getSSID().c_str(), CORE.getPASS().c_str());
				int status = WiFi.waitForConnectResult();
				return;
			}
		}
	scan:
		WiFi.scanDelete();
		WiFi.scanNetworks(true);	
		
}

void loop() {
	taskController.run();	
	dnsServer.processNextRequest();
	webSocket.loop();
	
	//powerSwitchInterrupt();
	//randomSeed(millis());	
	/*http.begin("http://192.168.4.1", 80, "/slv");	
	int httpCode = http.POST("wt="+String(Scale.getUnits()));
	//int httpCode = http.GET();
	if(httpCode == 200) {
		String payload = http.getString();
		USE_SERIAL.println(payload);
	}else{
		USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}
		
	http.end();*/	
}

void onSTAGotIP(const WiFiEventStationModeGotIP& evt) {
	taskConnectWiFi.pause();
	webSocket.begin(evt.gw.toString(), 80, "/ss");
	//Serial.println(String(evt.channel));
	//WiFi.softAPdisconnect();
	//WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASSWORD, evt.channel); //Устанавливаем канал как роутера
	COUNT_FLASH = 50;
	COUNT_BLINK = 3000;
}

void onSTADisconnected(const WiFiEventStationModeDisconnected& evt) {
	//Serial.println("DisconnectStation");
	WiFi.scanDelete();
	WiFi.scanNetworks(true);
	taskConnectWiFi.resume();
	COUNT_FLASH = 500;
	COUNT_BLINK = 500;
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	switch(type) {
		case WStype_DISCONNECTED:
			//taskPower.updateCache();
			//taskWeight.pause();			//Останавливаем измерение веса
		break;
		case WStype_CONNECTED:
			{
				//taskPower.pause(); 
				String s = String(WiFi.localIP().toString());
				webSocket.sendTXT("{\"ip\":\""+s+"\"}");
			//taskWeight.resume();		//Запускаем измерение веса
			}
			
		break;
		case WStype_TEXT:
			{
				//String msg = (char *)payload;
				/*for(size_t i=0; i < length; i++) {
					msg += (char) payload[i];
				}*/
				DynamicJsonBuffer jsonBuffer(length);
				JsonObject& json = jsonBuffer.parseObject(payload);
				if (json.containsKey("up")) {
					JsonObject& up = json["up"];
					Scale.doValueUpdate(up);
					
					/*JsonObject& slave = json["slave"];
					_weight = slave["w"].as<float>();
					_charge = slave["c"];
					_stableWeight = json["slave"]["s"].as<bool>();*/
					//_charge = json["slave"]["c"];
					//setWeight(slave["w"].as<float>());
					//setStable(slave["s"].as<bool>());
				}else if (json.containsKey("tp")){
					Scale.tare();	
				}
				/*if(msg.equals("/tp")){					
					Scale.tare();
				}	*/
			}
		break;
		case WStype_BIN:
		
		break;
	}

}
