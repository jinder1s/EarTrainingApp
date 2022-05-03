/*
  ==============================================================================

    MainApplication.h
    Created: 2 May 2022 9:58:36pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once




#include <JuceHeader.h>
#include "MidiDeviceListBox.h"


//==============================================================================
/*
*/
class MainApplication  : public AudioAppComponent,
                         private Timer,
                         private MidiKeyboardState::Listener,
                         private MidiInputCallback,
                         private AsyncUpdater
{
public:
    MainApplication();
    ~MainApplication() override;
    
    //=======================AFrom Audio App demo=======================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    
    //=======From MiDI demo
    void timerCallback() override;
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage& message) override;
    void handleAsyncUpdate() override;
    
    ///2
    void openDevice (bool isInput, int index);
    
    void closeDevice (bool isInput, int index);


    int getNumMidiInputs() const noexcept;


    int getNumMidiOutputs() const noexcept;

    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput) const noexcept;


    
    //========================================

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //===============================
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent midiKeyboard;
    std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;
    ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;
    //=================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainApplication)
};

