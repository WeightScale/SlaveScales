#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
//#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266httpUpdate.h>
#include <WiFiUdp.h>

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

#include "StreamString.h"
#include "Core.h"
#include "HttpUpdater.h"
#include "tools.h"
#include "Version.h"


//HttpUpdaterClass httpUpdater;
String updaterError;
int command;

HttpUpdaterClass::HttpUpdaterClass(const String& username, const String& password)
:_username(username)
,_password(password),_authenticated(false)
{}

/*
void HttpUpdaterClass::setup(BrowserServerClass *server, const char * path, const char * username, const char * password){
	_server = server;
	_username = (char *)username;
	_password = (char *)password;

	// handler for the /update form page						
	_server->on(path, HTTP_GET, handleUpdatePage);						/ * ���������� �������� * /

	// handler for the /update form POST (once file upload finishes)
	_server->on(path, HTTP_POST, handleEndUpdate, handleStartUpdate);	/ * ������� ���������� �������� * /
	
	_server->on("/hu", HTTP_GET, handleHttpStartUpdate);				/ * ���������� ���� �������� address/hu?host=sdb.net.ua/update.php * /
}*/

bool HttpUpdaterClass::canHandle(AsyncWebServerRequest *request){
	if(request->url().equalsIgnoreCase("/update")){
		return true;
	}
	return false;
}

void HttpUpdaterClass::handleRequest(AsyncWebServerRequest *request){
	if(_username.length() && _password.length() && !request->authenticate(_username.c_str(), _password.c_str()))
		return request->requestAuthentication();
	_authenticated = true;
	if(request->method() == HTTP_GET){
		request->send_P(200, TEXT_HTML, serverIndex);	
	}else if (request->method()==HTTP_POST){
		digitalWrite(2, LOW); //led off
		if (command == U_SPIFFS){
			//delay(1000);
			CoreMemory.save();
			request->redirect("/");
			return;
		}
		AsyncWebServerResponse *response = request->beginResponse_P(200, TEXT_HTML, successResponse);
		response->addHeader("Connection", "close");
		request->send(response);
		request->onDisconnect([](){ESP.reset();});
	}
}

void HttpUpdaterClass::handleUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
	digitalWrite(LED, !digitalRead(LED));	//led on
	
	if(!index){
		updaterError = String();
		if(!_authenticated){
			updaterError = "filed authenticated";
			return;
		}
		size_t size;
		if(filename.indexOf("spiffs.bin",0) != -1) {
			command = U_SPIFFS;
			size = ((size_t) &_SPIFFS_end - (size_t) &_SPIFFS_start);
		}else if(filename.indexOf("ino.bin",0) != -1) {
			command = U_FLASH;
			size = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		}else{
			updaterError = "error file";
			return;
		}
		
		Update.runAsync(true);
		if(!Update.begin(size, command)){
			setUpdaterError();
		}
	}
	if(!Update.hasError()){
		if(Update.write(data, len) != len){
			setUpdaterError();
		}
	}
	if(final){
		if(!Update.end(true)){
			setUpdaterError();
		}
	}		
}

void setUpdaterError(){	
	StreamString str;
	Update.printError(str);
	updaterError = str.c_str();
}

/*
void handleUpdatePage(AsyncWebServerRequest *request){
	if(!request->authenticate(httpUpdater.getUserName(), httpUpdater.getPassword()))
		return request->requestAuthentication();
	httpUpdater.setAuthenticated(true);
	request->send_P(200, PSTR(TEXT_HTML), serverIndex);		
}*/

/*
void handleStartUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
	digitalWrite(LED, LOW);
	
	if(!index){
		updaterError = String();
		if(!httpUpdater.getAuthenticated()){
			return;
		}
		size_t size;
		if(filename.indexOf("spiffs.bin",0) != -1) {
			command = U_SPIFFS;
			size = ((size_t) &_SPIFFS_end - (size_t) &_SPIFFS_start);
		}else if(filename.indexOf("ino.bin",0) != -1) {
			command = U_FLASH;
			size = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		}else{
			updaterError = "�� ������ ����";
			return;
		}		
		
		Update.runAsync(true);
		if(!Update.begin(size, command)){
			setUpdaterError();
		}
	}
	if(!Update.hasError()){
		if(Update.write(data, len) != len){
			setUpdaterError();
		}
	}
	if(final){
		if(!Update.end(true)){
			setUpdaterError();
		}
	}
	delay(0);	
}*/

/*
void handleEndUpdate(AsyncWebServerRequest * request){
	if (updaterError && updaterError[0] != 0x00) {
		request->send(200, F(TEXT_HTML), String(F("Update error: ")) + updaterError);
	} else {
		if (command == U_SPIFFS){
			delay(2000);
			CORE.saveSettings();
			Scale.saveDate();
			request->redirect("/");
			return;
		}
		request->client()->setNoDelay(true);
		request->send_P(200, PSTR(TEXT_HTML), successResponse);
		delay(100);
		request->client()->stop();
		ESP.restart();
	}
}*/

void HttpUpdaterClass::handleHttpStartUpdate(AsyncWebServerRequest * request){										/* ���������� ���� �������� address/hu?host=sdb.net.ua/update.php */
	if(!request->authenticate(_username.c_str(), _password.c_str()))
		return request->requestAuthentication();
	if(request->hasArg("host")){
		String host = request->arg("host");
		//_server->send(200, "text/plain", host);
		ESPhttpUpdate.rebootOnUpdate(false);
		digitalWrite(LED, LOW);
		String url = String("http://");
		url += host;
		t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(url,SPIFFS_VERSION);
		if (ret == HTTP_UPDATE_OK){
			CoreMemory.save();
			ret = ESPhttpUpdate.update(url, SKETCH_VERSION);
		}
		switch(ret) {
			case HTTP_UPDATE_FAILED:
				request->send(404, "text/plain", ESPhttpUpdate.getLastErrorString());
			break;
			case HTTP_UPDATE_NO_UPDATES:
				request->send(304, "text/plain", "���������� �� ���������");
			break;
			case HTTP_UPDATE_OK:
				request->client()->stop();
				ESP.restart();
			break;
		}
		
	}
	digitalWrite(LED, HIGH);		
};
	