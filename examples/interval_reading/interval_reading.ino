#include <STM32L0.h>
#include <ponsel_sensor.h>

ponsel_sensor ctzn(Serial1, 50, CTZN);
ponsel_sensor optod(Serial1, 10, OPTOD);

void setup()
{
    // Prepare serial for debug
    Serial.begin(115200);

    // Prepare serial for RS485 comunication
    Serial1.begin(9600);

    Serial.println("Serials prepared");

    if (!ctzn.begin()) {
        Serial.println("Error ctzn begin");
    }
    if (!optod.begin()) {
        Serial.println("Error optod begin");
    }
}

void loop()
{
    if (ctzn.read_measurements()) {
        ctzn.print_measurements();
    }

    if (optod.read_measurements()) {
        optod.print_measurements();
    }
    delay(1000);
}

