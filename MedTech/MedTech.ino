#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 4);



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

int fentanylSevereDuration = 2000;
int fentanylMildDuration = 1000;

int morphineSevereDuration = 4000;
int morphineMildDuration = 3000;

const int fentanylPin = 8;
const int morphinePin = 9;

bool administered = false;
String administeredDrug = "No medicine";
unsigned long lastAdministeredTime = 0;
unsigned long lastClockUpdate = 0;

String password = "Rex";

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  

  pinMode(fentanylPin, OUTPUT);
  pinMode(morphinePin, OUTPUT);

  digitalWrite(fentanylPin, LOW);
  digitalWrite(morphinePin, LOW);
}

void loop() {

  if (quitProgram) {
    printToBoth("Program terminated by doctor.");
    return;
  }

  if (administered && (millis() - lastAdministeredTime < delayAmount)) {
    printToBoth("Waiting for required delay time before next administration.");
    displayLastTimeAdministered();
    delay(1000);
    return;
  } else {
    administered = false;
  }

  changingMeasures();
  if (quitProgram) {
    printToBoth("Program terminated by doctor.");
    return;
  }
  
  severityScoreCalculator();
  if (severityScore > 61.67) {
    administerDrug("Fentanyl", 0.075, fentanylSevereDuration, fentanylPin);
    delayAmount = delayAmountFentanyl;
    if (administered) {
      administeredDrug = "Severe Fentanyl";
    }
  }else if (54.67 <= severityScore && severityScore <= 61.67) {
    administerDrug("Fentanyl", 0.038, fentanylMildDuration, fentanylPin);
    delayAmount = delayAmountFentanyl;
    if (administered) {
      administeredDrug = "Mild Fentanyl";
    }
  }else if (46.33 <= severityScore && severityScore < 54.67 ) {
    administerDrug("Morphine", 7, morphineSevereDuration, morphinePin);
    delayAmount = delayAmountMorphine;
    if (administered) {
      administeredDrug = "Severe Morphine";
    }
  }else if (25.50 <= severityScore && severityScore < 46.33) {
    administerDrug("Morphine", 3, morphineMildDuration, morphinePin);
    delayAmount = delayAmountMorphine;
    if (administered) {
      administeredDrug = "Mild Morphine";
    }
  } else {
    printToBoth("You have a severity score of " + String(severityScore) + " there is no need for any medicine.");
  }

  if (quitProgram) {
    printToBoth("Program terminated by doctor.");
    return;
  }

  outPutPastValue(nociceptorValue, heartRateValue, oxygenSaturation, severityScore, administeredDrug);
  

  if (millis() - lastClockUpdate >= 1000) {
    lastClockUpdate = millis();
    displayLastTimeAdministered();
  }
}

void severityScoreCalculator(){
  severityScore = ((nociceptorValue * nociceptorWeight) + 
                  ((heartRateValue - 72) * heartRateWeight)) + 
                  ((100 - oxygenSaturation) * oxygenSaturationWeight);
}

void changingMeasures(){
  delay(1);
  nociceptorValue = getValidatedInput("Nociceptor value (0-100):", 0, 100);
  if (quitProgram){
    return;
  }
  delay(1);
  heartRateValue = getInput("Heart rate value:");
  if (quitProgram){
    return;
  }
  delay(1);
  oxygenSaturation = getInput("Oxygen saturation percentage(Enter as integer):");
  if (quitProgram) {
    return;
  }
  
}

int getInput(String prompt){
  int tempValue;
  bool validInput = false;

  while (!validInput) {
    printToBoth(prompt);

    delay(1);
    while (Serial.available() > 0) {
    Serial.read();
    }

    while(Serial.available() == 0);

    if (!Serial.peek() || Serial.peek() < '0' || Serial.peek() > '9') {
      printToBoth("Invalid input. Please enter a valid number.");
      while (Serial.available() > 0) Serial.read();
      continue;
    }

    tempValue = Serial.parseInt();
    Serial.peek();
    printToBoth(String(tempValue));
  
    printToBoth("Are you sure this number " + String(tempValue) + " is correct? (Y/N/Q)");
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
    printToBoth(prompt);

    delay(1);
    while (Serial.available() > 0) {
    Serial.read();
    }
    while(Serial.available() == 0);

    if (!Serial.peek() || Serial.peek() < '0' || Serial.peek() > '9') {
      printToBoth("Invalid input. Please enter a valid number.");
      while (Serial.available() > 0) Serial.read();
      continue;
    }

    Serial.peek();
    tempValue = Serial.parseInt();
    printToBoth(String(tempValue));

    if (tempValue < minVal || tempValue > maxVal) {
      printToBoth("Invalid input. Please enter a value between " + String(minVal) + " and " + String(maxVal) + ".");
    } else {
      printToBoth("Are you sure this number " + String(tempValue) + " is correct? (Y/N/Q)");
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

  if (!checkPassword()) {
    printToBoth("Incorrect password. Access denied.");
    return;
  }

  printToBoth("Patient has a severity score of " + String(severityScore) + ". You will administer " + String(dosage) + " mg of " + drugName + ".");


  printToBoth("Are you sure you want to administer " + String(dosage) + " mg of " + drugName + "? (Y/N/Q)");

  doctorConfirmation = confirmation();
  if(quitProgram){
    return;
  }
  if (doctorConfirmation) {
    printToBoth("Administering " + String(dosage) + " mg of " + drugName + "...");
    motorActivity(pin, duration);
    administered = true;
    lastAdministeredTime = millis();
  }
}


bool confirmation() {
  while (Serial.available() > 0) {
    Serial.read();
  }
  while (true) {
    while (Serial.available() == 0);
    char confirm = Serial.read();
    if (confirm == 'Y' || confirm == 'y') {
      return true;
    } else if (confirm == 'Q' || confirm == 'q') {
      quitProgram = true;
      return false;
    } else if (confirm == 'N' || confirm == 'n') {
      return false;
    } else {
      printToBoth("Invalid input. Please enter Y, N, or Q.");
      delay(1);
      while (Serial.available() > 0) Serial.read();
    }
  }
}

void motorActivity(int pin, int duration) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void outPutPastValue(int nociceptorCurrent, int heartRateCurrent, int oxygenSaturationCurrent, float severityCurrent, String medicineCurrent) {
  printToBoth("Past Nociceptor value: " + String(nociceptorCurrent));
  delay(2000);
  printToBoth("Past heart rate current: " + String(heartRateCurrent));
  delay(2000);
  printToBoth("Past oxygen saturation percentage: " + String(oxygenSaturationCurrent) + "%");
  delay(2000);
  printToBoth("Past severity score: " + String(severityScore));
  delay(2000);
  printToBoth("Past medicine administered: " + medicineCurrent);
  delay(2000);
}

void displayLastTimeAdministered() {
  if (administered) {
    printToBoth("Time Since Last Dose: " + formatTime(millis() - lastAdministeredTime));
  } else {
    printToBoth("No medicine administered.");
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

bool checkPassword() {
  String enteredPassword = "";

  printToBoth("Please enter the password to continue:");

  delay(1);
  while (Serial.available() > 0) {
    Serial.read();  
  }

  while (true) {

    while (Serial.available() == 0);
    enteredPassword = Serial.readString();

    enteredPassword.trim();

    if (enteredPassword == password) {
      return true;
    } else {
      printToBoth("Incorrect password. Please try again.");
    }

    if (enteredPassword == "Q" || enteredPassword == "q") {
      quitProgram = true;
      return false; 
    }
  }
}

void printToBoth(String message) {
  Serial.println(message);

  lcd.clear();
  int messageLength = message.length();
  int row = 0;
  int col = 0;

  for (int i = 0; i < messageLength; i++) {
    if (col == 16) {
      col = 0;
      row++;
      if (row == 4) {
        break;
      }
      lcd.setCursor(col, row);
    }
    lcd.print(message.charAt(i));
    col++;
  }
}