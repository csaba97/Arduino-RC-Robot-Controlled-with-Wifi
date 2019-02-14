
#include <Servo.h>
#include <Wire.h>
//slave

String inputString = ""; 
// Pinii motor 1
#define mpin00 6
#define mpin01 5
// Pinii motor 2
#define mpin10 11
#define mpin11 3
#define pinServo 8
Servo srv;

bool led_left_on = false;
bool led_right_on = false;
long timeLed = 0;
long steps = 0;
int led_left = 13;
int led_right = 9;
const int trigPin = 4;//9
const int echoPin = 12;//12

#define speedsSize 7
int speedsOrig[] = {0,40,70,80,90,120, 150};
int speeds[] = {0,40,70,80,90,120, 150};
int plus = 0;
volatile int lastLen = 0;
volatile int dist = 10;
volatile bool canPark = false;
float distance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  float distance= duration*0.034/2;
  return distance;
}

void up(int d, int speed)
{
  StartMotor (mpin00, mpin01, 0 , speeds[speed]+plus); 
  StartMotor (mpin10, mpin11, 0,  speeds[speed]); 
  delay (d);
}
void down(int d, int speed)
{
  StartMotor (mpin00, mpin01, 1 , speeds[speed]+plus); 
  StartMotor (mpin10, mpin11, 1,  speeds[speed]); 
  delay (d);
}
void left(int d, int speed)
{
  StartMotor (mpin00, mpin01, 1 , speeds[speed]+plus); 
  StartMotor (mpin10, mpin11, 0,  speeds[speed]); 
  delay (d);
}
void right(int d, int speed)
{
  StartMotor (mpin00, mpin01, 0 , speeds[speed]+plus); 
  StartMotor (mpin10, mpin11, 1,  speeds[speed]); 
  delay (d);
}

bool findSpace()
{
      Serial.println("find space"); 
       do{
        up(10,3);
        //Serial.println(distance());
       }while(distance() < 15);
       delayStopped(1);
      //free spcace
      //delay a little bit
      delay(2000);

      //go back until second box
      int len=0;
      do{
        len ++;
        up(10,3);
        //Serial.println(distance());
       }while(distance() > 15);
      delayStopped(1);
      Serial.print("len = ");
      Serial.println(len);
      lastLen = len;
      if(len > 50) return true;
        return false;
}

void parkCar(){
  int maxi = 0;
  while(findSpace()==false && maxi<4){
    maxi++;
  }
      Serial.println("space found"); 
      delayStopped(1);
      delay(500);
      
      down(350,3);
      delayStopped(1);
      delay(500);

      int rotate = 600;
      
      left(rotate,2);
      delayStopped(1);
      delay(500);

      
      down(600,3);
      delayStopped(1);
      delay(500);


      right(rotate,2);
      delayStopped(1);
      delay(500);
}

void setup() {
         //i2C 
       inputString.reserve(200); 
       Wire.begin(8);                /* join i2c bus with address 8 */
       Wire.onReceive(receiveEvent); /* register receive event */
       Wire.onRequest(requestEvent); /* register request event */
       //
       // configurarea pinilor motor ca iesire, initial valoare 0
       digitalWrite(mpin00, 0);
       digitalWrite(mpin01, 0);
       digitalWrite(mpin10, 0);
       digitalWrite(mpin11, 0);
       pinMode(led_left,OUTPUT);
       pinMode(led_right,OUTPUT);
       pinMode (mpin00, OUTPUT);
       pinMode (mpin01, OUTPUT);
       pinMode (mpin10, OUTPUT);
       pinMode (mpin11, OUTPUT);
       pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
       pinMode(echoPin, INPUT);
       Serial.begin(9600);
       srv.attach(pinServo);
       srv.write(0);
       delay(1000); 
       srv.detach(); 
        
      
}
// Functie pentru controlul unui motor
// Intrare: pinii m1 si m2, directia si viteza
void StartMotor (int m1, int m2, int forward, int speed)
{

       if (speed==0) // oprire
       {
       digitalWrite(m1, 0); 
       digitalWrite(m2, 0);
       }
       else
       {
       if (forward)
       {
       digitalWrite(m2, 0);
       analogWrite (m1, speed); // folosire PWM
       }
       else
       {
       digitalWrite(m1, 0);
       analogWrite(m2, speed);
       }
       }
}
// Functie de siguranta
// Executa oprire motoare, urmata de delay
void delayStopped(int ms)
{
   StartMotor (mpin00, mpin01, 0, 0);
   StartMotor (mpin10, mpin11, 0, 0);
   delay(ms);
}


void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    if(c =='#'){//new command
      inputString = "";
    }
    else inputString+=c;
  }
 if(inputString == "output1on"){
     Serial.println("1on"); 
     up(1,4);
 }
 else if(inputString == "output1off"){
     Serial.println("1off"); 
     delayStopped(1);
 }
 else if(inputString == "output2on"){
     left(1,4);
     led_left_on = true;
     digitalWrite(led_left,HIGH);
     Serial.println("2on"); 
 }
 else if(inputString == "output2off"){
     led_left_on = false;
     digitalWrite(led_left,LOW);
     delayStopped(1);
     Serial.println("2off"); 
 }
 else if(inputString == "output3on"){
     led_right_on = true;
     digitalWrite(led_right,HIGH);
     right(1,4);
     Serial.println("3on"); 
 }
 else if(inputString == "output3off"){
     led_right_on = false;
     digitalWrite(led_right,LOW);
     delayStopped(1);
     Serial.println("3off"); 
 }
 else if(inputString == "output4on"){
     down(1,4);
     Serial.println("4on"); 
 }
 else if(inputString == "output4off"){
     delayStopped(1);
     Serial.println("4off"); 
 }
 else if(inputString == "park"){
    if(!canPark){
       Serial.println("park car start"); 
       canPark = true;
    }
 }
 else if(inputString.indexOf("speedDif") > -1){//this must be before "speed"
     inputString.replace("speedDif","");
     plus = inputString.toInt(); 
     Serial.print("speed difference changed to...."); 
     Serial.println(plus); 

 }
 else if(inputString.indexOf("speed") > -1){
     inputString.replace("speed","");
     int speedI = inputString.toInt(); 
     Serial.print("speed changed to...."); 
     Serial.println(speedI); 
     Serial.print("new speed array..."); 
     for(int i=0;i<speedsSize;i++){
        speeds[i] = int((speedI/50.0f)*speedsOrig[i]);
        if(speeds[i]>250)
          speeds[i] = 250;
        Serial.print(speeds[i]);
        Serial.print(" ");
     }
     Serial.println(); 
 }
 //Serial.println(inputString);             /* to newline */
 
}

// function that executes whenever data is requested from master
void requestEvent() {
 //
 //int len = 5;
 Wire.write(dist);
 Wire.write(lastLen);
}






void loop() {
  //blink led
  steps++;
  if(led_right_on){
    //Serial.print("millis()="); 
    //Serial.println(millis()); 
    //Serial.println(timeLed); 
    if(millis()-timeLed<600)
      digitalWrite(led_right,LOW); 
    else if(millis()-timeLed<1200)
      digitalWrite(led_right,HIGH);
    else timeLed = millis();
  }

  if(led_left_on){
    if(millis()-timeLed<600)
      digitalWrite(led_left,LOW); 
    else if(millis()-timeLed<1200)
      digitalWrite(led_left,HIGH);
    else timeLed = millis();
  }



   
  delay(50);
  if(steps>1){
      dist = (int)distance(); 
      if(canPark){
        parkCar(); 
        canPark = false;
        Serial.println("park car end"); 
      }

      steps = 0;
  }
} 
