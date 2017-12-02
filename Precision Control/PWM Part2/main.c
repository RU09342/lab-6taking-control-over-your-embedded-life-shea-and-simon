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

void UARTInit();
void PWMInit();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;                   //Disable high impedance mode.

    void PWMInit();
    void UARTInit();

    __enable_interrupt();
    __bis_SR_register(GIE);
    while (1)
    {
     // For debugger
    }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
        while(!(UCA0IFG & UCTXIFG)); // Wait until buffer is ready
        UCA0TXBUF = UCA0RXBUF;       // Send Byte
        TA0CCR1 = UCA0RXBUF;         // CCR1 = incoming byte
}

void PWMInit()
{
    /* P1.0 Signal that turns fan on/off.*/
    P1DIR |= BIT2;                          // PWM output to P1.2
    P1SEL |= BIT2;                          // GPIO select for P1.2
    TA0CCTL1 = OUTMOD_7;                    // Set/Reset
    TA0CTL = TASSEL_2 + MC_1 +TACLR ;       // SMCLK, Up-Mode, TAR reset
    TA0CCR0 = 100;
    TA0CCR1 = 50;
}

void UARTInit()
{
            P3SEL |= BIT3;                      // P3.3 => TX
            P3SEL |= BIT4;                      // P3.4 => RX
            UCA0CTL1 |= UCSWRST;                // Resets state machine
            UCA0CTL1 |= UCSSEL_2;               // SMCLK
            UCA0BR0 = 6;                        // 9600 Baud Rate
            UCA0BR1 = 0;                        // 9600 Baud Rate
            UCA0MCTL |= UCBRS_0;                // Modulation
            UCA0MCTL |= UCBRF_13;               // Modulation
            UCA0MCTL |= UCOS16;                 // Modulation
            UCA0CTL1 &= ~UCSWRST;               // Initializes the state machine
            UCA0IE |= UCRXIE;                   // Interrupt enable
}
