#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ADSRComponent : public juce::Component
{
public:
    ADSRComponent(juce::AudioProcessorValueTreeState& valueTreeState, 
                  const juce::String& attackParam = "attack",
                  const juce::String& decayParam = "decay", 
                  const juce::String& sustainParam = "sustain",
                  const juce::String& releaseParam = "release")
        : apvts(valueTreeState)
    {
        attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, attackParam, attackSlider);
        decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, decayParam, decaySlider);
        sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, sustainParam, sustainSlider);
        releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, releaseParam, releaseSlider);
        
        attackSlider.onValueChange = [this] { repaint(); };
        decaySlider.onValueChange = [this] { repaint(); };
        sustainSlider.onValueChange = [this] { repaint(); };
        releaseSlider.onValueChange = [this] { repaint(); };
        
        // Add sliders to component (they will be positioned in resized())
        addAndMakeVisible(attackSlider);
        addAndMakeVisible(decaySlider);
        addAndMakeVisible(sustainSlider);
        addAndMakeVisible(releaseSlider);
        
        setOpaque(false);
    }
    
    void mouseDown(const juce::MouseEvent& event) override
    {
        auto pos = event.getPosition().toFloat();
        auto bounds = getLocalBounds().toFloat().reduced(20, 15);
        
        float attack = static_cast<float>(attackSlider.getValue());
        float decay = static_cast<float>(decaySlider.getValue());
        float sustain = static_cast<float>(sustainSlider.getValue());
        float release = static_cast<float>(releaseSlider.getValue());
        float totalTime = attack + decay + 1.0f + release;
        
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        
        attackWidth = (attack / totalTime) * width;
        decayWidth = (decay / totalTime) * width;
        sustainWidth = (1.0f / totalTime) * width;
        releaseWidth = (release / totalTime) * width;
        
        // Calculate interactive point positions
        float peakX = bounds.getX() + attackWidth;
        float peakY = bounds.getY();
        float sustainX = bounds.getX() + attackWidth + decayWidth;
        float sustainY = bounds.getY() + (1.0f - sustain) * height;
        
        // Check if clicking near interactive dots (with larger hit radius)
        float hitRadius = 15.0f;
        
        if (pos.getDistanceFrom(juce::Point<float>(peakX, peakY)) < hitRadius)
        {
            dragMode = DragMode::Attack;
        }
        else if (pos.getDistanceFrom(juce::Point<float>(sustainX, sustainY)) < hitRadius)
        {
            dragMode = DragMode::Sustain;
        }
        else
        {
            // Fallback to region-based detection
            float relativeX = (pos.x - bounds.getX()) / bounds.getWidth();
            float attackEnd = attack / totalTime;
            float decayEnd = attackEnd + (decay / totalTime);
            float sustainEnd = decayEnd + (1.0f / totalTime);
            
            if (relativeX < attackEnd)
                dragMode = DragMode::Attack;
            else if (relativeX < decayEnd)
                dragMode = DragMode::Decay;
            else if (relativeX < sustainEnd)
                dragMode = DragMode::Sustain;
            else
                dragMode = DragMode::Release;
        }
        
        mouseDrag(event);
    }
    
    void mouseDrag(const juce::MouseEvent& event) override
    {
        auto pos = event.getPosition().toFloat();
        auto bounds = getLocalBounds().toFloat().reduced(20, 15);
        
        if (dragMode == DragMode::Sustain)
        {
            // Sustain is controlled by vertical position
            float relativeY = juce::jlimit(0.0f, 1.0f, 1.0f - ((pos.y - bounds.getY()) / bounds.getHeight()));
            sustainSlider.setValue(relativeY, juce::sendNotificationSync);
        }
        else
        {
            // Time-based parameters controlled by horizontal position
            float relativeX = juce::jlimit(0.0f, 1.0f, (pos.x - bounds.getX()) / bounds.getWidth());
            
            // Map to time range (0.001 to 5 seconds with skew)
            float value = std::pow(relativeX, 3.0f) * 5.0f;
            value = juce::jmax(0.001f, value);
            
            switch (dragMode)
            {
                case DragMode::Attack:
                    attackSlider.setValue(value, juce::sendNotificationSync);
                    break;
                case DragMode::Decay:
                    decaySlider.setValue(value, juce::sendNotificationSync);
                    break;
                case DragMode::Release:
                    releaseSlider.setValue(value, juce::sendNotificationSync);
                    break;
                default:
                    break;
            }
        }
        
        repaint();
    }
    
    void mouseUp(const juce::MouseEvent&) override
    {
        dragMode = DragMode::None;
    }

    void resized() override
    {
        // Don't layout if we don't have valid bounds yet
        if (getWidth() == 0 || getHeight() == 0)
            return;
            
        auto bounds = getLocalBounds().reduced(10);
        
        // Position sliders in a row at the bottom
        int sliderWidth = bounds.getWidth() / 4;
        int sliderHeight = 20;
        int y = bounds.getBottom() - sliderHeight - 5;
        
        attackSlider.setBounds(bounds.getX(), y, sliderWidth, sliderHeight);
        decaySlider.setBounds(bounds.getX() + sliderWidth, y, sliderWidth, sliderHeight);
        sustainSlider.setBounds(bounds.getX() + 2 * sliderWidth, y, sliderWidth, sliderHeight);
        releaseSlider.setBounds(bounds.getX() + 3 * sliderWidth, y, sliderWidth, sliderHeight);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Draw background
        g.setColour(juce::Colour(0xff0a0a15));
        g.fillRoundedRectangle(bounds, 10.0f);
        
        // Draw grid
        g.setColour(juce::Colour(0xff1a1a2e).brighter(0.3f));
        for (int i = 1; i < 5; ++i)
        {
            float y = bounds.getHeight() * i / 5.0f;
            g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
        }
        
        // Draw ADSR envelope
        drawEnvelope(g, bounds.reduced(20, 15));
        
        // Draw border
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.reduced(1), 10.0f, 3.0f);
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, 10.0f, 1.5f);
        
        // Draw labels with values
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(juce::FontOptions(11.0f));
        
        auto labelBounds = bounds.reduced(25, 10);
        
        auto attackLabelArea = labelBounds.removeFromLeft(attackWidth);
        g.drawText(juce::String("A: ") + juce::String(attackSlider.getValue(), 2) + "s", 
                   attackLabelArea.removeFromBottom(15), juce::Justification::centred);
        
        auto decayLabelArea = labelBounds.removeFromLeft(decayWidth);
        g.drawText(juce::String("D: ") + juce::String(decaySlider.getValue(), 2) + "s", 
                   decayLabelArea.removeFromBottom(15), juce::Justification::centred);
        
        auto sustainLabelArea = labelBounds.removeFromLeft(sustainWidth);
        g.drawText(juce::String("S: ") + juce::String(sustainSlider.getValue() * 100.0, 0) + "%", 
                   sustainLabelArea.removeFromBottom(15), juce::Justification::centred);
        
        auto releaseLabelArea = labelBounds.removeFromLeft(releaseWidth);
        g.drawText(juce::String("R: ") + juce::String(releaseSlider.getValue(), 2) + "s", 
                   releaseLabelArea.removeFromBottom(15), juce::Justification::centred);
        
        // Draw instruction text
        g.setFont(juce::FontOptions(9.0f));
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawText("Click and drag to adjust", bounds.removeFromTop(12), juce::Justification::centred);
    }

private:
    enum class DragMode
    {
        None,
        Attack,
        Decay,
        Sustain,
        Release
    };
    
    DragMode dragMode = DragMode::None;
    
    void drawEnvelope(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        float attack = static_cast<float>(attackSlider.getValue());
        float decay = static_cast<float>(decaySlider.getValue());
        float sustain = static_cast<float>(sustainSlider.getValue());
        float release = static_cast<float>(releaseSlider.getValue());
        
        // Normalize times for display (max 5 seconds each)
        float totalTime = attack + decay + 1.0f + release; // 1 second sustain display
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        
        attackWidth = (attack / totalTime) * width;
        decayWidth = (decay / totalTime) * width;
        sustainWidth = (1.0f / totalTime) * width;
        releaseWidth = (release / totalTime) * width;
        
        juce::Path envelopePath;
        
        // Start point (0, bottom)
        float x = bounds.getX();
        float y = bounds.getBottom();
        envelopePath.startNewSubPath(x, y);
        
        // Attack - rise to peak
        x += attackWidth;
        y = bounds.getY();
        envelopePath.lineTo(x, y);
        
        // Decay - fall to sustain level
        x += decayWidth;
        y = bounds.getY() + (1.0f - sustain) * height;
        envelopePath.lineTo(x, y);
        
        // Sustain - hold at sustain level
        x += sustainWidth;
        envelopePath.lineTo(x, y);
        
        // Release - fall to zero
        x += releaseWidth;
        y = bounds.getBottom();
        envelopePath.lineTo(x, y);
        
        // Draw glow effect
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.15f));
        g.strokePath(envelopePath, juce::PathStrokeType(8.0f));
        
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.4f));
        g.strokePath(envelopePath, juce::PathStrokeType(4.0f));
        
        // Main line
        g.setColour(juce::Colour(0xff00d4ff));
        g.strokePath(envelopePath, juce::PathStrokeType(2.5f));
        
        // Fill under the curve with gradient
        juce::Path fillPath = envelopePath;
        fillPath.lineTo(x, bounds.getBottom());
        fillPath.lineTo(bounds.getX(), bounds.getBottom());
        fillPath.closeSubPath();
        
        juce::ColourGradient gradient(
            juce::Colour(0xff00d4ff).withAlpha(0.3f), bounds.getCentreX(), bounds.getY(),
            juce::Colour(0xff00d4ff).withAlpha(0.05f), bounds.getCentreX(), bounds.getBottom(),
            false);
        g.setGradientFill(gradient);
        g.fillPath(fillPath);
        
        // Draw interactive points at envelope vertices with glow for easier interaction
        float peakX = bounds.getX() + attackWidth;
        float peakY = bounds.getY();
        float sustainX = bounds.getX() + attackWidth + decayWidth;
        float sustainY = bounds.getY() + (1.0f - sustain) * height;
        
        // Attack peak point - larger with glow
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.2f));
        g.fillEllipse(peakX - 8, peakY - 8, 16, 16);
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.5f));
        g.fillEllipse(peakX - 6, peakY - 6, 12, 12);
        g.setColour(juce::Colour(0xff00d4ff));
        g.fillEllipse(peakX - 5, peakY - 5, 10, 10);
        
        // Sustain level point - larger with glow
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.2f));
        g.fillEllipse(sustainX - 8, sustainY - 8, 16, 16);
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.5f));
        g.fillEllipse(sustainX - 6, sustainY - 6, 12, 12);
        g.setColour(juce::Colour(0xff00d4ff));
        g.fillEllipse(sustainX - 5, sustainY - 5, 10, 10);
        
        // Release end point (slightly faded)
        float releaseX = bounds.getX() + attackWidth + decayWidth + sustainWidth + releaseWidth;
        float releaseY = bounds.getBottom();
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.15f));
        g.fillEllipse(releaseX - 6, releaseY - 6, 12, 12);
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
        g.fillEllipse(releaseX - 4, releaseY - 4, 8, 8);
    }

    juce::AudioProcessorValueTreeState& apvts;
    
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    
    float attackWidth = 0, decayWidth = 0, sustainWidth = 0, releaseWidth = 0;
};
