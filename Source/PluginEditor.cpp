#include "PluginProcessor.h"
#include "PluginEditor.h"

BasicleanSynthAudioProcessorEditor::BasicleanSynthAudioProcessorEditor(BasicleanSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), midiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard), midiListener(p)
{
    // Apply custom look and feel
    setLookAndFeel(&customLookAndFeel);
    
    // Configure oscilloscope
    addAndMakeVisible(oscilloscope);
    
    // Setup audio callback for oscilloscope (safely set via the processor's method)
    audioProcessor.setAudioBufferReadyCallback([this](const juce::AudioBuffer<float>& buffer)
    {
        oscilloscope.pushBuffer(buffer);
    });
    
    // Setup ADSR1 vertical sliders
    auto setupADSRSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& labelText)
    {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(slider);
        
        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        addAndMakeVisible(label);
    };
    
    setupADSRSlider(attackSlider, attackLabel, "A");
    setupADSRSlider(decaySlider, decayLabel, "D");
    setupADSRSlider(sustainSlider, sustainLabel, "S");
    setupADSRSlider(releaseSlider, releaseLabel, "R");
    
    // Setup ADSR2 vertical sliders
    setupADSRSlider(attackSlider2, attackLabel2, "A");
    setupADSRSlider(decaySlider2, decayLabel2, "D");
    setupADSRSlider(sustainSlider2, sustainLabel2, "S");
    setupADSRSlider(releaseSlider2, releaseLabel2, "R");
    
    // Attach ADSR1 parameters
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "release", releaseSlider);
    
    // Attach ADSR2 parameters
    attackAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "attack2", attackSlider2);
    decayAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "decay2", decaySlider2);
    sustainAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "sustain2", sustainSlider2);
    releaseAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "release2", releaseSlider2);
    
    // Helper lambda to configure knobs
    auto setupKnob = [this](juce::Slider& knob, juce::Label& label, const juce::String& labelText)
    {
        knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(knob);
        
        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        addAndMakeVisible(label);
    };
    
    // Configure waveform knob
    waveformKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    waveformKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    waveformKnob.setTextBoxIsEditable(false); // Make waveform value read-only
    waveformKnob.setRange(0.0, 4.0, 1.0);
    waveformKnob.setTextValueSuffix(" ");
    addAndMakeVisible(waveformKnob);
    
    // Custom text from value function
    waveformKnob.textFromValueFunction = [](double value) {
        switch ((int)value)
        {
            case 0: return juce::String("Sine");
            case 1: return juce::String("Square");
            case 2: return juce::String("Triangle");
            case 3: return juce::String("Saw");
            case 4: return juce::String("Noise");
            default: return juce::String("Sine");
        }
    };
    
    // Attach to parameter
    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "waveform", waveformKnob);
    
    // Update oscilloscope when waveform changes
    waveformKnob.onValueChange = [this]
    {

        // Waveform changes are reflected in actual audio now
        repaint();
    };
    
    waveformLabel.setText("WAVEFORM", juce::dontSendNotification);
    waveformLabel.setJustificationType(juce::Justification::centred);
    waveformLabel.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    addAndMakeVisible(waveformLabel);
    
    // Setup octave combo box
    octaveBox.addItem("-2", 1);
    octaveBox.addItem("-1", 2);
    octaveBox.addItem("0", 3);
    octaveBox.addItem("+1", 4);
    octaveBox.addItem("+2", 5);
    octaveBox.setSelectedId(3); // Default to "0"
    addAndMakeVisible(octaveBox);
    
    octaveLabel.setText("OCTAVE", juce::dontSendNotification);
    octaveLabel.setJustificationType(juce::Justification::centred);
    octaveLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    addAndMakeVisible(octaveLabel);
    
    // Attach octave parameter
    octaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "octave", octaveBox);
    
    // Setup volume knob
    setupKnob(volumeKnob, volumeLabel, "VOLUME");
    
    // Attach volume parameter
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "volume", volumeKnob);
    
    // Setup superwave toggle button
    supersawButton.setButtonText("SUPERWAVE");
    supersawButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(supersawButton);
    
    // Attach superwave parameter
    supersawAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "superwave", supersawButton);
    
    // Setup width knob (initially hidden)
    setupKnob(widthKnob, widthLabel, "WIDTH");
    widthKnob.setVisible(false);
    widthLabel.setVisible(false);
    
    // Attach width parameter
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "width", widthKnob);
    
    // Setup strength knob (initially hidden)
    setupKnob(strengthKnob, strengthLabel, "STRENGTH");
    strengthKnob.setVisible(false);
    strengthLabel.setVisible(false);
    
    // Attach strength parameter
    strengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "strength", strengthKnob);
    
    // Add listener for superwave button to show/hide controls
    supersawButton.onStateChange = [this]
    {
        bool isSuperwaveEnabled = supersawButton.getToggleState();
        widthKnob.setVisible(isSuperwaveEnabled);
        widthLabel.setVisible(isSuperwaveEnabled);
        strengthKnob.setVisible(isSuperwaveEnabled);
        strengthLabel.setVisible(isSuperwaveEnabled);
        resized(); // Trigger layout update
    };
    
    // Setup filter type combo box
    filterTypeComboBox.addItem("LPF", 1);
    filterTypeComboBox.addItem("HPF", 2);
    filterTypeComboBox.addItem("BPF", 3);
    filterTypeComboBox.addItem("NF", 4);
    filterTypeComboBox.addItem("CF", 5);
    filterTypeComboBox.setSelectedId(1); // Default to LPF
    addAndMakeVisible(filterTypeComboBox);
    
    // Attach filter type parameter
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "filterType", filterTypeComboBox);
    
    // Setup cutoff knob
    setupKnob(cutoffKnob, cutoffLabel, "CUT");
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "cutoff", cutoffKnob);
    
    // Setup resonance knob
    setupKnob(resonanceKnob, resonanceLabel, "RES");
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "resonance", resonanceKnob);
    
    // Setup Q factor knob
    setupKnob(qFactorKnob, qFactorLabel, "QF");
    qFactorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "qFactor", qFactorKnob);
    
    // ===== FILTER2 CONTROLS =====
    
    // Setup filter2 type combo box
    filterTypeComboBox2.addItem("LPF", 1);
    filterTypeComboBox2.addItem("HPF", 2);
    filterTypeComboBox2.addItem("BPF", 3);
    filterTypeComboBox2.addItem("NF", 4);
    filterTypeComboBox2.addItem("CF", 5);
    filterTypeComboBox2.setSelectedId(1); // Default to LPF
    addAndMakeVisible(filterTypeComboBox2);
    
    // Attach filter2 type parameter
    filterTypeAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "filterType2", filterTypeComboBox2);
    
    // Setup cutoff knob 2
    setupKnob(cutoffKnob2, cutoffLabel2, "CUT");
    cutoffAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "cutoff2", cutoffKnob2);
    
    // Setup resonance knob 2
    setupKnob(resonanceKnob2, resonanceLabel2, "RES");
    resonanceAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "resonance2", resonanceKnob2);
    
    // Setup Q factor knob 2
    setupKnob(qFactorKnob2, qFactorLabel2, "QF");
    qFactorAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "qFactor2", qFactorKnob2);
    
    // ===== OSC2 CONTROLS =====
    
    // Configure waveform knob 2
    waveformKnob2.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    waveformKnob2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    waveformKnob2.setTextBoxIsEditable(false); // Make waveform value read-only
    waveformKnob2.setRange(0.0, 4.0, 1.0);
    waveformKnob2.setTextValueSuffix(" ");
    addAndMakeVisible(waveformKnob2);
    
    // Custom text from value function for OSC2
    waveformKnob2.textFromValueFunction = [](double value) {
        switch ((int)value)
        {
            case 0: return juce::String("Sine");
            case 1: return juce::String("Square");
            case 2: return juce::String("Triangle");
            case 3: return juce::String("Saw");
            case 4: return juce::String("Noise");
            default: return juce::String("Sine");
        }
    };
    
    // Attach to parameter
    waveformAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "waveform2", waveformKnob2);
    
    waveformLabel2.setText("WAVEFORM", juce::dontSendNotification);
    waveformLabel2.setJustificationType(juce::Justification::centred);
    waveformLabel2.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    addAndMakeVisible(waveformLabel2);
    
    // Setup octave combo box 2
    octaveBox2.addItem("-2", 1);
    octaveBox2.addItem("-1", 2);
    octaveBox2.addItem("0", 3);
    octaveBox2.addItem("+1", 4);
    octaveBox2.addItem("+2", 5);
    octaveBox2.setSelectedId(3); // Default to "0"
    addAndMakeVisible(octaveBox2);
    
    octaveLabel2.setText("OCTAVE", juce::dontSendNotification);
    octaveLabel2.setJustificationType(juce::Justification::centred);
    octaveLabel2.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    addAndMakeVisible(octaveLabel2);
    
    // Attach octave parameter 2
    octaveAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "octave2", octaveBox2);
    
    // Setup volume knob 2
    setupKnob(volumeKnob2, volumeLabel2, "VOLUME");
    
    // Attach volume parameter 2
    volumeAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "volume2", volumeKnob2);
    
    // Setup superwave toggle button 2
    supersawButton2.setButtonText("SUPERWAVE");
    supersawButton2.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(supersawButton2);
    
    // Attach superwave parameter 2
    supersawAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "superwave2", supersawButton2);
    
    // Setup width knob 2 (initially hidden)
    setupKnob(widthKnob2, widthLabel2, "WIDTH");
    widthKnob2.setVisible(false);
    widthLabel2.setVisible(false);
    
    // Attach width parameter 2
    widthAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "width2", widthKnob2);
    
    // Setup strength knob 2 (initially hidden)
    setupKnob(strengthKnob2, strengthLabel2, "STRENGTH");
    strengthKnob2.setVisible(false);
    strengthLabel2.setVisible(false);
    
    // Attach strength parameter 2
    strengthAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "strength2", strengthKnob2);
    
    // Add listener for superwave button 2 to show/hide controls
    supersawButton2.onStateChange = [this]
    {
        bool isSuperwaveEnabled = supersawButton2.getToggleState();
        widthKnob2.setVisible(isSuperwaveEnabled);
        widthLabel2.setVisible(isSuperwaveEnabled);
        strengthKnob2.setVisible(isSuperwaveEnabled);
        strengthLabel2.setVisible(isSuperwaveEnabled);
        resized(); // Trigger layout update
    };
    
    // Setup MIDI keyboard
    midiKeyboard.setMidiChannel(1);
    midiKeyboard.setAvailableRange(0, 108); // C-2 to C8 (full MIDI range)
    midiKeyboard.setKeyWidth(16.0f); // Wider keys to fill the full width better
    keyboardState.addListener(&midiListener);
    addAndMakeVisible(midiKeyboard);
    
    // Update visibility of superwave controls based on current button states
    bool isSuperwaveEnabled = supersawButton.getToggleState();
    widthKnob.setVisible(isSuperwaveEnabled);
    widthLabel.setVisible(isSuperwaveEnabled);
    strengthKnob.setVisible(isSuperwaveEnabled);
    strengthLabel.setVisible(isSuperwaveEnabled);
    
    bool isSuperwave2Enabled = supersawButton2.getToggleState();
    widthKnob2.setVisible(isSuperwave2Enabled);
    widthLabel2.setVisible(isSuperwave2Enabled);
    strengthKnob2.setVisible(isSuperwave2Enabled);
    strengthLabel2.setVisible(isSuperwave2Enabled);
    
    setSize(900, 700);
    
    // Make the window non-resizable
    setResizable(false, false);
}

BasicleanSynthAudioProcessorEditor::~BasicleanSynthAudioProcessorEditor()
{
    // Clear the audio callback to prevent crashes when audio thread is still running
    audioProcessor.setAudioBufferReadyCallback(nullptr);
    
    setLookAndFeel(nullptr);
}

void BasicleanSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Modern gradient background
    g.fillAll(customLookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
    
    auto bounds = getLocalBounds();
    
    // Add subtle gradient overlay
    juce::ColourGradient gradient(juce::Colour(0xff1a1a2e), bounds.getX(), bounds.getY(),
                                  juce::Colour(0xff0f0f1e), bounds.getX(), bounds.getBottom(),
                                  false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);
    
    // Draw section borders and titles
    auto drawSectionBorder = [&](const juce::Rectangle<int>& sectionBounds, const juce::String& title) {
        if (!sectionBounds.isEmpty())
        {
            g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
            g.drawRoundedRectangle(sectionBounds.toFloat().reduced(2), 8.0f, 2.0f);
            
            // Section title - positioned to overlap the top border
            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(14.0f, juce::Font::bold));
            juce::Rectangle<int> titleBounds = sectionBounds.reduced(10, 0);
            titleBounds.setHeight(20);
            titleBounds.setY(sectionBounds.getY() - 2); // Slightly above the border
            g.drawFittedText(title, titleBounds, juce::Justification::centred, 1);
        }
    };
    
    drawSectionBorder(osc1SectionBounds, "OSC1");
    drawSectionBorder(osc2SectionBounds, "OSC2");
    drawSectionBorder(oscilloscopeSectionBounds, "OSCILLOSCOPE");
    drawSectionBorder(adsrSectionBounds, "ENVELOPE 1");
    drawSectionBorder(filterSectionBounds, "FILTER 1");
    drawSectionBorder(adsrSectionBounds2, "ENVELOPE 2");
    drawSectionBorder(filterSectionBounds2, "FILTER 2");
}

void BasicleanSynthAudioProcessorEditor::resized()
{
    // Don't layout if we don't have valid bounds yet
    if (getWidth() == 0 || getHeight() == 0)
        return;
        
    auto bounds = getLocalBounds();
    
    // Reserve space for MIDI keyboard at bottom
    auto midiKeyboardArea = bounds.removeFromBottom(25);
    midiKeyboard.setBounds(midiKeyboardArea);
    
    auto contentArea = bounds.reduced(20);
    
    // Top row: OSC1 (left) and OSC2 (right) - give less height since they don't need as much vertical space
    int topRowHeight = static_cast<int>(contentArea.getHeight() * 0.35f);
    auto topRow = contentArea.removeFromTop(topRowHeight); // 35% for oscillators
    int halfWidth = topRow.getWidth() / 2;
    
    // Reserve space for oscilloscope below
    auto oscilloscopeArea = contentArea.removeFromBottom(140); // Increased oscilloscope height
    
    // Bottom row: ADSR1, Filter1, ADSR2, Filter2 - give more height for envelope visualization
    auto bottomRow = contentArea; // Remaining ~65% for ADSR and filters
    
    // Top-left: OSC1 section
    auto topLeftSection = topRow.removeFromLeft(halfWidth);
    osc1SectionBounds = topLeftSection;
    auto osc1Content = topLeftSection.reduced(20).withTrimmedTop(25);
    
    // Top-right: OSC2 section
    auto topRightSection = topRow.removeFromLeft(halfWidth);
    osc2SectionBounds = topRightSection;
    auto osc2Content = topRightSection.reduced(20).withTrimmedTop(25);
    
    // ===== OSC1 LAYOUT =====
    // Divide OSC1 section into top (knobs) and bottom (superwave controls)
    int knobSize = 80;
    bool isSuperwaveEnabled = supersawButton.getToggleState();
    auto osc1KnobsArea = osc1Content.removeFromTop(knobSize + 25); // Space for knobs and labels
    auto osc1SupersawArea = osc1Content; // Remaining space for superwave controls
    
    // Position waveform, volume, and superwave knobs in OSC1 top area (octave moved below)
    int numKnobsOSC1 = isSuperwaveEnabled ? 4 : 2;
    int spacingOSC1 = (osc1KnobsArea.getWidth() - (knobSize * numKnobsOSC1)) / (numKnobsOSC1 + 1);
    
    osc1KnobsArea.removeFromLeft(spacingOSC1);
    auto waveformBounds = osc1KnobsArea.removeFromLeft(knobSize);
    waveformKnob.setBounds(waveformBounds);
    waveformLabel.setBounds(waveformBounds.getX(), waveformBounds.getY() - 20,
                           waveformBounds.getWidth(), 18);
    
    osc1KnobsArea.removeFromLeft(spacingOSC1);
    auto volumeBounds = osc1KnobsArea.removeFromLeft(knobSize);
    volumeKnob.setBounds(volumeBounds);
    volumeLabel.setBounds(volumeBounds.getX(), volumeBounds.getY() - 20,
                         volumeBounds.getWidth(), 18);
    
    if (isSuperwaveEnabled)
    {
        osc1KnobsArea.removeFromLeft(spacingOSC1);
        auto widthBounds = osc1KnobsArea.removeFromLeft(knobSize);
        widthKnob.setBounds(widthBounds);
        widthLabel.setBounds(widthBounds.getX(), widthBounds.getY() - 20,
                            widthBounds.getWidth(), 18);
        
        osc1KnobsArea.removeFromLeft(spacingOSC1);
        auto strengthBounds = osc1KnobsArea.removeFromLeft(knobSize);
        strengthKnob.setBounds(strengthBounds);
        strengthLabel.setBounds(strengthBounds.getX(), strengthBounds.getY() - 20,
                               strengthBounds.getWidth(), 18);
    }
    
    // Position octave dropdown and superwave button side by side below the waveform
    int controlWidth = 100;  // Width for each control (dropdown and button)
    int controlHeight = 30; // Height for both
    int spacing = 20;       // Space between controls
    int totalWidth = (controlWidth * 2) + spacing;

    // Center them horizontally in the bottom area
    int startX = osc1SupersawArea.getX() + (osc1SupersawArea.getWidth() - totalWidth) / 2;
    int startY = osc1SupersawArea.getY() + (osc1SupersawArea.getHeight() - controlHeight) / 2;

    // Octave dropdown on the left
    octaveBox.setBounds(startX, startY, controlWidth, controlHeight);
    octaveLabel.setBounds(startX, startY, controlWidth, 15);

    // Superwave button on the right
    supersawButton.setBounds(startX + controlWidth + spacing, startY, controlWidth, controlHeight);
    supersawLabel.setText("SUPERWAVE", juce::dontSendNotification);
    supersawLabel.setJustificationType(juce::Justification::centred);
    supersawLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    supersawLabel.setBounds(startX + controlWidth + spacing, startY - 18, controlWidth, 16);
    
    // ===== OSC2 LAYOUT =====
    // Divide OSC2 section into top (knobs) and bottom (superwave controls)
    bool isSuperwave2Enabled = supersawButton2.getToggleState();
    auto osc2KnobsArea = osc2Content.removeFromTop(knobSize + 25); // Space for knobs and labels
    auto osc2SupersawArea = osc2Content; // Remaining space for superwave controls
    
    // Position waveform, volume, and superwave knobs in OSC2 top area (octave moved below)
    int numKnobsOSC2 = isSuperwave2Enabled ? 4 : 2;
    int spacingOSC2 = (osc2KnobsArea.getWidth() - (knobSize * numKnobsOSC2)) / (numKnobsOSC2 + 1);
    
    osc2KnobsArea.removeFromLeft(spacingOSC2);
    auto waveformBounds2 = osc2KnobsArea.removeFromLeft(knobSize);
    waveformKnob2.setBounds(waveformBounds2);
    waveformLabel2.setBounds(waveformBounds2.getX(), waveformBounds2.getY() - 20,
                            waveformBounds2.getWidth(), 18);
    
    osc2KnobsArea.removeFromLeft(spacingOSC2);
    auto volumeBounds2 = osc2KnobsArea.removeFromLeft(knobSize);
    volumeKnob2.setBounds(volumeBounds2);
    volumeLabel2.setBounds(volumeBounds2.getX(), volumeBounds2.getY() - 20,
                          volumeBounds2.getWidth(), 18);
    
    if (isSuperwave2Enabled)
    {
        osc2KnobsArea.removeFromLeft(spacingOSC2);
        auto widthBounds2 = osc2KnobsArea.removeFromLeft(knobSize);
        widthKnob2.setBounds(widthBounds2);
        widthLabel2.setBounds(widthBounds2.getX(), widthBounds2.getY() - 20,
                             widthBounds2.getWidth(), 18);
        
        osc2KnobsArea.removeFromLeft(spacingOSC2);
        auto strengthBounds2 = osc2KnobsArea.removeFromLeft(knobSize);
        strengthKnob2.setBounds(strengthBounds2);
        strengthLabel2.setBounds(strengthBounds2.getX(), strengthBounds2.getY() - 20,
                                strengthBounds2.getWidth(), 18);
    }
    
    // Position octave dropdown and superwave button side by side below the waveform
    int controlWidth2 = 100;  // Width for each control (dropdown and button)
    int controlHeight2 = 30; // Height for both
    int spacing2 = 20;       // Space between controls
    int totalWidth2 = (controlWidth2 * 2) + spacing2;

    // Center them horizontally in the bottom area
    int startX2 = osc2SupersawArea.getX() + (osc2SupersawArea.getWidth() - totalWidth2) / 2;
    int startY2 = osc2SupersawArea.getY() + (osc2SupersawArea.getHeight() - controlHeight2) / 2;

    // Octave dropdown on the left
    octaveBox2.setBounds(startX2, startY2, controlWidth2, controlHeight2);
    octaveLabel2.setBounds(startX2, startY2, controlWidth2, 15);

    // Superwave button on the right
    supersawButton2.setBounds(startX2 + controlWidth2 + spacing2, startY2, controlWidth2, controlHeight2);
    supersawLabel2.setText("SUPERWAVE", juce::dontSendNotification);
    supersawLabel2.setJustificationType(juce::Justification::centred);
    supersawLabel2.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    supersawLabel2.setBounds(startX2 + controlWidth2 + spacing2, startY2 - 18, controlWidth2, 16);
    

    
    // Bottom row: ADSR1 (left), Filter1 (center-left), ADSR2 (center-right), Filter2 (right)
    int quarterWidth = bottomRow.getWidth() / 4;
    
    // Bottom-left: ADSR1 section
    auto bottomLeftSection = bottomRow.removeFromLeft(quarterWidth);
    adsrSectionBounds = bottomLeftSection;
    auto adsrContent = bottomLeftSection.reduced(20).withTrimmedTop(25);
    
    // Layout ADSR1 vertical sliders horizontally
    int sliderWidth = adsrContent.getWidth() / 4;
    int sliderHeight = adsrContent.getHeight() - 15; // Leave space for labels above
    
    attackSlider.setBounds(adsrContent.getX(), adsrContent.getY() + 15, sliderWidth, sliderHeight);
    attackLabel.setBounds(adsrContent.getX(), adsrContent.getY(), sliderWidth, 15);
    
    decaySlider.setBounds(adsrContent.getX() + sliderWidth, adsrContent.getY() + 15, sliderWidth, sliderHeight);
    decayLabel.setBounds(adsrContent.getX() + sliderWidth, adsrContent.getY(), sliderWidth, 15);
    
    sustainSlider.setBounds(adsrContent.getX() + 2 * sliderWidth, adsrContent.getY() + 15, sliderWidth, sliderHeight);
    sustainLabel.setBounds(adsrContent.getX() + 2 * sliderWidth, adsrContent.getY(), sliderWidth, 15);
    
    releaseSlider.setBounds(adsrContent.getX() + 3 * sliderWidth, adsrContent.getY() + 15, sliderWidth, sliderHeight);
    releaseLabel.setBounds(adsrContent.getX() + 3 * sliderWidth, adsrContent.getY(), sliderWidth, 15);
    
    // Bottom-center-left: Filter1 section
    auto bottomCenterLeftSection = bottomRow.removeFromLeft(quarterWidth);
    filterSectionBounds = bottomCenterLeftSection;
    auto filterContent = bottomCenterLeftSection.reduced(20).withTrimmedTop(25);
    
    // Position filter type combo box
    int comboBoxHeight = 25;
    auto comboBoxArea = filterContent.removeFromTop(comboBoxHeight + 10);
    
    int comboBoxWidth = 120; // Reasonable width for combo box
    int comboBoxX = comboBoxArea.getX() + (comboBoxArea.getWidth() - comboBoxWidth) / 2;
    filterTypeComboBox.setBounds(comboBoxX, comboBoxArea.getY(), comboBoxWidth, comboBoxHeight);
    
    filterContent.removeFromTop(15); // Spacing between combo box and knobs
    
    // Filter controls below buttons (larger knobs for better usability)
    int smallKnobSize = 70; // Increased from 60 to 70
    int numControls = 3;
    int controlSpacing = (filterContent.getWidth() - (smallKnobSize * numControls)) / (numControls + 1);
    
    filterContent.removeFromLeft(controlSpacing);
    auto cutoffBounds = filterContent.removeFromLeft(smallKnobSize);
    cutoffKnob.setBounds(cutoffBounds);
    cutoffLabel.setBounds(cutoffBounds.getX(), cutoffBounds.getY() + 20,
                         cutoffBounds.getWidth(), 16);
    
    filterContent.removeFromLeft(controlSpacing);
    auto resonanceBounds = filterContent.removeFromLeft(smallKnobSize);
    resonanceKnob.setBounds(resonanceBounds);
    resonanceLabel.setBounds(resonanceBounds.getX(), resonanceBounds.getY() +20,
                            resonanceBounds.getWidth(), 16);
    
    filterContent.removeFromLeft(controlSpacing);
    auto qFactorBounds = filterContent.removeFromLeft(smallKnobSize);
    qFactorKnob.setBounds(qFactorBounds);
    qFactorLabel.setBounds(qFactorBounds.getX(), qFactorBounds.getY() + 20,
                          qFactorBounds.getWidth(), 16);
    
    // Bottom-center-right: ADSR2 section
    auto bottomCenterRightSection = bottomRow.removeFromLeft(quarterWidth);
    adsrSectionBounds2 = bottomCenterRightSection;
    auto adsrContent2 = bottomCenterRightSection.reduced(20).withTrimmedTop(25);
    
    // Layout ADSR2 vertical sliders horizontally
    attackSlider2.setBounds(adsrContent2.getX(), adsrContent2.getY() + 15, sliderWidth, sliderHeight);
    attackLabel2.setBounds(adsrContent2.getX(), adsrContent2.getY(), sliderWidth, 15);
    
    decaySlider2.setBounds(adsrContent2.getX() + sliderWidth, adsrContent2.getY() + 15, sliderWidth, sliderHeight);
    decayLabel2.setBounds(adsrContent2.getX() + sliderWidth, adsrContent2.getY(), sliderWidth, 15);
    
    sustainSlider2.setBounds(adsrContent2.getX() + 2 * sliderWidth, adsrContent2.getY() + 15, sliderWidth, sliderHeight);
    sustainLabel2.setBounds(adsrContent2.getX() + 2 * sliderWidth, adsrContent2.getY(), sliderWidth, 15);
    
    releaseSlider2.setBounds(adsrContent2.getX() + 3 * sliderWidth, adsrContent2.getY() + 15, sliderWidth, sliderHeight);
    releaseLabel2.setBounds(adsrContent2.getX() + 3 * sliderWidth, adsrContent2.getY(), sliderWidth, 15);
    
    // Bottom-right: Filter2 section
    auto bottomRightSection = bottomRow;
    filterSectionBounds2 = bottomRightSection;
    auto filterContent2 = bottomRightSection.reduced(20).withTrimmedTop(25);
    
    // Position filter2 type combo box
    auto comboBoxArea2 = filterContent2.removeFromTop(comboBoxHeight + 10);
    comboBoxArea2.removeFromTop(5); // Small top margin
    
    int comboBoxX2 = comboBoxArea2.getX() + (comboBoxArea2.getWidth() - comboBoxWidth) / 2;
    filterTypeComboBox2.setBounds(comboBoxX2, comboBoxArea2.getY(), comboBoxWidth, comboBoxHeight);
    
    filterContent2.removeFromTop(15); // Spacing between combo box and knobs
    
    // Filter2 controls below buttons (larger knobs for better usability)
    filterContent2.removeFromLeft(controlSpacing);
    auto cutoffBounds2 = filterContent2.removeFromLeft(smallKnobSize);
    cutoffKnob2.setBounds(cutoffBounds2);
    cutoffLabel2.setBounds(cutoffBounds2.getX(), cutoffBounds2.getY() + 20,
                          cutoffBounds2.getWidth(), 16);
    
    filterContent2.removeFromLeft(controlSpacing);
    auto resonanceBounds2 = filterContent2.removeFromLeft(smallKnobSize);
    resonanceKnob2.setBounds(resonanceBounds2);
    resonanceLabel2.setBounds(resonanceBounds2.getX(), resonanceBounds2.getY() + 20,
                             resonanceBounds2.getWidth(), 16);
    
    filterContent2.removeFromLeft(controlSpacing);
    auto qFactorBounds2 = filterContent2.removeFromLeft(smallKnobSize);
    qFactorKnob2.setBounds(qFactorBounds2);
    qFactorLabel2.setBounds(qFactorBounds2.getX(), qFactorBounds2.getY() + 20,
                           qFactorBounds2.getWidth(), 16);
    
    // Oscilloscope below bottom row
    oscilloscopeSectionBounds = oscilloscopeArea;
    auto oscilloscopeContent = oscilloscopeArea.reduced(20).withTrimmedTop(25);
    oscilloscope.setBounds(oscilloscopeContent);
}

void BasicleanSynthAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    // Filter selection is now handled by ComboBox attachments
}

void BasicleanSynthAudioProcessorEditor::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    // This method is not used anymore - MIDI comes from the keyboard component
}

void BasicleanSynthAudioProcessorEditor::updateFilterButtons(int selectedFilterType)
{
    // JUCE gestisce automaticamente la selezione esclusiva con radioGroupId
    // Ma dobbiamo aggiornare il parametro quando cambia
    auto filterTypeParam = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.getAPVTS().getParameter("filterType"));
    if (filterTypeParam)
        filterTypeParam->setValueNotifyingHost(selectedFilterType / 4.0f); // Normalize to 0-1 range for 5 options
}

void BasicleanSynthAudioProcessorEditor::updateFilterButtons2(int selectedFilterType)
{
    // JUCE gestisce automaticamente la selezione esclusiva con radioGroupId
    // Ma dobbiamo aggiornare il parametro quando cambia
    auto filterTypeParam = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.getAPVTS().getParameter("filterType2"));
    if (filterTypeParam)
        filterTypeParam->setValueNotifyingHost(selectedFilterType / 4.0f); // Normalize to 0-1 range for 5 options
}

void BasicleanSynthAudioProcessorEditor::buttonStateChanged(juce::Button* button)
{
    // Filter selection is now handled by ComboBox attachments
}
