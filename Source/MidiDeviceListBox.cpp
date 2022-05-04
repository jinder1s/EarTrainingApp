/*
  ==============================================================================

    MidiDeviceListBox.cpp
    Created: 2 May 2022 10:38:33pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#include "MidiDeviceListBox.h"
#include "MainComponent.h"
#include <JuceHeader.h>

//==============================================================================
MidiDeviceListBox::MidiDeviceListBox(const String &name,
                                     MainComponent &contentComponent,
                                     bool isInputDeviceList)
    : ListBox(name, this), parent(contentComponent),
      isInput(isInputDeviceList) {
  setOutlineThickness(1);
  setMultipleSelectionEnabled(true);
  setClickingTogglesRowSelection(true);
}

//==============================================================================
int MidiDeviceListBox::getNumRows() {
  return isInput ? parent.getNumMidiInputs() : parent.getNumMidiOutputs();
}

void MidiDeviceListBox::paintListBoxItem(int rowNumber, Graphics &g, int width,
                                         int height, bool rowIsSelected) {
  auto textColour = getLookAndFeel().findColour(ListBox::textColourId);

  if (rowIsSelected)
    g.fillAll(textColour.interpolatedWith(
        getLookAndFeel().findColour(ListBox::backgroundColourId), 0.5));

  g.setColour(textColour);
  g.setFont((float)height * 0.7f);

  if (isInput) {
    if (rowNumber < parent.getNumMidiInputs())
      g.drawText(parent.getMidiDevice(rowNumber, true)->deviceInfo.name, 5, 0,
                 width, height, Justification::centredLeft, true);
  } else {
    if (rowNumber < parent.getNumMidiOutputs())
      g.drawText(parent.getMidiDevice(rowNumber, false)->deviceInfo.name, 5, 0,
                 width, height, Justification::centredLeft, true);
  }
}

//==============================================================================
void MidiDeviceListBox::selectedRowsChanged(int) {
  auto newSelectedItems = getSelectedRows();
  if (newSelectedItems != lastSelectedItems) {
    for (auto i = 0; i < lastSelectedItems.size(); ++i) {
      if (!newSelectedItems.contains(lastSelectedItems[i]))
        parent.closeDevice(isInput, lastSelectedItems[i]);
    }

    for (auto i = 0; i < newSelectedItems.size(); ++i) {
      if (!lastSelectedItems.contains(newSelectedItems[i]))
        parent.openDevice(isInput, newSelectedItems[i]);
    }

    lastSelectedItems = newSelectedItems;
  }
}

//==============================================================================
void MidiDeviceListBox::syncSelectedItemsWithDeviceList(
    const ReferenceCountedArray<MidiDeviceListEntry> &midiDevices) {
  SparseSet<int> selectedRows;
  for (auto i = 0; i < midiDevices.size(); ++i)
    if (midiDevices[i]->inDevice.get() != nullptr ||
        midiDevices[i]->outDevice.get() != nullptr)
      selectedRows.addRange(Range<int>(i, i + 1));

  lastSelectedItems = selectedRows;
  updateContent();
  setSelectedRows(selectedRows, dontSendNotification);
}
