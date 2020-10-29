#ifndef pinout_mappings_H
#define pinout_mappings_H

#ifdef __cplusplus
extern "C" {
#endif

#define BOOST_EN        PB9
#define UART_INH        PB6
#define UART_SEL_A      PB12
#define UART_SEL_B      PB5

// On Pira_Smart_R485_Add-On board the driver control is switched, thats why we
// also switch logic here.
#define DRIVER_EN       PB15
#define ENABLE_DRIVER   LOW
#define DISABLE_DRIVER  HIGH


#ifdef __cplusplus
}
#endif
#endif /* pinout_mappings_H */
/*** end of file ***/
