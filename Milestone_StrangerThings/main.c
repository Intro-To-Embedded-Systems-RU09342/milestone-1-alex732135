//MSP430-G2553
//Alex Steel and Ryan Boylan
//Milestone 1


#include <msp430.h>

void Timers(void);          //Declare Timers function
void LED(void);             //Declare LED function
void UART(void);            //Declare UART function

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               //stop watchdog timer

    Timers();               //Call Timers function
    LED();                  //Call LED function
    UART();                 //Call UART function

    __bis_SR_register(LPM0_bits + GIE);     //low power mode and interrupt enabled
}

void Timers(void)           //Timers function
{
    //timer A0
    TA0CTL |= TASSEL_2 + MC_1;     //set smclk, up mode
    TA0CCTL1 |= OUTMOD_7;   //set/reset output
    TA0CCR0 = 255;          //pwm period
    TA0CCR1 = 0;            //initialize red pwm

    //timer A1
    TA1CTL |= TASSEL_2 + MC_1;     //set smclk, up mode
    TA1CCTL1 |= OUTMOD_7;   //set/reset output
    TA1CCTL2 |= OUTMOD_7;   //set/reset output
    TA1CCR0 = 255;          //pwm period
    TA1CCR1 = 0;            //initialize green pwm
    TA1CCR2 = 0;            //initialize blue pwm

}

void LED(void)              //LED function
{
    P1DIR |= BIT6;          //Set Red LED Direction Register to output
    P1SEL |= BIT6;          //Enable pwm for Red LED
    P2DIR |= (BIT1 + BIT5); //Set Green(Bit1) and Blue(BIT5) LED Direction Register to output
    P2SEL |= (BIT1 + BIT5); //Enable pwm for Green and Blue LED

}

void UART(void)             //UART function
{
    P1SEL |= BIT1 + BIT2;   //P1.1 = RXD P1.2 = TXD
    P1SEL2 |= BIT1 + BIT2;  //P1.1 = RXD P1.2 = TXD

    UCA0CTL1 |= UCSSEL_2;   //smclk
    UCA0BR0 = 104;          //1MHz 9600 baud rate
    UCA0BR1 = 0;            //1MHz 9600 baud rate
    UCA0MCTL = UCBRS0;      //modulation UBRSx = 1
    UCA0CTL1 &= ~UCSWRST;   //initialize usci state machine

    IE2 |= UCA0RXIE;        //enable RX interrupt
}

int length = 0;             //initialize length variable
int counter=1;              //initialize counter variable
int red_pwm=0;              //initialize red_pwm variable
int green_pwm=0;            //initialize green_pwm variable
int blue_pwm=0;             //initialize blue_pwm variable

#pragma vector=USCIAB0RX_VECTOR     //Interrupt routine
__interrupt void TESTTHING(void)
{
  while (!(IFG2&UCA0TXIFG));        //Wait until a byte is ready, is USCI_A0 TX buffer ready?
  if(counter==1)                    //Receive first signal
  {
      UCA0TXBUF=UCA0RXBUF-3;        //Output signal is input signal minus the 3 pwm values we use
      length=UCA0RXBUF-3;           //set length to input signal - 3
  }
  else if(counter<5)                // since we start at 1, this will occur when counter = 2, 3, and 4, which are the R G B values
  {
      switch(counter)               //switch statement used for looking at 2nd 3rd and 4th bytes of input signal
      {
      case 2:                       //2nd input signal
          red_pwm=UCA0RXBUF;        //set byte = red_pwm
          break;                    //break from case
      case 3:                       //3rd input signal
          green_pwm=UCA0RXBUF;      //set byte = green_pwm
          break;                    //break from case
      case 4:                       //4th input signal
          blue_pwm=UCA0RXBUF;       //set byte = blue_pwm
          break;                    //break from case
      default:                      //default case if no case is satisfied
          break;                    //do nothing and break from case
      }

  }
  else                              //if we aren't at the first byte or any of the R G B values, output the Byte
    UCA0TXBUF = UCA0RXBUF;
  if(counter==length+3)             //If we have the carriage return byte, then reset back to 0, which immediately gets set to 1
  {
      counter=0;                    //reset counter back to zero
      TA0CCR1=red_pwm;              //set red_pwm variable to its timer
      TA1CCR1=green_pwm;            //set green_pwm variable to its timer
      TA1CCR2=blue_pwm;             //set blue_pwm variable to its timer
  }

  counter++;                        //increment counter by 1
}
