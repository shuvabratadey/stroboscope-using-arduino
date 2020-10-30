#include<ShiftLCD.h>

#define freq_pin A0
#define duty_pin A1
#define Led_pin 9
#define BtnUp 4
#define Led_ON PORTB|=B00000010
#define Led_OFF PORTB&=~B00000010

#define on_time true
#define off_time false

#define UpperFrequency 100
#define LowerFrequency 2
float freq, pre_freq, precesion, multiple_freq, multiple = 1.0;
int Freq, duty_cycle = 50, pre_duty_cycle = 1;
bool set_precesion = false, state = false;;
ShiftLCD lcd(A2, A3, A4);    //(Data,Latch,Clock)[Yellow:- pin2, Green:- pin3, Blue:- pin4]
unsigned long Micros, time[2];

void startUp()
{
  lcd.setCursor(2, 0);
  lcd.print("Stroboscope");
  lcd.setCursor(1, 1);
  lcd.print("Made by shuva");
  delay(1000);
  lcd.clear();
}

void setup() {
    pinMode(Led_pin,OUTPUT);
    pinMode(BtnUp,INPUT);
    lcd.begin(16,2);
    attachInterrupt(0,multipleByTwo,RISING);
    attachInterrupt(1,checkWhichPin,RISING);
    startUp();
}

void loop() {
    freq = map(analogRead(freq_pin),0,1023,UpperFrequency,LowerFrequency);

    if(freq != multiple_freq)
    {
        multiple = 1.0;
    }

    if(!set_precesion)
    {
        precesion = map(analogRead(duty_pin), 0, 1023, 99, 0);
    }
    else
    {
        duty_cycle = map(analogRead(duty_pin),0,1023,50,1);
    }
    
    Freq = freq;
    freq = int(Freq * multiple);
    freq += precesion / 100;

    if(freq != pre_freq)
    {
        lcd.setCursor(0,0);
        lcd.print("FPM = ");
        lcd.print(int(freq*60));
        lcd.print("      ");
        lcd.setCursor(0,1);
        lcd.print("F=");
        lcd.print(freq);
        lcd.print("Hz ");
        if(freq < 10)
            lcd.print("  ");
        else if(freq < 100)
            lcd.print(" ");
        pre_freq = freq;
        time[on_time] = (duty_cycle * (1/freq) * 10000); //u_sec
        time[off_time] = ((1/freq) * 1000000) - time[on_time]; //u_sec
        Led_ON;
        state = true;
    }
    
    if(duty_cycle!=pre_duty_cycle)
    {
        lcd.setCursor(10,1);
        lcd.print(" D=");
        if(duty_cycle < 10)
            lcd.print("0");
        lcd.print(duty_cycle);
        lcd.print("%  ");
        pre_duty_cycle = duty_cycle;
        time[on_time] = (duty_cycle * (1/freq) * 10000); //u_sec
        time[off_time] = ((1/freq) * 1000000) - time[on_time]; //u_sec
        Led_ON;
        state = true;
    }

    if(micros() - Micros >= time[state])
    {
        state =! state;
        if(state)
            Led_ON;
        else
            Led_OFF;
        Micros = micros();
    }
}

unsigned long preMillis1,preMillis2;

void multipleByTwo()
{
    if(millis() - preMillis1 > 250)
    {
        if((Freq * (multiple * 2)) <= (UpperFrequency * 2))
        {
            multiple_freq = Freq;
            multiple *= 2;
        }
        preMillis1 = millis();
    }
}

void checkWhichPin()
{
    if(millis() - preMillis2 > 250)
    {
        if(digitalRead(BtnUp) == HIGH)
        {
            if((Freq * (multiple / 2)) >= LowerFrequency)
            {
                multiple_freq = Freq;
                multiple /= 2;
            }
        }
        else
        {
            set_precesion =! set_precesion;
        }
        preMillis2 = millis();
    }
}
