#include <ponsel_sensor.h>


static bool init_done = false;
static pinout _pins;

static void pre_tran()
{
  digitalWrite(_pins.driver_en, _pins.enable_driver);
}

static void post_tran() 
{
  digitalWrite(_pins.driver_en, _pins.disable_driver);
}

void ponsel_sensor::rs485_board_init()
{
    //Prepare GPIO
    pinMode(_pins.driver_en, OUTPUT);
    pinMode(_pins.boost_en, OUTPUT);
    pinMode(_pins.uart_inh, OUTPUT);
    pinMode(_pins.uart_sel_a, OUTPUT);
    pinMode(_pins.uart_sel_b, OUTPUT);

    // Turn on 5V BOOST converter
    digitalWrite(_pins.boost_en, HIGH);

    // Drive INH low to enable MUX outputs, SEL A and B have to be low so
    // we are talking to uart 1, which leads to RS485 driver chip.
    digitalWrite(_pins.uart_inh,   LOW);
    digitalWrite(_pins.uart_sel_a, LOW);
    digitalWrite(_pins.uart_sel_b, LOW);
}

ponsel_sensor::ponsel_sensor(Stream &serial, 
                             uint8_t address, 
                             enum sensor_type type,
                             struct pinout pins)
{
    _serial = &serial;
    _address = address;
    _type = type;
    _measurements = {};
    _pins = pins;
}

bool ponsel_sensor::begin()
{
    uint8_t result;

    // Initialize water meter communication
    _sensor.begin(_address, *_serial);
    
    if (!init_done) {
        rs485_board_init();
        init_done = true;
    }

    _sensor.preTransmission(pre_tran);
    _sensor.postTransmission(post_tran);

    // Continiously pool for delay value, 
    // this can fail once, but it succeds afterwards

    for(int i = 0; i < 3; i++)
    {
        // Read one register from DELAY_ADDR
        result = _sensor.readHoldingRegisters(DELAY_ADDR, 1);

        if (result == _sensor.ku8MBSuccess) {
            _delay = _sensor.getResponseBuffer(0x00);
            return true;
        }
        delay(500);
    }
    return false;
}

uint16_t ponsel_sensor::get_uint16_value(uint8_t index)
{
    return _sensor.getResponseBuffer(index);
}

float ponsel_sensor::get_float_value(uint8_t index)
{
    union convert {
        struct {
            uint16_t low;
            uint16_t high;
        };
        float f;
    }tofloat;

    tofloat.high = _sensor.getResponseBuffer(index);
    tofloat.low  = _sensor.getResponseBuffer(index + 1);

    return tofloat.f; 
}

void ponsel_sensor::print_measurements()
{
    if (_type == CTZN) {
        Serial.println("----------------------------------------------");
        Serial.print("Temperature:                    ");
        Serial.print(_measurements.param1);
        Serial.println(" °C");
        Serial.print("Conductivity CTZ:               ");
        Serial.print(_measurements.param2);
        Serial.println(" mS/ms");
        Serial.print("Salinity:                       ");
        Serial.print(_measurements.param3);
        Serial.println(" g/Kg");
        Serial.print("Conductivity CTZ no temp. comp: ");
        Serial.print(_measurements.param4);
        Serial.println(" mS/ms");
        Serial.println("----------------------------------------------");
    }
    else if (_type == OPTOD) {
        Serial.println("----------------------------------------------");
        Serial.print("Temperature:                    ");
        Serial.print(_measurements.param1);
        Serial.println(" °C");
        Serial.print("Oxygen:                         ");
        Serial.print(_measurements.param2);
        Serial.println(" %Sat");
        Serial.print("Oxygen:                         ");
        Serial.print(_measurements.param3);
        Serial.println(" mg/L");
        Serial.print("Oxygen:                         ");
        Serial.print(_measurements.param4);
        Serial.println(" ppm");
        Serial.println("----------------------------------------------");
    }
}

void ponsel_sensor::store_measurements()
{
    _measurements.param0 = get_uint16_value(0);
    _measurements.param1 = get_float_value(1);
    _measurements.param2 = get_float_value(3);
    _measurements.param3 = get_float_value(5);
    _measurements.param4 = get_float_value(7);
}

struct measurements ponsel_sensor::get_measurements()
{
    return _measurements;
}

bool ponsel_sensor::verfy_status_param()
{
    // 000: Measurement OK
    // 001: Measurement OK, but out of specifications
    // 010: Measurement OK with INFO 1
    // 011: Measurement OK with INFO 2
    // 100: Measurement impossible, out of specifications
    // 101: Measurement impossible with INFO 3
    // 110: Measurement impossible with INFO 4
    // 111: measurement under way (not yet available)
 
    // First param returned is status param
    uint8_t status = (_measurements.param0 >> 2) & 0x1;
    if (status == 0x1) {
        return false;
    }
    else {
        return true;
    }
}

bool ponsel_sensor::read_measurements()
{
        // Read 6 registers, status reg and plus 5 easurements 
        // 0x001F = 0b00111111
        _sensor.setTransmitBuffer(0, 0x001F);  
        
        // Write Transmit buffer to RUN_MEASURE_ADDR and write only one word
        uint8_t result = _sensor.writeMultipleRegisters(RUN_MEASURE_ADDR, 1);

        // Read 11 registers, one int and 5 floats, which is 11 bytes
        int read_reg_num = 11;

        // Required delay that we set begin method
        delay(_delay);

        result = _sensor.readHoldingRegisters(READ_MEASURE_ADDR, read_reg_num);

        if (result == _sensor.ku8MBSuccess) {
            store_measurements();
            verfy_status_param();
            return true;
        }
        else {
            return false;
        }
}

// calibration procedure
// Write '0x00000000 ' at 0x014C address and also 0x0001 at 0x004C address.



