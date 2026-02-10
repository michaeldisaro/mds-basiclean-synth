#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Modern color scheme
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff1a1a2e));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xff00d4ff));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00d4ff));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2d2d44));
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0f0f1e));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff2d2d44));
        setColour(juce::Label::textColourId, juce::Colour(0xffcccccc));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        // Draw outer glow
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                   arcRadius, arcRadius, 0.0f,
                                   rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Draw filled arc
        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                  arcRadius, arcRadius, 0.0f,
                                  rotaryStartAngle, toAngle, true);

            auto fillColour = findColour(juce::Slider::rotarySliderFillColourId);
            g.setColour(fillColour);
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // Glow effect
            g.setColour(fillColour.withAlpha(0.3f));
            g.strokePath(valueArc, juce::PathStrokeType(lineW + 4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Draw thumb
        juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                     bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillEllipse(juce::Rectangle<float>(lineW * 1.5f, lineW * 1.5f).withCentre(thumbPoint));
        
        // Inner shadow on thumb
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillEllipse(juce::Rectangle<float>(lineW * 1.2f, lineW * 1.2f).withCentre(thumbPoint));
    }

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto* l = LookAndFeel_V4::createSliderTextBox(slider);
        l->setFont(juce::FontOptions(14.0f));
        l->setJustificationType(juce::Justification::centred);
        return l;
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto radius = 8.0f; // Fixed radius for consistent circle size
        auto circleCentreX = bounds.getX() + radius + 5.0f; // Position circle on the left with some padding
        auto circleCentreY = bounds.getCentreY();

        // Draw outer circle
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.fillEllipse(circleCentreX - radius, circleCentreY - radius, radius * 2.0f, radius * 2.0f);

        // Draw border
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawEllipse(circleCentreX - radius, circleCentreY - radius, radius * 2.0f, radius * 2.0f, 2.0f);

        if (button.getToggleState())
        {
            // Draw inner filled circle when selected
            auto innerRadius = radius * 0.6f;
            g.setColour(juce::Colour(0xFF00FFFF)); // Cyan color
            g.fillEllipse(circleCentreX - innerRadius, circleCentreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

            // Add glow effect
            g.setColour(juce::Colour(0xFF00FFFF).withAlpha(0.3f));
            g.fillEllipse(circleCentreX - radius * 0.8f, circleCentreY - radius * 0.8f, radius * 1.6f, radius * 1.6f);
        }

        // Draw text label to the right of the circle
        auto textBounds = bounds.withLeft(circleCentreX + radius + 8.0f); // Text starts after circle
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText(button.getButtonText(), textBounds, juce::Justification::centredLeft, true);
    }
};
