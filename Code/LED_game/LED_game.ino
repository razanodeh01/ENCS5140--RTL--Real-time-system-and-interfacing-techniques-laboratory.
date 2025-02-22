/********************************************************************
 * BLYNK TEMPLATE AND AUTH INFO
 *******************************************************************/
#define BLYNK_TEMPLATE_ID "TMPL6qjLU24fy"
#define BLYNK_TEMPLATE_NAME "LED Memory Game"
#define BLYNK_AUTH_TOKEN "n1YxtgISFB5Rj4ZEHhtmAeMYBdQpYPnp"

/********************************************************************
 * LIBRARIES
 *******************************************************************/
#include <WiFi.h>                    
#include <BlynkSimpleEsp32.h>        
#include <LiquidCrystal_PCF8574.h>   
#include <Arduino.h>                 

/********************************************************************
 * USER CONFIGURATIONS
 *******************************************************************/
char ssid[] = "";              
char pass[] = "";       

/********************************************************************
 * LCD CONFIGURATION
 *******************************************************************/
LiquidCrystal_PCF8574 lcd(0x27);    

/********************************************************************
 * LED PINS CONFIGURATION
 *******************************************************************/
const int ledPins[] = {12, 14, 26, 27}; 
const int numLeds = 4;                 

/********************************************************************
 * GAME VARIABLES
 *******************************************************************/
// Arrays to store the sequence and the player's input
int sequence[22];
int playerInput[22];

// Game progress tracking
int level = 1, mistakes = 0, score = 0;
bool gameStarted = false;     // True after Start button pressed
bool sequenceShown = false;   // True after LED sequence displayed
bool endGameDisplayed = false;// True after "Game Over" or "Game Ended" displayed
int inputIndex = 0;           // Tracks the player's input position

/********************************************************************
 * DEBOUNCE VARIABLES
 *******************************************************************/
unsigned long lastInputTime = 0;
const unsigned long debounceDelay = 500;

/********************************************************************
 * BLYNK VIRTUAL PINS
 *******************************************************************/
#define V_START    V0
#define V_BUTTON_1 V1
#define V_BUTTON_2 V2
#define V_BUTTON_3 V3
#define V_BUTTON_4 V4
#define V_RESTART  V6

/********************************************************************
 * FREERTOS TASK HANDLES
 *******************************************************************/
TaskHandle_t taskLED, taskLCDUpdate;

/********************************************************************
 * FINAL SCREEN FLAG
 * When true, prevents LCDUpdateTask from overwriting final messages.
 *******************************************************************/
bool finalScreen = false;

/********************************************************************
 * BLYNK HANDLERS
 *******************************************************************/
// Start button handler
BLYNK_WRITE(V_START) {
  int startState = param.asInt();
  if (startState == 1) {
    // If the switch is turned ON, show "LED Game Started.. !)" then reset the game
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED Game");
    lcd.setCursor(0, 1);
    lcd.print("Started.. !)");
    
    // Suspend LCD updates temporarily (so the text won't be overwritten immediately)
    vTaskSuspend(taskLCDUpdate); 
    vTaskDelay(3000 / portTICK_PERIOD_MS); 
    vTaskResume(taskLCDUpdate); 
    resetGame();
  } else {
    // If the switch is turned OFF, end the game manually
    endGame(true);
  }
}

// Restart button handler
BLYNK_WRITE(V_RESTART) {
  int restartState = param.asInt();
  if (restartState == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The Game");
    lcd.setCursor(0, 1);
    lcd.print("restarted..!");
    vTaskDelay(4000 / portTICK_PERIOD_MS); 
    resetGame();
  }
}

// Button handlers for player's input
BLYNK_WRITE(V_BUTTON_1) { processInput(1); }
BLYNK_WRITE(V_BUTTON_2) { processInput(2); }
BLYNK_WRITE(V_BUTTON_3) { processInput(3); }
BLYNK_WRITE(V_BUTTON_4) { processInput(4); }

/********************************************************************
 * GAME FUNCTIONS
 *******************************************************************/
// Resets all game variables to start anew
void resetGame() {
  level = 1;
  mistakes = 0;
  score = 0;
  inputIndex = 0;
  gameStarted = true;
  sequenceShown = false;
  endGameDisplayed = false;
  finalScreen = false;  
  generateSequence();
}

// Generates a random LED sequence for the current level
void generateSequence() {
  for (int i = 0; i < level + 2; i++) {
    int newLED = random(1, numLeds + 1); 
    sequence[i] = newLED;
  }
  inputIndex = 0;
  sequenceShown = false;
}

// Handles player's button input with software debouncing
void processInput(int button) {
  unsigned long currentTime = millis();
  if (currentTime - lastInputTime < debounceDelay) {
    return; // Ignore extra presses within the debounce delay
  }
  lastInputTime = currentTime;

  // Only accept input if the game is running and the sequence has been shown
  if (!gameStarted || !sequenceShown) return;

  // Record the player's button
  playerInput[inputIndex] = button;

  // Check if it matches the correct sequence
  if (playerInput[inputIndex] != sequence[inputIndex]) {
    mistakes++;
    if (mistakes >= 3) {
      // Game over if 3 mistakes
      endGame(false);
      return;
    }
  } else {
    // Correct input, increment score
    score++;
    inputIndex++;
    // If the player has matched the entire sequence, go to next level
    if (inputIndex > level + 1) {
      levelUp();
    }
  }
}

// Ends the game and displays appropriate messages
void endGame(bool manuallyEnded) {
  gameStarted = false;
  lcd.clear();

  if (manuallyEnded) {
    // Manually ended game: "Game Ended.." for 4 seconds, then blank
    lcd.print("Game Ended..");
    finalScreen = true;  
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    lcd.clear();
    endGameDisplayed = true;    
  } else {
    // Lost the game due to 3 mistakes: "Game Over :(\n3 Mistakes!"
    lcd.print("Game Over :(");
    lcd.setCursor(0, 1);
    lcd.print("3 Mistakes!");
    endGameDisplayed = true;
    finalScreen = true;
    
    // Keep the "Game Over" for 3 seconds
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // Then prompt for Restart
    lcd.clear();
    lcd.print("To restart");
    lcd.setCursor(0, 1);
    lcd.print("Click Restart");  
  }
}

// Called when the player correctly repeats the entire sequence
void levelUp() {
  level++;
  
  // If the user reaches (level > 20) => game is won
  if (level > 20) {
    lcd.clear();
    lcd.print("You Win!");
    finalScreen = true;
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // Prompt to restart
    lcd.clear();
    lcd.print("To restart:");
    lcd.setCursor(0, 1);
    lcd.print("Click Restart");
    gameStarted = false;
    return;
  }

  // Otherwise, proceed to the next level
  mistakes = 0; // Reset mistakes for the new level
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level up!");
  lcd.setCursor(0, 1);
  lcd.print("Going to Level:");
  lcd.print(level);

  // Suspend LCD updates briefly to prevent overwriting
  vTaskSuspend(taskLCDUpdate);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  vTaskResume(taskLCDUpdate);

  generateSequence();
}

/********************************************************************
 * FREERTOS TASKS
 *******************************************************************/
// This task displays the LED sequence, then prints "Your turn!"
void LEDSequenceTask(void *parameter) {
  while (true) {
    if (gameStarted && !sequenceShown) {
      // Initial instruction
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Watch carefully");
      lcd.setCursor(0, 1);
      lcd.print("and Memorize ..!");
      vTaskDelay(2000 / portTICK_PERIOD_MS);

      // Blink each LED in the sequence
      for (int i = 0; i < level + 2; i++) {
        int ledIndex = sequence[i] - 1; 
        if (ledIndex >= 0 && ledIndex < numLeds) {
          digitalWrite(ledPins[ledIndex], HIGH);
          vTaskDelay(500 / portTICK_PERIOD_MS);
          digitalWrite(ledPins[ledIndex], LOW);
          vTaskDelay(500 / portTICK_PERIOD_MS);
        }
      }

      // After showing all LEDs, notify the user it's their turn
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your turn!");
      vTaskDelay(2000 / portTICK_PERIOD_MS);

      // Mark sequence as shown, so we can accept inputs
      sequenceShown = true;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// This task updates the LCD with score/mistakes if allowed (finalScreen==false)
void LCDUpdateTask(void *parameter) {
  while (true) {
    // If we're on a final screen (Game Over, You Win, etc.), do nothing
    if (finalScreen) {
      vTaskDelay(500 / portTICK_PERIOD_MS);
      continue;
    }

    // Otherwise, refresh the LCD
    lcd.clear();

    // If game hasn't ended yet
    if (!endGameDisplayed) {
      // If the game is started, show Score/Mistakes
      if (gameStarted) {
        lcd.setCursor(0, 0);
        lcd.print("Score: ");
        lcd.print(score);
        lcd.setCursor(0, 1);
        lcd.print("Mistakes: ");
        lcd.print(mistakes);
      } 
      // Else, if the game hasn't started, show "Welcome"
      else {
        lcd.setCursor(0, 0);
        lcd.print("Welcome to our");
        lcd.setCursor(0, 1);
        lcd.print("LED Game!");
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/********************************************************************
 * ARDUINO SETUP
 *******************************************************************/
void setup() {
  Serial.begin(115200);

  // Initialize LED pins
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Initialize I2C, LCD, and backlight
  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.setBacklight(1); 
  lcd.clear();
  lcd.print("Setup the Game.");

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Seed the random number generator
  randomSeed(analogRead(0));

  // Create FreeRTOS tasks (LED Sequence & LCD Updates)
  xTaskCreatePinnedToCore(LEDSequenceTask, "LEDSequence", 2048, NULL, 1, &taskLED, 1);
  xTaskCreatePinnedToCore(LCDUpdateTask,  "LCDUpdate",   2048, NULL, 1, &taskLCDUpdate, 1);

  Serial.println("Setup complete.");
}

/********************************************************************
 * ARDUINO MAIN LOOP
 *******************************************************************/
void loop() {
  Blynk.run();
}