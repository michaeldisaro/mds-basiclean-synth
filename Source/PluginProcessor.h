#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

class BasicleanSynthAudioProcessor : public juce::AudioProcessor
{
public:
    BasicleanSynthAudioProcessor();
    ~BasicleanSynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void addMidiMessage(const juce::MidiMessage& message)
    {
        const juce::ScopedLock lock(midiMessageLock);
        pendingMidiMessages.addEvent(message, 0);
    }
    
    void setAudioBufferReadyCallback(std::function<void(const juce::AudioBuffer<float>&)> callback)
    {
        const juce::ScopedLock lock(callbackLock);
        onAudioBufferReady = callback;
    }

private:
    juce::Synthesiser synth;
    juce::AudioProcessorValueTreeState apvts;
    
    juce::MidiBuffer pendingMidiMessages;
    juce::CriticalSection midiMessageLock;
    
    std::function<void(const juce::AudioBuffer<float>&)> onAudioBufferReady;
    juce::CriticalSection callbackLock;
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highPassFilter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicleanSynthAudioProcessor)
};
