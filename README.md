# 🎮 LED Memory Game - ESP32 & FreeRTOS.

## 📌 Project Overview
This project brings a classic memory game to life using ESP32, FreeRTOS, and Blynk. The goal is to replicate LED light sequences displayed by the ESP32 using virtual buttons in the Blynk app. As the game progresses, sequences become more complex, testing the player's memory and reflexes.

✅ **Built using FreeRTOS** on ESP32 for real-time task management.

✅ **Controlled via the Blynk app** with virtual LED buttons.

✅ **Dynamic difficulty levels** - starts easy and gets harder.

✅ **LCD display integration** for score tracking and prompts.

✅ **Interactive & Engaging** for players of all skill levels.

------------------------------------------------------------------------------------------------------------------------------
## 🔥 Key Features
1️⃣ **Dynamic LED Pattern Generation** - The game generates a random sequence that increases in length with each level.

2️⃣ **User Interaction via Blynk** - Players input the pattern using virtual buttons in the Blynk app.

3️⃣ **FreeRTOS Task Management** - Efficient task handling for LED control, user input, and display updates.

4️⃣ **Game Score Tracking** - Displays current score & mistakes on an LCD.

5️⃣ **Difficulty Scaling** - Starts with 3 LEDs and increases up to 22 LEDs at level 20.

------------------------------------------------------------------------------------------------------------------------------

## ⚙️ How It Works
🔹 **Step 1:** Press the Start Game button in the Blynk app.

🔹 **Step 2:** The ESP32 generates an LED sequence (e.g., LED 1 → LED 3 → LED 2).

🔹 **Step 3:** Players must repeat the sequence correctly using virtual buttons.

🔹 **Step 4:** If correct, the next level starts with a longer sequence.

🔹 **Step 5:** If incorrect, the mistake counter increases (max 3 mistakes per level).

🔹 **Step 6:** The game ends if mistakes exceed the limit or if the player reaches level 20.

------------------------------------------------------------------------------------------------------------------------------

## 🛠️ Hardware & Software Requirements
✔ ESP32.

✔ Breadboard.

✔ 4 LEDs (different colors).

✔ Resistors (220Ω, 4.7KΩ, 10KΩ).

✔ LCD Display.

✔ Blynk App (Android/iOS).

------------------------------------------------------------------------------------------------------------------------------
## 📱 Blynk App Interface
Below is a screenshot of the Blynk app interface, which allows users to interact with the game by replicating the LED patterns.

<img src="https://github.com/user-attachments/assets/ba83be4d-1dd3-4344-b98b-91cc1565865c" alt="Blynk App Interface" width="200"/>


------------------------------------------------------------------------------------------------------------------------------
## 🎬 Demo: Playing the LED Memory Game
Watch the real-time demo of the LED Memory Game in action! 🎥

https://github.com/user-attachments/assets/8e9da3e2-8132-4517-90e1-b57a7ce193e9
