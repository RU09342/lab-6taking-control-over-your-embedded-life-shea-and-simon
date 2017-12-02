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
#include <msp430.h>

void UARTInit();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    P1DIR = BIT2    //Set P1.2 to MSB output
          + BIT3    //Set P1.3 to MSB output
          + BIT4    //Set P1.4 to MSB output
          + BIT5;   //Set P1.5 to MSB output
    P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5);

    P2DIR = BIT4    //Set P2.4 to output
          + BIT5;   //Set P2.5 to output
    P2OUT &= ~(BIT4 + BIT5);

    P4DIR = BIT0    //Set P4.0 to LSB output
          + BIT3;   //Set P4.3 to LSB output
    P4OUT &= ~(BIT0 + BIT3);

    void UARTInit();

    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0, interrupts enabled
    __no_operation(); // For debugger
}

// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{

    while (!(UCA0IFG&UCTXIFG));         // check if USCI_A0 TX buffer ready
    char characterIN = UCA0RXBUF;       //Take the input number from UART
    UCA0TXBUF = UCA0RXBUF;
    int output[8];                      // Create a fixed size array
    int i;
    for (i = 0; i < 8; i++) {           //Put each bit of the input buffer in to the appropriate output
        output[i] = (characterIN >> i) & 1;
    }
    if(output[0] == 1){
        P1OUT |= BIT2; //Most significant bit to P1.2
    }
    else{
        P1OUT &= ~BIT2; // Continue with the less significant bits
    }
    if(output[1] == 1){
        P1OUT |= BIT3;
    }
    else{
        P1OUT &= ~BIT3;
    }
    if(output[2] == 1){
        P1OUT |= BIT4;
    }
    else{
        P1OUT &= ~BIT4;
    }
    if(output[3] == 1){
        P1OUT |= BIT5;
    }
    else{
        P1OUT &= ~BIT5;
    }
    if(output[4] == 1){
        P2OUT |= BIT4;
    }
    else{
        P2OUT &= ~BIT4;
    }
    if(output[5] == 1){
        P2OUT |= BIT5;
    }
    else{
        P2OUT &= ~BIT5;
    }
    if(output[6] == 1){
        P4OUT |= BIT0;
    }
    else{
        P4OUT &= ~BIT0;
    }
    if(output[7] == 1){
        P4OUT |= BIT3; // Least significant bit to P4.3
    }
    else{
        P4OUT &= ~BIT3;
    }
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
