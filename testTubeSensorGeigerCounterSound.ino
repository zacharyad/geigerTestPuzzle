#include <JC_Button.h>

//int isDebug = true;
int isDebug = false;

//button
ToggleButton playSoundButton(2);
bool toggledOn = false;

//Pin Vars
int speakerPin = 4;
int hallSensorPin1 = A6;
int hallSensorPin2 = A5;
int light = 3;
int vial1 = A1;
int vial2 = A2;
int vial3 = A3;
int vial4 = A4;

//Speeds assigned to each MagForm +/-. SpeedOne = slowest, speedFour = fastest
int speedOne = 250;
int speedTwo = 130;
int speedThree = 90;
int speedFour = 20;

int relayPin = 12;

//Global vars
int resetCount = 0;
// State of the sensor reading dictating speed of geiger output to speaker.
int state = 0;
int vialState = 0;

void success(){
  if(isDebug) {Serial.println("success: In setup");}
  //trip relay for 12v Lock
  digitalWrite(relayPin, HIGH);
  delay(5000);
  digitalWrite(relayPin, LOW);
}

void setup() {
  if(isDebug) {Serial.println("Setup: Start");}
  
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  digitalWrite(relayPin, LOW);
  
  pinMode(vial1, INPUT);
  pinMode(vial2, INPUT);
  pinMode(vial3, INPUT);
  pinMode(vial4, INPUT);
  
  pinMode(speakerPin, OUTPUT);
  pinMode(hallSensorPin1, INPUT);
  pinMode(hallSensorPin2, INPUT);
  
  pinMode(light, OUTPUT);
  
  delay(1000);
  if(isDebug) {Serial.println("Sensor Calibrated");}
  if(isDebug) {Serial.println("Setup: End");}

  playSoundButton.begin();
}


void loop() {
  if(playSoundButton.read()){
    playSoundButton.toggleState();
    state = 0;
    SM_GiegerVials();
  } else {
    vialState = 0;
    digitalWrite(light, HIGH);
//    resetCount = 0
    SM_Gieger();
  }
}

// Speed of the geiger sound func
void giegerSoundLoop(int intensity){
  if(isDebug) {Serial.println("giegerSoundLoop: Start");}
    int randomSpeed = random(intensity - 20, intensity + 30);
    
    analogWrite(speakerPin, 255);
    delay(randomSpeed);
    analogWrite(speakerPin, 0);
    delay(randomSpeed);

    if(intensity == speedTwo && playSoundButton.pressedFor(15000)){
          resetCount++;
          Serial.println("D");
          success();
          resetCount = 0;
    }
}

void SM_GiegerVials(){
  switch(vialState){
    case 0:
    if(isDebug){Serial.println("vialState ZERO");}
    digitalWrite(light, LOW);
    vialState = 1;
    break;
    
    case 1:
    int high = 350; //less than this
    int medHigh = 440; // // apply deviation above this to get range
    int medLow = 564; // apply deviation above this to get range
    int low = 575; // higher than this
    int dev = 15;

    int vialPlace1 = analogRead(vial1);
    int vialPlace2 = analogRead(vial2);
    int vialPlace3 = analogRead(vial3);
    int vialPlace4 = analogRead(vial4);

    bool vial1Placed = false;
    bool vial2Placed = false;
    bool vial3Placed = false;
    bool vial4Placed = false;

    if(vialPlace1 < high){
      vial1Placed = true;
      Serial.println("high");
    } else {
      vial1Placed = false;
    }

    if (vialPlace2 > medHigh - dev && vialPlace2 < medHigh + dev){
      vial2Placed = true;
      Serial.println("medHigh");
    } else {
      vial2Placed = true;
    }

    if (vialPlace3 > medLow - dev && vialPlace3 < medLow + dev){
      vial3Placed = true;
      Serial.println("medLow");
    } else {
      vial3Placed = false;
    }

    if (vialPlace4 > low){
      vial4Placed = true;
      Serial.println("low");
    } else {
      vial4Placed = false;
    }
 
    if(vial1Placed && vial2Placed && vial3Placed && vial4Placed){
      vial1Placed = false;
      vial2Placed = false;
      vial3Placed = false;
      vial4Placed = false;
      
      success();
      vialState = 2;
    } else {
      if(isDebug){ Serial.println("still Not correctYet"); }
    }
    
    break;
    
    case 2:
    delay(3000);
    vialState = 0;
    break;
    
    default:
    break; 
  }

delay(50);
  
}

//State Machine for reading sensor and picking state to play geiger sound on repeat untill new state is read from sensor
void SM_Gieger(){
  // reading sensor to determine speed/state
  int reading1 = (514 - analogRead(hallSensorPin1));
  int reading2 = (514 - analogRead(hallSensorPin2));
 
  if(isDebug) {Serial.println(reading1);}

  if(reading1 != reading2){
    if(reading1 < -150){
      reading1 = 0;
    } 
    if(reading2 < -150){
      reading2 = 0;
    } 
  }

  if (reading1 > 101 || reading2 > 101 ){
    if(isDebug) {Serial.println("Magnet is High");}
    state = 4;
  }

  
  if(reading1 > 10 && reading1 <= 100 || reading2 > 10 && reading2 <= 100){
    if(isDebug) {Serial.println("Magnet is Medium-High");}
    state = 3;
  }
  
  if (reading1 < -9 && reading1 >= -40){
     if(isDebug) {Serial.println("Magnet is Medium-Low");}
     state = 2;
  } 
  
  if(reading1 < -24){
    if(isDebug) {Serial.println("Magnet is Low");}
    state = 1;
  }

  if(reading1 >= -10 && reading1 <= 10){
    if(isDebug) {Serial.println("Magnet is Neutral, no sound");}
    state = 0;
  }
  
// State Management
  switch(state){
    case 0:
      if(isDebug) {Serial.println("State 0");}
      digitalWrite(light, HIGH);
      delay(100);
    break;
    
    case 1:
      if(isDebug) {Serial.println("State 1");}
      giegerSoundLoop(speedOne);
    break;
    
    case 2:
      if(isDebug) {Serial.println("State 2");}
      giegerSoundLoop(speedTwo);
    break;
    
    case 3:
      if(isDebug) {Serial.println("State 3");}
      giegerSoundLoop(speedThree);
    break;
    case 4:
      if(isDebug) {Serial.println("State 4");}
      giegerSoundLoop(speedFour);
    break;
    
    default:
      if(isDebug) {Serial.println("ERROR: State is out of bounds");}
    break;
  }
}
