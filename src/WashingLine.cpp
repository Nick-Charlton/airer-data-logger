// Include Particle Device OS APIs
#include "Particle.h"
#include "HX711ADC.h"
#include <HX711ADC.h>
#include <string>
// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
 
int pinInterrupt = A0;

int DOUT1 = A1;
int CLK1 = A2;
int DOUT2 = A3;
int CLK2 = A4;
bool shouldGetWeight = false;
HX711ADC scale1;
HX711ADC scale2;

int calibration_factor1 = -43500; //around -33000 for tightened bolts, around 44000 for loose bolts
int calibration_factor2 = -43500; 

int weightFunc(const char *topic, const char *data) {
    shouldGetWeight = true;
 return 0;
}
/* //CALLIBRATION CODE//
void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale2.begin(DOUT2, CLK2);
  scale2.set_scale();
  scale2.tare(); //Reset the scale to 0

  long zero_factor = scale2.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale2.set_scale(calibration_factor2); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale2.get_units(), 1);
  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor2: ");
  Serial.print(calibration_factor2);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor2 += 500;
    else if(temp == '-' || temp == 'z')
      calibration_factor2 -= 500;
  }
} 
*/
 int Count = 0;
//const std::chrono::milliseconds publishPeriod = 30s; // value publish period
const char *eventName = "Washing_Line_Data"; //event name to publish data to
unsigned long lastPublish;
int windSpeed[30];
int strain1[30];
int strain2[30];
void publishData();

void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");

  WiFi.connect();

  while (WiFi.connecting()) {
    delay(1000);
    Serial.println("Connecting...");
  }

  if (WiFi.ready()) {
    Serial.println("Connected to Wi-Fi!");
  } else {
    Serial.println("Failed to connect to Wi-Fi.");
    // You may add additional handling here if the connection fails.
  }
}

void onChange() {
  if (digitalRead(pinInterrupt) == LOW )
    Count++;
}
void setup() {
  Serial.begin(9600);
  WiFi.setCredentials(wifiSSID, wifiPassword);
  connectToWiFi(); // Custom function to connect to WiFi
  pinMode(pinInterrupt, INPUT_PULLUP);// set the interrupt pin
  attachInterrupt(pinInterrupt, onChange, FALLING);
  scale1.begin(DOUT1, CLK1);  //initialise the first ADC board
  scale2.begin(DOUT2, CLK2);  //initialise the second ADC board
  scale1.set_scale(calibration_factor1); //apply the calibration factor to strain gauge 1
  scale1.tare();
  scale2.set_scale(calibration_factor2); //apply the callibration factor to strain gauge 2
  scale2.tare();
  Particle.subscribe("get-weight", weightFunc, MY_DEVICES);
}

void loop() {
  int i = 0;
 while(i<30) {   
    windSpeed[i] = Count;
    Count = 0;
    strain1[i] = scale1.get_units() * 10;
    delay(500);
    strain2[i] = scale2.get_units() * 10;
    delay(500);
    i++;
  }
  if (Particle.connected()) {
    publishData();
  }
}
void publishData() {
  char buffer[1024];

  snprintf(buffer, sizeof(buffer), "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",
  windSpeed[0], windSpeed[1], windSpeed[2], windSpeed[3], windSpeed[4], windSpeed[5], windSpeed[6], windSpeed[7], windSpeed[8], windSpeed[9], windSpeed[10], windSpeed[11], windSpeed[12], windSpeed[13], windSpeed[14], windSpeed[15], 
  windSpeed[16], windSpeed[17], windSpeed[18], windSpeed[19], windSpeed[20], windSpeed[21], windSpeed[22], windSpeed[23], windSpeed[24], windSpeed[25], windSpeed[26], windSpeed[27], windSpeed[28], windSpeed[29], strain1[0], strain1[1], strain1[2], strain1[3],
  strain1[4], strain1[5], strain1[6], strain1[7], strain1[8], strain1[9], strain1[10], strain1[11], strain1[12], strain1[13], strain1[14], strain1[15], strain1[16], strain1[17], strain1[18], strain1[19], strain1[20], strain1[21], strain1[22], strain1[23], 
  strain1[24], strain1[25], strain1[26], strain1[27], strain1[28], strain1[29], strain2[0], strain2[1], strain2[2], strain2[3], strain2[4], strain2[5], strain2[6], strain2[7], strain2[8], strain2[9], strain2[10], strain2[11], strain2[12], strain2[13], 
  strain2[14], strain2[15], strain2[16], strain2[17], strain2[18], strain2[19], strain2[20], strain2[21], strain2[22], strain2[23], strain2[24], strain2[25], strain2[26], strain2[27], strain2[28], strain2[29]);

  Particle.publish(eventName, buffer, PRIVATE);
  Log.info("published: %s", buffer);
} 


/* *TEST CODE*
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000;    // the debounce time; increase if the output flickers
 
int pinInterrupt = A0;
 
int Count = 0;
 
void onChange()
{
  if ( digitalRead(pinInterrupt) == LOW )
    Count++;
}
 
 
void setup()
{
  Serial.begin(115200); //Initialize serial port
  pinMode( pinInterrupt, INPUT_PULLUP);// set the interrupt pin
  //Enable
  attachInterrupt(pinInterrupt, onChange, FALLING);
}
 
void loop()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    Serial.print((Count * 8.75)/100);
    Count = 0;
    Serial.println("m/s");
  }
  delay(1);
}*/