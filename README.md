# ATLAS Collision Data Analysis

A high-performance toolkit for loading, querying, and visualizing 100,000 proton–proton collision events from the ATLAS experiment at the Large Hadron Collider (LHC). This project measures energy-conversion efficiency (rest energy out / 13 TeV) and explores particle production patterns (electrons, muons, photons, jets, taus) using a C++ command-line interface (CLI) powered by PDCurses and a Python-based analysis and visualization pipeline.

## Table of Contents
- [Project Structure](#project-structure)
- [Data](#data)
- [Prerequisites](#prerequisites)
- [Setup & Build](#setup--build)
    - [C++ (CLI)](#c++-cli)
    - [Python (Analysis & Visualization)](#python-analysis--visualization)
- [Usage](#usage)
- [CMake Configuration](#cmake-configuration)
- [Notes & Troubleshooting](#notes--troubleshooting)
- [References & Credits](#references--credits)

## Project Structure

```
ATLAS-Collision-Data-Analysis/
├── CMakeLists.txt                # CMake configuration
├── README.md                     # Project documentation
├── lib/
│   └── pdcurses.a                # Prebuilt PDCurses (MinGW) archive
├── include/
│   ├── pdcurses/                 # PDCurses headers (curses.h, panel.h, etc.)
│   ├── CollisionEvent.h          # Event data structure
│   ├── DataLoader.h              # Data loading utilities
│   ├── DataStructure.h           # Core data structures
│   ├── GridBucketing.h           # Grid-based spatial indexing
│   └── KDTree.h                  # KD-tree spatial indexing
├── src/
│   ├── main.cpp                  # CLI entry point
│   ├── DataLoader.cpp            # Data loading implementation
│   ├── KDTree.cpp                # KD-tree implementation
│   └── GridBucketing.cpp         # Grid-bucketing implementation
├── data/
│   ├── collision_data.bin        # Preprocessed binary data
│   ├── all_events.csv            # CSV export of events
│   ├── range_query_results.csv   # Range query output
│   ├── performance_results.csv   # Performance metrics
│   └── DAOD_PHYSLITE.*.root      # Raw ATLAS ROOT files
├── scripts/
│   ├── process_data.py           # Converts ROOT files to binary
│   └── analyze_collision_data.py # Generates interactive visualizations
├── plots/                        # Output directory for HTML plots
└── requirements.txt              # Python dependencies
```

## Data

Raw ATLAS `DAOD_PHYSLITE` ROOT files are sourced from the [CERN Open Data Portal](https://opendata.cern.ch/record/80001). This project uses files 1, 2, 3, 4, 6, 7, and 8 (first row in the file index under "List files"). Download these `.root` files into the `data/` directory before processing.

**Note**: The `collision_data.bin` file is included in `data/` for immediate querying, so you can skip data conversion if desired.

## Prerequisites

- **Operating System**: Windows (PDCurses via MinGW recommended)
- **C++**:
    - Compiler supporting C++17 (MinGW-w64 or MSVC)
    - CMake ≥ 3.10
- **Python**:
    - Python 3.11 (3.8+ compatible)
    - `venv` for environment isolation
    - Dependencies listed in `requirements.txt`

## Setup & Build

### C++ (CLI)

1. **Clone the repository**:
   ```bash
   git clone <repo-url>
   cd ATLAS-Collision-Data-Analysis
   ```

2. **PDCurses setup**:
    - The repository includes `lib/pdcurses.a` (MinGW-built) and headers in `include/pdcurses/`.
    - To use a custom PDCurses build, replace these files with your own.

3. **Create a build directory**:
   ```bash
   mkdir build
   cd build
   ```

4. **Configure and generate**:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH="<absolute-path-to-project>"
   ```
   Example:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH="C:/Users/You/ATLAS-Collision-Data-Analysis"
   ```

5. **Build the project**:
   ```bash
   cmake --build . --config Release
   ```
   The `analysis.exe` executable will be generated in `build/Release/` (or `build/Debug/`).

### Python (Analysis & Visualization)

1. **Create and activate a virtual environment**:
   ```bash
   python -m venv .venv
   .venv\Scripts\activate
   ```

2. **Install dependencies**:
   ```bash
   pip install --upgrade pip
   pip install -r requirements.txt
   ```

3. **Prepare data (optional)**:
   To regenerate `collision_data.bin` from ROOT files:
   ```bash
   cd scripts
   python process_data.py
   ```
   This script reads `.root` files from `data/` and writes `collision_data.bin`.

## Usage

1. **Run the C++ CLI**:
   ```bash
   cd build/Release
   .\analysis.exe
   ```
   **Menu options**:
    - **Load Data**: Choose KD-Tree or Grid-Bucketing; loads from `collision_data.bin` or regenerates `all_events.csv`.
    - **Query Events**:
        - Range Query: Outputs to `data/range_query_results.csv`.
        - Extremum Query: Finds the maximum-efficiency event.
    - **Generate Performance Report**: Outputs to `data/performance_results.csv`.
    - **Exit**.

2. **Generate Visualizations**:
   Ensure `all_events.csv` exists, then run:
   ```bash
   cd scripts
   python analyze_collision_data.py
   ```
   This generates interactive HTML plots in `plots/`:
    - `efficiency_distribution.html`
    - `particle_multiplicity.html`
    - `efficiency_vs_total_particles.html`
    - `particle_composition_by_efficiency.html`
    - `correlation_heatmap.html`
    - `particle_co_occurrence.html`

   Open these files in a web browser to explore the visualizations.

## CMake Configuration

```cmake
cmake_minimum_required(VERSION 3.10)
project(ATLASCollisionDataAnalysis)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/pdcurses)

# Source files
add_executable(analysis
    src/main.cpp
    src/DataLoader.cpp
    src/KDTree.cpp
    src/GridBucketing.cpp
)

# Link the prebuilt PDCurses (MinGW) archive
target_link_libraries(analysis PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/pdcurses.a
)
```

**CLion Tip**: By default, CLion creates a `cmake-build-debug/` folder. Configure your Run/Debug settings to launch `analysis.exe` from this directory.

## Notes & Troubleshooting

- Ensure ROOT file names in `data/` match those expected by `process_data.py`.
- The included `collision_data.bin` allows `Load Data` to work without running `process_data.py`.
- **Terminal rendering**: Use Windows Terminal or another ANSI-compatible emulator for optimal PDCurses output.
- If Python visualization fails, confirm the virtual environment is activated and all `requirements.txt` packages are installed.
- For CMake issues, verify the `CMAKE_PREFIX_PATH` points to the project root.

## References & Credits

- **CERN Open Data Portal**: [https://opendata.cern.ch/record/80001](https://opendata.cern.ch/record/80001)
- **PDCurses**: [https://pdcurses.org/](https://pdcurses.org/)
- **Uproot**: [https://uproot.readthedocs.io/](https://uproot.readthedocs.io/)
- **Plotly**: [https://plotly.com/](https://plotly.com/)
- **NumPy & SciPy**: [https://numpy.org/](https://numpy.org/), [https://scipy.org/](https://scipy.org/)