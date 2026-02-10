#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthVoice.h"

juce::AudioProcessorValueTreeState::ParameterLayout BasicleanSynthAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "waveform",
        "Waveform",
        juce::StringArray{ "Sine", "Square", "Triangle", "Saw", "Noise" },
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "attack",
        "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "decay",
        "Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "sustain",
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "release",
        "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "volume",
        "Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "superwave",
        "Superwave",
        false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "width",
        "Width",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.1f),
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "strength",
        "Strength",
        juce::NormalisableRange<float>(3.0f, 4.0f, 0.1f),
        3.5f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "filterType",
        "Filter Type",
        juce::StringArray{ "Low Pass", "High Pass", "Band Pass", "Notch", "Comb" },
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "cutoff",
        "Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
        1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "resonance",
        "Resonance",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f),
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "qFactor",
        "Q Factor",
        juce::NormalisableRange<float>(0.01f, 10.0f, 0.01f),
        1.0f));

    // OSC2 parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "waveform2",
        "Waveform 2",
        juce::StringArray{ "Sine", "Square", "Triangle", "Saw", "Noise" },
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "volume2",
        "Volume 2",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "superwave2",
        "Superwave 2",
        false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "width2",
        "Width 2",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.1f),
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "strength2",
        "Strength 2",
        juce::NormalisableRange<float>(3.0f, 4.0f, 0.1f),
        3.5f));

    // Envelope 2 parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "attack2",
        "Attack 2",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "decay2",
        "Decay 2",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "sustain2",
        "Sustain 2",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "release2",
        "Release 2",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.4f));

    // Filter 2 parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "filterType2",
        "Filter Type 2",
        juce::StringArray{ "Low Pass", "High Pass", "Band Pass", "Notch", "Comb" },
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "cutoff2",
        "Cutoff 2",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
        1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "resonance2",
        "Resonance 2",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f),
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "qFactor2",
        "Q Factor 2",
        juce::NormalisableRange<float>(0.01f, 10.0f, 0.01f),
        1.0f));

    // Octave parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "octave",
        "Octave",
        juce::StringArray{ "-2", "-1", "0", "+1", "+2" },
        2)); // Default to "0"

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "octave2",
        "Octave 2",
        juce::StringArray{ "-2", "-1", "0", "+1", "+2" },
        2)); // Default to "0"

    return layout;
}

BasicleanSynthAudioProcessor::BasicleanSynthAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Add sine wave voices to the synthesiser
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthVoice());

    synth.addSound(new SynthSound());
}

BasicleanSynthAudioProcessor::~BasicleanSynthAudioProcessor()
{
}

const juce::String BasicleanSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicleanSynthAudioProcessor::acceptsMidi() const
{
    return true;
}

bool BasicleanSynthAudioProcessor::producesMidi() const
{
    return false;
}

bool BasicleanSynthAudioProcessor::isMidiEffect() const
{
    return false;
}

double BasicleanSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicleanSynthAudioProcessor::getNumPrograms()
{
    return 1;
}

int BasicleanSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicleanSynthAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String BasicleanSynthAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void BasicleanSynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void BasicleanSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    filter.prepare(spec);
    filter.reset();
    
    highPassFilter.prepare(spec);
    highPassFilter.reset();
    
    // Initialize high-pass filter at 20Hz
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f, 0.707f);
}

void BasicleanSynthAudioProcessor::releaseResources()
{
}

bool BasicleanSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void BasicleanSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();
    
    // Add any pending MIDI messages from the UI
    {
        const juce::ScopedLock lock(midiMessageLock);
        midiMessages.addEvents(pendingMidiMessages, 0, buffer.getNumSamples(), 0);
        pendingMidiMessages.clear();
    }
    
    // Update parameters for all voices
    auto waveformParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("waveform"));
    int waveformType = waveformParam->getIndex();
    
    auto attackParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("attack"));
    auto decayParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("decay"));
    auto sustainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("sustain"));
    auto releaseParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("release"));
    auto volumeParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("volume"));
    auto superwaveParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("superwave"));
    auto widthParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("width"));
    auto strengthParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("strength"));
    
    // OSC2 parameters
    auto waveform2Param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("waveform2"));
    int waveformType2 = waveform2Param->getIndex();
    
    auto attack2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("attack2"));
    auto decay2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("decay2"));
    auto sustain2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("sustain2"));
    auto release2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("release2"));
    auto volume2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("volume2"));
    auto superwave2Param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("superwave2"));
    auto width2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("width2"));
    auto strength2Param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("strength2"));
    
    // Octave parameters
    auto octaveParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("octave"));
    int octaveValue = octaveParam->getIndex() - 2; // Convert 0-4 to -2 to +2
    
    auto octave2Param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("octave2"));
    int octave2Value = octave2Param->getIndex() - 2; // Convert 0-4 to -2 to +2
    
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            // OSC1 parameters
            voice->setWaveform(waveformType);
            voice->setADSR(attackParam->get(), decayParam->get(), 
                          sustainParam->get(), releaseParam->get());
            voice->setVolume(volumeParam->get());
            voice->setSupersaw(superwaveParam->get());
            voice->setWidth(widthParam->get());
            voice->setStrength(strengthParam->get());
            voice->setOctave(octaveValue);
            
            // OSC2 parameters
            voice->setWaveform2(waveformType2);
            voice->setADSR2(attack2Param->get(), decay2Param->get(), 
                           sustain2Param->get(), release2Param->get());
            voice->setVolume2(volume2Param->get());
            voice->setSupersaw2(superwave2Param->get());
            voice->setWidth2(width2Param->get());
            voice->setStrength2(strength2Param->get());
            voice->setOctave2(octave2Value);
        }
    }
    
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // Apply fixed high-pass filter at 20Hz
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        highPassFilter.process(context);
    }
    
    // Apply filter
    auto filterTypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("filterType"));
    auto cutoffParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("cutoff"));
    auto resonanceParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("resonance"));
    auto qFactorParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("qFactor"));
    
    int filterType = filterTypeParam->getIndex();
    float cutoff = cutoffParam->get();
    float resonance = resonanceParam->get();
    float qFactor = qFactorParam->get();
    
    // Update filter coefficients
    auto sampleRate = getSampleRate();
    
    switch (filterType)
    {
        case 0: // Low Pass
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff, qFactor);
            break;
        case 1: // High Pass
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoff, qFactor);
            break;
        case 2: // Band Pass
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, cutoff, qFactor);
            break;
        case 3: // Notch
        {
            // Implementazione manuale del filtro notch usando Q factor
            double omega = 2.0 * juce::MathConstants<double>::pi * cutoff / sampleRate;
            double alpha = std::sin(omega) / (2.0 * qFactor);
            double cosOmega = std::cos(omega);
            
            double b0 = 1.0;
            double b1 = -2.0 * cosOmega;
            double b2 = 1.0;
            double a0 = 1.0 + alpha;
            double a1 = -2.0 * cosOmega;
            double a2 = 1.0 - alpha;
            
            // Normalizza
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            
            *filter.state = juce::dsp::IIR::Coefficients<float>(b0, b1, b2, 1.0, a1, a2);
            break;
        }
        case 4: // Comb
        {
            // Filtro comb semplificato con delay e feedback
            // Usa cutoff per determinare il delay time, resonance per il feedback, qFactor per il dampening
            double feedback = resonance / 10.0; // Feedback da resonance (0-1)
            double dampening = 1.0 - (qFactor / 10.0); // Dampening da Q factor
            
            // Coefficienti per un filtro comb semplice
            double b0 = 1.0;
            double b1 = 0.0;
            double b2 = 0.0;
            double a0 = 1.0;
            double a1 = -dampening * feedback;
            double a2 = dampening * dampening;
            
            *filter.state = juce::dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
            break;
        }
    }
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
    
    // Send audio to oscilloscope if callback is set
    {
        const juce::ScopedLock lock(callbackLock);
        if (onAudioBufferReady)
            onAudioBufferReady(buffer);
    }
}

bool BasicleanSynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BasicleanSynthAudioProcessor::createEditor()
{
    return new BasicleanSynthAudioProcessorEditor(*this);
}

void BasicleanSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BasicleanSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicleanSynthAudioProcessor();
}
