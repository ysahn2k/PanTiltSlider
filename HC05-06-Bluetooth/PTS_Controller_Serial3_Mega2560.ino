// Pin Assignment
#define swPinH1V1 23
#define swPinH2V1 29
#define swPinH1V2 31
#define swPinH2V2 33
#define swPinH1V3 39
#define swPinH2V3 45
#define swPinH1V4 22
#define swPinH2V4 28
#define swPinH1V5 30
#define swPinH2V5 36
#define swPinH1V6 38
#define swPinH2V6 44
#define swPinH3V1 49
#define swPinH3V2 53
#define swPinH3V3 46
#define swPinH3V4 50

// Instruction Assignment
#define INSTRUCTION_BYTES_SLIDER_PAN_TILT_SPEED 4
#define INSTRUCTION_STEP_MODE 'm'
#define INSTRUCTION_PAN_DEGREES 'p'
#define INSTRUCTION_TILT_DEGREES 't'
#define INSTRUCTION_ENABLE 'e'
#define INSTRUCTION_SET_PAN_SPEED 's'
#define INSTRUCTION_SET_TILT_SPEED 'S'
#define INSTRUCTION_INVERT_PAN 'i'
#define INSTRUCTION_INVERT_TILT 'I'
#define INSTRUCTION_SET_PAN_HALL_OFFSET 'o'
#define INSTRUCTION_SET_TILT_HALL_OFFSET 'O'
#define INSTRUCTION_SET_HOMING 'H'
#define INSTRUCTION_TRIGGER_SHUTTER 'c'
#define INSTRUCTION_AUTO_HOME 'A'
#define INSTRUCTION_DEBUG_STATUS 'R'
#define INSTRUCTION_EXECUTE_MOVES ';'
#define INSTRUCTION_ADD_POSITION '#'
#define INSTRUCTION_STEP_FORWARD '>'
#define INSTRUCTION_STEP_BACKWARD '<'
#define INSTRUCTION_JUMP_TO_START '['
#define INSTRUCTION_JUMP_TO_END ']'
#define INSTRUCTION_EDIT_ARRAY 'E'
#define INSTRUCTION_ADD_DELAY 'd'
#define INSTRUCTION_EDIT_DELAY 'D'
#define INSTRUCTION_CLEAR_ARRAY 'C'
#define INSTRUCTION_SAVE_TO_EEPROM 'U'
#define INSTRUCTION_PANORAMICLAPSE 'L'
#define INSTRUCTION_ANGLE_BETWEEN_PICTURES 'b'
#define INSTRUCTION_DELAY_BETWEEN_PICTURES 'B'
#define INSTRUCTION_TIMELAPSE 'l'
#define INSTRUCTION_SLIDER_MILLIMETRES 'x'
#define INSTRUCTION_INVERT_SLIDER 'j'
#define INSTRUCTION_SET_SLIDER_SPEED 'X'
#define INSTRUCTION_ORIBIT_POINT '@'
#define INSTRUCTION_CALCULATE_TARGET_POINT 'T'
#define INSTRUCTION_ACCEL_ENABLE 'a'
#define INSTRUCTION_PAN_ACCEL_INCREMENT_DELAY 'q'
#define INSTRUCTION_TILT_ACCEL_INCREMENT_DELAY 'Q'
#define INSTRUCTION_SLIDER_ACCEL_INCREMENT_DELAY 'w'
#define INSTRUCTION_SCALE_SPEED 'W'

bool DEBUG = true;

const int XdeadRangeLow  = 511 - 30;
const int XdeadRangeHigh = 512 + 30;
const int YdeadRangeLow  = 511 - 30;
const int YdeadRangeHigh = 512 + 30;
const int ZdeadRangeLow  = 511 - 30;
const int ZdeadRangeHigh = 512 + 30;

int X_max = 1023;
int Y_max = 1023;
int Z_max = 1023;

int out_min = -1023;
int out_max = 1023;

bool firstConnect = true;

short shortVals[3] = {0, 0, 0};
short XShort = 0;
short YShort = 0;
short ZShort = 0;
short oldShortVal0 = 0;
short oldShortVal1 = 0;
short oldShortVal2 = 0;

int joyXread;
int joyX;
int joyYread;
int joyY;
int joyZread;
int joyZ;



int DebounceTime = 20;

void setup() {
  Serial.begin(57600);
  Serial3.begin(57600);
  
  pinMode(swPinH1V1, INPUT_PULLUP);
  pinMode(swPinH2V1, INPUT_PULLUP);
  pinMode(swPinH1V2, INPUT_PULLUP);
  pinMode(swPinH2V2, INPUT_PULLUP);
  pinMode(swPinH1V3, INPUT_PULLUP);
  pinMode(swPinH2V3, INPUT_PULLUP);
  pinMode(swPinH1V4, INPUT_PULLUP);
  pinMode(swPinH2V4, INPUT_PULLUP);
  pinMode(swPinH1V5, INPUT_PULLUP);
  pinMode(swPinH2V5, INPUT_PULLUP);
  pinMode(swPinH1V6, INPUT_PULLUP);
  pinMode(swPinH2V6, INPUT_PULLUP);
  pinMode(swPinH3V1, INPUT_PULLUP);
  pinMode(swPinH3V2, INPUT_PULLUP);
  pinMode(swPinH3V3, INPUT_PULLUP);
  pinMode(swPinH3V4, INPUT_PULLUP);
}

void loop() {

  // Calculate Joystick
  joyXread = analogRead(A0);
  if (joyXread < XdeadRangeLow) {
    XShort = map(joyXread, 0, XdeadRangeLow, out_min, 0);
  }
  else if (joyXread > XdeadRangeHigh) {
    XShort = map(joyXread, XdeadRangeHigh, X_max, 0, out_max);
  }
  else {
    XShort = 0;                                               // deadzone around center value
  }

  joyYread = analogRead(A1);
  if (joyYread < YdeadRangeLow) {
    YShort = map(joyYread, 0, YdeadRangeLow, out_min, 0);
  }
  else if (joyYread > YdeadRangeHigh) {
    YShort = map(joyYread, YdeadRangeHigh, Y_max, 0, out_max);
  }
  else {
    YShort = 0;                                               // deadzone around center value
  }

  joyZread = analogRead(A2);
  if (joyZread < ZdeadRangeLow) {
    ZShort = map(joyZread, 0, ZdeadRangeLow, out_min, 0);
  }
  else if (joyZread > ZdeadRangeHigh) {
    ZShort = map(joyZread, ZdeadRangeHigh, Z_max, 0, out_max);
  }
  else {
    ZShort = 0;                                               // deadzone around center value
  }

  shortVals[0] = ZShort;
  shortVals[1] = XShort;
  shortVals[2] = YShort;

  // Send Joystick & Left / Right
  if (shortVals[0] != oldShortVal0 || shortVals[1] != oldShortVal1 || shortVals[2] != oldShortVal2) {   // IF input has changed
    sendSliderPanTiltStepSpeed(INSTRUCTION_BYTES_SLIDER_PAN_TILT_SPEED, shortVals);                     // Send the combned values
    
    oldShortVal0 = shortVals[0];      // Store as old values
    oldShortVal1 = shortVals[1];      // Store as old values
    oldShortVal2 = shortVals[2];      // Store as old values

    //delay(20);

    if ( DEBUG ) {
      Serial.print(XShort, DEC);
      Serial.print(" - ");
      Serial.print(YShort, DEC);
      Serial.print(" - ");
      Serial.println(ZShort);
    }
  }  
  
  // Buttons

  if(digitalRead(swPinH1V1) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH1V1) == LOW) {
      sendCharArray((char *)"#"); // ADD Position
    }
    while(digitalRead(swPinH1V1) == LOW) {    
    }
  }

  if(digitalRead(swPinH1V2) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH1V2) == LOW) {
      sendCharArray((char *)"E"); // EDIT Array
    }
    while(digitalRead(swPinH1V2) == LOW) {    
    }
  }

  if(digitalRead(swPinH1V3) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH1V3) == LOW) {
      sendCharArray((char *)"@1"); // Orbit Point
    }
    while(digitalRead(swPinH1V3) == LOW) {    
    }
  }

  if(digitalRead(swPinH1V4) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH1V4) == LOW) {
      sendCharArray((char *)"U"); // Save to EEPROM
    }
    while(digitalRead(swPinH1V4) == LOW) {    
    }
  }

  if(digitalRead(swPinH1V5) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH1V5) == LOW) {
      sendCharArray((char *)"C"); // Clear Array
    }
    while(digitalRead(swPinH1V5) == LOW) {    
    }
  }

  if(digitalRead(swPinH1V6) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH1V6) == LOW) {
      sendCharArray((char *)";1"); // Execute moves
    }
    while(digitalRead(swPinH1V6) == LOW) {    
    }
  }

  if(digitalRead(swPinH2V1) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH2V1) == LOW) {
      sendCharArray((char *)"["); // Jump to START
    }
    while(digitalRead(swPinH2V1) == LOW) {    
    }
  }

  if(digitalRead(swPinH2V2) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH2V2) == LOW) {
      sendCharArray((char *)"<"); // Step BACKWARD
    }
    while(digitalRead(swPinH2V2) == LOW) {    
    }
  }

  if(digitalRead(swPinH2V3) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH2V3) == LOW) {
      sendCharArray((char *)">"); // Step FORWARD
    }
    while(digitalRead(swPinH2V3) == LOW) {    
    }
  }

  if(digitalRead(swPinH2V4) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH2V4) == LOW) {
      sendCharArray((char *)"]"); // Jump to END
    }
    while(digitalRead(swPinH2V4) == LOW) {    
    }
  }

  if(digitalRead(swPinH2V5) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH2V5) == LOW) {
      sendCharArray((char *)"l"); // Time Lapse
    }
    while(digitalRead(swPinH2V5) == LOW) {    
    }
  }

  if(digitalRead(swPinH2V6) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH2V6) == LOW) {
      sendCharArray((char *)"L"); // Panoramic Lapse
    }
    while(digitalRead(swPinH2V6) == LOW) {    
    }
  }

  if(digitalRead(swPinH3V1) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH3V1) == LOW) {
      sendCharArray((char *)"H"); // Set Home
    }
    while(digitalRead(swPinH3V1) == LOW) {    
    }
  }

  if(digitalRead(swPinH3V2) == LOW) {
    delay(DebounceTime);
    if(digitalRead(swPinH3V2) == LOW) {
      sendCharArray((char *)"A"); // Auto Home
    }
    while(digitalRead(swPinH3V2) == LOW) {    
    }
  }
}

//--- Functions ---------------------------------------------------------------------------
void sendCharArray(char *array) {
  //Serial3.write((uint8_t)array);
  Serial3.write(array, (int)strlen(array));
  
  if ( DEBUG ) {
    Serial.println((char)*array);
  }
}

void sendSliderPanTiltStepSpeed(int command, short * arr) {
  byte data[7];                               // Data array to send

//  data[0] = command;
//  data[1] = (arr[0] >> 8); // Gets the most significant byte
//  data[2] = (arr[0] & 0xFF); // Gets the second most significant byte
//  data[3] = (arr[1] >> 8);
//  data[4] = (arr[1] & 0xFF);
//  data[5] = (arr[2] >> 8);
//  data[6] = (arr[2] & 0xFF); // Gets the least significant byte

  // To match the format of Send2ByteNumber(MIT APP INVENTOR) exchanged the MSB(8bytes) and the LSB(8bytes)
  data[0] = command;
  data[2] = (arr[0] >> 8); // Gets the most significant byte
  data[1] = (arr[0] & 0xFF); // Gets the second most significant byte
  data[4] = (arr[1] >> 8);
  data[3] = (arr[1] & 0xFF);
  data[6] = (arr[2] >> 8);
  data[5] = (arr[2] & 0xFF); // Gets the least significant byte

  Serial3.write(data, sizeof(data)); // Send the command and the 6 bytes of data

  delay(50);

  if ( DEBUG ) {
    Serial.print(data[0]);
    Serial.print(data[1]);
    Serial.print(data[2]);
    Serial.print(data[3]);
    Serial.print(data[4]);
    Serial.print(data[5]);
    Serial.println(data[6]);
  }
}
