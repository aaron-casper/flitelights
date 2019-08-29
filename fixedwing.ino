/*
flightlights - fixed wing

This version of flightlights includes configurations for fixed-wing aircraft.  red/green FAA, landing lights, and
machine-guns are included.  Feel free to edit this source and re-burn it to your board.

 -floz

TODO:

!!clean everything up
!select functions to actually burn to new kits
!fine-tune anything else that's left
!!!develop "interactive" programming mode - done via flashes to set
battery timeout in mins.

*/

#include <Adafruit_NeoPixel.h> // include the NeoPixel library.  super handy, that.

#ifdef __AVR__          //some magic sauce for AVR microcontrollers?
#include <avr/power.h>  //some magic sauce for AVR microcontrollers?
#endif                  //some magic sauce for AVR microcontrollers?

//data-pin for the neopixel LEDs (output)
#define LED_PIN 6

//channel on your hobby receiver connects to this digital pin (input)
#define PWM_SOURCE 5

//how many LEDs in the chain?
#define NUM_PIXELS 2

#define VERSION 1.2
//how long between strobe flashes/battery checks
const long interval = 5050;

// how long until battery warning?
const long battery_timeout = 420000; 

//not really .45, 45ms maximum random on for machinegun fire effect
const long gun_caliber = 45; 

//timer var
unsigned long previousMillis = 0;

//serial console input
String inputString = "";         // a String to hold incoming data

//flag (is input string done?)
boolean stringComplete = false;  // whether the string is complete

// el togglodyte supremo!
// vars used when performing strobe
int MaxLightState = 6;// one over the total number of choices you have.
int LightState = 0; //pick one start with (0 = navlights, 1 = landing lights, 4 = machinegun)

int LastLightState = 0;

//magic sauce for neopixel/ws2812b LEDs
//check strandtest or other examples for further info
//you can instantiate multiples, define a new pin for each.
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ400);


void setup() 
{
    //this should be the first void ____ () function in this program
    randomSeed(analogRead(0)); //random seed for machine-gun flash effect (works on rf/emf noise, funnily enough)
    leds.begin(); // init the LED strand instance
    leds.setPixelColor(0,255,0,0);
    leds.setPixelColor(1,255,0,0);
    leds.show(); // Initialize all pixels
    delay(250);
    leds.setPixelColor(0,0,255,0);
    leds.setPixelColor(1,0,255,0);
    leds.show(); // Initialize all pixels
    delay(250);
    leds.setPixelColor(0,0,0,255);
    leds.setPixelColor(1,0,0,255);
    leds.show(); // Initialize all pixels
    delay(250);
    leds.setPixelColor(0,0,0,0);
    leds.setPixelColor(1,0,0,0);
    leds.show(); // Initialize all pixels
    // chose to add color by color init to ensure LED is functional on boot
    Serial.begin(9600); // start serial output
    inputString.reserve(200);
    Serial.print(" flightlights fixed wing ver: ");
    Serial.print(VERSION);
    Serial.println(" boot complete!");
}

void(* resetFunc) (void) = 0;
//define each of your effect functions here:

void nav_lights(){
    //red/green FAA style
    leds.setPixelColor(0, 0,255,0);
    leds.setPixelColor(1,255,0,0);
    leds.show();
    
}

void landing_lights(){
       //bright warm white (r,g,b)
      leds.setPixelColor(0, 255,255,196);
      leds.setPixelColor(1, 255,255,196);
      leds.show();
}

void battery_timer(){
   //after x minutes (set above) give the pilot a warning flash
   //and then go to critical flasher after two more minutes

     //two minutes over, go red
     if (millis() >= battery_timeout + 120000){          
        LightState = 5;
        return;                                        
     }   
     
      leds.setPixelColor(0, 255,255,0);
      leds.setPixelColor(1, 0,0,0);
      leds.show();
      delay(150);
      leds.setPixelColor(1, 255,255,0);
      leds.setPixelColor(0, 0,0,0);
      leds.show();
      delay(150);
         
 }
 
void critical_flash(){
      leds.setPixelColor(0, 255,0,0);
      leds.setPixelColor(1, 0,0,0);
      leds.show();
      delay(150);
      leds.setPixelColor(1, 255,0,0);
      leds.setPixelColor(0, 0,0,0);
      leds.show();
      delay(150);  
    
 }
 
void strobe(){
      // added strobe thingie - provides a quick flash and returns to nav red/green cleanly
      leds.setPixelColor(0, 255,255,255);
      leds.setPixelColor(1, 255,255,255);
      leds.show();
      delay(60);
      LightState = LastLightState;
      return;
}

void strobe_loop(){
  // rapid wingtip flashes of white/off (find my crashed plane mode)
      leds.setPixelColor(0, 255,255,255);
      leds.setPixelColor(1, 255,255,255);
      leds.show();
      delay(100);
      leds.setPixelColor(0, 0,0,0);
      leds.setPixelColor(1, 0,0,0);
      leds.show();
      delay(100);
}

void machine_gun_loop(){
       /* rapid fire-like flashes at random intervals
        *  
        * gives a nice random fire effect, provided as a well-documented example
        * of how to write your own functions.
        * 
        * if you had other lights and machineguns, you would need to specify what
        * pixel/LEDs were where by changing the ID
        * for example: leds.setPixelColor(ID,red,green,blue);
         */       
      int randNumber = random(gun_caliber);   //generate a random number 0 to 45
      leds.setPixelColor(0, 255,128,0);        //set a nice orangeish fire color for first pixel
      leds.setPixelColor(1, 0,0,0);            //turn second pixel off
      leds.show();                             // make it so!
      delay(randNumber);                      //wait random amount of milliseconds, see random 0 to 60 above
      randNumber = random(gun_caliber+10);     //get a new random number
      leds.setPixelColor(0, 0,0,0);            //turn first pixel off
      leds.setPixelColor(1, 255,128,0);       //set a nice orangeish fire color for second pixel
      
      leds.show();                              // make it so!
      delay(randNumber);                        //wait random amount of milliseconds again
}

void lookupLightState(){
   if (LightState == 0){
      Serial.println("nav lights");
    }
    if (LightState == 1){
      Serial.println("landing lights");
    }
    if (LightState == 2){
      Serial.println("strobe lights");
    }
    if (LightState == 3){
      Serial.println("battery alert");
    }
    if (LightState == 4){
      Serial.println("pewpewpew");
    }                         
    if (LightState == 5){
      Serial.println("critical flash");
    }
}
void loop() {
    
      //serial console - used for debugging/testing porpoises
      // print the string when a newline arrives:
    if (stringComplete) {
    Serial.print("ACK ");
    Serial.print(inputString);
  
  
  //parser starts here   
  
   if (inputString == "test\n"){  //give some diagnostic info
    Serial.print("flightLights - Program version: ");
    Serial.println(VERSION);
    Serial.print("program run time: ");
    Serial.print(millis());
    Serial.println(" ms");
    Serial.print("Current Mode: ");
    lookupLightState();
    int pwmin = pulseIn(PWM_SOURCE, HIGH, 20000);     //read the PWM line (default pin 5)
    Serial.print("PWM input: ");
    Serial.println(pwmin);
   
   }else if(inputString == "change\n"){ //iterate through states
    LightState++;
      if (LightState >= MaxLightState) {
        LightState = 0;
      }
      Serial.print("New Mode: ");
      lookupLightState(); 
     
    }else if(inputString == "reboot\n"){
    Serial.println("rebooting");
    delay(1000);
    resetFunc();
    }else{
    Serial.println("unknown command, valid commands are:"); //return "huh?" catch any non-defined commands this way
    Serial.println("test - show diagnostic data");
    Serial.println("change - change mode");
    Serial.println("reboot - reboot light controller");
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
//end of parser


     //debug output starts here
     /*Serial.println(millis()); //show the counter for rapid pass/fail check that program is running
       uncomment for debugging otherwise the console is spammmmmm  
     */
     
     /*  
      Serial.print("PWM: ");
      Serial.print(pwmin);     
      if (LightState == 0){
      Serial.print(" - nav lights - ");
     }
     
     if (LightState == 1){
      Serial.print(" - landing lights - ");
     }
     
     if (LightState == 2){
      Serial.print(" - strobe lights - ");
     }
     Serial.print(" time: ");
     Serial.println(millis());
     delay(100); 
     */
     
     //uncomment for debugging otherwise the console is spammmmmm  
     //debug output stops here, actual loop code begins

     // how are the lights commanded within this program?
     // LightState is an iterable int that represents individual functions
     // that the lights can perform.  It is used to snap quickly back to FAA red/green
     // after a fast strobe, and just kind of grew from there.
   
    if (LightState == 0){
      nav_lights();
    }
    if (LightState == 1){
      landing_lights();
    }
    if (LightState == 2){
      strobe_loop();
    }
    if (LightState ==3){
      battery_timer();
    }
    if (LightState ==4){      //add extra functions like this.
      machine_gun_loop();     //they can point to any function you want
    }                         //muzzle-flash here
    if (LightState ==5){
      critical_flash();       //alert that something is amiss.
    }



    
  //running timer to check for strobe interval
  //blatantly borrowed from "blink without delay" example for Arduino
 unsigned long currentMillis = millis();
                                                        //someone else's work, from blinkwithoutdelay example.
  if (currentMillis - previousMillis >= interval) {     //has the delay between strobes happened?
  Serial.print("program run time: ");
  Serial.println(millis());
  if (millis() >= battery_timeout){                     //battery timer check
        LightState =3;
        return;                                         //ymmv, comment out?
     }
    previousMillis = currentMillis;
    if (LightState != 0){                               //strobe check, if we're not using nav lights, don't bother
      return;
     }
     LastLightState = LightState;                       //var to return to nav lights after strobe
       strobe();                                        //give us a quick flash
     }



     
      // below this is the PWM input from the hobby receiver
      // any modes you want reachable via your radio must be mapped here.
      // simply add a new case X: / break; statement as shown below
      // and create the function at the top of this program
      
     int pwmin = pulseIn(PWM_SOURCE, HIGH, 20000);     //read the PWM line (default pin 5)
     
     int selection = map(pwmin, 1000,1900,0,4);        //map the value to however many choices you want to have
                                                       //from your radio.  this should match the code below
     
     switch (selection) {  //what mode are we in according to pwm?
     case 0:
     LightState = 0; //nav lights             
     break;
     case 1:
     LightState = 1; //landing lights
     break;
     case 2:
     LightState = 2; // strobes
     break;
     case 3:                                          
     LightState = 3;  //battery flasher
     break;
     case 4:          //add extra functions by adding case 5, case 6, etc.  be sure to change the mapping above.
     LightState = 4;  //machine_gun_loop
     break;
     }
    
      
   }  
   
    /*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
