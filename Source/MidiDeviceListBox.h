/*
  ==============================================================================

    MidiDeviceListBox.h
    Created: 2 May 2022 10:38:33pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once

#include "MidiDeviceListBox.h"
#include <JuceHeader.h>

//==============================================================================
struct MidiDeviceListEntry : ReferenceCountedObject {
  MidiDeviceListEntry(MidiDeviceInfo info) : deviceInfo(info) {}

  MidiDeviceInfo deviceInfo;
  std::unique_ptr<MidiInput> inDevice;
  std::unique_ptr<MidiOutput> outDevice;

  using Ptr = ReferenceCountedObjectPtr<MidiDeviceListEntry>;
};

//==============================================================================
/*
 */
// Forward Declaration
class MainComponent;

struct MidiDeviceListBox : public ListBox, private ListBoxModel {
  MidiDeviceListBox(const String &name, MainComponent &contentComponent,
                    bool isInputDeviceList);
  int getNumRows() override;
  void paintListBoxItem(int rowNumber, Graphics &g, int width, int height,
                        bool rowIsSelected) override;
  void selectedRowsChanged(int) override;
  void syncSelectedItemsWithDeviceList(
      const ReferenceCountedArray<MidiDeviceListEntry> &midiDevices);

private:
  //==============================================================================
  MainComponent &parent;
  bool isInput;
  SparseSet<int> lastSelectedItems;
};
