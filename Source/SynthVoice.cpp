#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    // Set default ADSR parameters for OSC1
    adsrParams.attack = 0.1f;
    adsrParams.decay = 0.3f;
    adsrParams.sustain = 0.7f;
    adsrParams.release = 0.4f;
    adsr.setParameters(adsrParams);
    
    // Set default ADSR parameters for OSC2
    adsrParams2.attack = 0.1f;
    adsrParams2.decay = 0.3f;
    adsrParams2.sustain = 0.7f;
    adsrParams2.release = 0.4f;
    adsr2.setParameters(adsrParams2);

    // Aggiunto: Inizializza il filtro passa alto
    updateHighPassFilter();
}

void SynthVoice::updateHighPassFilter()
{
    // Configura il coefficiente per filtro passa alto 2° ordine (Q = 0.707 per risposta Butterworth-like)
    // Slope 12 dB (2° ordine)
    highPassFilter1.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), cutoffFreq, 0.707));
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

float SynthVoice::getWaveformSample()
{
    float sample = 0.0f;
    
    // Generate base waveform sample
    float baseSample = 0.0f;
    
    switch (currentWaveform)
    {
        case 0: // Sine
            baseSample = std::sin(currentAngle);
            break;
            
        case 1: // Square
            baseSample = (std::sin(currentAngle) >= 0.0f) ? 1.0f : -1.0f;
            break;
            
        case 2: // Triangle
        {
            auto angle = std::fmod(currentAngle, juce::MathConstants<double>::twoPi);
            if (angle < juce::MathConstants<double>::pi)
                baseSample = -1.0f + (2.0f * angle / juce::MathConstants<float>::pi);
            else
                baseSample = 1.0f - (2.0f * (angle - juce::MathConstants<float>::pi) / juce::MathConstants<float>::pi);
            break;
        }
            
        case 3: // Saw
            baseSample = (1.0f / juce::MathConstants<float>::pi) * 
                         (std::fmod(currentAngle, juce::MathConstants<double>::twoPi) - juce::MathConstants<float>::pi);
            break;
            
        case 4: // Noise
            baseSample = random.nextFloat() * 2.0f - 1.0f;
            break;
    }
    
    // Apply supersaw detuning if enabled
    if (supersawEnabled)
    {
        sample = 0.0f;
        for (int i = 0; i < 7; ++i)
        {
            // Use the accumulated detuned angle
            double sawAngle = supersawAngles[i];
            
            // Apply the base waveform to the detuned angle
            float detunedSample = 0.0f;
            switch (currentWaveform)
            {
                case 0: // Sine
                    detunedSample = std::sin(sawAngle);
                    break;
                    
                case 1: // Square
                    detunedSample = (std::sin(sawAngle) >= 0.0f) ? 1.0f : -1.0f;
                    break;
                    
                case 2: // Triangle
                {
                    auto angle = std::fmod(sawAngle, juce::MathConstants<double>::twoPi);
                    if (angle < juce::MathConstants<double>::pi)
                        detunedSample = -1.0f + (2.0f * angle / juce::MathConstants<float>::pi);
                    else
                        detunedSample = 1.0f - (2.0f * (angle - juce::MathConstants<float>::pi) / juce::MathConstants<float>::pi);
                    break;
                }
                    
                case 3: // Saw
                    detunedSample = (1.0f / juce::MathConstants<float>::pi) * 
                                   (std::fmod(sawAngle, juce::MathConstants<double>::twoPi) - juce::MathConstants<float>::pi);
                    break;
                    
                case 4: // Noise
                    detunedSample = baseSample; // Noise doesn't need detuning
                    break;
            }
            
            sample += detunedSample; // Mix all voices
        }
        sample /= 7.0f; // Normalize
    }
    else
    {
        sample = baseSample;
    }
    
    return sample;
}

float SynthVoice::getWaveformSample2()
{
    float sample = 0.0f;
    
    // Generate base waveform sample
    float baseSample = 0.0f;
    
    switch (currentWaveform2)
    {
        case 0: // Sine
            baseSample = std::sin(currentAngle2);
            break;
            
        case 1: // Square
            baseSample = (std::sin(currentAngle2) >= 0.0f) ? 1.0f : -1.0f;
            break;
            
        case 2: // Triangle
        {
            auto angle = std::fmod(currentAngle2, juce::MathConstants<double>::twoPi);
            if (angle < juce::MathConstants<double>::pi)
                baseSample = -1.0f + (2.0f * angle / juce::MathConstants<float>::pi);
            else
                baseSample = 1.0f - (2.0f * (angle - juce::MathConstants<float>::pi) / juce::MathConstants<float>::pi);
            break;
        }
            
        case 3: // Saw
            baseSample = (1.0f / juce::MathConstants<float>::pi) * 
                         (std::fmod(currentAngle2, juce::MathConstants<double>::twoPi) - juce::MathConstants<float>::pi);
            break;
            
        case 4: // Noise
            baseSample = random.nextFloat() * 2.0f - 1.0f;
            break;
    }
    
    // Apply supersaw detuning if enabled
    if (supersawEnabled2)
    {
        sample = 0.0f;
        for (int i = 0; i < 7; ++i)
        {
            // Use the accumulated detuned angle
            double sawAngle = supersawAngles2[i];
            
            // Apply the base waveform to the detuned angle
            float detunedSample = 0.0f;
            switch (currentWaveform2)
            {
                case 0: // Sine
                    detunedSample = std::sin(sawAngle);
                    break;
                    
                case 1: // Square
                    detunedSample = (std::sin(sawAngle) >= 0.0f) ? 1.0f : -1.0f;
                    break;
                    
                case 2: // Triangle
                {
                    auto angle = std::fmod(sawAngle, juce::MathConstants<double>::twoPi);
                    if (angle < juce::MathConstants<double>::pi)
                        detunedSample = -1.0f + (2.0f * angle / juce::MathConstants<float>::pi);
                    else
                        detunedSample = 1.0f - (2.0f * (angle - juce::MathConstants<float>::pi) / juce::MathConstants<float>::pi);
                    break;
                }
                    
                case 3: // Saw
                    detunedSample = (1.0f / juce::MathConstants<float>::pi) * 
                                   (std::fmod(sawAngle, juce::MathConstants<double>::twoPi) - juce::MathConstants<float>::pi);
                    break;
                    
                case 4: // Noise
                    detunedSample = baseSample; // Noise doesn't need detuning
                    break;
            }
            
            sample += detunedSample; // Mix all voices
        }
        sample /= 7.0f; // Normalize
    }
    else
    {
        sample = baseSample;
    }
    
    return sample;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                               juce::SynthesiserSound*,
                               int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;
    currentAngle2 = 0.0;
    level = velocity * 1.0;

    // Calculate frequency from MIDI note number
    double cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    // Apply octave shift for OSC1
    double shiftedFrequency = cyclesPerSecond * std::pow(2.0, octave);
    double cyclesPerSample = shiftedFrequency / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    
    // Apply octave shift for OSC2
    double shiftedFrequency2 = cyclesPerSecond * std::pow(2.0, octave2);
    double cyclesPerSample2 = shiftedFrequency2 / getSampleRate();

    angleDelta2 = cyclesPerSample2 * 2.0 * juce::MathConstants<double>::pi;
    
    // Initialize superwave angles and deltas for OSC1 if superwave is enabled
    if (supersawEnabled)
    {
        // Calculate base frequency from angleDelta
        double baseFrequency = (angleDelta * getSampleRate()) / (2.0 * juce::MathConstants<double>::pi);
        
        for (int i = 0; i < 7; ++i)
        {
            supersawAngles[i] = 0.0;
            
            // Calculate detuning based on position from center
            int position = i - 3; // -3, -2, -1, 0, 1, 2, 3
            double detuneHz = 0.0;
            
            if (position != 0)
            {
                int absPosition = std::abs(position);
                if (absPosition == 1)
                    detuneHz = width * strength / 3.0;
                else if (absPosition == 2)
                    detuneHz = width * strength / 2.0;
                else if (absPosition == 3)
                    detuneHz = width * strength / 1.0;
                
                // Round to nearest integer Hz
                detuneHz = std::round(detuneHz);
                
                // Apply sign
                if (position < 0)
                    detuneHz = -detuneHz;
            }
            
            // Calculate detuned frequency
            double detunedFreq = baseFrequency + detuneHz;
            supersawAngleDeltas[i] = (detunedFreq / getSampleRate()) * 2.0 * juce::MathConstants<double>::pi;
        }
    }
    
    // Initialize superwave angles and deltas for OSC2 if superwave is enabled
    if (supersawEnabled2)
    {
        // Calculate base frequency from angleDelta2
        double baseFrequency2 = (angleDelta2 * getSampleRate()) / (2.0 * juce::MathConstants<double>::pi);
        
        for (int i = 0; i < 7; ++i)
        {
            supersawAngles2[i] = 0.0;
            
            // Calculate detuning based on position from center
            int position = i - 3; // -3, -2, -1, 0, 1, 2, 3
            double detuneHz = 0.0;
            
            if (position != 0)
            {
                int absPosition = std::abs(position);
                if (absPosition == 1)
                    detuneHz = width2 * strength2 / 3.0;
                else if (absPosition == 2)
                    detuneHz = width2 * strength2 / 2.0;
                else if (absPosition == 3)
                    detuneHz = width2 * strength2 / 1.0;
                
                // Round to nearest integer Hz
                detuneHz = std::round(detuneHz);
                
                // Apply sign
                if (position < 0)
                    detuneHz = -detuneHz;
            }
            
            // Calculate detuned frequency
            double detunedFreq = baseFrequency2 + detuneHz;
            supersawAngleDeltas2[i] = (detunedFreq / getSampleRate()) * 2.0 * juce::MathConstants<double>::pi;
        }
    }
    
    // Reset and start ADSR envelopes
    adsr.setSampleRate(getSampleRate());
    adsr.noteOn();
    
    adsr2.setSampleRate(getSampleRate());
    adsr2.noteOn();
    
    // If sustain is 0, calculate when to auto-release (after attack+decay)
    shouldAutoRelease = (sustainLevel <= 0.0f);
    if (shouldAutoRelease)
    {
        attackDecayTime = adsrParams.attack + adsrParams.decay;
        currentTime = 0.0;
    }
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
        adsr2.noteOff();
    }
    else
    {
        clearCurrentNote();
        angleDelta = 0.0;
        angleDelta2 = 0.0;
        adsr.reset();
        adsr2.reset();
    }
}

void SynthVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
}

void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                     int startSample, int numSamples)
{
    if (angleDelta != 0.0 || angleDelta2 != 0.0)
    {
        while (--numSamples >= 0)
        {
            auto envelopeValue1 = adsr.getNextSample();
            auto envelopeValue2 = adsr2.getNextSample();
            
            // Track time for auto-release when sustain is 0
            if (shouldAutoRelease)
            {
                currentTime += 1.0 / getSampleRate();
                if (currentTime >= attackDecayTime)
                {
                    adsr.noteOff();
                    adsr2.noteOff();
                    shouldAutoRelease = false;
                }
            }
            
            // Generate samples from both oscillators
            auto osc1Sample = (float)(getWaveformSample() * level * volume * envelopeValue1);
            auto osc2Sample = (float)(getWaveformSample2() * level * volume2 * envelopeValue2);
            
            // Mix both oscillators
            auto currentSample = osc1Sample + osc2Sample;
            
            // Aggiunto: Applica filtro passa alto solo se superwave è attivo per almeno un oscillatore
            if (supersawEnabled || supersawEnabled2)
            {
                // Applica filtro 2° ordine per slope 12 dB
                currentSample = highPassFilter1.processSingleSampleRaw(currentSample);
            }

            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample(i, startSample, currentSample);

            currentAngle += angleDelta;
            currentAngle2 += angleDelta2;
            ++startSample;
            
            // Update supersaw angles if supersaw is enabled for OSC1
            if (supersawEnabled)
            {
                for (int i = 0; i < 7; ++i)
                {
                    supersawAngles[i] += supersawAngleDeltas[i];
                }
            }
            
            // Update supersaw angles if supersaw is enabled for OSC2
            if (supersawEnabled2)
            {
                for (int i = 0; i < 7; ++i)
                {
                    supersawAngles2[i] += supersawAngleDeltas2[i];
                }
            }
            
            // Stop voice if both envelopes are finished
            if (!adsr.isActive() && !adsr2.isActive())
            {
                clearCurrentNote();
                angleDelta = 0.0;
                angleDelta2 = 0.0;
                break;
            }
        }
    }
}
