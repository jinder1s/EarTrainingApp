/*
  ==============================================================================

    MainApplication.cpp
    Created: 2 May 2022 9:58:36pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#include "MainApplication.h"
#include <JuceHeader.h>

//==============================================================================
MainApplication::MainApplication()
    : midiKeyboard(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
      midiInputSelector(
          new MidiDeviceListBox("Midi Input Selector", *this, true)),
      midiOutputSelector(
          new MidiDeviceListBox("Midi Output Selector", *this, false)) {
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.

  addLabelAndSetStyle(midiInputLabel);
  addLabelAndSetStyle(midiOutputLabel);
  addLabelAndSetStyle(incomingMidiLabel);
  addLabelAndSetStyle(outgoingMidiLabel);

  midiKeyboard.setName("MIDI Keyboard");
  addAndMakeVisible(midiKeyboard);

  midiMonitor.setMultiLine(true);
  midiMonitor.setReturnKeyStartsNewLine(false);
  midiMonitor.setReadOnly(true);
  midiMonitor.setScrollbarsShown(true);
  midiMonitor.setCaretVisible(false);
  midiMonitor.setPopupMenuEnabled(false);
  midiMonitor.setText({});
  addAndMakeVisible(midiMonitor);

  if (!BluetoothMidiDevicePairingDialogue::isAvailable())
    pairButton.setEnabled(false);

  addAndMakeVisible(pairButton);
  pairButton.onClick = [] {
    RuntimePermissions::request(RuntimePermissions::bluetoothMidi,
                                [](bool wasGranted) {
                                  if (wasGranted)
                                    BluetoothMidiDevicePairingDialogue::open();
                                });
  };
  keyboardState.addListener(this);

  addAndMakeVisible(midiInputSelector.get());
  addAndMakeVisible(midiOutputSelector.get());

  setSize(732, 520);

  startTimer(500);
}

MainApplication::~MainApplication() {
  stopTimer();
  midiInputs.clear();
  midiOutputs.clear();
  keyboardState.removeListener(this);

  midiInputSelector.reset();
  midiOutputSelector.reset();
}

//====================
void MainApplication::prepareToPlay(int samplesPerBlockExpected,
                                    double sampleRate) {}
void MainApplication::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {}
void MainApplication::releaseResources() {}
//====================

void MainApplication::paint(juce::Graphics &g) {
  /* This demo code just fills the component's background and
     draws some placeholder text to get you started.

     You should replace everything in this method with your own
     drawing code..
  */

  g.fillAll(getLookAndFeel().findColour(
      juce::ResizableWindow::backgroundColourId)); // clear the background

  g.setColour(juce::Colours::grey);
  g.drawRect(getLocalBounds(), 1); // draw an outline around the component

  g.setColour(juce::Colours::white);
  g.setFont(14.0f);
  g.drawText("MainApplication", getLocalBounds(), juce::Justification::centred,
             true); // draw some placeholder text
}

void MainApplication::resized() {
  auto margin = 10;

  midiInputLabel.setBounds(margin, margin, (getWidth() / 2) - (2 * margin), 24);

  midiOutputLabel.setBounds((getWidth() / 2) + margin, margin,
                            (getWidth() / 2) - (2 * margin), 24);

  midiInputSelector->setBounds(margin, (2 * margin) + 24,
                               (getWidth() / 2) - (2 * margin),
                               (getHeight() / 2) - ((4 * margin) + 24 + 24));

  midiOutputSelector->setBounds((getWidth() / 2) + margin, (2 * margin) + 24,
                                (getWidth() / 2) - (2 * margin),
                                (getHeight() / 2) - ((4 * margin) + 24 + 24));

  pairButton.setBounds(margin, (getHeight() / 2) - (margin + 24),
                       getWidth() - (2 * margin), 24);

  outgoingMidiLabel.setBounds(margin, getHeight() / 2,
                              getWidth() - (2 * margin), 24);
  midiKeyboard.setBounds(margin, (getHeight() / 2) + (24 + margin),
                         getWidth() - (2 * margin), 64);

  incomingMidiLabel.setBounds(margin,
                              (getHeight() / 2) + (24 + (2 * margin) + 64),
                              getWidth() - (2 * margin), 24);

  auto y = (getHeight() / 2) + ((2 * 24) + (3 * margin) + 64);
  midiMonitor.setBounds(margin, y, getWidth() - (2 * margin),
                        getHeight() - y - margin);
}

void MainApplication::sendToOutputs(const MidiMessage &msg) {
  for (auto midiOutput : midiOutputs)
    if (midiOutput->outDevice.get() != nullptr)
      midiOutput->outDevice->sendMessageNow(msg);
}

void MainApplication::timerCallback() {
  updateDeviceList(true);
  updateDeviceList(false);
}

void MainApplication::handleNoteOn(MidiKeyboardState *, int midiChannel,
                                   int midiNoteNumber, float velocity) {
  MidiMessage m(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
  m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
  sendToOutputs(m);
}

void MainApplication::handleNoteOff(MidiKeyboardState *, int midiChannel,
                                    int midiNoteNumber, float velocity) {
  MidiMessage m(MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity));
  m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
  sendToOutputs(m);
}
void MainApplication::handleIncomingMidiMessage(MidiInput * /*source*/,
                                                const MidiMessage &message) {
  // This is called on the MIDI thread
  const ScopedLock sl(midiMonitorLock);
  incomingMessages.add(message);
  triggerAsyncUpdate();
}

void MainApplication::handleAsyncUpdate() {
  // This is called on the message loop
  Array<MidiMessage> messages;

  {
    const ScopedLock sl(midiMonitorLock);
    messages.swapWith(incomingMessages);
  }

  String messageText;

  for (auto &m : messages)
    messageText << m.getDescription() << "\n";

  midiMonitor.insertTextAtCaret(messageText);
}

void MainApplication::openDevice(bool isInput, int index) {
  if (isInput) {
    jassert(midiInputs[index]->inDevice.get() == nullptr);
    midiInputs[index]->inDevice =
        MidiInput::openDevice(midiInputs[index]->deviceInfo.identifier, this);

    if (midiInputs[index]->inDevice.get() == nullptr) {
      DBG("MidiDemo::openDevice: open input device for index = " << index
                                                                 << " failed!");
      return;
    }

    midiInputs[index]->inDevice->start();
  } else {
    jassert(midiOutputs[index]->outDevice.get() == nullptr);
    midiOutputs[index]->outDevice =
        MidiOutput::openDevice(midiOutputs[index]->deviceInfo.identifier);

    if (midiOutputs[index]->outDevice.get() == nullptr) {
      DBG("MidiDemo::openDevice: open output device for index = "
          << index << " failed!");
    }
  }
}

void MainApplication::closeDevice(bool isInput, int index) {
  if (isInput) {
    jassert(midiInputs[index]->inDevice.get() != nullptr);
    midiInputs[index]->inDevice->stop();
    midiInputs[index]->inDevice.reset();
  } else {
    jassert(midiOutputs[index]->outDevice.get() != nullptr);
    midiOutputs[index]->outDevice.reset();
  }
}

void MainApplication::closeUnpluggedDevices(
    const Array<MidiDeviceInfo> &currentlyPluggedInDevices,
    bool isInputDevice) {
  ReferenceCountedArray<MidiDeviceListEntry> &midiDevices =
      isInputDevice ? midiInputs : midiOutputs;

  for (auto i = midiDevices.size(); --i >= 0;) {
    auto &d = *midiDevices[i];

    if (!currentlyPluggedInDevices.contains(d.deviceInfo)) {
      if (isInputDevice ? d.inDevice.get() != nullptr
                        : d.outDevice.get() != nullptr)
        closeDevice(isInputDevice, i);

      midiDevices.remove(i);
    }
  }
}

void MainApplication::updateDeviceList(bool isInputDeviceList) {
  auto availableDevices = isInputDeviceList ? MidiInput::getAvailableDevices()
                                            : MidiOutput::getAvailableDevices();

  if (hasDeviceListChanged(availableDevices, isInputDeviceList)) {

    ReferenceCountedArray<MidiDeviceListEntry> &midiDevices =
        isInputDeviceList ? midiInputs : midiOutputs;

    closeUnpluggedDevices(availableDevices, isInputDeviceList);

    ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;

    // add all currently plugged-in devices to the device list
    for (auto &newDevice : availableDevices) {
      MidiDeviceListEntry::Ptr entry = findDevice(newDevice, isInputDeviceList);

      if (entry == nullptr)
        entry = new MidiDeviceListEntry(newDevice);

      newDeviceList.add(entry);
    }

    // actually update the device list
    midiDevices = newDeviceList;

    // update the selection status of the combo-box
    if (auto *midiSelector = isInputDeviceList ? midiInputSelector.get()
                                               : midiOutputSelector.get())
      midiSelector->syncSelectedItemsWithDeviceList(midiDevices);
  }
}

ReferenceCountedObjectPtr<MidiDeviceListEntry>
MainApplication::findDevice(MidiDeviceInfo device, bool isInputDevice) const {
  const ReferenceCountedArray<MidiDeviceListEntry> &midiDevices =
      isInputDevice ? midiInputs : midiOutputs;

  for (auto &d : midiDevices)
    if (d->deviceInfo == device)
      return d;

  return nullptr;
}

bool MainApplication::hasDeviceListChanged(
    const Array<MidiDeviceInfo> &availableDevices, bool isInputDevice) {
  ReferenceCountedArray<MidiDeviceListEntry> &midiDevices =
      isInputDevice ? midiInputs : midiOutputs;

  if (availableDevices.size() != midiDevices.size())
    return true;

  for (auto i = 0; i < availableDevices.size(); ++i)
    if (availableDevices[i] != midiDevices[i]->deviceInfo)
      return true;

  return false;
}

void MainApplication::addLabelAndSetStyle(Label &label) {
  label.setFont(Font(15.00f, Font::plain));
  label.setJustificationType(Justification::centredLeft);
  label.setEditable(false, false, false);
  label.setColour(TextEditor::textColourId, Colours::black);
  label.setColour(TextEditor::backgroundColourId, Colour(0x00000000));

  addAndMakeVisible(label);
}

int MainApplication::getNumMidiInputs() const noexcept {
  return midiInputs.size();
}

int MainApplication::getNumMidiOutputs() const noexcept {
  return midiOutputs.size();
}

ReferenceCountedObjectPtr<MidiDeviceListEntry>
MainApplication::getMidiDevice(int index, bool isInput) const noexcept {
  return isInput ? midiInputs[index] : midiOutputs[index];
}
