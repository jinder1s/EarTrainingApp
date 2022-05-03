/*
  ==============================================================================

    MainApplication.h
    Created: 2 May 2022 9:58:36pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once

#include "MidiDeviceListBox.h"
#include <JuceHeader.h>

//==============================================================================
/*
 */
class MainApplication : public AudioAppComponent,
                        private Timer,
                        private MidiKeyboardState::Listener,
                        private MidiInputCallback,
                        private AsyncUpdater {
public:
  MainApplication();
  ~MainApplication() override;

  //=======================AFrom Audio App
  // demo=======================================================
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void
  getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  //=======From MiDI demo
  void timerCallback() override;
  void handleNoteOn(MidiKeyboardState *, int midiChannel, int midiNoteNumber,
                    float velocity) override;
  void handleNoteOff(MidiKeyboardState *, int midiChannel, int midiNoteNumber,
                     float velocity) override;

  void openDevice(bool isInput, int index);
  void closeDevice(bool isInput, int index);

  int getNumMidiInputs() const noexcept;
  int getNumMidiOutputs() const noexcept;

  ReferenceCountedObjectPtr<MidiDeviceListEntry>
  getMidiDevice(int index, bool isInput) const noexcept;

private:
  void handleIncomingMidiMessage(MidiInput * /*source*/,
                                 const MidiMessage &message) override;
  void handleAsyncUpdate() override;
  void sendToOutputs(const MidiMessage &msg);

  bool hasDeviceListChanged(const Array<MidiDeviceInfo> &availableDevices,
                            bool isInputDevice);

  ReferenceCountedObjectPtr<MidiDeviceListEntry>
  findDevice(MidiDeviceInfo device, bool isInputDevice) const;
  /// 2
  void
  closeUnpluggedDevices(const Array<MidiDeviceInfo> &currentlyPluggedInDevices,
                        bool isInputDevice);

  void updateDeviceList(bool isInputDeviceList);

  void addLabelAndSetStyle(Label &label);

  //========================================

  void paint(juce::Graphics &) override;
  void resized() override;

  //===============================
  Label midiInputLabel{"Midi Input Label", "MIDI Input:"};
  Label midiOutputLabel{"Midi Output Label", "MIDI Output:"};
  Label incomingMidiLabel{"Incoming Midi Label", "Received MIDI messages:"};
  Label outgoingMidiLabel{"Outgoing Midi Label",
                          "Play the keyboard to send MIDI messages..."};

  TextButton pairButton{"MIDI Bluetooth devices..."};

  MidiKeyboardState keyboardState;
  MidiKeyboardComponent midiKeyboard;

  TextEditor midiMonitor{"MIDI Monitor"};

  std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;
  ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;

  CriticalSection midiMonitorLock;
  Array<MidiMessage> incomingMessages;
  //=================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainApplication)
};
