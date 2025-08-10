#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"

#include "ADC.h"
#include "Ultrasonic.h"




#define GreenLedPin         GPIO_PIN_3
#define BLUE_LED            GPIO_PIN_2

#define LASER_PIN           GPIO_PIN_5
#define LASER_PORT          GPIO_PORTA_BASE

#define MagneticSensorPin   GPIO_PIN_4
#define MagneticSensorPort  GPIO_PORTB_BASE

#define TRIGGER_PIN         GPIO_PIN_3
#define ECHO_PIN            GPIO_PIN_2

#define ECHO_PORT           GPIO_PORTB_BASE

#define TRIGGER_PORT        GPIO_PORTA_BASE

#define ADC_PORT            GPIO_PORTE_BASE
#define ADC_PIN             GPIO_PIN_3

#define ISR_PORT            GPIO_PORTB_BASE

#define ADC_ISR_PORT        GPIO_PORTE_BASE


void BluetoothSendString(const char *str);
void PortFIntHandler(void);
void stopSystem(void);
void startSystem(void);

volatile uint8_t systemRunning = 0;
uint32_t adc_value;



int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;

    // Configure PF0 and PF4 as inputs (assuming buttons are connected to these pins)
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(TRIGGER_PORT, TRIGGER_PIN);
    GPIOPinTypeGPIOOutput(LASER_PORT, LASER_PIN);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GreenLedPin);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BLUE_LED);

    GPIOPinTypeGPIOInput(MagneticSensorPort, MagneticSensorPin);

    Ultrasonic_Init();

    initADC();
    Timer3_Count_Init();

    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_FALLING_EDGE);

    // Enable interrupts on Port F for buttons


    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    GPIOIntRegister(GPIO_PORTF_BASE, PortFIntHandler);


    /* Bluetooth Initialization */
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
        GPIOPinConfigure(GPIO_PC6_U3RX);
        GPIOPinConfigure(GPIO_PC7_U3TX);
        GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
        UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE));
        UARTEnable(UART3_BASE);


    IntMasterEnable();



    while (1)
    {
        //BluetoothSendString("bara !!\n");

        while(systemRunning==1)
        {
            /*if(systemRunning==1)
            {
                BluetoothSendString("gowa !!\n");
            }*/


      //  BluetoothSendString("awel if !!\n");



             readADCValue(&adc_value);


             if (adc_value <= 2048 )
             {
                 BluetoothSendString("FIREEEE !!\n");
                 GPIOPinWrite(LASER_PORT, LASER_PIN, LASER_PIN);
                 Delay_MicroSecond(1200000);
                 GPIOPinWrite(LASER_PORT, LASER_PIN, 0);
                 Delay_MicroSecond(800000);
             }

             else
                 GPIOPinWrite(LASER_PORT, LASER_PIN, 0);

            // BluetoothSendString("tany if !!\n");
             if (GPIOPinRead(MagneticSensorPort, MagneticSensorPin) == MagneticSensorPin)
             {
                 // Turn on the green LED
                 BluetoothSendString("DOOR OPENED !!\n");
                 GPIOPinWrite(LASER_PORT, LASER_PIN, LASER_PIN);
                 Delay_MicroSecond(2500000);
                 GPIOPinWrite(LASER_PORT, LASER_PIN, 0);
                 Delay_MicroSecond(1000000);
             }
            // BluetoothSendString("talet if !!\n");
             if (Ultrasonic_MeasureDistance() < 100)
                    {
                        BluetoothSendString("INTRUSION !!\n");

                        GPIOPinWrite(LASER_PORT, LASER_PIN, LASER_PIN);
                        Delay_MicroSecond(5500000);
                        GPIOPinWrite(LASER_PORT, LASER_PIN, 0);
                        Delay_MicroSecond(1000000);
                     }
                    else
                    {
                        GPIOPinWrite(LASER_PORT, LASER_PIN, 0);
                        Delay_MicroSecond(10);
                    }
        }
    }
}


void BluetoothSendString(const char *str)
{
    while (*str)
    {
        UARTCharPut(UART3_BASE, *str);
        str++;

        SysCtlDelay(SysCtlClockGet() / 300);
    }
}

void startSystem() {

    systemRunning = 1;
}

void stopSystem(void) {

    systemRunning = 0;
}

void PortFIntHandler(void) {
    // Clear the interrupt flags
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // Check which button was pressed
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0) {
        systemRunning = 1;
        BluetoothSendString("System Started\n");
    }

    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0) {
        systemRunning = 0;
        BluetoothSendString("System Stopped\n");
    }

}

