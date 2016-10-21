#include <RFduinoBLE.h>

int dim = 2;
int cct = 3;
int hue = 4;

int button = 5;

int debounce_time = 10;
int debounce_timeout = 100;

uint8_t advertise[] =
{
  0x09,  // length
  0x09,  // complete local name type
  0x41,  // 'A'
  0x4C,  // 'L'
  0x44,  // 'D'
  0x2D,  // '-'
  0x30,  // '0'
  0x30,  // '0'
  0x30,  // '0'
  0x31,  // '1'
  
  0x02,  // length
  0x01,  // flags type
  0x06,  // le general discovery mode | br edr not supported

  0x02,  // length
  0x0A,  // tx power level
  0x04,  // +4dBm
};

void setup() {
  //RFduinoBLE_advdata = advertise;
  //RFduinoBLE_advdata_len = sizeof(advertise);
  
  pinMode(dim, OUTPUT);
  pinMode(cct, OUTPUT);
  pinMode(hue, OUTPUT);

  // button press will be shown on the iPhone app)
  pinMode(button, INPUT);

  RFduinoBLE.deviceName = "ALD-0001";
  RFduinoBLE.advertisementData = "10v-dch";
  RFduinoBLE.advertisementInterval = 250;
  RFduinoBLE.txPowerLevel = +4;
  RFduinoBLE.begin();
  
  //Serial.begin(9600);
  //Serial.println("started");
  
}

int debounce(int state)
{
  int start = millis();
  int debounce_start = start;
  
  while (millis() - start < debounce_timeout)
    if (digitalRead(button) == state)
    {
      if (millis() - debounce_start >= debounce_time)
        return 1;
    }
    else
      debounce_start = millis();

  return 0;
}

int delay_until_button(int state)
{
  // set button edge to wake up on
  if (state)
    RFduino_pinWake(button, HIGH);
  else
    RFduino_pinWake(button, LOW);
    
  do
    // switch to lower power mode until a button edge wakes us up
    RFduino_ULPDelay(INFINITE);
  while (! debounce(state));
  
  // if multiple buttons were configured, this is how you would determine what woke you up
  if (RFduino_pinWoke(button))
  {
    // execute code here
    RFduino_resetPinWake(button);
  }
}

void loop() {
  delay_until_button(HIGH);
  RFduinoBLE.send(1);
  
  delay_until_button(LOW);
  RFduinoBLE.send(0);
}

void RFduinoBLE_onDisconnect()
{
  analogWrite(dim, 0);
}

void RFduinoBLE_onReceive(char *data, int len)
{
  int val = 0;
  
  if (len == 4) {
    String valstr;
    valstr += data[1];
    valstr += data[2];
    valstr += data[3];
    int val = valstr.toInt();
    if (data[0] == 'd') {
      analogWrite(dim, val);
    } else if (data[0] == 'c') {
      analogWrite(cct, val);
    } else if (data[0] == 'h') {
      analogWrite(hue, val);
    }
  }
}
