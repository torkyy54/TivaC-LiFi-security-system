#include "Ultrasonic.h"



static uint32_t duration3 = 0;
static uint32_t distance1 = 0;

void Ultrasonic_Init(void) {
    // Enable GPIO peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure trigger pin as output
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, TRIGGER_PIN);

    // Configure echo pin as input
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ECHO_PIN);

    // Configure Timer 3 for pulse width measurement
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
    TimerControlEvent(TIMER3_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES);
    TimerEnable(TIMER3_BASE, TIMER_A);
}

uint32_t Ultrasonic_MeasureDistance(void) {
    // Trigger ultrasonic sensor
    GPIOPinWrite(GPIO_PORTA_BASE, TRIGGER_PIN, 0);
    Delay_MicroSecond(10000);
    GPIOPinWrite(GPIO_PORTA_BASE, TRIGGER_PIN, TRIGGER_PIN);
    Delay_MicroSecond(10000);
    GPIOPinWrite(GPIO_PORTA_BASE, TRIGGER_PIN, 0);

    // Measure pulse duration using Timer 3
    TimerIntClear(TIMER3_BASE, TIMER_CAPA_EVENT);
    while (!(TimerIntStatus(TIMER3_BASE, false))) {}

    uint32_t t1 = TimerValueGet(TIMER3_BASE, TIMER_A);
    TimerIntClear(TIMER3_BASE, TIMER_CAPA_EVENT);
    while (!(TimerIntStatus(TIMER3_BASE, false))) {}

    uint32_t t2 = TimerValueGet(TIMER3_BASE, TIMER_A);
    duration3 = (t2 - t1) & 0x00FFFFFF;

    // Calculate distance using the speed of sound
    distance1 = ((float)duration3 * 0.00340000) / 2;

    return distance1;
}

void Delay_MicroSecond(int time)
{
    // Enable Timer 1 for microsecond delay
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1)) {}

        // Configure Timer 1 for periodic down counter
        TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);

        // Set the load value for the desired delay
        TimerLoadSet(TIMER1_BASE, TIMER_A, 16 - 1);

        // Enable Timer 1
        TimerEnable(TIMER1_BASE, TIMER_A);

        // Loop for the specified delay time
        int i=0;
        for ( i = 0; i < time; i++) {
            while (TimerIntStatus(TIMER1_BASE, 0) == 0);    // Wait till captured
            TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);  // Clear Timer 1A timeout flag
        }

        // Disable Timer 1
        TimerDisable(TIMER1_BASE, TIMER_A);
}

// Function to initialize Timer 3 for pulse width measurement
void Timer3_Count_Init(void) {
    // Enable Timer 3 and GPIOB peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure Timer 3 input capture pin (PB2)
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinConfigure(GPIO_PB2_T3CCP0);

    // Configure Timer 3 for capturing pulse time up events on both edges
    TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
    TimerControlEvent(TIMER3_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES);

    // Enable Timer 3
    TimerEnable(TIMER3_BASE, TIMER_A);
}

// Function to measure pulse duration using Timer 3
uint32_t Timer3_Count_Ret(void) {
    // Clear Timer 3A capture event
    TimerIntClear(TIMER3_BASE, TIMER_CAPA_EVENT);

    // Wait for a capture event
    while (!(TimerIntStatus(TIMER3_BASE, false))) {}

    // Record the start time
    uint32_t t1 = TimerValueGet(TIMER3_BASE, TIMER_A);

    // Clear Timer 3A capture event
    TimerIntClear(TIMER3_BASE, TIMER_CAPA_EVENT);

    // Wait for another capture event
    while (!(TimerIntStatus(TIMER3_BASE, false))) {}

    // Record the end time
    uint32_t t2= TimerValueGet(TIMER3_BASE, TIMER_A);

    // Return the pulse duration
    return (t2 - t1) & 0x00FFFFFF;
}



