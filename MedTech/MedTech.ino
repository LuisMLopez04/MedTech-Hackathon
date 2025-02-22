int nociceptorValue;
int heartRateValue;
int oxygenSaturation;

float nociceptorWeight = 2.0/3;
float heartRateWeight = 1.0/6;
float oxygenSaturationWeight = 1.0/6;

float severityScore = 0;

float delayAmountMorphine = (4 * 3600000);
float delayAmountFentanyl = 3600000;
float delayAmount = 0;

bool doctorConfirmation = false;
bool valueConfirmation = false;
bool quitProgram = false;

int fentanylSevereDuration = 0;
int fentanylMildDuration = 0;

int morphineSevereDuration = 0;
int morphineMildDuration = 0;

const int fentanylPin = 5;
const int morphinePin = 8;

bool administered = false;
String administeredDrug = "No medicine";
unsigned long lastAdministeredTime = 0;
unsigned long lastClockUpdate = 0;


void setup() {
  Serial.begin(9600);
  pinMode(fentanylPin, OUTPUT);
  pinMode(morphinePin, OUTPUT);
}

void loop() {
  if (quitProgram) {
    Serial.println("Program terminated by doctor.");
    return;
  }
  changingMeasures();
  if (quitProgram) {
    Serial.println("Program terminated by doctor.");
    return;
  }
  
  severityScoreCalculator();
  if (severityScore > 80.4) {
    administerDrug("Fentanyl", 0.075, fentanylSevereDuration, fentanylPin);
    delayAmount = delayAmountFentanyl;
    if (administered) {
      administeredDrug = "Severe Fentanyl";
    }
  }else if (72.2 <= severityScore && severityScore <= 80.4) {
    administerDrug("Fentanyl", 0.038, fentanylMildDuration, fentanylPin);
    delayAmount = delayAmountFentanyl;
    if (administered) {
      administeredDrug = "Mild Fentanyl";
    }
  }else if (60.6 <= severityScore && severityScore < 72.2 ) {
    administerDrug("Morphine", 7, morphineSevereDuration, morphinePin);
    delayAmount = delayAmountMorphine;
    if (administered) {
      administeredDrug = "Severe Morphine";
    }
  }else if (40.8 <= severityScore && severityScore < 60.6) {
    administerDrug("Morphine", 3, morphineMildDuration, morphinePin);
    delayAmount = delayAmountMorphine;
    if (administered) {
      administeredDrug = "Mild Morphine";
    }
  } else {
    Serial.println("You have a severity score of " + String(severityScore) + " there is no need for any medicine.");
  }

  if (quitProgram) {
    Serial.println("Program terminated by doctor.");
    return;
  }
  if (administered) {
    delay(delayAmount);
  }
  outPutPastValue(nociceptorValue, heartRateValue, oxygenSaturation, severityScore, administeredDrug);
  
  if (millis() - lastClockUpdate >= 1000) {
    lastClockUpdate = millis();
    displayLastTimeAdministered();
  }
}

void severityScoreCalculator(){
  severityScore = ((nociceptorValue * nociceptorWeight) + 
                  (heartRateValue * heartRateWeight)) + 
                  ((100 - oxygenSaturation) * oxygenSaturationWeight);
}

void changingMeasures(){
  nociceptorValue = getValidatedInput("Nociceptor value (0-100):", 0, 100);
  if (quitProgram){
    return;
  }

  heartRateValue = getInput("Heart rate value:");
  if (quitProgram){
    return;
  }

  oxygenSaturation = getInput("Oxygen saturation percentage(Enter as percentage):");
  if (quitProgram) {
    return;
  }
  
}

int getInput(String prompt){
  int tempValue;
  bool validInput = false;

  while (!validInput) {
    Serial.println(prompt);

    while(Serial.available() == 0);

    while (Serial.available() > 0) {
      Serial.read();
    }

    while (Serial.available() == 0);
    tempValue = Serial.parseInt();
    Serial.println(String(tempValue));
  
    Serial.println("Are you sure this " + String(tempValue) + " is correct? (Y/N/Q)");
    valueConfirmation = confirmation();
    if (quitProgram){
      return 0;
    }
    if (valueConfirmation) {
      validInput = true;
    }
  }
  valueConfirmation = false;
  return tempValue;
}

int getValidatedInput(String prompt, int minVal, int maxVal) {
  int tempValue;
  bool validInput = false;

  while (!validInput) {
    Serial.println(prompt);

    while(Serial.available() == 0);

    while (Serial.available() > 0) {
      Serial.read();
    }

    while (Serial.available() == 0);
    tempValue = Serial.parseInt();
    Serial.println(String(tempValue));

    if (tempValue < minVal || tempValue > maxVal) {
      Serial.println("Invalid input. Please enter a value between " + String(minVal) + " and " + String(maxVal) + ".");
    } else {
      Serial.println("Are you sure this " + String(tempValue) + " is correct? (Y/N/Q)");
      valueConfirmation = confirmation();
      if (quitProgram){
        return 0;
      }
      if (valueConfirmation) {
        validInput = true;
      }
    }
  }
  return tempValue;
}

void administerDrug(String drugName, float dosage, int duration, int pin) {
  Serial.println("Patient has a severity score of " + String(severityScore) + ". You will administer " + String(dosage) + " mg of " + drugName + ".");


  Serial.println("Are you sure you want to administer " + String(dosage) + " mg of " + drugName + "? (Y/N/Q)");

  doctorConfirmation = confirmation();
  if(quitProgram){
    return;
  }
  if (doctorConfirmation) {
    Serial.println("Administering " + String(dosage) + " mg of " + drugName + "...");
    motorActivity(pin, duration);
    administered = true;
    lastAdministeredTime = millis();
  }
}


bool confirmation() {
  while (Serial.available() == 0);
  char confirm = Serial.read();
  if (confirm == 'Y' || confirm == 'y'){
    return true;
  } else if (confirm == 'Q' || confirm == 'q') {
    quitProgram = true;
    return false;
  } else {
    return false;
  }
}

void motorActivity(int pin, int duration) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void outPutPastValue(int nociceptorCurrent, int heartRateCurrent, int oxygenSaturationCurrent, float severityCurrent, String medicineCurrent) {
  Serial.println("Past Nociceptor value: " + String(nociceptorCurrent));
  Serial.println("Past heart rate current: " + String(heartRateCurrent));
  Serial.println("Past oxygen saturation percentage: %" + String(oxygenSaturationCurrent));
  Serial.println("Past severity score: " + String(severityScore));
  Serial.println("Past medicine administered: " + medicineCurrent);
}

void displayLastTimeAdministered() {
  if (administered) {
    Serial.println("Time Since Last Dose: " + formatTime(millis() - lastAdministeredTime));
  } else {
    Serial.println("No medicine administered.");
  }
}

String formatTime(unsigned long milliseconds) {
  unsigned long totalSeconds = milliseconds / 1000;
  int hours = (totalSeconds / 3600) % 24;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;

  String timeString = (hours < 10 ? "0" : "") + String(hours) + ":" +
                      (minutes < 10 ? "0" : "") + String(minutes) + ":" +
                      (seconds < 10 ? "0" : "") + String(seconds);
  return timeString;
}
