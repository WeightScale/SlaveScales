/*
 * slave_config.h
 *
 * Created: 06.10.2018 15:38:51
 *  Author: Kostya
 */ 


#ifndef SLAVE_CONFIG_H_
#define SLAVE_CONFIG_H_

#define HTML_PROGMEM          //Использовать веб страницы из flash памяти

#ifdef HTML_PROGMEM
	#include "Page.h"
#endif

#define MIN_CHG 500			//ADC = (Vin * 1024)/Vref  Vref = 1V  Vin = 0.49v  3.5v-4.3v

#endif /* SLAVE_CONFIG_H_ */