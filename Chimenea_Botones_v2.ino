// Define pin connections
#define dirPin 2
#define stepPin 3
#define openButton 4
#define closeButton 5
#define closedLimitSwitch 6 // top limit switch corresponds to closed door
#define openLimitSwitch 7   // bottom limit switch corresponds to open door
#define IsOpenLED 8
#define IsClosedLED 9
#define enable 10
#define ESPsignal 11

boolean openPressed = false;
boolean closePressed = false;
boolean IsOpen = false;
boolean IsClosed = false;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(IsOpenLED, OUTPUT);
  pinMode(IsClosedLED, OUTPUT);

  pinMode(openButton, INPUT_PULLUP);        // open chimney door
  pinMode(closeButton, INPUT_PULLUP);       // close chimney door
  pinMode(closedLimitSwitch, INPUT_PULLUP); // goes to limit switch at the top
  pinMode(openLimitSwitch, INPUT_PULLUP);   // goes to limit switch at bottom
  pinMode(ESPsignal, INPUT_PULLUP);         // ESP trigger signal to close chimney
}

void loop() {
  digitalWrite(enable, HIGH); // stepper is OFF, only ON if it has to move
  readButtons();
  readESPsignal();
  actOnButtons();
}

void readButtons() {
  closePressed = false; openPressed = false;
  if (digitalRead(openButton) == LOW) {
    openPressed = true;
    btnBounce(openButton);
  }
  if (digitalRead(closeButton) == LOW) {
    closePressed = true;
    btnBounce(closeButton);
  }
}

void btnBounce(int btnPin) {
  while (digitalRead(btnPin) == LOW);
  delay(20);
}

void actOnButtons() {
  readStates();
  if (openPressed == true) { 
    //OPEN();
    digitalWrite(enable, LOW);
    digitalWrite(dirPin, HIGH);
    
    while (IsOpen == false) {
      STEP();
      if( closePressed == true || openPressed == true) {return;}      
    }
  }
  
  if (closePressed == true) {
    //CLOSE();
    digitalWrite(enable, LOW);
    digitalWrite(dirPin, LOW);
    
    while (IsClosed == false) {
      STEP();
      if( closePressed == true || openPressed == true) {return;}
    }       
  }
  
}

void STEP() {
  // Spin the stepper motor
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(700);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(700);
  readStates();
  readButtons();
}

void readStates() {
  IsOpen = false;
  IsClosed = false;
  digitalWrite(IsOpenLED, LOW);
  digitalWrite(IsClosedLED, LOW); 
   
  if (digitalRead(openLimitSwitch) == LOW) {
    IsOpen = true;
    digitalWrite(IsOpenLED, HIGH);
    digitalWrite(IsClosedLED, LOW);
  }
  if (digitalRead(closedLimitSwitch) == LOW) {
    IsClosed = true;
    digitalWrite(IsClosedLED, HIGH);
    digitalWrite(IsOpenLED, LOW);
  }
}

void readESPsignal() {
  if (digitalRead(ESPsignal) == LOW) {
    delay(100);
    if (digitalRead(ESPsignal) == HIGH) {
      delay(50);
      if (digitalRead(ESPsignal) == LOW) {
        delay(50);
        if (digitalRead(ESPsignal) == HIGH) openPressed = true;
      }
    }
  }
  //while (digitalRead(ESPsignal) == LOW); // hold while it's LOW
  //delay(20); // avoid EMI bounce?
}
