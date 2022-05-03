/*
  ==============================================================================

    ListBoxTest.h
    Created: 2 May 2022 10:32:59pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once
// Forward Declaration
class MainApplication;

struct MidiDeviceListBox : public ListBox,
                           private ListBoxModel
{
    MidiDeviceListBox (const String& name,
                       MainApplication& contentComponent,
                       bool isInputDeviceList);

private:
    //==============================================================================
    MainApplication& parent;
    bool isInput;
    SparseSet<int> lastSelectedItems;
};
