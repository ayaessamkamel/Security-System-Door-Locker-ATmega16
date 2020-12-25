/******************************************************************************
 *
 * Module: INTERNAL EEPROM
 *
 * File Name: internal_eeprom.h
 *
 * Description: Header file for the internal eeprom
 *
 * Author: AYA ESSAM
 *
 *******************************************************************************/

#ifndef INTERNAL_EEPROM_H_
#define INTERNAL_EEPROM_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

void EEPROM_write(unsigned short uiAddress, unsigned char ucData) ;

unsigned char EEPROM_read(unsigned short uiAddress);

#endif /* INTERNAL_EEPROM_H_ */
