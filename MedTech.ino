int nociceptorValue = 0;
int heartRateValue = 0;
int oxygenSaturation = 0;

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

const int fentanylPin = 0;
const int morphinePin = 0;

bool administered = false;


void setup() {
  Serial.begin(9600);
  pinMode(fentanylPin, OUTPUT); //replace # with actual pin number
  pinMode(morphinePin, OUTPUT); //replace # with actual pin number
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
  }else if (72.2 <= severityScore && severityScore <= 80.4) {
    administerDrug("Fentanyl", 0.038, fentanylMildDuration, fentanylPin);
    delayAmount = delayAmountFentanyl;
  }else if (60.6 <= severityScore && severityScore < 72.2 ) {
    administerDrug("Morphine", 7, morphineSevereDuration, morphinePin);
    delayAmount = delayAmountMorphine;
  }else if (40.8 <= severityScore && severityScore < 60.6) {
    administerDrug("Morphine", 3, morphineMildDuration, morphinePin);
    delayAmount = delayAmountMorphine;
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
}

void severityScoreCalculator(){
  severityScore = ((nociceptorValue * nociceptorWeight) + (heartRateValue * heartRateWeight)) + ((100 - oxygenSaturation) * oxygenSaturationWeight);
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
    tempValue = Serial.parseInt();
  
    Serial.println("Are you sure this" + String(tempValue) + "is correct? (Y/N/Q)");
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
    while (Serial.available() == 0);
    tempValue = Serial.parseInt();

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
  Serial.println("Patient has a severity score of " + String(severityScore) + ". You will administer" + String(dosage) + " mg of " + drugName + ".");


  Serial.println("Are you sure you want to administer " + String(dosage) + " mg of" + drugName + "? (Y/N/Q)");

  doctorConfirmation = confirmation();
  if(quitProgram){
    return;
  }
  if (doctorConfirmation) {
    Serial.println("Administering " + String(dosage) + " mg of " + drugName + "...");
    motorActivity(pin, duration);
  }
  administered = true;
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
  digitalWrite(#, HIGH); //replace with pin
  delay(duration);
  digitalWrite(#, LOW); //replace with pin
}



//future idea - output the current inserted values