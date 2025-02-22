int nocicetporValue = 0;
int heartRateValue = 0;
int oxygenSaturation = 0;

float nociceptorWeight = 2/3;
float heartRateWeight = 1/6;
float oxygenSaturationWeight = 1/6;

float severityScore = 0;

float delayAmount = 0;

bool doctorConfirmation = false;

void setup() {
  Serial.begin(9600);
  pinMode(#, OUTPUT);
  pinMode(#, OUTPUT);
}

void loop() {
  readingMeasurements();

}

void readingMeasures(){
  if (Serial.available() > 0) {
    Serial.println("Nociceptor value:");
    nociceptorValue = Serial.parseInt();
    Serial.println("Heart Rate value:");
    heartRateValue = Serial.parseInt();
    Serial.println(" value:");
    nociceptorValue = Serial.parseInt();
  }
}

void severityScoreCalculator(){
  severityScore = ((nociceptorValue * nociceptorWeight) + (heartRateValue * heartRate))
}