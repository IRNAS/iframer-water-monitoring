#include "ponsel_sensor.h"


void preTransmission() {
  digitalWrite(DRIVER_EN, ENABLE_DRIVER);
}

void postTransmission() {
  digitalWrite(DRIVER_EN, DISABLE_DRIVER);
}

void rs485_board_init()
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
}

void ponsel_sensor_init(ModbusMaster &sensor, Stream &serial, uint8_t address)
{
    // Initialize water meter communication
    sensor.begin(address, serial);
    //sensor.preTransmission(preTransmission);
    //sensor.postTransmission(postTransmission);
}
