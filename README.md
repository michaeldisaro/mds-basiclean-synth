# BasicleanSynth - JUCE Audio Plugin

A clean and basic dual-oscillator synthesizer plugin built with JUCE framework. Supports VST3, AU, and Standalone formats.

## Features

- **Dual Oscillators**: Each with selectable waveforms (Sine, Square, Triangle, Saw, Noise, Supersaw)
- **ADSR Envelopes**: Independent attack, decay, sustain, release controls for each oscillator
- **Supersaw Detuning**: Adjustable width and strength for rich, chorus-like effects
- **Octave Shifting**: Range from -2 to +2 octaves for each oscillator
- **Built-in Oscilloscope**: Real-time visualization of the audio output
- **MIDI Support**: Responds to MIDI note on/off messages with polyphony
- **Custom UI**: Sleek interface with parameter controls

## Prerequisites

1. **JUCE Framework**: Version 7.x or higher (already included in the repository)
2. **CMake**: Version 3.15 or higher
3. **C++ Compiler**:
   - macOS: Xcode Command Line Tools
   - Windows: Visual Studio 2019 or later
   - Linux: GCC 7+ or Clang 6+

## Setup

The JUCE framework is already included in the `JUCE/` directory. No additional setup is required beyond the prerequisites.

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
# - VST3: ~/Library/Audio/Plug-Ins/VST3/Basiclean Synth.vst3
# - AU: ~/Library/Audio/Plug-Ins/Components/Basiclean Synth.component
# - Standalone: build/BasicleanSynth_artefacts/Standalone/Basiclean Synth.app
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

# The built plugins will be in build/BasicleanSynth_artefacts/
```

## Usage

1. Load the plugin in your DAW (Ableton, Logic Pro, FL Studio, Reaper, etc.)
2. Send MIDI notes to the plugin
3. Adjust oscillator waveforms, ADSR parameters, and other settings via the GUI
4. Use the built-in oscilloscope to visualize the output waveform
5. Experiment with supersaw detuning for richer sounds

The synth supports polyphony and responds to MIDI velocity and pitch bend.

## Project Structure

- `Source/PluginProcessor.*` - Main audio processing and plugin logic
- `Source/PluginEditor.*` - GUI interface and parameter controls
- `Source/SynthVoice.*` - Oscillator and synthesis implementation
- `Source/ADSRComponent.h` - ADSR envelope UI component
- `Source/OscilloscopeComponent.h` - Real-time waveform display
- `Source/CustomLookAndFeel.h` - Custom UI styling
- `CMakeLists.txt` - Build configuration
- `Resources/` - Icons and assets

## License

Customize based on your needs. JUCE has GPL and commercial licenses - see https://juce.com/juce-7-licence/
