// Using Teensy 3.0 and a Graupner GR12 with SUMD to have a USB Joystick interface
// Solder servo cable black to GND, red to VC USB, yellow signal to Pin 0
// Bind and configure GR12 or other receiver to send SUMD, connect servo cable to SUMD port
// Compile the sketch using a HID interface with joystick

// Uses very basic loop to read frames, no error correction

#define HWSERIAL Serial1

#define VENDOR_ID 0xA8 // Graupner Vendor ID
#define VALID_FRAME 0x01 // 0x01 is valid frame, 0x81 would be failsave, all others are garbage

byte incomingByte;
byte buf[2048]; // theoretical limit, 4+255*2   
int rcChannel[255];
int framePos = 0;
int numChannels = 255;
bool receiveFrame = false;
        
void setup() {
  // Serial.begin(9600);
  HWSERIAL.begin(115200);
}

void loop() {
              
  if (HWSERIAL.available() > 0) {
    incomingByte = HWSERIAL.read();

    // waiting for the start of a frame
    if (!receiveFrame && incomingByte == VENDOR_ID) {
      // start the frame
      framePos = 0;
      receiveFrame = true;
    } 
    // in a frame, capture data
    if (receiveFrame) {
      buf[framePos] = incomingByte;

      if (framePos == 1 && incomingByte != VALID_FRAME) {
        receiveFrame = false;
        numChannels = 255;
      }
      if (framePos == 2 ) {
        numChannels = incomingByte;
      }
      if (framePos == (3+numChannels*2+1)) {
        // we have a full packet.
        receiveFrame = false;

        // set channels
        for (int i=0; i<numChannels; i++) {
          rcChannel[i] = ((buf[3+2*i]<<8) + buf[3+2*i+1]) / 8;
        }  

        // set joysticks
        Joystick.Z(rcChannel[0]-1000);
        Joystick.Zrotate(rcChannel[1]-1000);

        Joystick.X(rcChannel[2]-1000);
        Joystick.Y(rcChannel[3]-1000);
        
        Joystick.sliderLeft(rcChannel[4]-1000);
        Joystick.sliderRight(rcChannel[5]-1000);
                       
      }
      framePos++;
    }
  }
}




