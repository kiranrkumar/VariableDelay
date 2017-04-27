/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class VariableDelayAudioProcessorEditor  : public AudioProcessorEditor, private Slider::Listener, private Button::Listener //add buttons and listeners and implement both of their virtual listener functions
{
public:
    VariableDelayAudioProcessorEditor (VariableDelayAudioProcessor&);
    ~VariableDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //create a slider with specified parameters
    void initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName, Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly, juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener = true);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VariableDelayAudioProcessor& processor;
    
    //Slider and Button callback overrides
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(Button* button) override;
    
    //slider and button objects
    Slider carFreq, delLfoFreq, delLfoDepth, outGain;
    
    //slider and button initial values
    float initCarFreq = 440.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariableDelayAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
