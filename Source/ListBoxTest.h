/*
  ==============================================================================

    ListBoxTest.h
    Created: 2 May 2022 10:32:59pm
    Author:  Manjinder Singh

  ==============================================================================
*/

#pragma once
// Forward Declaration
class MainComponent;

struct MidiDeviceListBox : public ListBox,
                           private ListBoxModel
{
    MidiDeviceListBox (const String& name,
                       MainComponent& contentComponent,
                       bool isInputDeviceList);

private:
    //==============================================================================
    MainComponent& parent;
    bool isInput;
    SparseSet<int> lastSelectedItems;
};
