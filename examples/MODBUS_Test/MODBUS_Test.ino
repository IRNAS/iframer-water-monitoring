#include <ModbusMaster.h>
#include <STM32L0.h>


#define BOOST_EN PB9
#define UART_INH   PB6
#define UART_SEL_A PB12
#define UART_SEL_B PB5

// On Pira_Smart_R485_Add-On board the driver control is switched, thats why we
// also switch logic here.
#define DRIVER_EN PB15
#define ENABLE_DRIVER LOW
#define DISABLE_DRIVER HIGH

#define WATER_METER_ADDR    (50)
#define RUN_MEASURE_ADDR    (0x0001)   //Run given measurement
#define DELAY_ADDR          (0x00A4)   //Approx Time needed to obtain all measurements
#define READ_MEASURE_ADDR   (0x0052)   //Run given measurement


ModbusMaster sensor;

uint8_t result;

void preTransmission() {
  digitalWrite(DRIVER_EN, ENABLE_DRIVER);
}

void postTransmission() {
  digitalWrite(DRIVER_EN, DISABLE_DRIVER);
}

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
    //Prepare GPIO
    pinMode(DRIVER_EN, OUTPUT);
    pinMode(BOOST_EN, OUTPUT);
    pinMode(UART_INH, OUTPUT);
    pinMode(UART_SEL_A, OUTPUT);
    pinMode(UART_SEL_B, OUTPUT);

    // Turn on 5V BOOST converter
    digitalWrite(BOOST_EN, HIGH);

    // Drive INH low to enable MUX outputs, SEL A and B have to be low so
    // we are talking to uart 1, which leads to RS485 driver chip.
    digitalWrite(UART_INH,   LOW);
    digitalWrite(UART_SEL_A, LOW);
    digitalWrite(UART_SEL_B, LOW);


    // Prepare serial for debug
    Serial.begin(115200);

    // Prepare serial for RS485 comunication
    Serial1.begin(9600);

    // Initialize water meter communication
    sensor.begin(WATER_METER_ADDR, Serial1);
    sensor.preTransmission(preTransmission);
    sensor.postTransmission(postTransmission);

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
