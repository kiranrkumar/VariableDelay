/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class VariableDelayAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    VariableDelayAudioProcessor();
    ~VariableDelayAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //custom functions to update phase values
    float updateCarPhase();
    float updateModPhase();
    
    float mOutputGain, mPrevOutputGain; //gains
    float mLfoDelFreq, mCarrierFreq; //frequencies
    float mLfoDepth, mPrevLfoDepth; //delay LFO amplitude
    float mCurSampleVal; //sample value
    float mLfoPhase, mCarrierPhase; //phase values
    float mSampleRate; //sample rate
    
    AudioSampleBuffer delayBuff; //to hold signal content to refer back to for delay purposes
    long delBuffSize;
    long delBuffReader, delBuffWriter;
    float maxDelSeconds;
    int trSamplesLeft; //number of samples left to transition from one value to the next (used for LFO depth to minimize clicks)

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariableDelayAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
