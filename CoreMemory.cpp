#include "CoreMemory.h"
#include "slave_config.h"

void CoreMemoryClass::init(){
	
	//eeprom.scales_value.password = "1234";
	begin(sizeof(MyEEPROMStruct));
	if (percentUsed() >= 0) {
		get(0, eeprom);
		}else{
		doDefault();
	}
}

bool CoreMemoryClass::save(){
	put(0, eeprom);
	return commit();
}

bool CoreMemoryClass::doDefault(){
	String u = "admin";
	String p = "1234";
	String apSsid = "Slave";
	u.toCharArray(eeprom.scales_value.user, u.length()+1);
	p.toCharArray(eeprom.scales_value.password, p.length()+1);
	u.toCharArray(eeprom.settings.scaleName, u.length()+1);
	p.toCharArray(eeprom.settings.scalePass, p.length()+1);
	apSsid.toCharArray(eeprom.settings.apSSID, apSsid.length()+1);
	eeprom.settings.bat_max = MIN_CHG + 1;
	return save();
}

CoreMemoryClass CoreMemory;

