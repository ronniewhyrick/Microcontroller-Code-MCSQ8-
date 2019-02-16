/* comb_lockshell.c - starter code for combination lock project */

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define SECCOUNT 125
#define COL1 PTAD_PTAD3
#define COL2 PTAD_PTAD2
#define REDLED PTAD_PTAD0
#define GREENLED PTAD_PTAD1
#define extracting_key 0
#define decode 1
#define wait4release 2
#define good 0
#define bad 1

    //unsigned char newkey=0,
  //  void waitp(void),dally1(void),decode(void),waitr(void),dally2(void);
   // void (*keystate=waitp)(void);        /* key state ptr */
  
  void badcode(void); /* called by last and wrong */
	void getkey(void);
	void keystate(void);
	void codestate(void);
 // void extracting_key1(void),extracting_key2(void),extracting_key3(void),extracting_key4(void),extracting_key5(void),checking(void),successful_entry(void),wrong(void);
  //void (*combostate=extracting_key1)(void);        /* code state ptr */  
  unsigned char combostate = extracting_key;
  unsigned char cdstate = good;
  unsigned char key=0;
	unsigned const char code[]= "12345";
	unsigned const char *nextdigit=code;
	unsigned const char asciikey[] = "123456789*0#";
	unsigned char badcount=0;
  unsigned char tpmcountdown = 0;
   
void main(void) {
  unsigned int i; 
 // combostate = extracting_key1;    
  SOPT1 = SOPT1_BKGDPE_MASK+SOPT1_RSTPE_MASK; //sets background-debug mode to pin 2
  ICSTRM = NV_ICSTRM; //trims oscillator to 8MHz
  for (i=0;i<400;i++); /* delay 2 ms (at 4 MHz) */ 
  ICSC2 = 0;  /* up speed to 8 MHz */
  PTADD = 0xF; /* set pin 16 and 15 as an output */
  PTBPE = 0xff; //all port B pull-up pins are enabled.
  
  
  PTADS = 3; /* make capable of driving LEDs */
  
  MTIMCLK = 0x08;  /* use bus clock divided by 256 */
  MTIMMOD = 249;   /* make period exactly 8 ms */
  MTIMSC = 0x40;   /* start timer and enable TOF ints */
	
	TPMSC = 0x8+0x7; //sets period to a little over 1 second 
	TPMC1SC = 0x54; // sets alarm OP settings 
	REDLED = 0;
	GREENLED = 0;

    EnableInterrupts; /* enable interrupts */

    for(;;)   /* loop forever */
    
	{
     	
	
	}
 }


//getkey(keycode);
//keycode is unsigned 7-bit ASCII
//keycode is returned after being deciphered to A- Register 
//after decoding perform 20s TPM OP compare timer (resets keystrokes after 20s)
//set tmp isr


void interrupt VectorNumber_Vmtim mtimisr()  ///// do i need this i thought the TPM timer did everything. 
 {
   if (MTIMSC_TOF){
   }
   MTIMSC_TOF = 0;  /* ack interrupt */
   
		  keystate(); //stop taking keys 
			//go to code decipher function 
 }

// place the 3 keypad decoding state functions here -- keystate
void keystate() {
  
	
	switch(combostate)   {
	                
		
		case extracting_key:
		
		if (((PTBD & 0x0F) != 0x0F) && (REDLED == 0) && (GREENLED == 0))//if key is pressed and no LEDs are on
    		{
    		  combostate = decode;
    		} 		 			
    else {
          combostate = extracting_key;
    }
          break;

		case decode: 
	
		    getkey();
		    codestate();
		    combostate = wait4release; 
		    
			  break; 
	
		case wait4release:
		  	
		  	if (((PTBD & 0x0F) == 0x0F) && (REDLED == 0) && (GREENLED == 0))//if key is pressed and no LEDs are on
    		{
    		  combostate = extracting_key;
    		} 
    		else
    		combostate = wait4release; 			
    			break;
		     
}
}

void codestate(void)

{
    switch(cdstate) {
    
    
      case good:
        if (key == '*')
            nextdigit = code;
        
        else if ((key == '#') && (*nextdigit == 0)) {
          
            	GREENLED = 1;
        			badcount = 0;
        			tpmcountdown = 3;  
        			nextdigit = &code[0];
         } 
        else if (key == *nextdigit)
                ++nextdigit;
        
        else cdstate = bad;
            
        break;
     
     case bad:
        if (key=='*'){
          nextdigit =code;
          cdstate = good;
        } 
        else if (key=='#') {
          
            if (badcount <= 2) {
            ++badcount;
            tpmcountdown = 2; 
            } else tpmcountdown = 10;
            REDLED = 1;
            nextdigit = code;
            cdstate = good;
        } 
    }
}
     
      

// the decode state must trigger a combostate function
//state one - wait: boolean loop - when new key is found - go to state 2
//state 2 - decode key: calls function getkey(keycode);
//takes keycode into next();
//state 3 - wait key -- boolean loop - waits for key pressed to be released. 


//processes value, when to light LED,
//also schedules a TPM O/P compare event to turn off LEDs

void getkey()
	{
		char tempkey = 0;
		if ((PTBD & 0xF) == 0x7) // 7 is 0111
			tempkey = 0; // row 1 assume column 1
		else if ((PTBD & 0xF) == 0xB) // B is 1011
			tempkey = 3; // row 2
		else if ((PTBD & 0xF) == 0xD) // D is 1101
			tempkey = 6; // row 3
		else
			tempkey = 9; // row 4
		COL1 = 1;
		if ((PTBD & 0x0F) == 0x0F)// no longer pressed 
		{
			key = asciikey[tempkey];
			COL1 = 0;
			return;                //// can this go here?
		}
		else
		{
			COL1 = 0;
		}
	 
		COL2 = 1;
		if ((PTBD & 0x0F) == 0x0F)	// turned off 
		{
			key = asciikey[tempkey + 1];
			COL2 = 0;
			return;
		}
		else
		{
			COL2 = 0;			//must be column 3 
			key = asciikey[tempkey + 2];
			return;
		}
	}




 

 
//tpm isr:
 interrupt VectorNumber_Vtpmch1 void tpmch1isr(void) { //capturing 20 seconds //// need help 
   if (TPMC1SC){
   }
    TPMC1SC_CH1F = 0;
    if (tpmcountdown == 0){
    	return;
			} 
			
	  if (--tpmcountdown == 0)
	      REDLED = 0;
	      GREENLED = 0;
	      
	  return; 
	
//can turn off both LEDs, reset State Machine variabel to the initial state 
    
/* end of source file */
}