
#include <Bounce2.h>

const int ledPin = LED_BUILTIN; // This code uses the built-in led for visual feedback that a message has been received
static const byte NSApin = D2;  // NSA, LOW wenn sich die Waehlscheibe dreht
static const byte NSIpin = D1; // NSI, Erzeugt bei der Zuckbewegung der Waehlscheibe die Pulse

/*
Project: Wifi controlled LED matrix display
NodeMCU pins    -> EasyMatrix pins
MOSI-D7-GPIO13  -> DIN
CLK-D5-GPIO14   -> Clk
GPIO0-D3        -> LOAD (CS)

*/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>



int pinCS = 0; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);



Bounce waehlpulse = Bounce();
Bounce scheibedreht = Bounce();

void setup()
  {    matrix.setIntensity(5); // Use a value between 0 and 15 for brightness

          // Adjust to your own needs
          //  matrix.setPosition(0, 1, 0); // The first display is at <0, 0>
           //  matrix.setPosition(0, 0, 0); // The second display is at <1, 0>
          // Adjust to your own needs
           matrix.setPosition(0, 0, 0); // The first display is at <0, 0>   
           matrix.setRotation(0, 3);  // Display 1 mit 90°
       



  pinMode(NSApin, INPUT_PULLUP);  // Set the button as an input
  pinMode(NSIpin, INPUT_PULLUP);  // Set the button as an input
  pinMode(ledPin,OUTPUT);

  waehlpulse.attach(NSIpin);
  waehlpulse.interval(5); // interval in ms
  scheibedreht.attach(NSApin);
  scheibedreht.interval(5); // interval in ms

  Serial.begin(115200);
	
}

unsigned int count=0;

char inttochar(int a)
{
  char b[2];
  String str;
  str = String(a);
  str.toCharArray(b,2);
  return b[0];
  
}

void loop() {
  waehlpulse.update();
  scheibedreht.update();

                                        
  if(scheibedreht.read() == LOW)    // Wenn sich die Waehlscheibe dreht...
  {
                                       
    if(waehlpulse.fallingEdge()) {  //... und wenn ein Impuls anliegt
      count++;                      //... Impuls zaehlen
    }  
  }
   else // Wenn sich die Waehlscheibe nicht dreht...
   {                          
      if(count != 0) //... und wenn wir vorher etwas gezaehlt haben
      {                
        matrix.fillScreen(LOW);
        if(count == 10) //... und wenn der Counter auf 10 steht
        {              
        
          Serial.println(0);         //... schreibe eine 0 auf dem keyboard
        
          matrix.drawChar(2, 1, inttochar(0), HIGH, LOW, 1);
        
        } 
        else   //... andernfalls, wenn der Counter nicht 0 aber auch nicht 10 ist
        {                     
          Serial.println(0+count);   //... schreibe den Counterwert auf dem Keyboard 
          matrix.drawChar(2, 1, inttochar(0+count), HIGH, LOW, 1);  //drawChar(0, 0, 0+count, HIGH, LOW, 1)
        }
        matrix.write(); // Send bitmap to display
      
        count = 0;                    // Counter zuruecksetzen
      
      }
    }
}

