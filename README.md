# BasicleanSynth - JUCE Audio Plugin

A clean and basic dual-oscillator synthesizer plugin built with JUCE framework. Supports VST3, AU, and Standalone formats.

## Prerequisites

1. **JUCE Framework**: Download from https://juce.com/get-juce/download
2. **CMake**: Version 3.15 or higher
3. **C++ Compiler**:
   - macOS: Xcode Command Line Tools
   - Windows: Visual Studio 2019 or later
   - Linux: GCC 7+ or Clang 6+

## Setup

1. Clone/download JUCE into this project directory:

```bash
git clone https://github.com/juce-framework/JUCE.git
```

Or download and extract JUCE into a folder named `JUCE` in this directory.

2. Make sure the JUCE folder is at: `mds-basiclean-synth/JUCE/`

## Build Instructions

### macOS

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# The built plugins will be in:
# - VST3: ~/Library/Audio/Plug-Ins/VST3/Simple Synth.vst3
# - AU: ~/Library/Audio/Plug-Ins/Components/Simple Synth.component
# - Standalone: build/SimpleSynth_artefacts/Standalone/Simple Synth.app
```

### Windows

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake (Visual Studio)
cmake .. -G "Visual Studio 16 2019"

# Build
cmake --build . --config Release

# Plugins will be copied to standard VST3 directory
```

### Linux

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# The built plugins will be in build/SimpleSynth_artefacts/
```

## Usage

1. Load the plugin in your DAW (Ableton, Logic Pro, FL Studio, Reaper, etc.)
2. Send MIDI notes to the plugin
3. Hear simple sine wave tones based on the MIDI input

The synth has 8 voices of polyphony and responds to MIDI note on/off messages with simple envelope (tail-off on note release).

## Project Structure

- `Source/PluginProcessor.*` - Main audio processing and plugin logic
- `Source/PluginEditor.*` - GUI interface
- `Source/SineWaveVoice.*` - Sine wave synthesis implementation
- `CMakeLists.txt` - Build configuration

## Next Steps

You can extend this synth by:

- Adding ADSR envelope controls
- Implementing different waveforms (saw, square, triangle)
- Adding filters and effects
- Creating a more sophisticated UI with parameter controls
- Adding presets system

## License

Customize based on your needs. JUCE has GPL and commercial licenses - see https://juce.com/juce-7-licence/
