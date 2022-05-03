/*
  ==============================================================================

    MainApplication.cpp
    Created: 2 May 2022 9:58:36pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainApplication.h"

//==============================================================================
MainApplication::MainApplication(): midiKeyboard(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
                                    midiInputSelector  (new MidiDeviceListBox ("Midi Input Selector",  *this, true)),
                                    midiOutputSelector (new MidiDeviceListBox ("Midi Output Selector", *this, false))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    setSize(800, 600);
    midiKeyboard.setName ("MIDI Keyboard");
    addAndMakeVisible (midiKeyboard);
}

MainApplication::~MainApplication()
{
}

void MainApplication::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("MainApplication", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void MainApplication::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto margin = 10;
    midiKeyboard.setBounds (margin, (getHeight() / 2) + (24 + margin), getWidth() - (2 * margin), 64);
}

//====================
void  MainApplication::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {
}
void  MainApplication::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) {
}
void  MainApplication::releaseResources() {
}
//====================

void MainApplication::timerCallback() {
}

void MainApplication::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    
    
}

void MainApplication::handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    
}
void MainApplication::handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage& message) {
}

void MainApplication::openDevice (bool isInput, int index)
{
    if (isInput)
    {
        jassert (midiInputs[index]->inDevice.get() == nullptr);
        midiInputs[index]->inDevice = MidiInput::openDevice (midiInputs[index]->deviceInfo.identifier, this);

        if (midiInputs[index]->inDevice.get() == nullptr)
        {
            DBG ("MidiDemo::openDevice: open input device for index = " << index << " failed!");
            return;
        }

        midiInputs[index]->inDevice->start();
    }
    else
    {
        jassert (midiOutputs[index]->outDevice.get() == nullptr);
        midiOutputs[index]->outDevice = MidiOutput::openDevice (midiOutputs[index]->deviceInfo.identifier);

        if (midiOutputs[index]->outDevice.get() == nullptr)
        {
            DBG ("MidiDemo::openDevice: open output device for index = " << index << " failed!");
        }
    }
}

void MainApplication::closeDevice (bool isInput, int index)
{
    if (isInput)
    {
        jassert (midiInputs[index]->inDevice.get() != nullptr);
        midiInputs[index]->inDevice->stop();
        midiInputs[index]->inDevice.reset();
    }
    else
    {
        jassert (midiOutputs[index]->outDevice.get() != nullptr);
        midiOutputs[index]->outDevice.reset();
    }
}

int MainApplication::getNumMidiInputs() const noexcept
{
    return midiInputs.size();
}

int MainApplication::getNumMidiOutputs() const noexcept
{
    return midiOutputs.size();
}

ReferenceCountedObjectPtr<MidiDeviceListEntry> MainApplication::getMidiDevice (int index, bool isInput) const noexcept
{
    return isInput ? midiInputs[index] : midiOutputs[index];
}



void MainApplication::handleAsyncUpdate() {
}
