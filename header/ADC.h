#ifndef __ADC_H__
#define __ADC_H__

void ADC_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    // ADEN : setting this bit enables analog-to-digital conversion.
    // ADSC : setting this bit starts the first conversion.
    // ADATE: setting this this bit enables auto-triggering. Since we are
    //          in Free Running mode, a new conversion will trigger whenever
    //          the previous conversion completes.
}

#endif