#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#define TRIGGER_PIN GPIO_PIN_3
#define ECHO_PIN GPIO_PIN_2

void Ultrasonic_Init(void);
uint32_t Ultrasonic_MeasureDistance(void);
void Timer3_Count_Init(void);
uint32_t Timer3_Count_Ret(void);
void Delay_MicroSecond(int time);

#endif /* ULTRASONIC_SENSOR_H */
