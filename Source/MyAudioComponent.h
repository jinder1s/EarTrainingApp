/*
  ==============================================================================

    MyAudioComponent.h
    Created: 4 May 2022 4:28:35pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MyAudioComponent  : public AudioAppComponent,
private Timer
{
public:
    MyAudioComponent();
    ~MyAudioComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // FFT stuff
    enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };
    
    //=======================AFrom Audio App demo=======================================================
    void timerCallback() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;
    void pushNextSampleIntoFifo (float sample) noexcept;
    void drawNextLineOfSpectrogram();

private:
    
    //=====FFT stuff =====
    dsp::FFT forwardFFT;
    Image spectrogramImage;

    float fifo [fftSize];
    float fftData [2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    // === Multi-timer
    int midiTimerID = 0;
    int fftTimerID = 1;
    //===============
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyAudioComponent)
};
