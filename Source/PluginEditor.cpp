/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
VariableDelayAudioProcessorEditor::VariableDelayAudioProcessorEditor (VariableDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);
    
    //create sliders
    
    //carrier Frequency
    initSlider(&carFreq, Slider::RotaryHorizontalVerticalDrag, "Carrier Freq", Slider::TextBoxRight, false, " Hz", 0.0, 2000.0, 1.0, initCarFreq);
    
    //delay LFO frequency
    initSlider(&delLfoFreq, Slider::RotaryHorizontalVerticalDrag, "LFO Freq", Slider::TextBoxRight, false, " Hz", 0.0, 50.0, 0.2, 0.0);
    
    //output volume in dB
    initSlider(&outGain, Slider::LinearHorizontal, "Out Gain", Slider::TextBoxBelow, false, " dB", -60.0, 0.0, 0.1, 0.0);
    
    //delay LFO depth
    initSlider(&delLfoDepth, Slider::LinearHorizontal, "LFO Depth", Slider::TextBoxBelow, false, "", 0.0, 5000.0, 1.0, 1500.0);
    
}

VariableDelayAudioProcessorEditor::~VariableDelayAudioProcessorEditor()
{
}

void VariableDelayAudioProcessorEditor::initSlider(
    Slider* slider, Slider::SliderStyle newStyle,
    juce::String newName,
    Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
    juce::String newSuffix,
    double newMin, double newMax, double newInc,
    double newValue,
    bool addListener)
{
    slider->setSliderStyle(newStyle);
    slider->setName(newName);
    slider->setTextBoxStyle(newTxtBoxPos, txtIsReadOnly, slider->getTextBoxWidth(), slider->getTextBoxHeight());
    slider->setTextValueSuffix(newSuffix);
    slider->setRange(newMin, newMax, newInc);
    slider->setValue(newValue);
    
    if (addListener)
    {
        slider->addListener(this);
    }
    
    addAndMakeVisible(slider);
    
}

//==============================================================================
void VariableDelayAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::black);
    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
    g.drawFittedText("Carrier Frequency", 50, 30, 100, 25, Justification::left, 1);
    g.drawFittedText("LFO Frequency", 250, 30, 100, 25, Justification::left, 1);
    g.drawFittedText("Gain", 10, 185, 100, 25, Justification::centred, 1);
    g.drawFittedText("Delay LFO Depth", 230, 185, 100, 25, Justification::centred, 1);
}

void VariableDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    carFreq.setBounds(50, 30, 150, 150);
    delLfoFreq.setBounds(245, 30, 200, 200);
    outGain.setBounds(25, 210, 200, 50);
    delLfoDepth.setBounds(225, 210, 200, 50);
}

void VariableDelayAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    //convert dB gain to linear gain
    if (slider->getName() == "Out Gain")
    {
        processor.mPrevOutputGain = processor.mOutputGain;
        processor.mOutputGain = Decibels::decibelsToGain(slider->getValue());
    }
    else if (slider->getName() == "Carrier Freq")
    {
        processor.mCarrierFreq = slider->getValue();
    }
    else if (slider->getName() == "LFO Freq")
    {
        //because of how the modulator gets treated (zero-min), we want to half this frequency so that there's basically one movement away from 0 per period rather than 2
        processor.mLfoDelFreq = slider->getValue() / 2;
    }
    else if (slider->getName() == "LFO Depth")
    {
        processor.mPrevLfoDepth = processor.mLfoDepth;
        processor.mLfoDepth = slider->getValue();
    }
}

void VariableDelayAudioProcessorEditor::buttonClicked(Button* button)
{
    
}
