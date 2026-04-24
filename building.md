# Building the Engine (Windows)

> Currently, only Windows is supported.

## Prerequisites

Make sure the following tools are installed and available in your system `PATH`:

* **Go**
* **Python** 
* **MSVC Build Tools**

  * Install via **Visual Studio Installer**
  * Include the **Desktop development with C++** workload

---

## Build Instructions

### Option 1: Visual Studio Community 2022

If you are using **Visual Studio Installer (Community 2022)**:

1. Run:

   ```
   ./script/windows/build-dawn.bat
   ```

2. Then start the engine:

   ```
   ./script/windows/run.bat
   ```

---

### Option 2: Other Visual Studio Versions

If you are using a different version of Visual Studio:

1. Open an **MSVC Developer Command Prompt**
   (This ensures the compiler and environment variables are set correctly.)

2. Run the setup build:

   ```
   python tools/build.py dawn-debug-setup
   ```

3. Run the engine:

   ```
   python tools/build.py run-native-debug
   ```

---

## Note

* Always run build commands from the **project root directory**.
