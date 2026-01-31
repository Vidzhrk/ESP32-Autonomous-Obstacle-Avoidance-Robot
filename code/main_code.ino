int motor_pin_A[4] = {23, 21, 14, 26};   // LF, LB, RF, RB
int motor_pin_B[4] = {22, 5, 27, 25};
int enable_pin[4]={19,18,32,33};
//int enable_pin[4]={0,1,2,3};
int current_Freq=500;
int duty_cycle=0;
int i;
int Y;
#define trig_front 2 // for ultrasonic sensor front 
#define echo_front 4
#define trig_back 15// for ultrasonic sensor back
#define echo_back 35
float time_front;
float distance_front;
float time_back;
float distance_back;
float distance_front_right;
float distance_front_left;
float distance_back_right;
float distance_back_left;
#define automatic_manual 34 //automatic to manual
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif
BluetoothSerial SerialBT;
#include <ESP32Servo.h>
Servo US1;
Servo US2;
String receivedChar;
int speed;
enum statem{
  IDLEM,
  FORWARDM,
  BACKWARDM,
  LEFTM,
  RIGHTM
 };
statem current_statem=IDLEM;
statem previous_statem=IDLEM;
enum statea{
  IDLE,
  MOVE_FORWARD,
  MOVE_BACKWARD,
  STOP,
  SUDDEN_STOP,
  LEFTA,
  RIGHTA
 };

enum MODE{
  MANUAL,
  AUTOMATIC
 };
volatile MODE input_mode=MANUAL;
statea current_statea=IDLE;
statea previous_statea=IDLE;
statea dirn=MOVE_FORWARD;
void IRAM_ATTR ISR_AutomaticToManual(){
  input_mode=MANUAL;
 }

void setup() {
  Serial.begin(9600);
  pinMode(trig_front,OUTPUT);
  pinMode(echo_front,INPUT);
  pinMode(trig_back,OUTPUT);
  pinMode(echo_back,INPUT);
  pinMode(automatic_manual, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(automatic_manual),ISR_AutomaticToManual,RISING); //buzzer
  for(i=0;i<4;i++){
    //ledcSetup(enable_pin[i], current_Freq, 8); 
    //ledcAttachPin(enable_pin[i], enable_pin[i]); 
    ledcAttach(enable_pin[i], current_Freq, 8);
    pinMode(motor_pin_A[i], OUTPUT);
    pinMode(motor_pin_B[i], OUTPUT);
    pinMode(enable_pin[i], OUTPUT);
  }
  SerialBT.begin("carrr");// set your willing name
  SerialBT.println("Choose Mode:");
  SerialBT.println("Press left buzzer for Manual");
  SerialBT.println("Press right buzzer for Automatic");
  SerialBT.println("Press the buzzers anytime to switch");
  delay(2000);
  SerialBT.setTimeout(2000);
  US1.attach(12);//gpio pin where servo is connected to
  US1.write(90); 
  US2.attach(13);//gpio pin where servo is connected to
  US2.write(90); 
 }
void obstacle_detected(){
 if (dirn==MOVE_FORWARD){
  ultrasonic_sensor();
  dirn=MOVE_BACKWARD;
  unsigned long t0 = millis();
  while((distance_front<10)&&(distance_back>10)&& ((millis() - t0) < 800)){
    move_backward(127);
    ultrasonic_sensor();
  }
  apply_brake();
  dirn=MOVE_FORWARD;
  
  US1.write(60);
  ultrasonic_sensor();
  distance_front_right=distance_front;
  US1.write(120);
  ultrasonic_sensor();
  distance_front_left=distance_front;
  US1.write(90);
  ultrasonic_sensor();
  if(distance_front_right>=distance_front_left){
    previous_statea=current_statea;
    current_statea=RIGHTA;
  }
  else if(distance_front_right<distance_front_left){
    previous_statea=current_statea;
    current_statea=LEFTA;
  }
  }
  else if (dirn==MOVE_BACKWARD){
  ultrasonic_sensor();
  dirn=MOVE_FORWARD;
  unsigned long t0 = millis();
  while((distance_front>10)&&(distance_back<10)&& ((millis() - t0) < 800)){
    move_forward(127);
    ultrasonic_sensor();
  }
  apply_brake();
  dirn=MOVE_BACKWARD;
  US2.write(60);
  delay(20);
  ultrasonic_sensor();
  distance_back_right=distance_back;
  US2.write(120);
  delay(20);
  ultrasonic_sensor();
  distance_back_left=distance_back;
  US2.write(90);
  ultrasonic_sensor();
  if(distance_back_right>=distance_back_left){
    previous_statea=current_statea;
    current_statea=RIGHTA;
  }
  else if(distance_back_right<distance_back_left){
    previous_statea=current_statea;
    current_statea=LEFTA;
  }
  }
 }
void move_forward(int duty_cycle){   //to go forward with x speed
  for (i=0;i<4;i++){
    digitalWrite(motor_pin_A[i],HIGH);
    digitalWrite(motor_pin_B[i],LOW);
    ledcWrite(enable_pin[i], duty_cycle);
  }
  }
void move_backward(int duty_cycle){
  for (i=0;i<4;i++){
    digitalWrite(motor_pin_A[i],LOW);
    digitalWrite(motor_pin_B[i],HIGH);
    ledcWrite(enable_pin[i], duty_cycle);
  }
  }
void turn_left(){
  for(i=0;i<2;i++){
    digitalWrite(motor_pin_A[i], LOW);
    digitalWrite(motor_pin_B[i], HIGH);
    ledcWrite(enable_pin[i], duty_cycle);
  }
  for(i=2;i<4;i++){
    digitalWrite(motor_pin_A[i], HIGH);
    digitalWrite(motor_pin_B[i], LOW);
    ledcWrite(enable_pin[i], duty_cycle);
  }
  }
void turn_right(){
    for(i=0;i<2;i++){
      digitalWrite(motor_pin_A[i], HIGH);
      digitalWrite(motor_pin_B[i], LOW);
      ledcWrite(enable_pin[i], duty_cycle);
    }
    for(i=2;i<4;i++){
      digitalWrite(motor_pin_A[i], LOW);
      digitalWrite(motor_pin_B[i], HIGH);
      ledcWrite(enable_pin[i], duty_cycle);
    }
  }
void apply_brake(){
  for (i=0;i<4;i++){
    ledcWrite(enable_pin[i], 0);
    digitalWrite(motor_pin_A[i],HIGH);
    digitalWrite(motor_pin_B[i],HIGH);
  }
 }
void sudden_stop(){
  for (i=0;i<4;i++){
    ledcWrite(enable_pin[i], 0);
    digitalWrite(motor_pin_A[i],HIGH);
    digitalWrite(motor_pin_B[i],HIGH);
  }
 }  
void ultrasonic_sensor(){
  digitalWrite(trig_front,LOW);
  delayMicroseconds(2);
  digitalWrite(trig_front,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_front,LOW);
  time_front= pulseIn(echo_front,HIGH,30000);//  30ms timeout
  distance_front=(time_front*0.0340)/2; // in cm
  digitalWrite(trig_back,LOW);
  delayMicroseconds(2);
  digitalWrite(trig_back,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_back,LOW);
  time_back= pulseIn(echo_back,HIGH,30000);//30ms timeout
  distance_back=(time_back*0.0340)/2; // in cm
 }
void SpeedControlInAutomaticForward(float distance_front){
  if (distance_front<=2){
    previous_statea=current_statea;
    current_statea=STOP;
    obstacle_detected();
    return;
  }
  if ((distance_front>2)&&(distance_front<=5)){
    previous_statea=current_statea;
    current_statea=STOP;
    obstacle_detected();
    return;
  }
  if ((distance_front>5)&&(distance_front<=10)){
    for (i=0;i<4;i++)
    {ledcWrite(enable_pin[i], 50);}
    previous_statea=current_statea;
    return;
    
  }
  if ((distance_front>10)&&(distance_front<=20)){
    for (i=0;i<4;i++)
    {ledcWrite(enable_pin[i], 100);}
    return;
  }
  if(distance_front>20){
    previous_statea=current_statea;
    current_statea=MOVE_FORWARD;
    return;
  }
  
 }
void SpeedControlInAutomaticBackward(float distance_back){
  if (distance_back<=2){
    previous_statea=current_statea;
    current_statea = STOP;
    obstacle_detected();
    return;
  }
  if ((distance_back>2)&&(distance_back<=5)){
    previous_statea=current_statea;
    current_statea = STOP;
    obstacle_detected();
    return;
  }
  if ((distance_back>5)&&(distance_back<=10)){
    for (i=0;i<4;i++)
    {ledcWrite(enable_pin[i], 50);}
    previous_statea=current_statea;
    return;
  }
  if ((distance_back>10)&&(distance_back<=20)){
    for (i=0;i<4;i++)
    {ledcWrite(enable_pin[i], 100);}
    previous_statea=current_statea;
    return;
  }
  if(distance_back>20){
    previous_statea=current_statea;
    current_statea=MOVE_BACKWARD;
    return;
  }
  
 }
int safe_to_move(){
  if(dirn==MOVE_FORWARD){
    if(distance_front>5){
      return 1;
    }
    else{
      return 0;
    }
  }
  else if(dirn==MOVE_BACKWARD){
    if(distance_back>5){
      return 1;
    }
    else{
      return 0;
    }
  }
  else{
    return 0;
  }
  
 }
void automatic_mode(){
  
  switch(current_statea){
        case IDLE:
          SerialBT.println("enter speed in percentage:");
          if(SerialBT.available()>0){
           Y=SerialBT.parseInt();
           duty_cycle=map(Y,0,100,0,255);
          }
          delay(5000);
          //direction
          SerialBT.println("Choose Direction:");
          SerialBT.println("Enter 0 for Forward");
          SerialBT.println("Enter 1 Backward");
          delay(5000);
          if (SerialBT.available()>0){
            Y=SerialBT.parseInt();
            if(Y==0){
              previous_statea=current_statea;
              current_statea=MOVE_FORWARD;
              }
            if(Y==1){
              previous_statea=current_statea;
              current_statea=MOVE_BACKWARD;
              }
          }
          break;
        
        case MOVE_FORWARD:
          ultrasonic_sensor();
          dirn=MOVE_FORWARD;
          if(safe_to_move()==1){
            move_forward(duty_cycle);
            if ((time_front==0) || (time_back==0) ){//timeout
              current_statea=SUDDEN_STOP;
            }
            SpeedControlInAutomaticForward(distance_front);
          }
          else{
            previous_statea=current_statea;
            current_statea=STOP;
          }

          break;

        case MOVE_BACKWARD:
          ultrasonic_sensor();
          dirn=MOVE_BACKWARD;
          if(safe_to_move()==1){
            move_backward(duty_cycle);
            if ((time_front==0) || (time_back==0) ){//timeout
              previous_statea=current_statea;
              current_statea=SUDDEN_STOP;
            }
            SpeedControlInAutomaticBackward(distance_back);
          }
          else{
            previous_statea=current_statea;
            current_statea=STOP;
          }
          break;
        case LEFTA:
          ultrasonic_sensor();
          turn_left();
          delay(80);
          apply_brake();
          previous_statea=current_statea;
          current_statea=dirn;
          break;
        case RIGHTA:
          ultrasonic_sensor();
          turn_right();delay(80);
          apply_brake();
          previous_statea=current_statea;
          current_statea=dirn;
          break;
        case SUDDEN_STOP:
          sudden_stop();
          current_statea=IDLE;
          break;

        case STOP:{
          apply_brake();
          ultrasonic_sensor();
          int K=safe_to_move();
          if (K==1){
            current_statea=previous_statea;
          }
          static unsigned long lastObs = 0;
          if (K == 0 && millis() - lastObs > 500){
            lastObs = millis();
            obstacle_detected();
          }
          break;
        }
        }
      }

void manual_mode(){
    apply_brake();
    if (SerialBT.available()>0){ 
      receivedChar=SerialBT.readString();
      // beacause data is sent as 8 bit ascii code so we convert into char to get the og one
      receivedChar.trim();//removes \r \n...
    }
    if(receivedChar=="a"){//move forward(set on bluetooth app)
      previous_statem=current_statem;
      current_statem=FORWARDM;
    }
    else if(receivedChar=="b"){//move backward(set on bluetooth app)
      previous_statem=current_statem;
      current_statem=BACKWARDM;
    }
    else if(receivedChar=="c"){//brake
      apply_brake();
      previous_statem=current_statem;
      current_statem=IDLEM;
    }
    else if(receivedChar == "l"){
      previous_statem=current_statem;
      current_statem=LEFTM;
      
    }
    else if(receivedChar == "r"){
      previous_statem=current_statem;
      current_statem=RIGHTM;
      
    }
  
    else if (receivedChar.toInt() > 0){
      speed=receivedChar.toInt();
      duty_cycle=map(speed,0,100,0,255);
      for (i = 0; i < 4; i++) {
        ledcWrite(enable_pin[i], duty_cycle);
      }
        
    }
    switch(current_statem){
      case IDLEM:
        Serial.println("Use the keys in blueetooth app to navigate");
        return;
      case FORWARDM:
        ultrasonic_sensor();//safety
        if(distance_front<5){
          apply_brake();
          return;
        }
        for (i=0;i<2;i++){
          digitalWrite(motor_pin_A[i],HIGH);
          digitalWrite(motor_pin_B[i],LOW);
          ledcWrite(enable_pin[i], duty_cycle);
        }
        for (i=2;i<4;i++){
          digitalWrite(motor_pin_A[i],LOW);
          digitalWrite(motor_pin_B[i],HIGH);
          ledcWrite(enable_pin[i], duty_cycle);
        }
        break;
      case BACKWARDM:
        ultrasonic_sensor();//safety
        if(distance_back<5){
          apply_brake();
          return;
        }
        for (i=0;i<2;i++){
          digitalWrite(motor_pin_A[i],LOW);
          digitalWrite(motor_pin_B[i],HIGH);
          ledcWrite(enable_pin[i], duty_cycle);
        }
        for (i=2;i<4;i++){
          digitalWrite(motor_pin_A[i],HIGH);
          digitalWrite(motor_pin_B[i],LOW);
          ledcWrite(enable_pin[i], duty_cycle);
        }
        break;
      case LEFTM:
        turn_left();
        break;
      case RIGHTM:
        turn_right();
        break;
    }
  }
void loop() {
  if(automatic_manual==1){
    input_mode==MANUAL;
  }
  else{
    input_mode=AUTOMATIC;
  }
  if (input_mode==AUTOMATIC){
    automatic_mode();
  } 
  if(input_mode==MANUAL){
    manual_mode();
  }
 }

