# 🌱 Smart Eco City Simulation (CLI Game)

A colorful and feature-rich command-line city simulation game built in **C++** that challenges players to manage a futuristic eco-city. Designed for immersive gameplay using emoji-based graphics, real-time stats, dynamic menus, and a smart upgrade/research system.

> 🛠 Created as a second-semester course project to demonstrate real-time simulation, visual CLI interfaces, sustainable development mechanics, and upgrade-based strategy.


## 🎮 Features
* 🗺 **Map Generation** with land/water distribution
* 🏗 **Interactive Building System** (Roads, Residential, Commercial, Forests, Airports & more)
* 📊 **Live Stats Panel** (Funds, Pollution, Temperature, Happiness, etc.)
* 📚 **Upgrade System** with dynamic cost/income/pollution modifiers
* 🔬 **Research Projects** unlocked through quiz-based progress
* ⚠️ **Climate Events** that affect gameplay (heatwaves, floods, economic booms)
* 💬 **Tooltips & Message Bar** for player guidance
* ⌨️ **Arrow Key Navigation** with dropdown tabs and colored CLI


## 📁 Project Structure
```
project_main.cpp         → All logic and simulation code
SmartCity_Questions.txt  → (Optional) Quiz questions for Smart City research
GreenCity_Questions.txt  → (Optional) Quiz questions for Green City research
SustainableCity_Questions.txt → (Optional) For advanced tech research
```
+------------------+
|  Main Loop       |
|  handle_Input()  |
+--------+---------+
         |
         v
+--------+---------+
| UI Menu System   |<---------+
+--------+---------+          |
         |                    |
         v                    |
+--------+---------+   +------+--------+
| Build/Remove Mode|   | Research &    |
+--------+---------+   | Upgrades      |
         |             +------+--------+
         v                    |
+--------+---------+          |
| Map Management   |          |
+--------+---------+          |
         |                    |
         v                    |
+--------+---------+          |
| Game State Engine|<---------+
+--------+---------+
         |
         v
+------------------+
| Visual Output    |
+------------------+

---

## 🔧 Technologies Used

| Technology    | Purpose                          |
| ------------- | -------------------------------- |
| **C++**       | Core language                    |
| `windows.h`   | Font resizing and screen control |
| `conio.h`     | Real-time keyboard input         |
| ANSI Colors   | CLI coloring and highlighting    |
| Emoji (UTF-8) | Game graphics and building icons |
| `fstream`     | Quiz question file handling      |

---

## 🚀 Getting Started

### ✅ Requirements

* Windows OS
* C++17 or later (with `g++`, Visual Studio, or Code::Blocks)

### ⚙️ Compile

```bash
g++ -std=c++17 project_main.cpp -o eco_city_sim
```

### ▶️ Run

```bash
./eco_city_sim
```

✅ Make sure your console supports UTF-8 and ANSI colors for best visuals.

---

## 🧠 Gameplay Overview

| Aspect          | Description                                                               |
| --------------- | ------------------------------------------------------------------------- |
| 🏙 Build        | Select from a variety of buildings using the BUILD tab                    |
| 🧹 Remove       | Switch to remove mode to clear unwanted buildings                         |
| 📈 Stats        | Use STATS tab to track city performance in real-time                      |
| 🚀 Advance Year | Progress one year to apply revenue, pollution, climate events, and growth |
| 🔬 R\&D         | Unlock new technologies by answering quiz questions correctly             |
| 🔧 Upgrades     | Improve building performance, reduce costs, and boost happiness           |

---

## 🌿 Educational Goals

This project simulates:

* Sustainable development
* Climate change consequences
* Budget management in urban planning
* Strategic upgrades and tech adoption

---
# Brainstorming Map
![image](https://github.com/user-attachments/assets/41bd8aae-0cb1-42b9-96ab-6d00a2664fec)


---
# Tasks Division
<img width="631" alt="Screenshot 2025-04-24 182651" src="https://github.com/user-attachments/assets/70cd911a-4281-4c19-bb4f-995a88d7445e" />

---

## 📸 Screenshots

> ![image](https://github.com/user-attachments/assets/c8ee2dc6-e363-4bc7-9ae4-455f6ddfddcb)


---

## 🙌 Credits

Developed by **Atta Ur Rahman Sheikh**
GIKI – BSc Artificial Intelligence – Semester 2
Special thanks to teammates Talal Iqbal, Mazhar Guftar, Ahmed Sajid and Mohsin.

---

## 📜 License

This project is open-source and free to use under the MIT License.

