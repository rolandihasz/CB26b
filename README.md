# CoffeeBreak (CB26 / CB26b)

**CoffeeBreak** is a high-performance desktop telemetry and monitoring platform built with **Qt 6** and **C++23**. It combines real-time video feed capture, custom embedded convolutional neural network (CNN) computer vision, hardware GPU and network diagnostics, interactive charting, and an encrypted SQLite database layer with user authentication.

---

## Features

### 1. EagleEye AI Computer Vision
- **In-Engine Neural Network:** Native C++ implementation of the `EagleEyeNet` convolutional neural network without heavyweight external runtime dependencies.
- **Lap & Round Detection:** Evaluates region-of-interest (ROI) frames from video streams to classify round numbers and monitor lap progression with confidence scoring.
- **Model Weights:** Pre-trained binary weights (`eagle_eye_weights.bin`) loaded automatically at startup.

### 2. Live Video Capture & HUD
- **Qt Multimedia Pipeline:** Integrates with `QMediaCaptureSession`, `QCamera`, and `QVideoSink` for high-throughput video streaming.
- **OBS Virtual Camera & Webcam Support:** Seamlessly connects to OBS Virtual Camera feeds or physical camera devices.
- **Live HUD Overlay:** Real-time visual overlay for ROI verification, calibration capture, and frame inspection.

### 3. Hardware Telemetry & Network Diagnostics
- **NVIDIA GPU Monitoring:** Queries GPU hardware statistics (temperature, fan speed, GPU utilization, and VRAM usage) via NVML / `nvidia-smi`.
- **IP Resolution:** Automatically resolves local network (LAN) IP addresses and queries public external IP status.
- **Display Profiling:** Screen resolution and display diagnostics output directly to the event log.

### 4. Interactive Real-Time Charts
- **Lap Time Chart (`LapTimeChart`):** Displays lap duration trends, average lap times, total elapsed time, and CSV import/export (`rounds.csv`).
- **Telemetry Waveform Chart (`SignalChart`):** Multi-channel real-time plotting of GPU utilization and temperature curves.
- **Wavepacket Signal Chart (`WavepacketChart`):** High-frequency signal visualization.

### 5. Local Database & Security
- **SQLite Engine:** Embedded storage (`coffeebreak.db`) for telemetry records, lap times (`roundsTable`), and system accounts (`users`).
- **Encrypted Authentication:** Cryptographic salted SHA-256 password hashing.
  - **Default Username:** `admin`
  - **Default Password:** `admin`
- **Encryption Ready:** Native AES-256-CBC cryptography via Windows CNG (`bcrypt.dll`) and support for SQLite `PRAGMA key` (SQLCipher / SEE).
- **Remote DB Support:** Extensible database drivers for remote PostgreSQL and MySQL servers.

### 6. Qt Designer UI Workflow
- Fully visual and editable form layouts:
  - `mainwindow.ui`: Main telemetry dashboard, charts, control tabs, and console log.
  - `logindialog.ui`: Startup login popup window with live clock, quick help, and power controls.

---

## System Requirements

| Requirement | Minimum / Recommended |
|---|---|
| **Operating System** | Windows 10 / 11 (64-bit) |
| **Framework** | Qt 6.8+ (tested with Qt 6.11.2) |
| **Qt Modules** | `core`, `gui`, `widgets`, `multimedia`, `multimediawidgets`, `charts`, `sql`, `network`, `concurrent` |
| **Compiler** | C++23 compliant (`clang++` 17+, `g++` 13+, or MSVC 2022) |
| **Build Tools** | `qmake` and `mingw32-make` (or Qt Creator) |
| **Hardware** | NVIDIA GPU (optional, for GPU telemetry), Web camera / OBS Virtual Camera |

---

## Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/rolandihasz/CB26b.git
cd CB26b
```

### 2. Build via Command Line (LLVM-MinGW / GCC)
Make sure Qt 6 binaries and your compiler are added to your system `PATH`:

```bash
# 1. Generate Makefiles from project file
qmake cb26.pro

# 2. Compile Release build
mingw32-make -j4
```

### 3. Build via Qt Creator
1. Open **Qt Creator**.
2. Select **File > Open File or Project...** and choose `cb26.pro`.
3. Select your Qt 6 Desktop kit (e.g. Qt 6.11.2 MinGW / Clang 64-bit).
4. Click **Build** (`Ctrl + B`) and **Run** (`Ctrl + R`).

---

## Launching & Authentication

When the application launches, the **CoffeeBreak Login** dialog appears:

1. **Username:** `admin`
2. **Password:** `admin`
3. **Database:** Select `SQLite Local DB` (default)
4. Click **Login** (or press `Enter`).

> **Tip:** You can edit the login window layout at any time by opening `logindialog.ui` in Qt Designer or Qt Creator.

---

## Project Structure

```text
cb26/
├── cb26.pro                  # qmake project definition & module dependencies
├── main.cpp                  # Application entry point & login execution
├── mainwindow.h / .cpp       # Main telemetry dashboard implementation
├── mainwindow.ui             # Qt Designer layout for the main window
├── logindialog.h / .cpp      # Authentication popup dialog implementation
├── logindialog.ui            # Qt Designer layout for the login window
├── databasemanager.h / .cpp  # SQLite/Remote database manager & encryption
├── telemetrymanager.h / .cpp # GPU (NVML) and network IP telemetry monitoring
├── eagleeye_classifier.h/.cpp# Native C++ CNN model forward pass
├── eagle_eye_weights.bin     # Pre-trained CNN weights
├── videofeedwidget.h / .cpp  # Video stream capture and HUD overlay
├── laptimechart.h / .cpp     # Lap timing & historical performance chart
├── signalchart.h / .cpp      # Multi-channel telemetry waveform chart
├── wavepacketchart.h / .cpp  # Signal wavepacket chart
├── icons.qrc                 # Embedded application icons & graphics
├── icons/                    # SVG and PNG UI icons
├── coffeebreak.db            # Local SQLite database
└── rounds.csv                # Historical lap and round benchmark data
```

---

## License & Support

CoffeeBreak © 2019–2026 | [LnRSoft.com](https://lnrsoft.com)
