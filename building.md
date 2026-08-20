## Note

- Always run build commands from the **project root directory**.

# Building the Engine for Windows

## Prerequisites

Make sure the following tools are installed and available in your system `PATH`:

- **UV**
- **Python**
- **MSVC Build Tools**
  - Install via **Visual Studio Installer**
  - Include the **Desktop development with C++** workload

---

## Build Instructions

1. Open an **MSVC Developer Command Prompt**
   (This ensures the compiler and environment variables are set correctly.)

2. Sync uv project:

   ```
   uv sync
   ```

3. Run the setup build:

   ```
   uv run cli.py setup
   ```

4. Run the engine:

   ```
   uv run cli.py run native
   ```

# Building the Engine for Linux 

## Prerequisites

Make sure the following tools are installed and available in your system `PATH`:

- **UV**
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
- **python3-dev**

## Build instructions

1. Sync uv project:

   ```
   uv run python cli.py setup
   ```

2. Run the setup build:

   ```
   uv run python cli.py setup
   ```

3. Run the engine:

   ```
   uv run python cli.py run native
   ```

# Building the Engine for Web

## Prerequisites

Download and install the **[Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)**.

After installation, activate the Emscripten environment and make sure the following tools are available in your system `PATH`:

* **Python**
* **CMake**
* **emcmake** (provided by the Emscripten SDK)

## Build Instructions

Follow the setup instructions for whatever OS your are using. Build and run the web target with:

```
uv run python cli.py run web
```
