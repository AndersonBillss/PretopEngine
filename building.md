## Note

- Always run build commands from the **project root directory**.

# Building the Engine for Windows

## Prerequisites

Make sure the following tools are installed and available in your system `PATH`:

- **Go**
- **Python**
- **MSVC Build Tools**
  - Install via **Visual Studio Installer**
  - Include the **Desktop development with C++** workload

---

## Build Instructions

### Option 1: Visual Studio Community 2022

If you are using **Visual Studio Installer (Community 2022)**:

1. Run:

   ```
   ./script/windows/setup.bat
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
   python tools/cli.py setup
   ```

3. Run the engine:

   ```
   python tools/cli.py run native
   ```

# Building the Engine for Linux 

## Prerequisites

Make sure the following tools are installed and available in your system `PATH`:

- **Python**
- **CMake**
- **Ninja**

Install the following dependencies:

- **libxrandr-dev**
- **libxinerama-dev**
- **libxcursor-dev**
- **libxi-dev**
- **libx11-xcb-dev**
- **libwayland-dev**
- **libxkbcommon-dev**
- **mesa-common-dev**
- **pkg-config**

## Build instructions

1. Run the setup build:

   ```
   python tools/cli.py setup
   ```

2. Run the engine:

   ```
   python tools/cli.py run native
   ```

# Building the Engine for Web

## Prerequisites

Download and install the **[Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)**.

After installation, activate the Emscripten environment and make sure the following tools are available in your system `PATH`:

* **Python**
* **CMake**
* **emcmake** (provided by the Emscripten SDK)

## Build Instructions

Unlike native builds, the web target does **not** require the Dawn setup step.

Build and run the web target with:

```sh
python tools/cli.py run web
```
