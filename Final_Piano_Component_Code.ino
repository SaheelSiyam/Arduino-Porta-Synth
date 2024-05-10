#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
// I2C address 0x27, 16 column and 2 rows
/*
Notes:
Limited to playing 1 key at a time.
Octave max and min = 1st octave(C0) to 5th octave(C5)
Volume can be changed via Potentiometer
LED Indicators on the octave shifters tell you whether 
or not you're at the upper/lower bound
*/
#include <math.h>
const int num_keys = 7;

//PINS
const int KEYS[] = {12, 11, 10, 9, 8, 7, 6};
const String NOTES[] = {"C", "D", "E", "F", "G", "A", "B"};
//the x-value for the default octave 3
int FREQS[] = {37, 39, 41, 42, 44, 46, 48};


//user octave
int octave = 3;
//default octave
const int def_octave = 3;
//the current frequency of the piano
double key_freq;
const int upp_bound = 6, low_bound = 1;

const int PM_Speak = A0;
const int PB_oct_plus = A2, PB_oct_min = A1;
const int LED_plus = 2, LED_min = 3;

int totalColumns = 16;
int totalRows = 2;


void setup() {
  
  for(int i = 0; i < num_keys; i++){
    pinMode(KEYS[i], INPUT);
  }
  
  pinMode(PM_Speak, INPUT);
  pinMode(PB_oct_plus, INPUT);
  pinMode(PB_oct_min, INPUT);
  
  pinMode(LED_plus, OUTPUT);
  pinMode(LED_min, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();
  
  Serial.begin(9600);
  startJingle();
  
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 1);         // move cursor to   (0, 0)
  lcd.print("Note: "); 
  lcd.setCursor(7, 1);         // move cursor to   (0, 0)
  lcd.print("Freq: ");
  
}

void loop() {

  for(int i = 0; i < num_keys; i++){
    
    lcd.clear();
    lcd.setCursor(0, 0);        
    lcd.print("Octave: "); 
    lcd.setCursor(8, 0);        
    lcd.print(octave); 
    lcd.setCursor(0, 1);         
    lcd.print("Note: "); 
    lcd.setCursor(7, 1);         
    lcd.print("Freq: ");  
    key_freq = octave_settings(i);
    lcd.setCursor(0, 0);         
    lcd.print("Octave: ");
    lcd.setCursor(8, 0);        
    lcd.print(octave); 
    
    if (digitalRead(KEYS[i])){
      // Serial.println(octave);
     
      lcd.setCursor(5, 1);         
      lcd.print(NOTES[i]);
      lcd.setCursor(6, 1);

      if(i > 4){
         lcd.print(octave+1);
         Serial.print("Octave up");
         Serial.println(octave+1);
      }else{
          lcd.print(octave);
         
      }
      
        
      while (digitalRead(KEYS[i])){
        
        lcd.setCursor(12, 1);         
        lcd.print(round(key_freq));
        tone(PM_Speak, key_freq, 50);
      }      
    } 
  }
}

double octave_settings(int key){
  
  if(digitalRead(PB_oct_plus)){
    
    // turn off the opposing LED
    digitalWrite(LED_min, LOW);
    octave+=1;
    Serial.println(octave);
    if(octave > upp_bound){
      octave = upp_bound;
      digitalWrite(LED_plus, HIGH);
      
      lcd.setCursor(0, 0);         // move cursor to   (0, 0)
      lcd.print("Max Oct Passed!"); 
      delay(2000);
      lcd.setCursor(0, 0);         // move cursor to   (0, 0)
      
      scrollMessage(0, "Shift Down-Press Red", 250, 16);
      delay(2000);


    }else{
      digitalWrite(LED_plus, LOW);
      delay(100);
      digitalWrite(LED_plus, HIGH);
      delay(200);
      digitalWrite(LED_plus, LOW);
    }
    
  }
  if(digitalRead(PB_oct_min)){

    // turn off the opposing LED
    digitalWrite(LED_plus, LOW);
    octave-=1;
    Serial.println(octave);
    if(octave < low_bound){
      Serial.println("Octave min");
      octave = low_bound;
      digitalWrite(LED_min, HIGH);
      lcd.setCursor(0, 0);         // move cursor to   (0, 0)
      lcd.print("Min Oct Passed "); 
      delay(2000);
      lcd.setCursor(0, 0);         // move cursor to   (0, 0)

      scrollMessage(0, "Shift Up-Press Blue", 250, 16);
      delay(2000);


    }else{
      digitalWrite(LED_min, LOW);
      delay(100);
      digitalWrite(LED_min, HIGH);
      delay(200);
      digitalWrite(LED_min, LOW);
    }
  }
  
  //Example: Key 3: E, from Octave 3 to Octave 1
  //octave difference: In this case: 1 - 3 = -2 (2 steps down)
  key_freq = octave - def_octave;
  //the x value to substitute: In this case: freq[2] = 41, - 2(12) = 17
  key_freq = FREQS[key] + 12*(key_freq);
  //get an actual frequency key from that
  key_freq = 30.8719 * pow(1.05946, key_freq);
  //
  return key_freq;
  
}


void startJingle(){
  
  lcd.setCursor(0, 0); 
  lcd.print("Arduino Piano");
  
  // https://microcontrollerslab.com/i2c-lcd-arduino-tutorial/
  
  //params in order: starting row, message, delay between shift, total number of columns
  scrollMessage(1, "Powered by Ethereality", 200, totalColumns);
  
  delay(1000);

  //jingle
  tone(PM_Speak, 329.63, 400); //E4
  delay(300);
  tone(PM_Speak, 293.66, 600); //C4
  delay(700);
  tone(PM_Speak, 349.23, 400);    //F#4 
  delay(300);
  tone(PM_Speak, 659.25, 500);    //F6
  delay(1000);

  //2 tone beep to start piano
  tone(PM_Speak, 82.41, 400);    //F#4 
  delay(300);
  tone(PM_Speak, 82.41, 400);    //F#4 

  
}

//Scroll message
void scrollMessage(int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}