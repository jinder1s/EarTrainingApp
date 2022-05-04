/*
  ==============================================================================

    MyAudioComponent.cpp
    Created: 4 May 2022 4:28:35pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MyAudioComponent.h"

//==============================================================================
MyAudioComponent::MyAudioComponent():
#ifdef JUCE_DEMO_RUNNER
AudioAppComponent (getSharedAudioDeviceManager (1, 0)),
#endif
forwardFFT(fftOrder), spectrogramImage(Image::RGB, 512, 512, true)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
#ifndef JUCE_DEMO_RUNNER
    std::cout << "Requesting permission";
    RuntimePermissions::request (RuntimePermissions::recordAudio,
                                 [this] (bool granted)
                                 {
        int numInputChannels = granted ? 2 : 0;
        setAudioChannels (numInputChannels, 2);
    });
#else
    setAudioChannels (2, 2);
#endif
    
    setSize(200, 200);
    
    startTimerHz(60);
}

MyAudioComponent::~MyAudioComponent()
{
    stopTimer();
    shutdownAudio();
}

void MyAudioComponent::paint (juce::Graphics& g)
{
    g.fillAll (Colours::black);

    g.setOpacity (1.0f);
    g.drawImage (spectrogramImage, getLocalBounds().toFloat());
}

void MyAudioComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void MyAudioComponent::prepareToPlay(int samplesPerBlockExpected,
                                    double sampleRate) {}
void MyAudioComponent::getNextAudioBlock(
                                        const juce::AudioSourceChannelInfo &bufferToFill) {
    
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        const auto* channelData = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);

        for (auto i = 0; i < bufferToFill.numSamples; ++i){
            pushNextSampleIntoFifo (channelData[i]);
        }

        bufferToFill.clearActiveBufferRegion();
    }
}

void MyAudioComponent::releaseResources() {}


void MyAudioComponent::pushNextSampleIntoFifo (float sample) noexcept
 {
     // if the fifo contains enough data, set a flag to say
     // that the next line should now be rendered..
     if (fifoIndex == fftSize)
     {
         if (! nextFFTBlockReady)
         {
             zeromem (fftData, sizeof (fftData));
             memcpy (fftData, fifo, sizeof (fifo));
             nextFFTBlockReady = true;
         }

         fifoIndex = 0;
     }

     fifo[fifoIndex++] = sample;
 }

void MyAudioComponent::drawNextLineOfSpectrogram()
{
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight   = spectrogramImage.getHeight();

    // first, shuffle our image leftwards by 1 pixel..
    spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform (fftData);

    // find the range of values produced, so we can scale our rendering to
    // show up the detail clearly
    auto maxLevel = FloatVectorOperations::findMinAndMax (fftData, fftSize / 2);

    for (auto y = 1; y < imageHeight; ++y)
    {
        auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
        auto fftDataIndex = jlimit (0, fftSize / 2, (int) (skewedProportionY * (int) fftSize / 2));
        auto level = jmap (fftData[fftDataIndex], 0.0f, jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

        spectrogramImage.setPixelAt (rightHandEdge, y, Colour::fromHSV (level, 1.0f, level, 1.0f));
    }
}

void MyAudioComponent::timerCallback() {

        if (nextFFTBlockReady)
        {
            drawNextLineOfSpectrogram();
            nextFFTBlockReady = false;
            repaint();
        }
    
}
