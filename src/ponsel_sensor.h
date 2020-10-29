#ifndef PONSEL_SENSOR_H
#define PONSEL_SENSOR_H

#include <ModbusMaster.h>
#include <STM32L0.h>
#include "pinout_mappings.h"
#include <Arduino.h>

#define WATER_METER_ADDR    (50)
#define RUN_MEASURE_ADDR    (0x0001)   //Run given measurement
#define DELAY_ADDR          (0x00A4)   //Approx Time needed to obtain all measurements
#define READ_MEASURE_ADDR   (0x0052)   //Run given measurement


void rs485_board_init();
void ponsel_sensor_init(ModbusMaster &sensor, Stream &serial, uint8_t address);





#endif /* PONSEL_SENSOR_H */
/*** end of file ***/
