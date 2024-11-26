// Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>            // OLED display library
#include <Adafruit_SSD1306.h>
#include <Keypad.h>                  // Keypad library

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Keypad matrix configuration
const byte ROWS = 4;               // Four rows
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

//Timer for clock
unsigned long previousMillis = 0; // Variable to store the previous millis for timekeeping
const long interval = 1000;       // Interval for updating time (1 second)
int hour = 0, minute = 0, second = 0; // Variables to track time
int day = 25, month = 11, year = 24;     // Variables to track date

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

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
  displayMainPage();
}

void loop() {

  // PROBELM SIAKKK ANJINGGGGGGG
  // Update time every second
  //unsigned long currentMillis = millis();
  //if (currentMillis - previousMillis >= interval) {
    //previousMillis = currentMillis;
    //updateDateTime();
    //displayMainPage();
  //}

  // Wait until a key is pressed
  char key = keypad.getKey();
  if (key) {
    // Print the key to the serial monitor
    Serial.print("Key Pressed: ");
    Serial.println(key);

    // Handle page navigation based on key press
    switch (key) {
      case '1':
      case '2':
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

  // Other logic for soil moisture sensor and relay control
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
  display.println("Irrigation");
  display.setCursor(0, 20);
  display.println("System");

  // Display "powered by AFA" with smaller text
  display.setTextSize(1); // Smaller text size for the subheading
  display.setCursor(0, 50);
  display.println("powered by AFA");

  display.display(); // Update the display
  delay(2000); // Keep the intro animation for 2 seconds
}

void updateDateTime() {
  // Increment the time
  second++;
  if (second >= 60) {
    second = 0;
    minute++;
    if (minute >= 60) {
      minute = 0;
      hour++;
      if (hour >= 24) {
        hour = 0;
        day++;
        if ((month == 2 && day > 28) ||
            (month == 4 || month == 6 || month == 9 || month == 11) && day > 30 ||
            day > 31) {
          day = 1;
          month++;
          if (month > 12) {
            month = 1;
            year++;
          }
        }
      }
    }
  }
}

void displayMainPage() {
  // Clear the display and set up the main page UI
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display current time (HH:MM:SS)
  display.setCursor(18, 0);
  display.setTextSize(2);
  if (hour < 10) display.print('0');
  display.print(hour);
  display.print(':');
  if (minute < 10) display.print('0');
  display.print(minute);
  display.print(':');
  if (second < 10) display.print('0');
  display.print(second);

  // Display current date (DD-MM-YY)
  display.setCursor(42, 20);
  display.setTextSize(1);
  if (day < 10) display.print('0');
  display.print(day);
  display.print('-');
  if (month < 10) display.print('0');
  display.print(month);
  display.print('-');
  if (year < 10) display.print('0');
  display.print(year);

  // Draw three text boxes representing the options
  int boxY = 30;
  display.setTextSize(2);
  display.drawRect(0, boxY, 40, 22, SSD1306_WHITE);
  display.setCursor(14, boxY + 4);
  display.println("1");
  display.setTextSize(1);
  display.setCursor(3, boxY + 25);
  display.println("Volume");
  
  display.setTextSize(2);
  display.drawRect(44, boxY, 40, 22, SSD1306_WHITE);
  display.setCursor(58, boxY + 4);
  display.println("2");
  display.setTextSize(1);
  display.setCursor(50, boxY + 25);
  display.println("Level");

  display.setTextSize(2);
  display.drawRect(88, boxY, 40, 22, SSD1306_WHITE);
  display.setCursor(102, boxY + 4);
  display.println("3");
  display.setTextSize(1);
  display.setCursor(97, boxY + 25);
  display.println("Pump");

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
      display.println("1");
      break;
    case 2:
      display.fillRect(44, boxY, 40, 22, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(2);
      display.setCursor(58, boxY + 4);
      display.println("2");
      break;
    case 3:
      display.fillRect(88, boxY, 40, 22, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setTextSize(2);
      display.setCursor(102, boxY + 4);
      display.println("3");
      break;
  }
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void handleMainPageConfirmation() {
  while (true) {
    // Wait for user input on the main page
    char key = keypad.getKey();
    if (key) {
      // Print the key to the serial monitor
      Serial.print("Main Page - Key Pressed: ");
      Serial.println(key);

      if (key == '#') {
        // Confirm selection and navigate to corresponding page
        switch (selectedOption) {
          case 1:
            displayPage1();
            handlePage1Input();
            return;
          case 2:
            displayPage("Page 2");
            return;
          case 3:
            displayPage("Page 3");
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

void displayPage(const char* pageTitle) {
  // Clear the display and show the selected page
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(pageTitle);
  display.display();
}

void displayPage1() {
  // Clear the display and set up Page 1 UI
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Draw four text boxes representing the options
  display.setCursor(20, 0);
  display.println("Choose a volume:");

  // Option 1
  display.drawRect(0, 16, 60, 16, SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println("100ml");

  // Option 2
  display.drawRect(64, 16, 60, 16, SSD1306_WHITE);
  display.setCursor(79, 20);
  display.println("200ml");

  // Option 3
  display.drawRect(0, 36, 60, 16, SSD1306_WHITE);
  display.setCursor(15, 40);
  display.println("300ml");

  // Option 4
  display.drawRect(64, 36, 60, 16, SSD1306_WHITE);
  display.setCursor(79, 40);
  display.println("400ml");

  display.display(); // Update the display with the new content
}

void handlePage1Input() {
  selectedOption = 0; // Reset selected option
  while (true) {
    // Wait for user input on Page 1
    char key = keypad.getKey();
    if (key) {
      // Print the key to the serial monitor
      Serial.print("Page 1 - Key Pressed: ");
      Serial.println(key);

      // Clear the display and redraw Page 1 UI with the selected box highlighted
      displayPage1();
      switch (key) {
        case '1':
          selectedOption = 1;
          display.fillRect(0, 16, 60, 16, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK);
          display.setCursor(15, 20);
          display.println("100ml");
          display.setTextColor(SSD1306_WHITE);
          display.display();
          break;
        case '2':
          selectedOption = 2;
          display.fillRect(64, 16, 60, 16, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK);
          display.setCursor(79, 20);
          display.println("200ml");
          display.setTextColor(SSD1306_WHITE);
          display.display();
          break;
        case '3':
          selectedOption = 3;
          display.fillRect(0, 36, 60, 16, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK);
          display.setCursor(15, 40);
          display.println("300ml");
          display.setTextColor(SSD1306_WHITE);
          display.display();
          break;
        case '4':
          selectedOption = 4;
          display.fillRect(64, 36, 60, 16, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK);
          display.setCursor(79, 40);
          display.println("400ml");
          display.setTextColor(SSD1306_WHITE);
          display.display();
          break;
        case '#':
          processSelectedOption(); // Call the new function
            return;
          break;
        case '*':
          // Return to main page
          displayMainPage();
          return;
        default:
          break;
      }
    }
  }
}

// New function to handle the logic when '#' is pressed
void processSelectedOption() {
  if (selectedOption != 0) {
    // Display confirmation message
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.print("Dispensing ");
    display.print(selectedOption);
    display.display();

    // Dispense based on the selected option
    switch (selectedOption) {
      case 1:
        dispense(3); // Dispense 100 ml (~3 seconds)
        break;
      case 2:
        dispense(6); // Dispense 200 ml (~6 seconds)
        break;
      case 3:
        dispense(9); // Dispense 300 ml (~9 seconds)
        break;
      case 4:
        dispense(12); // Dispense 400 ml (~12 seconds)
        break;
      default:
        break;
    }

    delay(2000); // Pause to display the confirmation
    displayMainPage(); // Return to the main page after dispensing
  }
}

// Function to handle dispensing
void dispense(int durationSeconds) {
  // Activate dispensing mechanism
  digitalWrite(relayPin, HIGH); // Replace `dispensePin` with your actual pin connected to the dispensing mechanism
  delay(durationSeconds * 1000);  // Wait for the specified duration
  digitalWrite(relayPin, LOW); // Stop dispensing
}
