#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class OscilloscopeComponent : public juce::Component, private juce::Timer
{
public:
    OscilloscopeComponent()
    {
        setOpaque(false);
        audioBuffer.setSize(1, 2048);
        audioBuffer.clear();
        startTimerHz(60); // 60 FPS refresh rate
    }

    void pushBuffer(const juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() > 0 && buffer.getNumSamples() > 0)
        {
            const juce::ScopedLock lock(bufferLock);
            
            // Copy samples to our display buffer
            int numSamples = juce::jmin(buffer.getNumSamples(), audioBuffer.getNumSamples());
            
            for (int i = 0; i < numSamples; ++i)
            {
                float sample = buffer.getSample(0, i);
                
                // Store in circular buffer
                audioBuffer.setSample(0, writePosition, sample);
                writePosition = (writePosition + 1) % audioBuffer.getNumSamples();
            }
        }
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Draw oscilloscope background
        g.setColour(juce::Colour(0xff0a0a15));
        g.fillRoundedRectangle(bounds, 10.0f);
        
        // Draw grid lines
        g.setColour(juce::Colour(0xff1a1a2e).brighter(0.3f));
        
        // Horizontal grid lines
        for (int i = 1; i < 5; ++i)
        {
            float y = bounds.getHeight() * i / 5.0f;
            g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
        }
        
        // Vertical grid lines
        for (int i = 1; i < 8; ++i)
        {
            float x = bounds.getWidth() * i / 8.0f;
            g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 1.0f);
        }
        
        // Draw center line (0V reference)
        g.setColour(juce::Colour(0xff2d2d44));
        g.drawLine(bounds.getX(), bounds.getCentreY(), bounds.getRight(), bounds.getCentreY(), 1.5f);
        
        // Draw waveform from actual audio
        drawWaveform(g, bounds);
        
        // Draw border with glow
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.reduced(1), 10.0f, 3.0f);
        
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, 10.0f, 1.5f);
    }

private:
    void timerCallback() override
    {
        repaint();
    }

    void drawWaveform(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const juce::ScopedLock lock(bufferLock);
        
        juce::Path waveformPath;
        auto width = bounds.getWidth() - 20.0f;
        auto height = bounds.getHeight() - 20.0f;
        auto centreY = bounds.getCentreY();
        auto startX = bounds.getX() + 10.0f;
        
        const int numPoints = juce::jmin(500, audioBuffer.getNumSamples());
        bool firstPoint = true;
        
        for (int i = 0; i < numPoints; ++i)
        {
            float x = startX + (width * i / (float)numPoints);
            
            // Read from circular buffer
            int readPos = (writePosition + i * (audioBuffer.getNumSamples() / numPoints)) % audioBuffer.getNumSamples();
            float sample = audioBuffer.getSample(0, readPos);
            
            // Clamp and scale sample - use full height when at maximum
            sample = juce::jlimit(-1.0f, 1.0f, sample);
            float y = centreY - (sample * height * 0.5f); // 0.5 gives full range from top to bottom
            
            if (firstPoint)
            {
                waveformPath.startNewSubPath(x, y);
                firstPoint = false;
            }
            else
            {
                waveformPath.lineTo(x, y);
            }
        }
        
        // Draw glow effect
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.15f));
        g.strokePath(waveformPath, juce::PathStrokeType(6.0f));
        
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.4f));
        g.strokePath(waveformPath, juce::PathStrokeType(3.0f));
        
        // Draw main waveform line
        g.setColour(juce::Colour(0xff00d4ff));
        g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
    }

    juce::AudioBuffer<float> audioBuffer;
    int writePosition = 0;
    juce::CriticalSection bufferLock;
};
