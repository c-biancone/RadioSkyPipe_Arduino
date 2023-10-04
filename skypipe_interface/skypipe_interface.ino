// Skeleton sketch for UDS connection to Radio-SkyPipe 
// there are a lot of commented out print statements that
// can be helpful in debugging.

// modified by Will Wright for use in the H1 Radio Telescope project for RIT MSD (Spring-2023 to Fall-2023)
// currently operates using 4 channels as opposed to the original 1 channel
// mofified by Chris Biancone for use on STM Nucleo F302R8 with ADS1115 for data acquisition
// and DS28B20 temperature sensor

// ADC CH1:


#include <Adafruit_ADS1X15.h>
#include <Wire.h>

// ADC
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// setup ADC variables
int16_t adc0, adc1, adc2, adc3;
float volts0, volts1, volts2, volts3;

// temp
int DSPIN = A0;  // temp sensor 1-wire pin
double temp = 0;

// SkyPipe
int POLL; // if =1 then data is polled by RSP using a GETD command
int STAT; // -1 = we were just stopped by a KILL command 0 = startup state 1 = INIT rcvd 2 = Ready to Go 3= Running

void setup() {
  // i2c bus
  //Wire.setSCL(PB_8);
  //Wire.setSDA(PB_9);
  Wire.begin();
  //Serial.begin(115200);
  //Serial.println("Hello!");

// SkyPipe
    STAT == 0;
    POLL == 0;
    Serial.begin(9600); // connect to the serial port
    //delay(1000);
    //Serial.print("^^1002Arduino UDS");
    //Serial.write(255);

// ADC
//                                                                ADS1015  ADS1115
  //                                                                -------  -------
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

    }


int incomingByte;

void loop() {
    // if we are pushing the data to RSP then we need to
    // establish our timing for sending new data.
    // here we are just doing a delay of 100ms to get a
    // sample rate of about 10 samples / sec.
    if (POLL == 0 && STAT ==3){
    delay(1000); // change to 1 Hz SR
    } 

while (Serial.available() > 0) {

    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    // if it's an K we stop (KILL):
    if (incomingByte == 'K') {
        //Serial.print("^^1002DEAD"); // Just for troubleshooting
        //Serial.write(255);
        //Serial.println("Arduino UDS");
        //GET PAST THE REST OF THE WORD by Reading it.
        delay(10); // not sure why these delays were needed
        incomingByte = Serial.read();
        delay(10);
        incomingByte = Serial.read();
        delay(10);
        incomingByte = Serial.read();
        incomingByte = 0;
        STAT=-1 ;
        }
        // if it's a capital I run the INIT code if any
        if (incomingByte == 'I' && STAT ==0) {
                //INIT
                // GET RID OF 'NIT'
                delay(10);
                incomingByte = Serial.read();
                delay(10);
                incomingByte = Serial.read();
                delay(10);
                incomingByte = Serial.read();
                incomingByte = 0;
                STAT = 1 ;
                //Serial.print("^^1002 INITIALIZED "); 
                //Serial.write(255);
        } 
        // if it's an L (ASCII 76) RSP will POLL for data
                if (incomingByte == 'L') {
                POLL = 1;
                // GET RID OF 'L'
                delay(10);
                incomingByte = Serial.read();
                incomingByte = 0;
                //Serial.print("^^1002 POLLING ");
                //Serial.write(255); 
        }

        // H sets it to push
        if (incomingByte == 'H') {
                POLL = 0;
                //Serial.print("^^1002 PUSHING ");
                //Serial.write(255); 
        }

        // if it's a C then Radio-SkyPipe is requesting number of channels
        if (incomingByte == 'C') {
                // change the last digit to = digit of channels of data (ex. 1), now 4
                delay(10);
                Serial.print("^^20134");
                Serial.write(255); // print result;
                STAT = 2; // ready to go
        }
        if (incomingByte == 'A' ) {
                // A means STAT was requested so send UDS ready message
                delay(10);
                Serial.print("^^1001");
                Serial.write(255); // print result;;
                // GET RID OF 'T'
                delay(10);
                incomingByte = Serial.read();
                incomingByte = 0;
                STAT=3;
                
        }
        // if it's an D we should send data to RSP:
        if (incomingByte == 'D' && POLL == 1 ) {
                //Serial.println(" DATA REQUEST RECEIVED ");
                GETD() ;
        }

        if (STAT== -1){
        STAT = 0;
    } 


    } 
    // we are finished processing any incoming commands from the PC
    // and we are not being polled so get a sample and send it
    if (POLL == 0 && STAT == 3) {
    GETD ();
    }
}


// This is where data is fetched and sent on to RSP.

long int dat; //may have to change type to suit your data
void GETD(){
        /*
		put data collection calls here
		*/
    // for arduino adc
		// dat0 = analogRead(A0); // example analog read 1
		// dat1 = analogRead(A1); // example analog read 2
		// dat2 = analogRead(A2); // example analog read 3
		// dat3 = analogRead(A3); // example analog read 4
		// for ads1115
    adc0 = ads.readADC_SingleEnded(0);
    volts0 = ads.computeVolts(adc0);
    adc1 = ads.readADC_SingleEnded(1);
    volts1 = ads.computeVolts(adc1);
    adc2 = ads.readADC_SingleEnded(2);
    volts2 = ads.computeVolts(adc2);
    adc3 = ads.readADC_SingleEnded(3);
    volts3 = ads.computeVolts(adc3);

		// channel 1
        Serial.print("#0"); // # followed by channel number of data
        Serial.print(volts0); // example read
        Serial.write(255);
        Serial.print("^^3001"); // This tells RSP to time stamp it
        Serial.write(255); // all commands end with this character.
		
		// channel 2
		Serial.print("#1"); // # followed by channel number of data
        Serial.print(volts1); // example read
        Serial.write(255);
        Serial.print("^^3001"); // This tells RSP to time stamp it
        Serial.write(255); // all commands end with this character.
		
		// channel 3
		Serial.print("#2"); // # followed by channel number of data
        Serial.print(volts2); // example read
        Serial.write(255);
        Serial.print("^^3001"); // This tells RSP to time stamp it
        Serial.write(255); // all commands end with this character.
		
		// channel 4
		Serial.print("#3"); // # followed by channel number of data
        Serial.print(volts3); // example read
        Serial.write(255);
        Serial.print("^^3001"); // This tells RSP to time stamp it
        Serial.write(255); // all commands end with this character.
        return;
}


/* Temp Sensor Functions */
boolean DS18B20_Init()
{
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(DSPIN, LOW);
  delayMicroseconds(750);//480-960
  digitalWrite(DSPIN, HIGH);
  pinMode(DSPIN, INPUT);
  int t = 0;
  while (digitalRead(DSPIN))
  {
    t++;
    if (t > 60) return false;
    delayMicroseconds(1);
  }
  t = 480 - t;
  pinMode(DSPIN, OUTPUT);
  delayMicroseconds(t);
  digitalWrite(DSPIN, HIGH);
  return true;
}
 
void DS18B20_Write(byte data)
{
  pinMode(DSPIN, OUTPUT);
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(10);
    if (data & 1) digitalWrite(DSPIN, HIGH);
    else digitalWrite(DSPIN, LOW);
    data >>= 1;
    delayMicroseconds(50);
    digitalWrite(DSPIN, HIGH);
  }
}
 
byte DS18B20_Read()
{
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(2);
  byte data = 0;
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(1);
    digitalWrite(DSPIN, HIGH);
    pinMode(DSPIN, INPUT);
    delayMicroseconds(5);
    data >>= 1;
    if (digitalRead(DSPIN)) data |= 0x80;
    delayMicroseconds(55);
    pinMode(DSPIN, OUTPUT);
    digitalWrite(DSPIN, HIGH);
  }
  return data;
}
 
int TempRead()
{
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0x44); // Send reading start conversion command
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value
  int temp = DS18B20_Read (); // Low byte
  temp |= DS18B20_Read () << 8; // High byte
  return temp;
}
