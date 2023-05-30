#include <18f25k22.h> 
#device ADC=10
#fuses INTRC_IO, NOMCLR, NOWDT, NOPLLEN
#use delay(internal = 8M)
#use rs232 (baud=9600, UART1, parity=N, stop=1, stream=stream1 ,ERRORS)
#use rs232 (baud=9600, UART2, parity=N, stop=1, stream=stream2 ,ERRORS)
#USE I2C(master, scl=PIN_C3, sda=PIN_C4,ADDRESS= 0x28)

#byte PORTA = 0xF80
#byte PORTB = 0xF81
#byte PORTC = 0xF82
#byte TRISA = 0xF92
#byte TRISB = 0xF93
#byte TRISC = 0xF94


#byte INTCON = 0xFF2
#byte INTCON2 = 0xFF1
#byte INTCON3 = 0xFF0
#byte PIE1 = 0xF9D
#byte PIE3 = 0xFA3
#bit PEIE = INTCON.6
#bit GIE = INTCON.7
#bit INT0IE = INTCON.4
#bit INT1IE = INTCON3.3
#bit INT2IE = INTCON3.4
#bit INTEDG0 = INTCON2.6
#bit INTEDG1 = INTCON2.5
#bit INTEDG2 = INTCON2.4

#bit RC1IE = PIE1.5
#bit RC2IE = PIE3.5

//Serial
#byte BAUDCON1 = 0xFB8
#byte RCSTA1 = 0xFAB
#byte TXSTA1 = 0xFAC
#byte SPBRG1 = 0xFAF
#byte SPBRGH1 = 0xFB0
#byte PIR1 = 0xF9E
#byte TXREG1 = 0xFAD
#byte RCREG1 = 0xFAE
#bit BRG16 = BAUDCON1.3
#bit SPEN = RCSTA1.7
#bit RX9 = RCSTA1.6
#bit CREN = RCSTA1.4

#bit TX9 = TXSTA1.6
#bit TXEN = TXSTA1.5
#bit SYNC = TXSTA1.4
#bit SENDB = TXSTA1.3
#bit TRMT = TXSTA1.1

#byte BAUDCON2 = 0xF70
#byte RCSTA2 = 0xF71
#byte TXSTA2 = 0xF72
#byte TXREG2 = 0xF73
#byte RCREG2 = 0xF74
#byte SPBRG2 = 0xF75
#bit TRMT2 = TXSTA2.1

#bit dust_light = PORTA.1

// timer0 16bit 250ms
#byte T0CON = 0xFD5
#byte TMR0L = 0xFD6
#byte TMR0H = 0xFD7

#bit TMR0ON = T0CON.7
#bit T08BIT = T0CON.6
#bit T0CS = T0CON.5
#bit T0SE = T0CON.4
#bit PSA = T0CON.3
#bit T0PS2 = T0CON.2
#bit T0PS1 = T0CON.1
#bit T0PS0 = T0CON.0
#bit TMR0IE = INTCON.5
int16 CNT=0;
float dust_value=0;
int v1[6];
int v2[6];
int8 data1=0,data2=0,data3=0,data4=0;
double water=0,temp=0;
int water2=0,temp2=0;
int mode1=0;
int data_stack=0;
int data[10],data_id=0,data_len=0;
int s_data1[1],s_data2[1],s_data3[4],s_data4[4];
int main_auto=0;
int dust_check=0, gas_check=0,hum_check=0,temp_check=0;
int gas=0;
void INIT_PORT()
{
   PORTA=0b00010000;
   TRISA=0x01;
   PORTB=0;
   TRISB=0xFF;
   PORTC=0b00010001;  // sensor vcc on, tx_en on
   TRISC=0b10000000;   // rx input
    SPEN = 1;
   TX9 = 1;
   TXEN = 1;
   SPBRG1=12;
}
void INTERRUPT_SET()
{
   PEIE=1;
   GIE=1;
   TMR0IE = 1;
   RC1IE = 1;
   RC2IE = 1;
   
}
void tx_data(int data) //Shut Data
{
   while(TRMT == 0);
   TXREG1 = data;
}
void tx_data2(int data)
{
    while(TRMT2 == 0);
   TXREG2 = data;
}
void main()
{
      delay_ms(100);
      INIT_PORT();
      INTERRUPT_SET();
      WHILE(1)
      {
      setup_adc_ports(sAN0); // dust
      setup_adc(ADC_CLOCK_INTERNAL );
      set_adc_channel(0);
      delay_ms(10);
         dust_light=1;
         delay_us(280);
         CNT = read_adc();
         delay_us(40);
         dust_light=0;
         delay_us(9680);
    dust_value = CNT; // dust adc
    
    if(CNT<=5)
    {
      s_data1[0]='0';
      dust_check=0;
    }
    if(CNT >= 5 && CNT<=600)
    {
      s_data1[0]='1';
      dust_check=1;
    }
    if(CNT >= 601)
    {
      s_data1[0]='2';
      dust_check=2;
    }
         if(!input(PIN_B4))  // gas
         {
            gas=1;
            s_data2[0]='1';
            gas_check=1;
         }
         else            // not gas
         {
            gas=0;
            s_data2[0]='0';
            gas_check=0;
         }
i2c_start();
i2c_write(0x51);
data1 = i2c_read(TRUE);  // water
data2 = i2c_read(TRUE); 
data3 = i2c_read(TRUE);  // temp
data4 = i2c_read(FALSE);
i2c_stop(); 
water=(double)(data1 & 0x3F);
water= water*256;
water +=(double)data2;
water = (double)((water-1700)/120);
water2=(int)water;
temp=(double)(data3);
temp=temp*64;
temp += (data4 >> 2)/4;
temp = temp*165;
temp = temp/16384;
temp -=40;
temp2=(int)temp;
v1[0]='C';
v1[1]=water2/1000+0x30;
v1[2]=water2%1000/100+0x30;
v1[3]=water2%100/10+0x30;
v1[4]=water2%10+0x30;
v2[0]='D';
v2[1]=temp2/1000+0x30;
v2[2]=temp2%1000/100+0x30;
v2[3]=temp2%100/10+0x30;
v2[4]=temp2%10+0x30;
s_data3[0]=v1[1];
s_data3[1]=v1[2];
s_data3[2]=v1[3];
s_data3[3]=v1[4];
s_data4[0]=v2[1];
s_data4[1]=v2[2];
s_data4[2]=v2[3];
s_data4[3]=v2[4];
if(water2>=68)
{
   hum_check=1;
}
if(water2<68)
{
   hum_check=0;
}
if(temp2>=30)
{
   temp_check=1;
}
if(temp2<30 && temp2>=16)
{
   temp_check=0;
}
if(temp2<16)
{
   temp_check=2;
}
tx_data('s');
tx_data('C');
tx_data('1');
tx_data('0');
tx_data(s_data1[0]);
tx_data(s_data2[0]);
tx_data(s_data3[0]);
tx_data(s_data3[1]);
tx_data(s_data3[2]);
tx_data(s_data3[3]);
tx_data(s_data4[0]);
tx_data(s_data4[1]);
tx_data(s_data4[2]);
tx_data(s_data4[3]);
tx_data('e');
if(main_auto==1 && mode1!=3)
   {
      if(gas_check==1)
      {
        tx_data2('s');
        tx_data2('A');
        tx_data2('1');
        tx_data2('2');   // close
        tx_data2('e');
      }
      if(gas_check==0)
      {
         if(hum_check==1)
         {
            tx_data2('s');
            tx_data2('A');
            tx_data2('1');
            tx_data2('2');   // close
            tx_data2('e');
         }
         if(hum_check==0)
         {
            if(temp_check==0) // nomal
            {
               if(dust_check==2)
               {
                  tx_data2('s');
                  tx_data2('A');
                  tx_data2('1');
                  tx_data2('1');   // open
                  tx_data2('e');
               }
            }
            if(temp_check==1) // hot
            {
                tx_data2('s');
                tx_data2('A');
                tx_data2('1');
                tx_data2('1');   // open
                tx_data2('e');
            }
            if(temp_check==2) // cold
            {
               tx_data2('s');
               tx_data2('A');
               tx_data2('1');
               tx_data2('2');   // close
               tx_data2('e');
            }
         }
      }
      
   }     
if(mode1==3)  // app to sensorbox
{
// sensorbox to main target
if(data_id=='E') // system auto on/off
{
   if(data[0]=='1')
   {
      main_auto=1;
   }
   if(data[0]=='0')
   {
      main_auto=0;
   }
}
if(data_id=='A' && main_auto==0)  // open close stop
{
   tx_data2('s');
   tx_data2('A');
   tx_data2('1');
   tx_data2(data[0]);
   tx_data2('e');
}
mode1=0;
}
delay_ms(100);
 } // while
}//MAIN

#INT_RDA    //  APP
void rda_irs()
{
   if(mode1==0) // stx id length
   {
      data_stack++;
      if(data_stack==1)
      {
       if(RCREG1!='s')
       {
          data_stack=0;
          return;
       }
      }
      if(data_stack==2)
      {
         data_id=RCREG1;
      }
      if(data_stack==3)      
      {
         data_len=RCREG1;
         data_len=data_len-0x30;
         mode1=1; data_stack=0; return;
      }
   }
   if(mode1 ==1) // data
   {
      data[data_stack]=RCREG1;
      data_stack++;
      if(data_stack == data_len)
      {
         mode1=2;
         data_stack=0;
         return;
      }
   }
   if(mode1==2) // etx
   {
      if(RCREG1=='e')
      {
         mode1=3;
      }
      return;
   }  
}

