#include <ModbusMaster.h>
#include <STM32L0.h>
#include "ponsel_sensor.h"

ModbusMaster sensor;


uint8_t result;

uint16_t get_int_value(ModbusMaster sensor, uint8_t index)
{
    return sensor.getResponseBuffer(index);
}

float get_float_value(ModbusMaster sensor, uint8_t index)
{
    union convert {
        struct {
            uint16_t low;
            uint16_t high;
        };
        float f;
    }tofloat;

    tofloat.high = sensor.getResponseBuffer(index);
    tofloat.low  = sensor.getResponseBuffer(index + 1);

    return tofloat.f; 
}


void setup()
{
    rs485_board_init();

    // Prepare serial for debug
    Serial.begin(115200);

    // Prepare serial for RS485 comunication
    Serial1.begin(9600);

    ponsel_sensor_init(sensor, Serial1, 50);


    // Continiously pool for delay value, this can fail once, but it succeds 
    // afterwards
    uint8_t delay_val = 0;
    Serial.println("Getting delay value");
    while(1) {
        // Read one register from DELAY_ADDR
        result = sensor.readHoldingRegisters(DELAY_ADDR, 1);

        if (result == sensor.ku8MBSuccess) {
            Serial.print("Received delay: ");
            Serial.print(sensor.getResponseBuffer(0x00));
            Serial.println(" ms");
            delay_val = sensor.getResponseBuffer(0x00);
            break;
        }
        else {
            Serial.println("Failed, trying again!");
        }
        delay(1000);
    }

    /* We can read from CTZN sensor four different measurements:
     * Parameter 0, Temperature,        unit - degrees
     * Parameter 1, Conducivity CTZ,    unit - mS/cm
     * Parameter 2, Salinity,           unit - g/Kg 
     * Parameter 3, Conductivity 
     *              without temp. 
     *              compensation        uint - ms/cm
     * Parameter 4, Is 
     */

    while(1)
    {
        Serial.println("Writing multiple registers");
        sensor.setTransmitBuffer(0, 0x1F);  // Read 6 registers, 
        // status reg and plus 5 easurements 
        result = sensor.writeMultipleRegisters(RUN_MEASURE_ADDR, 1);

        // Read 11 registers, one int and 5 floats, which is 11 bytes
        int read_reg_num = 11;

        delay(delay_val);
        result = sensor.readHoldingRegisters(READ_MEASURE_ADDR, read_reg_num);

        if (result == sensor.ku8MBSuccess)
        {
            Serial.println("Received: ");
            uint16_t status_reg = get_int_value(sensor, 0);
            float temp          = get_float_value(sensor, 1);
            float cond_CTZ      = get_float_value(sensor, 3);
            float salinity      = get_float_value(sensor, 5);
            float cond_no_comp  = get_float_value(sensor, 7);


            for (int i = 0; i < read_reg_num; i++)
            {
                Serial.print(" ");
                Serial.print("0x");
                Serial.print(sensor.getResponseBuffer(i), HEX);
            }
            Serial.println();

            Serial.print("Temp:         ");
            Serial.println(temp);
            Serial.print("Cond_CTZ:     ");
            Serial.println(cond_CTZ);
            Serial.print("Salinity:     ");
            Serial.println(salinity);
            Serial.print("Cond_no_comp: ");
            Serial.println(cond_no_comp);
            Serial.println("----------------------------------------");
            Serial.println();
        }
        else {
            Serial.print("Failed");
        }
        delay(1000);
    }
}

void loop()
{


}
