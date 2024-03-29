const int led13Pin = 13;

boolean led13 = true;
const int readPin0 = A0; // ADC0

struct Readings
{
  float milliAmps = 0.0;
  float power = 0.0;
  int sampleRate = 0;
  float rawRms = 0.0;
};
Readings readings;

struct Settings
{
  int echoReadings = 0;
  float numSamplesAvg = 10000.0;
  float numSamplesFilter = 20.0;
  float numSamplesRms = 10000.0;
  float adcTomA = .4096;
  float rmsOffset = 1.12;
  float acVolts = 240.0;
};
Settings settings;
float adcValue = 0.0;
float avgAdc = 0.0;
float rmsAdc = 0.0;
float avgRms = 0.0;
float adcFilter = 0.0;
int icount = 0;
unsigned long tstart = 0;
unsigned long now = 0;

void setup()
{
  Serial1.begin(115200);
  Serial.begin(9600);
  analogReadResolution(12);
//  analogReadAveraging(8);    
  
  pinMode(readPin0, INPUT); 
  pinMode(led13Pin, OUTPUT); 
  digitalWrite(led13Pin, led13);
    
}

void loop()
{
  now = millis();
  if ((now - tstart) >= 1000)
  {
    readings.sampleRate = icount;
    icount = 0;
    tstart = now;
  }
  else
  {
    ++icount;
  }
  while(Serial1.available() > 0)
  {
    Serial1.readBytes((uint8_t*) &settings, sizeof(settings));
/*    
    Serial.print("Settings: ");
    Serial.print(settings.echoReadings);
    Serial.print(", ");
    Serial.print(settings.numSamplesAvg);
    Serial.print(", ");
    Serial.print(settings.numSamplesFilter);
    Serial.print(", ");
    Serial.print(settings.numSamplesRms);
    Serial.print(", ");
    Serial.print(settings.adcTomA);
    Serial.print(", ");
    Serial.print(settings.rmsOffset);
    Serial.print(", ");
    Serial.println(settings.acVolts);
*/
  }
  adcValue = (float) analogRead(readPin0);
  adcFilter = adcFilter + (adcValue - adcFilter) / settings.numSamplesFilter;
  avgAdc = avgAdc + (adcValue - avgAdc) / settings.numSamplesAvg;
  rmsAdc = (adcFilter - avgAdc);
  rmsAdc = rmsAdc * rmsAdc;
  avgRms = avgRms + (rmsAdc - avgRms) / settings.numSamplesRms;
  if (settings.echoReadings > 0)
  {
    readings.rawRms = sqrt(avgRms);
    readings.milliAmps = (readings.rawRms - settings.rmsOffset) / settings.adcTomA;
    readings.power = readings.milliAmps * 0.001 * settings.acVolts;
    led13 = !led13;
    digitalWrite(led13Pin, led13);
    Serial1.write((uint8_t*)&readings, sizeof(readings));
    settings.echoReadings = 0;
/*
    Serial.print("Readings: ");
    Serial.print(readings.milliAmps);
    Serial.print(", ");
    Serial.print(readings.power);
    Serial.print(", ");
    Serial.print(readings.sampleRate);
    Serial.print(", ");
    Serial.println(readings.rawRms);
*/

  }
}
