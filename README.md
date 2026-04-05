# openDungeon

*A 2.5D Top-Down Dungeon Explorer*

**openDungeon** is a prototype for a 2.5D top-down dungeon exploration game, built with **C++20** and **SFML**. The project features a custom tilt-transformation engine to create an isometric visual effect from 2D coordinates, making it a unique showcase of modern C++ game development techniques.

---

## **Project Overview**

### **Key Features**

- **2.5D Isometric Rendering**: Custom tilt-transformation engine for a 3D-like experience.
- **Modular Design**: Clean separation of game logic, rendering, and data handling.
- **JSON-Based Configuration**: Map layouts and biome settings are defined in `GameMapConfig.json`.
- **Cross-Platform**: Supports **Windows** and **Linux** (Ubuntu/Debian).

### **Technologies Used**


| Technology        | Purpose                                 |
| ----------------- | --------------------------------------- |
| **C++20**         | Core game logic and rendering.          |
| **SFML**          | Windowing, input, and 2D graphics.      |
| **nlohmann_json** | JSON parsing for map data.              |
| **CMake**         | Build system and dependency management. |


---

## **Setup & Installation**

### **Prerequisites**

#### **Windows**

- **CLion** (recommended) or **Visual Studio 2022** (with "Desktop development with C++" workload).
- **CMake** (version 3.16+).
- **Git** (to clone the repository).

#### **Linux (Ubuntu/Debian)**

- **Compiler**: GCC 11 or Clang 13 (C++20 support required).
- **System Libraries**:
  ```bash
  sudo apt-get update
  sudo apt-get install libx11-dev libxrandr-dev libudev-dev libopengl-dev libflac-dev libvorbis-dev libopenal-dev libxcursor-dev libxinerama-dev
  ```

---

### **Step-by-Step Build Guide**

#### **1. Clone the Repository**

```bash
git clone https://github.com/yourusername/openDungeon.git
cd openDungeon
```

#### **2. Configure and Build**

- **CLion Users**:
    - Open the project in CLion.
    - Select **Release** as the build configuration.
    - Click the **Build** button (or press **Ctrl+F9**).
- **Command Line (CMake)**:
  ```bash
  cmake -B build -S .
  cmake --build build --config Release
  ```

#### **3. Prepare the Executable**

After building, copy the following files/folders to the same directory as the executable (`build/Release/` or `cmake-build-release/`):

- The `assets/` folder.
- The `GameMapConfig.json` file.

---

## **Running the Game**

### **Windows**

```bash
cd build\Release
openDungeon.exe
```

### **Linux**

```bash
cd build
./openDungeon
```

---

## **Controls**


| Key/Action       | Function                          |
| ---------------- | --------------------------------- |
| **W/A/S/D**      | Move the player character.        |
| **Left Control** | Hold to sprint (faster movement). |
| **Close Window** | Exit the game.                    |


---

## **Technical Deep Dive**

### **Rendering Engine**

- **Custom Vertex Array System**: Dynamically generates wall faces, shadows, and isometric effects based on tile adjacency.
- **SFML Integration**: Handles windowing, input, and 2D rendering.

### **Data Management**

- **JSON Configuration**: `GameMapConfig.json` defines map layouts, player start positions, and biome settings.
- **Modular Design**: Separates game logic, rendering, and data parsing for maintainability.

---

## **Troubleshooting**

### **Common Issues & Fixes**


| Issue                       | Solution                                                               |
| --------------------------- | ---------------------------------------------------------------------- |
| **"Assets Missing"**        | Copy `assets/` and `GameMapConfig.json` to the executable's directory. |
| **"SFML Not Found"**        | Delete the `build/` folder and reconfigure CMake.                      |
| **"CMake/Compiler Errors"** | Ensure C++20 support and correct toolchain configuration in CLion.     |


---

## **Project Structure**

```
openDungeon/
├── src/               # C++ source files
├── include/           # Header files
├── assets/            # Textures and game assets
├── GameMapConfig.json # Map and biome configurations
└── CMakeLists.txt     # Build configuration
```

---

## **License**

This project is licensed under the **MIT License**