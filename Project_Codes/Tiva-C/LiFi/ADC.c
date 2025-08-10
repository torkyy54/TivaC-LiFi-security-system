#include "ADC.h"


void initADC(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  // Enable Clock to GPIOE
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);   // AD0 clock enable

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);  // Enable PE3 as ADC input

    ADCSequenceDisable(ADC0_BASE, 3);  // Disable SS3 during configuration
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // Software trigger conversion
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END); // Get input from channel 0
    ADCSequenceEnable(ADC0_BASE, 3);   // Enable ADC0 sequencer 3
}

void readADCValue(uint32_t *adc_value) {
    ADCIntClear(ADC0_BASE, 3);  // Clear ADC interrupt status
    ADCProcessorTrigger(ADC0_BASE, 3);  // Enable SS3 conversion or start sampling data from AN0
    while (!ADCIntStatus(ADC0_BASE, 3, false)); // Wait until sample conversion completed
    ADCSequenceDataGet(ADC0_BASE, 3, adc_value); // Read ADC conversion result from SS3 FIFO
}

void initADCInterrupt(void) {
    ADCIntClear(ADC0_BASE, 0); // Clear ADC interrupt flag for sequence 0
    ADCIntEnable(ADC0_BASE, 0); // Enable ADC interrupt for sequence 0
    IntEnable(INT_ADC0SS0); // Enable ADC sequence 0 interrupt in the NVIC
}
