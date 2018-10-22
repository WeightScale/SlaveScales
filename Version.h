#ifndef _VERSION_h
#define _VERSION_h

#define SKETCH_VERSION "web_travers_slave_scales.002"
#define SPIFFS_VERSION "web_travers_slave_scales.001"

/*
������ �����

[���].[������].[���].[����������]


���			- ��� ����� ��� ��� ������
������		- ������ ��������� �����
���			- ��� ����� ino ��� spiff
����������	- bin

������: weight_scale.001.spiff.bin

�������� �������� �������������� �����

������ split
���� �� bin false
���� ��� spiff ��� ino �������� ��������������� ����� ���� spiffs sketch 
� ����� ���� ����� �� ����� � ��������� ������

������ ������� ���������� ���

    http.useHTTP10(true);
    http.setTimeout(8000);
    http.setUserAgent(F("ESP8266-http-Update"));
    http.addHeader(F("x-ESP8266-STA-MAC"), WiFi.macAddress());
    http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
    http.addHeader(F("x-ESP8266-free-space"), String(ESP.getFreeSketchSpace()));
    http.addHeader(F("x-ESP8266-sketch-size"), String(ESP.getSketchSize()));
    http.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
    http.addHeader(F("x-ESP8266-chip-size"), String(ESP.getFlashChipRealSize()));
    http.addHeader(F("x-ESP8266-sdk-version"), ESP.getSdkVersion());

    if(spiffs) {
	    http.addHeader(F("x-ESP8266-mode"), F("spiffs"));
	    } else {
	    http.addHeader(F("x-ESP8266-mode"), F("sketch"));
    }

    if(currentVersion && currentVersion[0] != 0x00) {
	    http.addHeader(F("x-ESP8266-version"), currentVersion);
    }

    const char * headerkeys[] = { "x-MD5" };
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    // track these headers
    http.collectHeaders(headerkeys, headerkeyssize);

*/

#endif //_VERSION_h