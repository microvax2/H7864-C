/*
 * H7864 Software
 * 
 * PIC12F675
 */

// XC8 header:
#include <xc.h>
//__CONFIG(0x005c);
#pragma config FOSC = 4
#pragma config WDTE = 1
#pragma config PWRTE = 1
#pragma config MCLRE = 0
#pragma config BOREN = 1

// HTPICC Header:
//#include <htc.h>
//#include <PIC12F675.H>
//__CONFIG(0x005c);
        


#define OPTION_MASK 	0b11000111
#define WDT_PRESCALER 	0b00001000

#define LINE_CLOCK GPIO0 // optocpoupler; provides 50Hz or 60Hz, if circuit is present and working
#define BPOK GPIO2       // POK signal to the QBUS; '1' forces an open collector to ground
#define BDCOK GPIO1      // DCOK signal to the QBUS; '1' forces an open collector to ground
#define BEVENTL GPIO5    // LINE CLOCK to the QBUS; used by PDP11 systems; open collector output
void init(void)
{
	ANSEL=0;
	CMCON=0x07;
	TRISIO = 0b00011001;   
    BPOK=1;
    BDCOK=1;
	T1CON=0x30; 
    TMR1IE=1;
    PEIE=1;
    WPU=0;
    OPTION_REG = OPTION_MASK | WDT_PRESCALER; //WDT has prescaler
}
#define RESET_TIME clock=0;clock=0;
char clock; 
char limit;
static bit safe_mode;
static void interrupt isr( void)
{
  TMR1IF=0;
  TMR1L=-123;
  TMR1H=0xff;
  clock++;
}
void identify_line_freq(void)
{
  static bit b;
  char i=0;
  char cnt_50=0;
 
  // detect a change on LINE_CLOCK input, otherwise go to safe mode operation
  // repeat twice, so we do not detect a glitch as a signal
  for (i=0; i<=1; i++)
    { b = LINE_CLOCK;
      RESET_TIME;
      while (b==LINE_CLOCK)
        {  
             CLRWDT(); 
             if (clock>=200) 
               { safe_mode=1;
                 return;
               }
            }
    }
  RESET_TIME;
  while (i<11)
    {      
      RESET_TIME;
      while (!LINE_CLOCK) CLRWDT();
      while (LINE_CLOCK) CLRWDT();
      if (clock>=18) cnt_50++;       
      i++;
    }
  if (cnt_50>8) limit=15; else limit=13;
}

void main( void)
{
    char i;
    static bit b;
    init();
   
    GIE=1;    
    TMR1H=0xff;
    TMR1L=-125;
    TMR1IF=0;
    TMR1ON=1;
    safe_mode=0;
    // test line frequency, if no signal at all - raise the safe_mode flag
    identify_line_freq();
   
    while (1)
      {
        /*
                            POWER - ON RESET SEQUENCE         
         */
         RESET_TIME;
         while (clock<=3) CLRWDT();    //3ms minimum from P/U, send DCOK
         BDCOK=0;
         RESET_TIME;
         while(clock<100) CLRWDT();    // then after 100ms, send POK
         BPOK=0; 
         // NO AC SIGNAL: "SAFE MODE"  IS JUST 50HZ GENERATION UNTIL POWER OFF
         while (safe_mode)
           { 
             RESET_TIME;
             while(clock<10) CLRWDT(); 
             BEVENTL=!BEVENTL;             
           }
         // ELSE
         // NORMAL OPERATION, GENERATE 50/60HZ BASED ON THE LINE INPUT
         // AND DETECT A POWER FAULT AND RESPOND ACCORDINGLY
         while (!BPOK)
          {
              b = !b;   
              BEVENTL=!BEVENTL; // generate a fake 50H in sync with the real 50hz
              RESET_TIME;
              while (LINE_CLOCK==b) 
              {
                CLRWDT();
                if (clock>=limit)
                  { BPOK=1; 
                  break;}
               }
          }
        //execution ends up here if a 50Hz edge is missing for more than the allowed max. time
        // the loop above already negated POK, now continue:        
        RESET_TIME;
        while (clock<4) CLRWDT();
        // at this point, the obligatory 4ms has passed
        BDCOK=1;
        RESET_TIME;
        while (clock<1) CLRWDT();
        // at this point, power down has been completed
        // option 1: GLITCH
        // in this case, continue following the line input for 1 second,
        // with POK and DCOK showing failed power. Then, start the main loop again for a new reset sequence etc
        // option 2: A REAL POWER FAILURE        
        // in this case, execution may or may not complete the following code,
        // the on-board capacitor has enough charge to keep us going for a few more ms, that's it
        for (i=0; i<=25; i++)
          {
           while (!LINE_CLOCK) CLRWDT();      
           while (LINE_CLOCK) CLRWDT();
          }
        // finished the orderly shutdown, if power is still on then a new RESET sequence follows:
      }
  
}
