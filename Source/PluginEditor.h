#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "OscilloscopeComponent.h"

class BasicleanSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Button::Listener
{
public:
    BasicleanSynthAudioProcessorEditor(BasicleanSynthAudioProcessor&);
    ~BasicleanSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    void buttonStateChanged(juce::Button* button) override;
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message);

private:
    class MidiKeyboardListener : public juce::MidiKeyboardState::Listener
    {
    public:
        MidiKeyboardListener(BasicleanSynthAudioProcessor& processor) : audioProcessor(processor) {}
        
        void handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override
        {
            juce::MidiMessage message = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
            audioProcessor.addMidiMessage(message);
        }
        
        void handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override
        {
            juce::MidiMessage message = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
            audioProcessor.addMidiMessage(message);
        }
        
    private:
        BasicleanSynthAudioProcessor& audioProcessor;
    };
    
    void updateFilterButtons(int selectedFilterType);
    void updateFilterButtons2(int selectedFilterType);
    
    BasicleanSynthAudioProcessor& audioProcessor;
    
    juce::Rectangle<int> filterSectionBounds;
    juce::Rectangle<int> osc1SectionBounds;
    juce::Rectangle<int> osc2SectionBounds;
    juce::Rectangle<int> oscilloscopeSectionBounds;
    juce::Rectangle<int> adsrSectionBounds;
    juce::Rectangle<int> adsrSectionBounds2;
    juce::Rectangle<int> filterSectionBounds2;
    
    CustomLookAndFeel customLookAndFeel;
    
    OscilloscopeComponent oscilloscope;
    
    // ADSR1 vertical sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    
    // ADSR2 vertical sliders
    juce::Slider attackSlider2;
    juce::Slider decaySlider2;
    juce::Slider sustainSlider2;
    juce::Slider releaseSlider2;
    juce::Label attackLabel2;
    juce::Label decayLabel2;
    juce::Label sustainLabel2;
    juce::Label releaseLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment2;
    
    juce::Slider waveformKnob;
    juce::Label waveformLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveformAttachment;
    
    // Octave controls
    juce::ComboBox octaveBox;
    juce::Label octaveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> octaveAttachment;
    
    juce::Slider volumeKnob;
    juce::Label volumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    
    juce::ToggleButton supersawButton;
    juce::Label supersawLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> supersawAttachment;
    
    juce::Slider widthKnob;
    juce::Label widthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    
    juce::Slider strengthKnob;
    juce::Label strengthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strengthAttachment;
    
    // OSC2 controls
    juce::Slider waveformKnob2;
    juce::Label waveformLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveformAttachment2;
    
    // Octave2 controls
    juce::ComboBox octaveBox2;
    juce::Label octaveLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> octaveAttachment2;
    
    juce::Slider volumeKnob2;
    juce::Label volumeLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment2;
    
    juce::ToggleButton supersawButton2;
    juce::Label supersawLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> supersawAttachment2;
    
    juce::Slider widthKnob2;
    juce::Label widthLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment2;
    
    juce::Slider strengthKnob2;
    juce::Label strengthLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strengthAttachment2;
    
    juce::ComboBox filterTypeComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    
    juce::Slider cutoffKnob;
    juce::Label cutoffLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    
    juce::Slider resonanceKnob;
    juce::Label resonanceLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    
    juce::Slider qFactorKnob;
    juce::Label qFactorLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qFactorAttachment;
    
    // Filter2 controls
    juce::ComboBox filterTypeComboBox2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment2;
    
    juce::Slider cutoffKnob2;
    juce::Label cutoffLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment2;
    
    juce::Slider resonanceKnob2;
    juce::Label resonanceLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment2;
    
    juce::Slider qFactorKnob2;
    juce::Label qFactorLabel2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qFactorAttachment2;
    
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent midiKeyboard;
    MidiKeyboardListener midiListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicleanSynthAudioProcessorEditor)
};
