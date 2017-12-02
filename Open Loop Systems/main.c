/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
#include "msp430.h"
#include <stdlib.h>

int RegTemp = 0; //Default Regulator Temperature
int Temp = 320;  // Default Room Temperature

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

    P1DIR |= BIT0;                          // Set P1.0 to output direction for PWM that controls fan
    P1OUT &= ~BIT0;

    TA0CCTL1 = OUTMOD_7;                    // Timer setup for PWM 50% Duty cycle
    TA0CTL = TASSEL_2 + MC_1 +TACLR ;
    TA0CCR0 = 1000;
    TA0CCR1 = 500;

    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings

    // ADC Initialization
    REFCTL0=REFON + REFVSEL_2;           // Use a reference voltage of 2.5V
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;   // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                // Use sampling timer, ADC12SC conversion start, clock=OSC
    ADC12CTL2 |= ADC12RES_2;             // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_3;           // ADC input select
    ADC12IER0 |= ADC12IE0;               // Enable ADC conversion complete interrupt
    ADC12CTL0 |= ADC12ENC;               // Enable conversion
    P1DIR &= ~BIT3;                      // P1.3 is set as input for ADC
    P1OUT &= ~BIT3;
    P1SEL0 |= BIT3;
    P1SEL1 |= BIT3;

    // UART Timer Initialization
    P2SEL0 |= BIT0 | BIT1;                    // P2.1 => TXD / P2.0 => RXD
    P2SEL1 &= ~(BIT0 | BIT1);
    CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;               // SMCLK
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __enable_interrupt();
    __bis_SR_register(GIE);
    while (1) // MSP reads ADC
    {
        __delay_cycles(1000);
        ADC12CTL0 |= ADC12ENC | ADC12SC;    // Start sampling/conversion
        __bis_SR_register(LPM0_bits | GIE); // LPM0, ADC12_ISR will force exit
        __no_operation();                   // For debugger
    }
   }

// ADC Interrupt
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
    {
    case ADC12IV_NONE:        break;    // Vector  0:  No interrupt
    case ADC12IV_ADC12OVIFG:  break;    // Vector  2:  ADC12MEMx Overflow
    case ADC12IV_ADC12TOVIFG: break;    // Vector  4:  Conversion time overflow
    case ADC12IV_ADC12HIIFG:  break;    // Vector  6:  ADC12BHI
    case ADC12IV_ADC12LOIFG:  break;    // Vector  8:  ADC12BLO
    case ADC12IV_ADC12INIFG:  break;    // Vector 10:  ADC12BIN
    case ADC12IV_ADC12IFG0:             // Vector 12:  ADC12MEM0 Interrupt
        // Sets these values to the actual temperature derived from the ADC
        RegTemp = ((ADC12MEM0*3.35)/4096)*1000; //changes duty cycle

        // Set Fan PWM according to the desired temperature

        if(RegTemp > Temp + 3)
        {
                TA0CCR1 = 1000; //Run fan at 100% Duty Cycle
        }
        else if(RegTemp < Temp - 3) //Check if the temperature of the regulator is less than desired temp
        {
                TA0CCR1 = 0;    //Run fan at 0% Duty Cycle
        }
        if(Temp >= 380 && Temp < 405)   // Perform if Temperature is 38-40.5 C
        {
                TA0CCR1 = ((Temp/10)*(-24.87)+1035.8) ; // y = -24.87x + 1035.8
        }
        else if(Temp >= 405 && Temp <= 430) // Perform if Temperature is 40.5-43 C
        {
                TA0CCR1 = ((Temp/10)*(-7.1429)+321.43) ; // y = -7.1429x + 321.4

        }
        else if(Temp >= 430 && Temp <= 480) // Perform if Temperature is 43-48 C
        {
                TA0CCR1 = ((Temp/10)*(-2.0833)+108.33) ; // y = -2.0833x + 108.33

        }
        else if(Temp >= 480 && Temp <= 540) // Perform if Temperature is 48-54 C
        {
                TA0CCR1 = ((Temp/10)*(-24.87)+1035.8) ; // y = -0.8333x + 49
        }
        else if(Temp >= 540 && Temp <= 710) // Perform if Temperature is 54-71 C
        {
                TA0CCR1 = ((Temp/10)*(-0.8333)+49) ; // y = -0.8333x + 49
        }


// Exit from LPM0 and continue executing main
        __bic_SR_register_on_exit(LPM0_bits);
        break;
    case ADC12IV_ADC12IFG1:   break;    // Vector 14:  ADC12MEM1
    case ADC12IV_ADC12IFG2:   break;    // Vector 16:  ADC12MEM2
    case ADC12IV_ADC12IFG3:   break;    // Vector 18:  ADC12MEM3
    case ADC12IV_ADC12IFG4:   break;    // Vector 20:  ADC12MEM4
    case ADC12IV_ADC12IFG5:   break;    // Vector 22:  ADC12MEM5
    case ADC12IV_ADC12IFG6:   break;    // Vector 24:  ADC12MEM6
    case ADC12IV_ADC12IFG7:   break;    // Vector 26:  ADC12MEM7
    case ADC12IV_ADC12IFG8:   break;    // Vector 28:  ADC12MEM8
    case ADC12IV_ADC12IFG9:   break;    // Vector 30:  ADC12MEM9
    case ADC12IV_ADC12IFG10:  break;    // Vector 32:  ADC12MEM10
    case ADC12IV_ADC12IFG11:  break;    // Vector 34:  ADC12MEM11
    case ADC12IV_ADC12IFG12:  break;    // Vector 36:  ADC12MEM12
    case ADC12IV_ADC12IFG13:  break;    // Vector 38:  ADC12MEM13
    case ADC12IV_ADC12IFG14:  break;    // Vector 40:  ADC12MEM14
    case ADC12IV_ADC12IFG15:  break;    // Vector 42:  ADC12MEM15
    case ADC12IV_ADC12IFG16:  break;    // Vector 44:  ADC12MEM16
    case ADC12IV_ADC12IFG17:  break;    // Vector 46:  ADC12MEM17
    case ADC12IV_ADC12IFG18:  break;    // Vector 48:  ADC12MEM18
    case ADC12IV_ADC12IFG19:  break;    // Vector 50:  ADC12MEM19
    case ADC12IV_ADC12IFG20:  break;    // Vector 52:  ADC12MEM20
    case ADC12IV_ADC12IFG21:  break;    // Vector 54:  ADC12MEM21
    case ADC12IV_ADC12IFG22:  break;    // Vector 56:  ADC12MEM22
    case ADC12IV_ADC12IFG23:  break;    // Vector 58:  ADC12MEM23
    case ADC12IV_ADC12IFG24:  break;    // Vector 60:  ADC12MEM24
    case ADC12IV_ADC12IFG25:  break;    // Vector 62:  ADC12MEM25
    case ADC12IV_ADC12IFG26:  break;    // Vector 64:  ADC12MEM26
    case ADC12IV_ADC12IFG27:  break;    // Vector 66:  ADC12MEM27
    case ADC12IV_ADC12IFG28:  break;    // Vector 68:  ADC12MEM28
    case ADC12IV_ADC12IFG29:  break;    // Vector 70:  ADC12MEM29
    case ADC12IV_ADC12IFG30:  break;    // Vector 72:  ADC12MEM30
    case ADC12IV_ADC12IFG31:  break;    // Vector 74:  ADC12MEM31
    case ADC12IV_ADC12RDYIFG: break;    // Vector 76:  ADC12RDY
    default: break;
    }
}

// UART Interrupt
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)

{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        while(!(UCA0IFG & UCTXIFG)); // Wait for buffer
        UCA0TXBUF = UCA0RXBUF;
        Temp = UCA0RXBUF*10; // Set the target temperature
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    }
}
