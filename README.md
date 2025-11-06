# 🐍 Reptile Rush

**Reptile Rush** is a modern take on the classic *Snake Game*, built using **C++** and **SFML (Simple and Fast Multimedia Library)**.
It features smooth animations, dynamic UI elements, score popups, and an elegant gameplay experience — all crafted with simplicity and precision.

---

## 🎮 Features

* 🧩 Classic snake gameplay with grid-based movement
* 🍎 Food and bonus food mechanics for scoring
* 🧱 Random obstacles for increased difficulty
* 🕹️ Pause/Resume and Game Over logic
* 💬 Real-time score and pop-up effects
* 🎨 Custom UI elements designed using SFML shapes and text

---

## ⚙️ Technologies Used

* **Language:** C++17
* **Graphics Library:** SFML 2.6.1 (GCC 14.2.0 MinGW SEH 64-bit)
* **IDE/Editor:** Visual Studio Code
* **Compiler:** MinGW-w64 (ucrt-posix-seh) 14.2.0

---

## 🚀 Setup & Run Instructions

### 1️⃣ Clone the Repository

```bash
git clone https://github.com/krishadoshi16/Reptile-rush.git
cd Reptile-rush
```

### 2️⃣ Install SFML

Download **SFML 2.6.1 (GCC 14.2.0 MinGW SEH 64-bit)**
from [https://www.sfml-dev.org/download.php](https://www.sfml-dev.org/download.php)
and extract it to:

```
C:\SFML
```

### 3️⃣ Compile the Game

In your terminal:

```bash
g++ main.cpp snake.cpp food.cpp ui.cpp -IC:/SFML/include -LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -o app.exe
```

### 4️⃣ Copy Required DLLs

Copy all `.dll` files from:

```
C:\SFML\bin
```

into your project folder (same directory as `app.exe`).

### 5️⃣ Run the Game

```bash
app.exe
```

---

## 📁 Project Structure

```
Reptile-rush/
│
├── main.cpp           # Game entry point
├── snake.cpp          # Snake logic
├── food.cpp           # Food & bonus mechanics
├── ui.cpp             # UI elements and score display
├── snake.hpp          # Snake class declarations
├── food.hpp           # Food class declarations
├── ui.hpp             # UI element structure
├── assets/            # (optional) fonts, images
├── README.md          # Project documentation
```

---

## 📜 License

This project is open source and available under the [MIT License](https://opensource.org/licenses/MIT).

---
