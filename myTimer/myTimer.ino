#include <Adafruit_NeoPixel.h>
#include <EasyButton.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        0
#define NUMPIXELS 16
#define BUTTON_PIN 3

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
EasyButton button(BUTTON_PIN);

int timer_led_on = NUMPIXELS-1;

uint32_t work = pixels.Color(255, 0, 0);
uint32_t relax = pixels.Color(0, 255, 0);
uint32_t color = work;
uint32_t off = pixels.Color(0, 0, 0);

unsigned long timeWork = 1500000;       // 26 minutes
unsigned long timeRelax = 300000;       // 5 minutes
unsigned long segmet_timestamp = 0;
unsigned long time_segment = timeWork / NUMPIXELS;

int i = 0;




#define DELAY_INIT 1000
#define DELAY_DROPS 100



const byte buttonPin = BUTTON_PIN;
const byte potPin = A1;

void timeSegmentCalc ()
{
  if (color == work)
  {
    time_segment = timeWork / NUMPIXELS;
  } else
  {
    time_segment = timeRelax / NUMPIXELS;
  }
}

void switchMode ()
{
  if (color == work)
  {
    color = relax;
    pixels.clear();
    pixels.show();
  } else
  {
    color = work;
    pixels.clear();
    pixels.show();
  }
  // Re-init counter
  pixels.clear();
  pixels.show();
  i = 0; 
  timer_led_on = NUMPIXELS-1;
  timeSegmentCalc();
}

void longPress()
{
  // Set time
  // While button pressed
  while(digitalRead(buttonPin) == LOW)
  {
    pixels.clear();
    pixels.show();
    delay(250);
    // Read potentiometer
    uint8_t timeRead = analogRead(potPin)/64;
    // Display read result
    for(int i=0; i<=timeRead; i++)
    {
      pixels.setPixelColor(i, color);
    }
    // Save selected time
    if (color == work)
    {
      // Multiply number of LED by 2 
      timeWork = (timeRead + 1) * 120000;       // minutes
    } else 
    {
      timeRelax = (timeRead + 1) * 120000;      // minutes
    }
    pixels.show();
    delay(250);
  }
  // Re-init counter
  pixels.clear();
  pixels.show();
  i = 0;
  timer_led_on = NUMPIXELS-1;
  timeSegmentCalc();
}

// Callback function to be called when the button is pressed.
void shortPress()
{
  switchMode();
}

void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  // Initialize LED
  pixels.begin();
  pixels.setBrightness(10);
  pixels.clear();

  // Initialize button
  button.begin();
  // Add the callback function to the button
  button.onPressedFor(2000, longPress);
  button.onPressed(shortPress);

  // Capture start time
  segmet_timestamp = millis();
}

void loop() {
  button.read();
  
  // Drops cycle
  for(i=0; i<=timer_led_on; i++) {
    button.read();

    pixels.setPixelColor(i, color);
    
    // Turn previous LED off
    if(i > 0) 
    {
      pixels.setPixelColor(i-1, off);
    }
    // Turn the last LED off
    if(i == 0) 
    {
      pixels.setPixelColor(timer_led_on-1, off);
    }
    // Update LED status
    pixels.show();
    // Initial drop delay
    if (i == 0)
    {
      delay(DELAY_INIT);
    }
    // Dripping delay between 
    delay(DELAY_DROPS);
  }



  // Timer LED increase every 1/16 of work/relax time
  if ((segmet_timestamp + time_segment) < millis())
  {
    timer_led_on = timer_led_on - 1;
    segmet_timestamp = millis();
  }

  // Switch mode when finish counting
  if (timer_led_on == 0)
  {
    switchMode();
  }
  
}
