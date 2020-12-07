#include <STM32L0.h>
#include <ponsel_sensor.h>

struct pinout pins =
{
    .boost_en = PB6,
    .uart_inh = -1,
    .uart_sel_a = -1,
    .uart_sel_b = -1,
    .driver_en = PA3,
    .enable_driver = LOW,
    .disable_driver = HIGH,
};

//ponsel_sensor ctzn(Serial1, 50, CTZN, pins);
ponsel_sensor optod(Serial1, 10, OPTOD, pins);

void setup()
{
    // Prepare serial for debug
    Serial.begin(115200);

    // Prepare serial for RS485 comunication
    Serial1.begin(9600);

    Serial.println("Serials prepared");

    //if (!ctzn.begin()) {
    //    Serial.println("Error ctzn begin");
    //}
    if (!optod.begin()) {
        Serial.println("Error optod begin");
    }
}


void loop()
{
    //if (ctzn.read_measurements()) {
    //    ctzn.print_measurements();
    //}

    if (optod.read_measurements()) {
        optod.print_measurements();
    }
    delay(1000);
}

