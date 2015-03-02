//  C8051F380/C8051F381 with three 7-segment displays
//  Copyright (c) 2014 Jesus Calvino-Fraga
//  ~C51~

#include <C8051F38x.h>
#include <stdio.h>

#define SYSCLK    48000000L // SYSCLK frequency in Hz#define SYSCLK      48000000L  // SYSCLK frequency in Hz
#define BAUDRATE      115200L  // Baud rate of UART in bps


#define SEG_F3  P0_1
#define SEG_G3  P0_2
#define SEG_E3  P0_3
#define SEG_D3  P0_6
#define SEG_A3  P0_7
#define SEG_B3  P1_0
#define SEG_C3  P1_1
#define SEG_DP3 P0_4
#define SEG_F2  P1_2
#define SEG_G2  P1_3
#define SEG_E2  P1_4
#define SEG_D2  P1_5
#define SEG_A2  P1_6
#define SEG_B2  P1_7
#define SEG_C2  P2_0
#define SEG_DP2 P2_1
#define SEG_F1  P3_0
#define SEG_G1  P2_7
#define SEG_E1  P2_6
#define SEG_D1  P2_5
#define SEG_A1  P2_4
#define SEG_B1  P2_3
#define SEG_C1  P2_2

code unsigned char seven_seg[] = { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
                                   0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E };

char _c51_external_startup (void)
{
	PCA0MD&=(~0x40) ;    // DISABLE WDT: clear Watchdog Enable bit
	VDM0CN=0x80; // enable VDD monitor
	RSTSRC=0x02|0x04; // Enable reset on missing clock detector and VDD

	// CLKSEL&=0b_1111_1000; // Not needed because CLKSEL==0 after reset
	#if (SYSCLK == 12000000L)
		//CLKSEL|=0b_0000_0000;  // SYSCLK derived from the Internal High-Frequency Oscillator / 4 
	#elif (SYSCLK == 24000000L)
		CLKSEL|=0b_0000_0010; // SYSCLK derived from the Internal High-Frequency Oscillator / 2.
	#elif (SYSCLK == 48000000L)
		CLKSEL|=0b_0000_0011; // SYSCLK derived from the Internal High-Frequency Oscillator / 1.
	#else
		#error SYSCLK must be either 12000000L, 24000000L, or 48000000L
	#endif
	OSCICN |= 0x03; // Configure internal oscillator for its maximum frequency
	
	// WARNING: Pin P0.5 is reserved for the serial port as RXD, and must always be configured
	// as an input. Therefore we can not use P0.5 to drive an LED.  We can use P0.4 (TXD) as long
	// as the current limitting resistor in series with the LED is 2k ohms or more.

	// Configure all the pins connected to the 7-segment displays as outputs
	P0MDOUT|=0b_1101_1110;
	P1MDOUT|=0b_1111_1111;
	P2MDOUT|=0b_1111_1111;
	P3MDOUT|=0b_0000_0001;
	XBR0=0x00;                     
	XBR1=0x40; // Enable crossbar and weak pull-ups
	
	return 0;
}

// Uses Timer3 to delay <us> micro-seconds. 
void Timer3us(unsigned long us)
{
	unsigned long i;               // usec counter
	
	// The input for Timer 3 is selected as SYSCLK by setting T3ML (bit 6) of CKCON:
	CKCON|=0b_0100_0000;
	
	TMR3RL = (-(SYSCLK)/1000000L); // Set Timer3 to overflow in 1us.
	TMR3 = TMR3RL;                 // Initialize Timer3 for first overflow
	
	TMR3CN = 0x04;                 // Sart Timer3 and clear overflow flag
	for (i = 0; i < us; i++)       // Count <us> overflows
	{
		while (!(TMR3CN & 0x80));  // Wait for overflow
		TMR3CN &= ~(0x80);         // Clear overflow indicator
	}
	TMR3CN = 0 ;                   // Stop Timer3 and clear overflow flag
}

void waitms (unsigned int ms)
{
	unsigned int j;
	for(j=ms; j!=0; j--)
	{
		Timer3us(249);
		Timer3us(249);
		Timer3us(249);
		Timer3us(250);
	}
}

void Send_7Seg (unsigned int x)
{
	// The accumulator in the C8051F381 is bit addressable!
	ACC=seven_seg[x/100];
	SEG_A3=ACC_0;
	SEG_B3=ACC_1;
	SEG_C3=ACC_2;
	SEG_D3=ACC_3;
	SEG_E3=ACC_4;
	SEG_F3=ACC_5;
	SEG_G3=ACC_6;
	
	ACC=seven_seg[(x/10)%10];
	SEG_A2=ACC_0;
	SEG_B2=ACC_1;
	SEG_C2=ACC_2;
	SEG_D2=ACC_3;
	SEG_E2=ACC_4;
	SEG_F2=ACC_5;
	SEG_G2=ACC_6;
	
	ACC=seven_seg[x%10];
	SEG_A1=ACC_0;
	SEG_B1=ACC_1;
	SEG_C1=ACC_2;
	SEG_D1=ACC_3;
	SEG_E1=ACC_4;
	SEG_F1=ACC_5;
	SEG_G1=ACC_6;
}
// PeriodF38x.c: Measure the period of a signal on pin P0.1.
//
// By:  Jesus Calvino-Fraga (c) 2008-2015
//
// The next line clears the "C51 command line options:" field when compiling with CrossIDE
//  ~C51~  

unsigned char overflow_count;

void PORT_Init (void)
{
	P0MDOUT |= 0x10; // Enable UART TX as push-pull output
	XBR0=0b_0000_0001; // Enable UART on P0.4(TX) and P0.5(RX)                    
	XBR1=0b_0101_0000; // Enable crossbar.  Enable T0 input.
	XBR2=0b_0000_0000;
}

void SYSCLK_Init (void)
{
	// CLKSEL&=0b_1111_1000; // Not needed because CLKSEL==0 after reset
#if (SYSCLK == 12000000L)
	//CLKSEL|=0b_0000_0000;  // SYSCLK derived from the Internal High-Frequency Oscillator / 4 
#elif (SYSCLK == 24000000L)
	CLKSEL|=0b_0000_0010; // SYSCLK derived from the Internal High-Frequency Oscillator / 2.
#elif (SYSCLK == 48000000L)
	CLKSEL|=0b_0000_0011; // SYSCLK derived from the Internal High-Frequency Oscillator / 1.
#else
	#error SYSCLK must be either 12000000L, 24000000L, or 48000000L
#endif
	OSCICN |= 0x03;   // Configure internal oscillator for its maximum frequency
	RSTSRC  = 0x04;   // Enable missing clock detector
}
 
void UART0_Init (void)
{
	SCON0 = 0x10;
   
#if (SYSCLK/BAUDRATE/2L/256L < 1)
	TH1 = 0x10000-((SYSCLK/BAUDRATE)/2L);
	CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
	CKCON |=  0x08;
#elif (SYSCLK/BAUDRATE/2L/256L < 4)
	TH1 = 0x10000-(SYSCLK/BAUDRATE/2L/4L);
	CKCON &= ~0x0B; // T1M = 0; SCA1:0 = 01                  
	CKCON |=  0x01;
#elif (SYSCLK/BAUDRATE/2L/256L < 12)
	TH1 = 0x10000-(SYSCLK/BAUDRATE/2L/12L);
	CKCON &= ~0x0B; // T1M = 0; SCA1:0 = 00
#else
	TH1 = 0x10000-(SYSCLK/BAUDRATE/2/48);
	CKCON &= ~0x0B; // T1M = 0; SCA1:0 = 10
	CKCON |=  0x02;
#endif
	TL1 = TH1;      // Init Timer1
	TMOD &= ~0xf0;  // TMOD: timer 1 in 8-bit autoreload
	TMOD |=  0x20;                       
	TR1 = 1; // START Timer1
	TI = 1;  // Indicate TX0 ready
}

void TIMER0_Init(void)
{
	TMOD&=0b_1111_0000; // Set the bits of Timer/Counter 0 to zero
	TMOD|=0b_0000_0001; // Timer/Counter 0 used as a 16-bit timer
	TR0=0; // Stop Timer/Counter 0
}

void main (void) 
{
	float period;
	
	PCA0MD &= ~0x40; // WDTE = 0 (clear watchdog timer enable)
	PORT_Init();     // Initialize Port I/O
	SYSCLK_Init ();  // Initialize Oscillator
	UART0_Init();    // Initialize UART0
	TIMER0_Init();
    
    printf("\x1b[2J"); // Clear screen using ANSI escape sequence.

	printf ("Period measurement at pin P0.1 using Timer 0.\n"
	        "File: %s\n"
	        "Compiled: %s, %s\n\n",
	        __FILE__, __DATE__, __TIME__);
	
    while (1)
    {
    	// Reset the counter
		TL0=0; 
		TH0=0;
		TF0=0;
		overflow_count=0;
		
		while(P0_0!=0); // Wait for the signal to be zero
		while(P0_0!=1); // Wait for the signal to be one
		TR0=1; // Start the timer
		while(P0_0!=0) // Wait for the signal to be zero
		{
			if(TF0==1) // Did the 16-bit timer overflow?
			{
				TF0=0;
				overflow_count++;
			}
		}
		while(P0_0!=1) // Wait for the signal to be one
		{
			if(TF0==1) // Did the 16-bit timer overflow?
			{
				TF0=0;
				overflow_count++;
			}
		}
		TR0=0; // Stop timer 0, the 24-bit number [overflow_count-TH0-TL0] has the period!
		period=(overflow_count*65536.0+TH0*256.0+TL0)*(12.0/SYSCLK);
		period = 60/period;
		// Send the period to the serial port
		printf( "%f \n\r" , period);
		Send_7Seg(period);
		waitms(200);
	//	printf ("Period measurement at pin P0.1 using Timer 0.\n"
	  //      "File: %s\n"
	    //    "Compiled: %s, %s\n\n",
	      //  __FILE__, __DATE__, __TIME__);
  
   }	
}


 
/*void main (void)
{
	unsigned int j;
	int pulse_flag;
	int bpm = 0;
	bpm = 0;
	j=0;
	pulse_flag=0;
	while(1)
	{
		Send_7Seg(bpm);
		if(P0_0 != 0){
		if(pulse_flag==0)
		 bpm++;
		 pulse_flag=0;
		pulse_flag = 1;}
		if(P0_0 == 0){
		pulse_flag=0;}
		waitms(3);
	}
}

		//SEG_DP3=!SEG_DP3; Decimal point
		//SEG_DP2=!SEG_DP3;*/