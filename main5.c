#include <18f25k22.h>
#fuses INTRC_IO, NOMCLR, NOWDT, NOPLLEN

#use delay(internal = 8M) 
#use rs232 (baud=9600, UART2, parity=N, stop=1, stream=stream2 ,ERRORS)
#use rs232 (baud=9600, UART1, parity=N, stop=1, stream=stream1 ,ERRORS)
#USE PWM(CCP4, BITS=8, duty=99)

// PORT Address
#byte PORTA = 0xF80
#byte PORTB = 0xF81
#byte PORTC = 0xF82

// TRIS Address
#byte TRISA = 0xF92
#byte TRISB = 0xF93
#byte TRISC = 0xF94

// UART
#byte BAUDCON1 = 0xFB8
#byte RCSTA1 = 0xFAB
#byte TXSTA1 = 0xFAC
#byte TXREG1 = 0xFAD
#byte RCREG1 = 0xFAE
#byte SPBRG1 = 0xFAF

#byte BAUDCON2 = 0xF70
#byte RCSTA2 = 0xF71
#byte TXSTA2 = 0xF72
#byte TXREG2 = 0xF73
#byte RCREG2 = 0xF74
#byte SPBRG2 = 0xF75

#byte TMR0L = 0xFD6
#byte TMR0H = 0xFD7
#byte T0CON = 0xFD5

#byte INTCON = 0xFF2
#byte INTCON2 = 0xFF1
#byte INTCON3 = 0xFF0

#bit TMR0ON = T0CON.7
#bit T08BIT = T0CON.6
#bit T0CS = T0CON.5
#bit T0SE = T0CON.4
#bit PSA = T0CON.3
#bit T0PS2 = T0CON.2
#bit T0PS1 = T0CON.1
#bit T0PS0 = T0CON.0

#byte T1CON = 0xFCD
#byte T1GCON = 0xFCC
#byte TMR1H = 0xFCF
#byte TMR1L = 0xFCE
#bit TMR1ON = T1CON.0
#byte T3GCON = 0xFB4
#byte T3CON = 0xFB1
#byte TMR3H = 0xFB3
#byte TMR3L = 0xFB2
#bit TMR3ON = T3CON.0
#byte T5GCON = 0xF4D
#byte T5CON = 0xF4E
#byte TMR5H = 0xF50
#byte TMR5L = 0xF49
#bit TMR5ON = T5CON.0

#byte PIE3 = 0xFA3
#byte PIR3 = 0xFA4
#byte PIE5 = 0xF7D
#byte PIR2 = 0xFA1
#byte PIR1 = 0xF9E
#byte PIE1 = 0xF9D
#bit RC1IE = PIE1.5
#bit RC2IE = PIE3.5


#byte TMR2 = 0xFBC
#byte T2CON = 0xFBA
#byte PR2 = 0xFBB
#bit TMR2ON = T2CON.2
#bit T2CKPS0 = T2CON.0
#bit T2CKPS1 = T2CON.1

#bit BRG16 = BAUDCON1.3
#bit RC1IF = PIR1.5
//pwm
#byte CCP4CON = 0xF57
#byte CCPR4L = 0xF58
#byte CCPR4H = 0xF59
#byte CCPTMRS1 = 0xF48



#bit PEIE = INTCON.6
#bit GIE = INTCON.7
#bit INT0IE = INTCON.4
#bit INT1IE = INTCON3.3
#bit INT2IE = INTCON3.4
#bit INTEDG0 = INTCON2.6
#bit INTEDG1 = INTCON2.5
#bit INTEDG2 = INTCON2.4

#bit TMR0IE = INTCON.5
#bit TMR1IE = PIE1.0
#bit TMR5IE = PIE5.1

#bit DC4B0 = CCP4CON.4
#bit DC4B1 = CCP4CON.5
#bit CCP4M0 = CCP4CON.0
#bit CCP4M1 = CCP4CON.1
#bit CCP4M2 = CCP4CON.2
#bit CCP4M3 = CCP4CON.3
#bit C4TSEL0= CCPTMRS1.0
#bit C4TSEL1= CCPTMRS1.1
#bit motor1 = PORTA.0
#bit motor2 = PORTA.1

#bit SPEN = RCSTA1.7
#bit RX9 = RCSTA1.6
#bit CREN = RCSTA1.4

#bit TX9 = TXSTA1.6
#bit TXEN = TXSTA1.5
#bit SYNC = TXSTA1.4
#bit SENDB = TXSTA1.3
#bit TRMT = TXSTA1.1
#bit TRMT2 = TXSTA2.1


void INIT_PORT()
{
   PORTA = 0x00;
   TRISA=0b00010000;
   PORTB = 0x06;
   TRISB=0b10100110;
   PORTC = 0x40;
   TRISC = 0x85; //PORTC.7 -> input
   

}

void interrupt_set()
{
   
   PEIE = 1;
   INTEDG1=0;
   INTEDG2=0;
   GIE = 1;
   TMR0IE = 1;
   TMR1IE = 1;
   TMR5IE = 1;
   
   INT1IE = 1;
   INT2IE = 1;
   
   RC1IE = 1;
   RC2IE = 1;
}

void TMRSET()
{
  //Timer0
   T08BIT = 0;
   T0CS = 0;
   T0SE = 0;
   PSA = 1;
   T0PS2=0;
   T0PS1=0;
   T0PS0=0;
   TMR0L=0xAF;
   TMR0H=0x3C;
   //Timer1
   T1CON = 0b10000100;
   
   //Timer3
   T5CON = 0b10000100;
   TMR1H = 0;
   TMR1L = 0;
   TMR5H = 0;
   TMR5L = 0;

}



void tx_data(CHAR TXD)
{
   while(TRMT==0);
   TXREG1 = TXD;
   
}
void tx_data2(CHAR TXD)
{
   while(TRMT2==0);
   TXREG2 = TXD;
   
}
void ASCII(char *tx)
{
   while(*tx!=0x00)
   {
      TX_DATA(*tx++);
   }
}

INT stack=0;
int data2_stack=0;
int data2[10],data2_id=0,data2_len=0;
int mode2=0;


void main(void)
{
    delay_ms(500);
    INIT_PORT();
      interrupt_set();
      pwm_off();
      TMRSET();
      motor1=1;
      motor2=0;
      TMR0ON = 0;
      TMR1ON = 1;
      TMR5ON = 1;
      while(1)
      {
     
         
         if(mode2==3) // app
         {
            if(data2_id=='A')
            {
               if(data2[0]=='1')  // start
               {
                   pwm_off();
                   motor1=1;
                   motor2=0;
                   pwm_on();
               }
               if(data2[0]=='2')   // reverse
               {
                  pwm_off();
                   motor1=0;
                   motor2=1;
                   pwm_on();
               }  
               if(data2[0]=='0')  // stop
               {
                  pwm_off();
                  stack=0;
               }
            }

            mode2=0;
            
         }
         
         
      }
}

#INT_EXT1
void ext1_irs()
{

    pwm_off();
    
}

#INT_EXT2
void ext2_irs()
{
    pwm_off();

}


#INT_RDA2 // app
void rda2_irs()
{
    if(mode2==0) // stx id length
   {
      data2_stack++;
      
      if(data2_stack==1)
      {
       if(RCREG2!='s')
       {
       data2_stack=0;
       return;
       }
      }
      
      if(data2_stack==2)
      {
         data2_id=RCREG2;
      }
      
      if(data2_stack==3)      
      {
         data2_len=RCREG2;
         data2_len=data2_len-0x30;
         mode2=1; data2_stack=0; return;
      }
   }
   
   if(mode2 ==1) // data
   {
      data2[data2_stack]=RCREG2;
      data2_stack++;
      if(data2_stack == data2_len)
      {
         mode2=2;
         data2_stack=0;
         return;
      }
   }
   
   if(mode2==2) // etx
   {
      if(RCREG2=='e')
      {
         mode2=3;
      }
      return;
   }  
}
