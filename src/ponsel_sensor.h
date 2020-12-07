#ifndef PONSEL_SENSOR_H
#define PONSEL_SENSOR_H

#include <ModbusMaster.h>
#include <STM32L0.h>
#include <Arduino.h>
#include "pinout_mappings.h"

#define WATER_METER_ADDR    (50)
#define RUN_MEASURE_ADDR    (0x0001)   //Run given measurement
#define DELAY_ADDR          (0x00A4)   //Approx Time needed to obtain all measurements
#define READ_MEASURE_ADDR   (0x0052)   //Run given measurement


enum sensor_type {
    OPTOD,
    CTZN
};

struct measurements {
    uint16_t param0;
    float    param1;
    float    param2;
    float    param3;
    float    param4;
};

struct pinout {
    int32_t boost_en;
    int32_t uart_inh;
    int32_t uart_sel_a;
    int32_t uart_sel_b;
    int32_t driver_en;
    int32_t enable_driver;
    int32_t disable_driver;
};

class ponsel_sensor
{
    public:
        ponsel_sensor(Stream &serial, 
                      uint8_t address, 
                      enum sensor_type type, 
                      struct pinout pins);
        bool begin();
        void print_measurements();
        bool read_measurements();
        struct measurements get_measurements();

    private:
        ModbusMaster _sensor;

        Stream * _serial;
        uint8_t _address;
        enum sensor_type _type;

        uint16_t _delay;
        struct measurements _measurements;

        void rs485_board_init();
        uint16_t get_uint16_value(uint8_t index);
        float get_float_value(uint8_t index);
        void store_measurements();
        bool verfy_status_param();
};

#endif /* PONSEL_SENSOR_H */
/*** end of file ***/
