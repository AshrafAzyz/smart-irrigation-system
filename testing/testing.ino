// Libraries
#include <EEPROM.h>                  // EEPROM ibrary
#include <Adafruit_GFX.h>            // OLED display library
#include <Adafruit_SSD1306.h>        // OLED display library
#include <Keypad.h>                  // Keypad library

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Adafruit_SSD1306 display object created
extern Adafruit_SSD1306 display;

struct Clock {
  unsigned int hours;
  unsigned int minutes;
  unsigned int seconds;
  unsigned int day;
  unsigned int month;
  unsigned int year;
};

// Keypad matrix configuration
const byte ROWS = 4;               // Four rows configuration 
const byte COLS = 3;               // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 13}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin configurations for the relay and soil moisture sensor
const int relayPin = 4;            // Output pin for the relay 
const int waterSensorPin = 5;      // Input pin for the soil moisture sensor 
const int moistureAnalogPin = A0;  // Analog input pin for the soil moisture sensor

// Variables for sensor readings and display
int value;                        // Variable to store the ADC value
float voltage;                    // Variable to store the calculated voltage
int currentSelection = 0;         // Variable to track current selection (1 to 4)
int selectedOption = 0;           // Variable to store the selected option

// Global variables for time tracking
unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Interval to update the clock (1 second)
int day = 27;
int month = 11;
int year = 2024; // Settings starting day/month/year manually

int hours = 0;
int minutes = 0;
int seconds = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set up the relay and sensor pins
  pinMode(relayPin, OUTPUT);      // Set relay pin as output
  pinMode(waterSensorPin, INPUT); // Set sensor pin as input
  pinMode(moistureAnalogPin, INPUT); // Set analog pin as input

  // Set up the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();

  // Display the main page UI
  //introAnimation();
}

void loop() {

  //Displaying main page
  displayMainPage();
  // Always update the clock
  updateClock();

  // Wait until a key is pressed
  char key = keypad.getKey();
  if (key) {
    // Print the key to the serial monitor
    Serial.print(F("Key Pressed: "));
    Serial.println(key);

    // Handle page navigation based on key press
    switch (key) {
      case '1': //Passing input to case 3
      case '2': //Passing input 
      case '3':
        // Highlight the selected option and wait for confirmation
        selectedOption = key - '0';
        highlightMainOption(selectedOption);
        handleMainPageConfirmation();
        break;
      case '*':
        // Return to main page
        displayMainPage();
        break;
      default:
        break;
    }
  }

  //Soil moisture sensor and relay control reading 
  int sensorValue = digitalRead(waterSensorPin); // FOR CONDITION
  value = analogRead(moistureAnalogPin);  // Read the ADC value FOR OLED
  voltage = map(value, 0, 1023, 0, 3300) / 1000.0; // Convert ADC value to voltage

  // Control the relay based on the sensor state
  if (sensorValue == LOW) { 
    // Soil is wet, turn off the pump
    digitalWrite(relayPin, LOW);    
  } else { 
    // Soil is dry, turn on the pump
    digitalWrite(relayPin, HIGH);   
  }
}

void introAnimation() {
  // Clear display
  display.clearDisplay();
  
  // Display "Irrigation System" with larger text
  display.setTextSize(2); // Large text size for title
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Irrigation"));
  display.setCursor(0, 20);
  display.println(F("System"));

  // Display "powered by AFA" with smaller text
  display.setTextSize(1); // Smaller text size for the subheading
  display.setCursor(0, 50);
  display.println(F("powered by AFA"));

  display.display(); // Update the display
  delay(2000); // Keep the intro animation for 2 seconds
}

void displayMainPage() {

  // Clear the display and set up the main page UI
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  //Display Clock
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 0);
  
  // Display time in HH:MM format
  display.print((hours < 10) ? "0" : ""); // Add leading zero for single-digit hours
  display.print(hours);
  display.print(":");
  display.print((minutes < 10) ? "0" : ""); // Add leading zero for single-digit minutes
  display.print(minutes);

  // Display the current date in DD/MM/YYYY format
  display.setTextSize(1);
  display.setCursor(35, 20);
  display.print((day < 10) ? "0" : "");
  display.print(day);
  display.print("/");
  display.print((month < 10) ? "0" : "");
  display.print(month);
  display.print("/");
  display.print(year);

  // Draw three text boxes representing the options
  int boxY = 30;
  display.setTextSize(2);
  display.drawRect(0, boxY, 40, 22, SSD1306_WHITE);
  display.setCursor(14, boxY + 4);
  display.println(F("1"));
  display.setTextSize(1);
  display.setCursor(3, boxY + 25);
  display.println(F("Volume"));
  
  display.setTextSize(2);
  display.drawRect(44, boxY, 40, 22, SSD1306_WHITE);
  display.setCursor(58, boxY + 4);
  display.println(F("2"));
  display.setTextSize(1);
  display.setCursor(50, boxY + 25);
  display.println(F("Level"));

  display.setTextSize(2);
  display.drawRect(88, boxY, 40, 22, SSD1306_WHITE);
  display.setCursor(102, boxY + 4);
  display.println(F("3"));
  display.setTextSize(1);
  display.setCursor(97, boxY + 25);
  display.println(F("Info"));

  display.display(); // Update the display with the new content
}

void highlightMainOption(int option) {

  displayMainPage(); // Clear any previous highlights
  int boxY = 30;
  switch (option) {
    case 1:
      display.fillRect(0, boxY, 40, 22, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(2);
      display.setCursor(14, boxY + 4);
      display.println(F("1"));
      break;
    case 2:
      display.fillRect(44, boxY, 40, 22, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(2);
      display.setCursor(58, boxY + 4);
      display.println(F("2"));
      break;
    case 3:
      display.fillRect(88, boxY, 40, 22, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(2);
      display.setCursor(102, boxY + 4);
      display.println(F("3"));
      break; 
  }
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void handleMainPageConfirmation() {
  while (true) {
    //Continue update clock in loop
    updateClock();
    // Wait for user input on the main page
    char key = keypad.getKey();
    if (key) {
      // Print the key to the serial monitor
      Serial.print(F("Main Page - Key Pressed: "));
      Serial.println(key);

      if (key == '#') {
        // Confirm selection and navigate to corresponding page
        switch (selectedOption) {
          case 1:
            displayPage1();
            handlePage1Input();
            return;
          case 2:
            displayPage2();
            return;
          case 3:
            displayPage3();
            return;
        }
      } else if (key == '*') {
        // Cancel and return to main page
        displayMainPage();
        return;
      } else if (key >= '1' && key <= '3') {
        // Allow changing selection without returning to the main page
        selectedOption = key - '0';
        highlightMainOption(selectedOption);
      }
    }
  }
}

void displayPage1() {
  // Clear the display and set up Page 1 UI
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Draw four text boxes representing the options
  display.setCursor(20, 0);
  display.println(F("Choose a volume:"));

  // Option 1
  display.drawRect(0, 16, 60, 16, SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println(F("100ml"));

  // Option 2
  display.drawRect(64, 16, 60, 16, SSD1306_WHITE);
  display.setCursor(79, 20);
  display.println(F("200ml"));

  // Option 3
  display.drawRect(0, 36, 60, 16, SSD1306_WHITE);
  display.setCursor(15, 40);
  display.println(F("300ml"));

  // Option 4 (Custom)
  display.drawRect(64, 36, 60, 16, SSD1306_WHITE);
  display.setCursor(75, 40);
  display.println(F("Custom"));

  display.display(); // Update the display with the new content
}

void handlePage1Input() {
  selectedOption = 0; // Reset selected option
  bool hasSelection = false; // Track if an option is currently selected

  // Wait for user input on Page 1
  while (true) {
    //Update Clock
    updateClock();
    char key = keypad.getKey();
    if (key) {
      // Print the key to the serial monitor
      Serial.print(F("Page 1 - Key Pressed: "));
      Serial.println(key);

      if (key == '#') {
        // Confirm selection and proceed
        if (hasSelection) {
          // Save the current time to EEPROM when # is pressed
          saveClockToEEPROM();
          switch (selectedOption) {
            case 1:
              dispense(3); // Dispense 100 ml (~3 seconds)
              return;
            case 2:
              dispense(6); // Dispense 200 ml (~6 seconds)
              return;
            case 3:
              dispense(9); // Dispense 300 ml (~9 seconds)
              return;
            case 4:
              handleCustomVolumeInput(); // Call the custom volume input function
              return;
          }
        }
      } else if (key == '*') {
        // Handle clearing selection or returning to the main page
        if (hasSelection) {
          // Clear selection and refresh the page
          hasSelection = false;
          selectedOption = 0;
          displayPage1(); // Redraw the Page 1 UI without any selection
        } else {
          // If no selection, return to the main page
          displayMainPage();
          return;
        }
      } else if (key >= '1' && key <= '4') {
        // Allow changing selection without leaving the page
        selectedOption = key - '0';
        hasSelection = true; // Mark that an option is selected
        displayPage1(); // Refresh UI
        switch (selectedOption) {
          case 1:
            display.fillRect(0, 16, 60, 16, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(15, 20);
            display.println(F("100ml"));
            display.setTextColor(SSD1306_WHITE);
            break;
          case 2:
            display.fillRect(64, 16, 60, 16, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(79, 20);
            display.println(F("200ml"));
            display.setTextColor(SSD1306_WHITE);
            break;
          case 3:
            display.fillRect(0, 36, 60, 16, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(15, 40);
            display.println(F("300ml"));
            display.setTextColor(SSD1306_WHITE);
            break;
          case 4:
            display.fillRect(64, 36, 60, 16, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(75, 40);
            display.println(F("Custom"));
            display.setTextColor(SSD1306_WHITE);
            break;
        }
        display.display(); // Update the display
      }
    }
  }
}

void displayPage2() {
  while (true) {
    // Read the sensor value and convert to voltage
    int sensorValue = analogRead(moistureAnalogPin); // Replace with your analog pin
    float voltage = (sensorValue / 1023.0) * 3.3;    // 3.3 reference

    // Clear the display and show the sensor voltage
    display.clearDisplay();
    display.setTextSize(1);
    display.fillRect(0,0,128,24,SSD1306_WHITE );

    display.setTextColor(SSD1306_BLACK);
    display.setCursor(8, 2);
    display.print(F("Sensor Value: "));
    display.setCursor(86, 2);
    display.print(voltage, 2); // Display voltage with 2 decimal points
    display.print(F(" V"));
    
    // Display soil condition based on voltage
    display.setCursor(28, 12);
    if (voltage < 1.5) {
      display.println(F("Soil is normal"));
      smileFace();
    } else if (voltage >= 1.5) {
      display.println(F("Soil is dry"));
      sadFace();
    } else {
      display.println(F("Invalid voltage"));
    }
    
    display.display();

    // Check for user input to exit Page 2
    char key = keypad.getKey();
    if (key == '*') {
      // Return to the main page if '*' is pressed
      displayMainPage();
      return;
    }
  }
}

void displayPage3() {
  // Display static information about the system
  display.clearDisplay();
  display.setTextSize(1); // Smaller text size for more content
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println(F("Soil Monitoring Sys."));
  display.setCursor(0, 20);
  display.println(F("Last Manually Pump: "));

  display.setCursor(0,30);
  printSavedClock();

  display.display();

  // Wait for user input to return to the main page
  while (true) {
    char key = keypad.getKey();
    if (key == '*') {
      // Return to the main page if '*' is pressed
      displayMainPage();
      return;
    }
  }
}

void handleCustomVolumeInput() {
  int customVolume = 0; // Variable to store user input
  bool isConfirmed = false;

  while (!isConfirmed) {
    // Clear the display and prompt user for input
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Enter volume (ml):"));
    display.setCursor(0, 20);
    display.print(F("Current: "));
    display.print(customVolume);
    display.println(F(" ml"));
    display.display();

    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        int digit = key - '0';
        // Only append the digit if the current number has fewer than 4 digits
        if (customVolume < 1000) { // Less than 4 digits
          customVolume = customVolume * 10 + digit;
        } else {
          // If it would exceed 4 digits, reset to 0
          customVolume = 0;
        }
      } else if (key == '*') {
        // Clear input or go back to Page 1
        if (customVolume == 0) {
          // If input is already 0, return to Page 1
          displayPage1();
          handlePage1Input();
          return;
        } else {
          // Clear the input if a value has been entered
          customVolume = 0;
        }
      } else if (key == '#') {
        // Confirm input
        isConfirmed = true;
      }

      // Debugging log for monitoring customVolume
      Serial.print(F("Current custom Volume: "));
      Serial.println(customVolume);
    }
  }

  // Calculate dispensing time based on the pump's flow rate
  // Pump flow rate: 120 L/H = 120,000 ml/hour = 120,000 / 3600 ml/second
  const float flowRatePerSecond = 120000.0 / 3600.0; // ~33.33 ml/second
  int dispensingTime = customVolume / flowRatePerSecond; // Time in seconds

  // Call the dispensing function with calculated time
  dispense(dispensingTime);
}

void dispense(int durationSeconds) { // Function to handle dispensing with a countdown
  // Activate dispensing mechanism
  digitalWrite(relayPin, HIGH); // Start dispensing

  for (int remainingTime = durationSeconds; remainingTime > 0; remainingTime--) {
    // Clear the display and show the countdown
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.print(F("Dispensing..."));
    display.setCursor(0, 30);
    display.print(F("Time: "));
    display.print(remainingTime);
    display.print(F("s"));
    display.display();

    delay(1000); // Wait for 1 second
  }

  // Stop dispensing
  digitalWrite(relayPin, LOW);

  // Show completion message
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print(F("Completed!"));
  display.display();

  delay(2000); // Pause for 2 seconds to show the completion message
  displayMainPage();
}

int getDaysInMonth(int month, int year) { //
  switch (month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12: // 31 days
      return 31;
    case 4: case 6: case 9: case 11: // 30 days
      return 30;
    case 2: // February
      if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 29; // Leap year
      } else {
        return 28;
      }
    default:
      return 30; // Default fallback, shouldn't happen
  }
}

void updateClock(){ // Function to update clock time using TIMER
  // Update the clock display every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the clock was updated
    previousMillis = currentMillis;

    // Increment seconds
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
    }
    if (minutes >= 60) {
      minutes = 0;
      hours++;
    }
    if (hours >= 24) {
      hours = 0;
    }
    if (day > getDaysInMonth(month, year)) {
      day = 1;
      month++;
    }
    if (month > 12) {
      month = 1;
      year++;
    }
  }
}

void saveClockToEEPROM() {
  Clock currentClock = {hours, minutes, seconds, day, month, year};

  // Debug: Print data to Serial Monitor before saving
  Serial.print("Saving Clock Data: ");
  Serial.print(currentClock.hours); Serial.print(", ");
  Serial.print(currentClock.minutes); Serial.print(", ");
  Serial.print(currentClock.seconds); Serial.print(", ");
  Serial.print(currentClock.day); Serial.print(", ");
  Serial.print(currentClock.month); Serial.print(", ");
  Serial.println(currentClock.year);

  EEPROM.put(0, currentClock);
}


void printSavedClock() {
  Clock savedClock;
  EEPROM.get(0, savedClock);

  // Debug: Print retrieved data to Serial Monitor
  Serial.print("Retrieved Clock Data: ");
  Serial.print(savedClock.hours); Serial.print(", ");
  Serial.print(savedClock.minutes); Serial.print(", ");
  Serial.print(savedClock.seconds); Serial.print(", ");
  Serial.print(savedClock.day); Serial.print(", ");
  Serial.print(savedClock.month); Serial.print(", ");
  Serial.println(savedClock.year);

  display.setTextSize(1);

  if (savedClock.year > 2000) { // Validate year as a simple check for valid data
    display.print(F("Time: "));
    display.print((savedClock.hours < 10) ? "0" : "");
    display.print(savedClock.hours);
    display.print(":");
    display.print((savedClock.minutes < 10) ? "0" : "");
    display.print(savedClock.minutes);
    display.print(":");
    display.print((savedClock.seconds < 10) ? "0" : "");
    display.print(savedClock.seconds);
    display.println();
    display.print(F("Date: "));
    display.print((savedClock.day < 10) ? "0" : "");
    display.print(savedClock.day);
    display.print("/");
    display.print((savedClock.month < 10) ? "0" : "");
    display.print(savedClock.month);
    display.print("/");
    display.println(savedClock.year);
  } else {
    display.println(F("No valid data"));
  }

  display.display();
}


void smileFace() { // Function to display a smiling face
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Draw a circle for the face
  display.drawCircle(64, 45, 18, SSD1306_WHITE);  // Center (64, 32), radius 30

  // Draw the eyes
  display.fillCircle(55, 40, 2, SSD1306_WHITE);  // Left eye
  display.fillCircle(73, 40, 2, SSD1306_WHITE);  // Right eye

  // Draw a smiling mouth (top half of a circle pointing upward)
  for (int i = 0; i < 1; i++) {  // Adjust the thickness by looping
  display.drawCircleHelper(64, 45, 10 - i, 0b00001100, SSD1306_WHITE);  // Bottom half (centered at 180 degrees)
  }

  display.display();
}

void sadFace() { // Function to display a sad face
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Draw a circle for the face
  display.drawCircle(64, 45, 18, SSD1306_WHITE);  // Center (64, 32), radius 30

  // Draw the eyes
  display.fillCircle(55, 40, 2, SSD1306_WHITE);  // Left eye
  display.fillCircle(73, 40, 2, SSD1306_WHITE);  // Right eye

  // Draw a sad mouth (bottom half of a circle)
  for (int i = 0; i < 1; i++) {  // Adjust the thickness by looping
    display.drawCircleHelper(64, 55, 8 - i, 0b00000011, SSD1306_WHITE);  // Bottom half (centered at 180 degrees)
  }

  display.display();
}
