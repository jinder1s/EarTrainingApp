/*
  ==============================================================================

    MidiDeviceListBox.h
    Created: 2 May 2022 10:38:33pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MidiDeviceListBox.h"

//==============================================================================
struct MidiDeviceListEntry : ReferenceCountedObject
{
    MidiDeviceListEntry (MidiDeviceInfo info) : deviceInfo (info) {}

    MidiDeviceInfo deviceInfo;
    std::unique_ptr<MidiInput> inDevice;
    std::unique_ptr<MidiOutput> outDevice;

    using Ptr = ReferenceCountedObjectPtr<MidiDeviceListEntry>;
};


//==============================================================================
/*
*/
// Forward Declaration
class MainApplication;

struct MidiDeviceListBox : public ListBox,
                           private ListBoxModel
{
    MidiDeviceListBox (const String& name,
                       MainApplication& contentComponent,
                       bool isInputDeviceList);
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics& g,
                           int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged (int) override;
    void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices);

private:
    //==============================================================================
    MainApplication& parent;
    bool isInput;
    SparseSet<int> lastSelectedItems;
};
