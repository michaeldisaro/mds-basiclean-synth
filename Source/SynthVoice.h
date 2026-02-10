#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

// SynthSound - defines what notes this synth can play
class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() {}

    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};

// SynthVoice - generates the actual audio for the synthesizer
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound,
                   int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;

    void setWaveform(int waveformType) { currentWaveform = waveformType; }
    void setADSR(float attack, float decay, float sustain, float release)
    {
        adsrParams.attack = attack;
        adsrParams.decay = decay;
        adsrParams.sustain = sustain;
        adsrParams.release = release;
        adsr.setParameters(adsrParams);
        sustainLevel = sustain;
    }
    void setVolume(float vol) { volume = vol; }
    void setSupersaw(bool enabled) { supersawEnabled = enabled; }
    void setWidth(float w) { width = w; }
    void setStrength(float s) { strength = s; }
    void setOctave(int oct) { octave = oct; }

    // OSC2 methods
    void setWaveform2(int waveformType) { currentWaveform2 = waveformType; }
    void setADSR2(float attack, float decay, float sustain, float release)
    {
        adsrParams2.attack = attack;
        adsrParams2.decay = decay;
        adsrParams2.sustain = sustain;
        adsrParams2.release = release;
        adsr2.setParameters(adsrParams2);
        sustainLevel2 = sustain;
    }
    void setVolume2(float vol) { volume2 = vol; }
    void setSupersaw2(bool enabled) { supersawEnabled2 = enabled; }
    void setWidth2(float w) { width2 = w; }
    void setStrength2(float s) { strength2 = s; }
    void setOctave2(int oct) { octave2 = oct; }

private:
    void updateHighPassFilter();
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
    int currentWaveform = 0; // 0=sine, 1=square, 2=triangle, 3=saw, 4=noise, 5=supersaw
    juce::Random random;
    
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    float volume = 0.5f;
    float sustainLevel = 0.7f;
    float width = 50.0f; // Width for superwave detuning in Hz
    float strength = 5.0f; // Strength multiplier for detuning spread
    bool supersawEnabled = false;
    int octave = 0; // Octave shift: -2, -1, 0, +1, +2
    
    // OSC2 variables
    double currentAngle2 = 0.0;
    double angleDelta2 = 0.0;
    int currentWaveform2 = 0;
    juce::ADSR adsr2;
    juce::ADSR::Parameters adsrParams2;
    float volume2 = 0.0f;
    float sustainLevel2 = 0.7f;
    float width2 = 50.0f;
    float strength2 = 5.0f;
    bool supersawEnabled2 = false;
    int octave2 = 0; // Octave shift: -2, -1, 0, +1, +2
    
    // Supersaw specific variables for OSC1
    std::array<double, 7> supersawAngles = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 7> supersawAngleDeltas = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    // Supersaw specific variables for OSC2
    std::array<double, 7> supersawAngles2 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 7> supersawAngleDeltas2 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    // Aggiunto: Filtro passa alto per superwave
    juce::IIRFilter highPassFilter1; // Filtro 2° ordine per slope 12 dB
    
    // Parametri del filtro (modificabili)
    double cutoffFreq = 150.0; // Frequenza di cutoff in Hz
    
    bool shouldAutoRelease = false;
    double attackDecayTime = 0.0;
    double currentTime = 0.0;
    
    float getWaveformSample();
    float getWaveformSample2();
};
