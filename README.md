# Pain Administration & Feedback System

🚑 **An Arduino-based automated pain assessment and medication administration system.**

This project integrates real-time sensor data (nociceptor activity, heart rate, and oxygen saturation) to calculate a **severity score** and determine the appropriate dosage of Fentanyl or Morphine. Built with **C++ (Arduino)**, the system ensures secure and controlled drug delivery using authentication mechanisms, LCD feedback, and time-based dose regulation.

## Features

- 🩺 **Real-time Pain Assessment** – Uses sensor inputs to dynamically compute a severity score.
- 💉 **Automated Drug Administration** – Determines and dispenses the correct medication dosage.
- 🔐 **Secure Access Control** – Password-protected administration to prevent unauthorized use.
- 📟 **LCD & Serial Output** – Provides live feedback on patient status and drug administration history.
- ⏳ **Time-based Lockouts** – Prevents overdose by enforcing cooldown periods between doses.

## Technologies Used

- **Arduino (C++)** – Core programming for sensor data processing and control logic.
- **LiquidCrystal_I2C** – Library for LCD display integration.
- **Embedded Systems Design** – Real-time processing and hardware interaction.

## How It Works

1. The system takes in three key physiological parameters:
   - **Nociceptor Firing Levels**
   - **Heart Rate**
   - **Oxygen Saturation**
2. These values are processed using a weighted severity score formula.
3. Based on the severity score, the system determines the appropriate medication and dosage:
   - **Fentanyl or Morphine** is selected based on severity thresholds.
4. The system requires **password authentication** before administration.
5. Once confirmed, the appropriate dose is administered, and a **cooldown period** is enforced.
6. An **LCD display and serial monitor** provide real-time feedback and dosage history.

## Setup Instructions

1. **Hardware Requirements:**
   - Arduino Board
   - LCD Display (I2C)
   - Sensors for nociceptor activity, heart rate, and oxygen levels
   - Motor/Actuator for drug administration
2. **Software Requirements:**
   - Arduino IDE
   - LiquidCrystal_I2C Library
3. **Installation:**
   - Clone this repository:  
     ```sh
     git clone https://github.com/YOUR_PERSONAL_USERNAME/REPO_NAME.git
     ```
   - Open the `.ino` file in Arduino IDE.
   - Upload the code to your Arduino board.

## Usage

- Power on the system and follow the LCD prompts.
- Enter the required physiological values.
- Confirm medication administration through password authentication.
- Monitor dosage history and cooldown times via the LCD display.

## Future Improvements

- Integration with **IoT/cloud-based monitoring** for remote patient tracking.
- Advanced **machine learning algorithms** to refine dosage calculations.
- Expansion to support multiple medications and patient profiles.

## License

This project is open-source and available under the [MIT License](LICENSE).

---

### Contributors
- **[Luis-Manuel Lopez]** – Developer & Embedded Systems Engineer

For any inquiries, feel free to reach out or submit a pull request! 🚀
