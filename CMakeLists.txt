cmake_minimum_required(VERSION 3.15)

project(BasicleanSynth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
add_subdirectory(JUCE)

# Define the plugin
juce_add_plugin(BasicleanSynth
    COMPANY_NAME "MDSynth"
    PLUGIN_MANUFACTURER_CODE MDSS
    PLUGIN_CODE MDSBS001
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Basiclean Synth"
    ICON_BIG Resources/icon.png
    ICON_SMALL Resources/icon.png
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Instrument Synth
)

# Source files
target_sources(BasicleanSynth
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/SynthVoice.cpp
)

# Include directories
target_include_directories(BasicleanSynth
    PRIVATE
        Source
)

# Link JUCE modules
target_compile_definitions(BasicleanSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(BasicleanSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
