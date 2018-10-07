//#include <ESP8266WebServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ArduinoJson.h>
#include <Hash.h>
#include <AsyncJson.h>
#include <DNSServer.h>
#include <functional>
#include "BrowserServer.h"
#include "tools.h"
#include "Core.h"
#include "Version.h"
#include "HttpUpdater.h"
#include "slave_config.h"


/* */
//ESP8266HTTPUpdateServer httpUpdater;
/* Soft AP network parameters */
IPAddress apIP(192,168,5,1);
IPAddress netMsk(255, 255, 255, 0);

IPAddress lanIp;			// ���� ������� ��������� ip ������
IPAddress gateway;

BrowserServerClass browserServer(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;
//holds the current upload
//File fsUploadFile;

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */


BrowserServerClass::BrowserServerClass(uint16_t port) : AsyncWebServer(port) {}

BrowserServerClass::~BrowserServerClass(){}
	
void BrowserServerClass::begin() {
	
	/* Setup the DNS server redirecting all the domains to the apIP */
	dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	dnsServer.start(DNS_PORT, "*", apIP);	
	//_downloadHTTPAuth();
	_httpAuth.wwwUsername = "sa";
	_httpAuth.wwwPassword = "343434";
	ws.onEvent(onWsEvent);
	addHandler(&ws);
	CORE = new CoreClass(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str());
	addHandler(CORE);
	addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
	addHandler(new SPIFFSEditor(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str()));	
	addHandler(new HttpUpdaterClass("sa", "654321"));
	init();
	AsyncWebServer::begin(); // Web server start
}

void BrowserServerClass::init(){
	/*on("/wt",HTTP_GET, [](AsyncWebServerRequest * request){					/ * �������� ��� � �����. * /
		request->send(200, "text/html", String("{\"w\":\""+String(Scale.getBuffer())+"\",\"c\":"+String(CORE.getCharge())+",\"s\":"+String(Scale.getStableWeight())+"}"));	
	});	*/			
				
	on("/rc", reconnectWifi);									/* ��������������� �� WiFi. */
	//on("/sn",WebRequestMethod::HTTP_GET,handleAccessPoint);						/* ���������� ��������� ����� ������� */
	//on("/sn",WebRequestMethod::HTTP_POST, std::bind(&CoreClass::handleSetAccessPoint, CORE, std::placeholders::_1));					/* ���������� ��������� ����� ������� */
	//on("/settings.html", HTTP_ANY, std::bind(&CoreClass::saveValueSettingsHttp, CORE, std::placeholders::_1));					/* ������� �������� �������� ��� ��������� ��������. */
	on("/settings.json", handleSettings);
	on("/sv", handleScaleProp);									/* �������� ��������. */
	//on("/admin.html", std::bind(&BrowserServerClass::send_wwwauth_configuration_html, this, std::placeholders::_1));
	//on("/secret.json",handleFileReadAdmin);
	serveStatic("/secret.json", SPIFFS, "/").setDefaultFile("secret.json").setAuthentication(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str());
												
	/*on("/",[&](){												/ * ������� ��������. * /
		handleFileRead(uri());
		taskPower.resume();
	});		
	
	//on("/generate_204", [this](){if (!handleFileRead("/index.html"))	this->send(404, "text/plain", "FileNotFound");});  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
	//on("/fwlink", [this](){if (!handleFileRead("/index.html"))	this->send(404, "text/plain", "FileNotFound");});  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.	
	
	const char * headerkeys[] = {"User-Agent","Cookie"/ *,"x-SETNET"* /} ;
	size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
	//ask server to track these headers
	collectHeaders(headerkeys, headerkeyssize );*/
	//rewrite("/", "/settings.html");
	//serveStatic("/", SPIFFS, "/");
	//serveStatic("/", SPIFFS, "/").setDefaultFile("index-ap.html").setFilter(ON_AP_FILTER);
	//rewrite("/", "index-ap.html").setFilter(ON_AP_FILTER);
#ifdef HTML_PROGMEM
	on("/",[](AsyncWebServerRequest * reguest){	reguest->send_P(200,F("text/html"),settings_html);});								/* ������� ��������. */	
	on("/global.css",[](AsyncWebServerRequest * reguest){	reguest->send_P(200,F("text/css"),global_css);});					/* ����� */
#else
	serveStatic("/", SPIFFS, "/").setDefaultFile("settings.html");	
#endif
	onNotFound([](AsyncWebServerRequest *request){
		request->send(404);
	});
}

/*
void BrowserServerClass::send_wwwauth_configuration_html(AsyncWebServerRequest *request) {
	if (!checkAdminAuth(request))
		return request->requestAuthentication();
	if (request->args() > 0){  // Save Settings
		if (request->hasArg("wwwuser")){
			_httpAuth.wwwUsername = request->arg("wwwuser");
			_httpAuth.wwwPassword = request->arg("wwwpass");
		}		
		_saveHTTPAuth();
	}
	request->send(SPIFFS, request->url());
}*/

/*
bool BrowserServerClass::_saveHTTPAuth() {
	
	DynamicJsonBuffer jsonBuffer(256);
	//StaticJsonBuffer<256> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["user"] = _httpAuth.wwwUsername;
	json["pass"] = _httpAuth.wwwPassword;

	//TODO add AP data to html
	File configFile = SPIFFS.open(SECRET_FILE, "w");
	if (!configFile) {
		configFile.close();
		return false;
	}

	json.printTo(configFile);
	configFile.flush();
	configFile.close();
	return true;
}*/

/*
bool BrowserServerClass::_downloadHTTPAuth() {
	_httpAuth.wwwUsername = "sa";
	_httpAuth.wwwPassword = "343434";
	File configFile = SPIFFS.open(SECRET_FILE, "r");
	if (!configFile) {
		configFile.close();
		return false;
	}
	size_t size = configFile.size();

	std::unique_ptr<char[]> buf(new char[size]);
	
	configFile.readBytes(buf.get(), size);
	configFile.close();
	DynamicJsonBuffer jsonBuffer(256);
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) {
		return false;
	}	
	_httpAuth.wwwUsername = json["user"].as<String>();
	_httpAuth.wwwPassword = json["pass"].as<String>();
	return true;
}*/

bool BrowserServerClass::checkAdminAuth(AsyncWebServerRequest * r) {	
	return r->authenticate(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str());
}

/*
void BrowserServerClass::restart_esp() {
	String message = "<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/>";
	message += "<meta http-equiv='refresh' content='10; URL=/admin.html'>Please Wait....Configuring and Restarting.";
	send(200, "text/html", message);
	SPIFFS.end(); // SPIFFS.end();
	delay(1000);
	ESP.restart();
}*/

bool BrowserServerClass::isAuthentified(AsyncWebServerRequest * request){
	if (!request->authenticate(CORE->getNameAdmin(), CORE->getPassAdmin())){
		if (!checkAdminAuth(request)){
			return false;
		}
	}
	return true;
}

/*
void handleFileReadAuth(AsyncWebServerRequest * request){
	if (!browserServer.isAuthentified(request)){
		return request->requestAuthentication();
	}
	request->send(SPIFFS, request->url());
}*/

void handleSettings(AsyncWebServerRequest * request){
	if (!browserServer.isAuthentified(request))
		return request->requestAuthentication();
#ifdef HTML_PROGMEM
	AsyncResponseStream *response = request->beginResponseStream("application/json");
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	JsonObject& scale = root.createNestedObject(SCALE_JSON);
	scale["bat_max"] = CoreMemory.eeprom.settings.bat_max;
	scale["id_assid"] = CoreMemory.eeprom.settings.apSSID;
	scale["id_n_admin"] = CoreMemory.eeprom.settings.scaleName;
	scale["id_p_admin"] = CoreMemory.eeprom.settings.scalePass;
	scale["id_ssid"] = String(CoreMemory.eeprom.settings.wSSID);
	
	root.printTo(*response);
	request->send(response);
#else
	request->send(SPIFFS, request->url());
#endif
}

void handleScaleProp(AsyncWebServerRequest * request){
	if (!browserServer.isAuthentified(request))
		return request->requestAuthentication();
	AsyncJsonResponse * response = new AsyncJsonResponse();
	JsonObject& root = response->getRoot();
	root["id_local_host"] = WiFi.hostname();
	root["id_ap_ssid"] = String(WiFi.softAPSSID());
	root["id_ap_ip"] = toStringIp(WiFi.softAPIP());
	root["id_ip"] = toStringIp(WiFi.localIP());
	root["sl_id"] = String(Scale.getSeal());
	response->setLength();
	request->send(response);
	/*String values = "";
	values += "id_date|" + getDateTime() + "|div\n";
	values += "id_local_host|"+String(MY_HOST_NAME)+"/|div\n";
	values += "id_ap_ssid|" + String(SOFT_AP_SSID) + "|div\n";
	values += "id_ap_ip|" + toStringIp(WiFi.softAPIP()) + "|div\n";
	values += "id_ip|" + toStringIp(WiFi.localIP()) + "|div\n";
	values += "sl_id|" + String(Scale.getSeal()) + "|div\n";
	
	request->send(200, "text/plain", values);*/
}

void handleAccessPoint(AsyncWebServerRequest * request){
	if (!browserServer.isAuthentified(request))
		return request->requestAuthentication();
	request->send(200, TEXT_HTML, netIndex);	
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){	
		client->ping();	
	} else if(type == WS_EVT_DISCONNECT){
	} else if(type == WS_EVT_ERROR){
	} else if(type == WS_EVT_PONG){
	} else if(type == WS_EVT_DATA){
		//AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String msg = "";
		/*if(info->final && info->index == 0 && info->len == len){			
			if(info->opcode == WS_TEXT){
				for(size_t i=0; i < info->len; i++) {
					msg += (char) data[i];
				}
				if (msg.equals("/wt")){
					client->text(String("{\"w\":\""+String(Scale.getBuffer())+"\",\"c\":"+String(99)+",\"s\":"+String(true)+"}"));
				}
			} 
		}*/		
		for(size_t i=0; i < len; i++) {
			msg += (char) data[i];
		}
		if (msg.equals("/wt")){
			/*char buffer[10];		
			float w = Scale.forTest(ESP.getFreeHeap());
			Scale.formatValue(w,buffer);
			Scale.detectStable(w);*/			
			client->text(String("{\"w\":\""+String(Scale.getBuffer())+"\",\"c\":"+String(BATTERY.getCharge())+",\"s\":"+String(Scale.getStableWeight())+"}"));
		}
	}
}