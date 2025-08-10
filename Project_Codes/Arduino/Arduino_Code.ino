#include <LiquidCrystal_I2C.h>

#define    laser         2
#define    ledPin        13
#define    buzzerPin     12
#define    ldrPin        A0
#define    MuteButton    5

#define    FIRE          0
#define    MAGNET        1
#define    ULTRASONIC    2

int prevState = 0; // To track the previous state

LiquidCrystal_I2C lcd(0x27, 16, 2);


const float value = 3035;
uint16_t TimerCount = 0;


int  CaseChecker  = 0 ;


void setup() {

   Serial.begin(9600);
  //pinMode(LED_BUILTIN, OUTPUT);

  noInterrupts();                       // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = value;                        // preload timer
  TCCR1B  = 0x04;                       // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);               // enable timer overflow interrupt ISR
  interrupts();                         // enable all interrupts

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ldrPin, INPUT); 
  pinMode(laser, OUTPUT);
  digitalWrite(laser, HIGH); 
  pinMode(MuteButton, INPUT_PULLUP);

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();


}

ISR(TIMER1_OVF_vect)            // interrupt service routine for overflow
{
  TimerCount++;
  //Serial.println(TimerCount);

      if (0 <= TimerCount && TimerCount <= 1)
      {
        //Serial.println("I am Fireee");
        CaseChecker = FIRE;
      }
      if (2 <= TimerCount && TimerCount <= 3)
      {
        CaseChecker = MAGNET;
      }
      if (4 <= TimerCount && TimerCount <= 8)
      {
        CaseChecker = ULTRASONIC;
      }

}


void loop()
 {
  int ldrStatus = analogRead(ldrPin);
  int count = 0;
  lcd.clear();

  Serial.print(analogRead(ldrPin));

  while(analogRead(ldrPin) > 700)
    {
      
      lcd.setCursor(3,0);
      lcd.print("Safe");
    }

  while(analogRead(ldrPin) <= 700)
    {
      //Serial.println(count);
      if(count == 0)
        {
           Serial.println("Iam here");
          TCNT1 = value;
          TimerCount = 0;  
          count++;
        }
      else
          {
           // Serial.println("Iam here");
            count++;
          }
         
    }

  if (prevState == CaseChecker)
  {
     lcd.clear();
  }
  switch(CaseChecker)
  {
    case FIRE:
                    Serial.println("Firee");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("FIREEEEEE !!");
                  while(digitalRead(MuteButton)==HIGH)
                  {
                    tone(buzzerPin, 100);
                    digitalWrite(ledPin, HIGH);
                    delay(100);
                    digitalWrite(ledPin, LOW);
                    delay(100);
                  }
                   for(int i=0; i<=25; i++)
                   {
                    digitalWrite(ledPin, HIGH);
                    delay(100);
                    digitalWrite(ledPin, LOW);
                    delay(100);
                   }
                   noTone(buzzerPin); 
                                    
               break;

    case MAGNET:
                  Serial.println("Magnet");
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("DOOR OPENED !!");
                  while(digitalRead(MuteButton)==HIGH)
                  {
                    tone(buzzerPin, 100);
                    digitalWrite(ledPin, HIGH);
                    delay(100);
                    digitalWrite(ledPin, LOW);
                    delay(100);
                  }
                   for(int i=0; i<=25; i++)
                   {
                    digitalWrite(ledPin, HIGH);
                    delay(100);
                    digitalWrite(ledPin, LOW);
                    delay(100);
                   }
                   noTone(buzzerPin); 
                break;  

    case ULTRASONIC: 
                  Serial.println("Ultrasonic");     
                  lcd.clear();          
                  lcd.setCursor(0,0);
                  lcd.print("INTRUSION !!");
                  while(digitalRead(MuteButton)==HIGH)
                  {
                    tone(buzzerPin, 100);
                    digitalWrite(ledPin, HIGH);
                    delay(100);
                    digitalWrite(ledPin, LOW);
                    delay(100);
                  }
                   for(int i=0; i<=25; i++)
                   {
                    digitalWrite(ledPin, HIGH);
                    delay(100);
                    digitalWrite(ledPin, LOW);
                    delay(100);
                   }
                   noTone(buzzerPin); 
                break;
  }

  TimerCount =0;
  prevState = CaseChecker;
  count = 0;

}