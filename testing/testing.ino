// Libraries
#include <EEPROM.h>                  // Memory library
#include <SPI.h>
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
String currentNumber = "";        // Holds the current number entered from the keypad

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

  // if uncomment the introaniamtion, the message in the drawbox page1 will not be display
  //introAnimation();
  mainPage();  
}

void loop() {
  // Wait until a key is pressed
  char key = keypad.getKey();
  if (key) {
    //Clear current display
    display.clearDisplay();
    // Display the corresponding message based on the key pressed
    switch (key) {
    case '1':  // If '1' is pressed
        Page1();
        break;       

    case '2':  // If '2' is pressed
      display.setCursor(0, 0);
      display.setTextSize(1.5);
      display.print("Moisture Value: ");
      display.print(String(voltage));  // Display the moisture voltage
      display.setTextSize(1);
      display.setCursor(0, 55);
      display.print("Press * to go back to main menu");
      break;

    case '3':  // If '3' is pressed
      display.setCursor(0, 0);
      display.setTextSize(1.5);
      display.print("Exit");
      display.setCursor(0, 55);
      display.setTextSize(1);
      display.print("Press * to go back to main menu");
      break;

    case '*':
      display.clearDisplay();
      mainPage();  // Show the main menu page again
      display.display();  // Update the display
      break;

    default:    return;
  }
    // Update the display
    display.display();
  }

  // Other logic for soil moisture sensor and relay control
  int sensorValue = digitalRead(waterSensorPin);
  value = analogRead(moistureAnalogPin);  // Read the ADC value
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

//----------------------------------------------A N I M A T I O N--------------------------------------------//

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
  delay(2000); // Keep the intro animation for 3 seconds
  
}

void mainPage() {
  // Static time and date
  int fixedHour = 11;
  int fixedMinute = 56;
  int fixedSecond = 10;
  int fixedDay = 23;
  int fixedMonth = 11;
  int fixedYear = 2024;

  // Clear the display before drawing
  display.clearDisplay();

  // Draw the static time
  display.setTextSize(2);  // Large text for the clock
  display.setTextColor(SSD1306_WHITE);

  // Display time in fixed format
  display.setCursor(16, 0);  // Centered horizontally (72 is approx width of the time string)
  display.print("11:13:43");

  // Draw the static date
  display.setTextSize(1);  // Smaller text for the date
  display.setCursor(34, 20);  // Centered horizontally
  display.print("11-11-2011");
  
  // Draw the square boxes with numbers below the clock and date
  int boxSize = 21;  // Size of the box
  int margin = 20;   // Margin between boxes
  int startX = (SCREEN_WIDTH - (boxSize * 3 + margin * 2)) / 2;  // Starting X position for the first box
  int startY = (SCREEN_HEIGHT - boxSize) / 2 + 10;  // Adjust Y position below the clock and date
  
  // Box 1
  display.drawRect(startX, startY, boxSize, boxSize, SSD1306_WHITE);
  display.setTextSize(2);  // Bold text size
  display.setCursor(startX + (boxSize / 2) - 6, startY + (boxSize / 2) - 8);  // Adjust text position
  display.print("1");
  display.setTextSize(1); 
  display.setCursor(startX -10, startY + 24);
  display.print("Volume");
 
  // Box 2
  display.setTextSize(2);
  display.drawRect(startX + boxSize + margin, startY, boxSize, boxSize, SSD1306_WHITE);
  display.setCursor(startX + boxSize + margin + (boxSize / 2) - 6, startY + (boxSize / 2) - 8);
  display.print("2");
  display.setTextSize(1); 
  display.setCursor(startX + 35, startY + 24);
  display.print("Index");

  // Box 3
  display.setTextSize(2);
  display.drawRect(startX + 2 * (boxSize + margin), startY, boxSize, boxSize, SSD1306_WHITE);
  display.setCursor(startX + 2 * (boxSize + margin) + (boxSize / 2) - 6, startY + (boxSize / 2) - 8);
  display.print("3");
  display.setTextSize(1); 
  display.setCursor(startX + 75, startY + 24);
  display.print("System");
 
  // Update the display with the new content
  display.display();
}

/*void Page1() { // Function to display Page 1

  // Title
  display.setCursor(15, 0);
  display.setTextSize(1.5);
  display.print("Choose a volume:");

  // Draw square boxes with dynamic selection
  drawBox(18, 15, 60, 35, "1000ml", false);  // Select if '1'
  drawBox(68, 15, 110, 35, "2000ml", false); // Select if '2'
  drawBox(18, 40, 60, 60, "3000ml", false); // Select if '3'
  drawBox(68, 40, 110, 60, "Other", false); // Select if '4'

  // Instruction to go back
  display.setTextSize(1);
  display.setCursor(0, 55);

  display.display();  // Update the display
}*/

// Function to display Page 1
void Page1() {

  display.clearDisplay();
  // Title
  display.setCursor(15, 0);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1.5);
  display.print("Choose a volume:");

  display.setTextSize(2);
  display.setCursor(2, 18);
  display.print("1");
  display.setCursor(115, 18);
  display.print("2");
  display.setCursor(2, 43);
  display.print("3");
  display.setCursor(115, 43);
  display.print("4");

  char keyin = keypad.getKey();

  // Draw square boxes with dynamic selection
  drawBox(18, 15, 60, 35, "1000ml", false);  // Select if '1'
  drawBox(68, 15, 110, 35, "2000ml", true); // Select if '2'
  drawBox(18, 40, 60, 60, "3000ml", true); // Select if '3'
  drawBox(68, 40, 110, 60, "Other", true); // Select if '4'

  display.display();  // Update the display
}

void loadingCircle(float animationDelay) { 
  int centerX = SCREEN_WIDTH / 2;  // Center X coordinate of the circle
  int centerY = SCREEN_HEIGHT / 2; // Center Y coordinate of the circle
  int radius = 10;                 // Radius of the circle

  for (int angle = 0; angle < 360; angle += 15) {  // Increment angle in steps
    display.clearDisplay();

    // Draw static circle outline
    display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);

    // Calculate the position of the loading point
    float radian = angle * (PI / 180.0);
    int x = centerX + radius * cos(radian);
    int y = centerY + radius * sin(radian);

    // Draw the loading point
    display.fillCircle(x, y, 2, SSD1306_WHITE);

    display.display();
    delay(animationDelay); // Delay to create animation effect
  }
    display.clearDisplay();
    display.display();
}

void loadingAnimation() { // Loading animation for progress
  unsigned long startTime = millis();  // Record the start time
  unsigned long duration = 5000;  // Set duration for 5 seconds (5000 ms)
  
  int barWidth = 0;  // Initial width of the loading bar
  int barMaxWidth = SCREEN_WIDTH - 20;  // Maximum width of the loading bar
  
  while (millis() - startTime < duration) {  // Continue for 5 seconds
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print("Loading...");

    // Update the width of the loading bar
    barWidth = map(millis() - startTime, 0, duration, 0, barMaxWidth);

    // Draw the loading bar
    display.drawRect(10, 30, barMaxWidth, 10, SSD1306_WHITE);  // Draw the outer rectangle
    display.fillRect(10, 30, barWidth, 10, SSD1306_WHITE);     // Fill the progress part of the bar

    display.display();  // Update the display
    delay(40);  // Delay to make the animation visible
  }

  display.clearDisplay();
  display.setCursor(10, 30);
  display.display();
}

void drawBox(int x1, int y1, int x2, int y2, String text, bool inverted) { // Function to draw a box with optional inversion
  if (inverted) {
    // Draw a filled rectangle for inversion
    display.fillRect(x1, y1, x2 - x1, y2 - y1, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);  // Text color inverted
  } else {
    // Draw an outline rectangle
    display.drawRect(x1, y1, x2 - x1, y2 - y1, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);  // Normal text color
  }

  // Center the text inside the box
  int textWidth = text.length() * 6;  // Approximate width of the text
  int textX = x1 + (x2 - x1) / 2 - textWidth / 2;
  int textY = y1 + (y2 - y1) / 2 - 4;  // Adjust for text height

  display.setCursor(textX, textY);
  display.setTextSize(1);
  display.print(text);

  // Reset text color for other elements
  display.setTextColor(SSD1306_WHITE);
}
