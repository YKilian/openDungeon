<p align="center">
  <img src="assets/logo.svg" alt="openFM Logo" width="200">
</p>

# openDungeon

![C++](https://img.shields.io/badge/C%2B%2B-20-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![SFML](https://img.shields.io/badge/SFML-%238CC445.svg?style=for-the-badge&logo=sfml&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

**openDungeon** is a 2.5D dungeon exploration prototype built with **C++20** and **SFML**. It features a custom tilt-transformation engine for isometric rendering and is designed to be **easy to build and run** with minimal setup.

---

## Prerequisites

Ensure your system meets the following requirements:

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install g++ cmake libsfml-dev
```

### Windows

- Install **[CLion](https://www.jetbrains.com/clion/)** or **[Visual Studio 2022](https://visualstudio.microsoft.com/)** (with "Desktop development with C++" workload).
- Install **[CMake](https://cmake.org/download/)** (version 3.16 or higher).

---

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/openDungeon.git
cd openDungeon
```

### 2. Build the Project

#### Linux/macOS (Terminal)

```bash
mkdir build && cd build
cmake .. && cmake --build . --config Release
```

#### Windows (CLion/Visual Studio)

1. Open the project in **CLion** or **Visual Studio**.
2. Select **Release** as the build configuration.
3. Click **Build** (or press `Ctrl+F9` in CLion).

---

### 3. Run the Game

After building, the executable (`openDungeon` or `openDungeon.exe`) will be in the `build/` folder.

#### Linux/macOS

```bash
cd build
./openDungeon
```

#### Windows

```bash
cd build\Release
openDungeon.exe
```

---

## Controls


| Key/Action       | Function                          |
| ---------------- | --------------------------------- |
| **W/A/S/D**      | Move the player character.        |
| **Left Control** | Hold to sprint (faster movement). |
| **Close Window** | Exit the game.                    |


---

## Project Structure

```
openDungeon/
├── src/            # C++ source files
├── include/        # Header files
├── assets/         # Textures and game assets
├── GameMapConfig.json # Map and biome configurations
└── CMakeLists.txt  # Build configuration
```

---

## Troubleshooting

### "Assets or Config File Missing"

- Ensure `assets/` and `GameMapConfig.json` are in the same folder as the executable.

### "SFML Not Found"

- Reinstall SFML:
  ```bash
  sudo apt-get install libsfml-dev  # Linux
  ```
  For Windows, ensure **SFML is correctly linked** in your IDE.

### "CMake/Compiler Errors"

- Delete the `build/` folder and rebuild:
  ```bash
  rm -rf build && mkdir build && cd build && cmake .. && make
  ```

---

## License

This project is licensed under the **MIT License**.
