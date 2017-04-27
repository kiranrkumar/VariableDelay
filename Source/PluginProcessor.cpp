/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
VariableDelayAudioProcessor::VariableDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

VariableDelayAudioProcessor::~VariableDelayAudioProcessor()
{
}

//==============================================================================
const String VariableDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VariableDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VariableDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double VariableDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VariableDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VariableDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VariableDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String VariableDelayAudioProcessor::getProgramName (int index)
{
    return String();
}

void VariableDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VariableDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mCarrierFreq = 440.;
    mCarrierPhase = 0.;
    mLfoPhase = 0.0;
    mPrevLfoDepth = 2500; //samples
    mLfoDepth = 2500;
    mSampleRate = sampleRate;
    mPrevOutputGain = 1.0, mOutputGain = 1.0;
    maxDelSeconds = 3.0; //maximum 3 second delay
    delBuffSize = static_cast<int>(mSampleRate * maxDelSeconds);
    delBuffReader = 0, delBuffWriter = 0;
    
    delayBuff.setSize(1, delBuffSize, false, true);
    trSamplesLeft = 0;
    
    //initialize all delayBuff values to 0
    for (int i = 0; i < delBuffSize; ++i)
    {
        delayBuff.setSample(0, i, 0);
    }
}

void VariableDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VariableDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VariableDelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    
    //determine whether we are transitioning LFO Depth values
    bool trnLfoDepth = (mPrevLfoDepth != mLfoDepth);
    if (trnLfoDepth)
    {
        trSamplesLeft = buffer.getNumSamples();
    }
    //generate a basic sine tone
    float curSineValue;
    float curDelLfoValue;
    
    //loop through every sample in the current buffer
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        
        //find the current carrier sine wave value
        curSineValue = sinf(updateCarPhase());
        //write the unmodified sine wave sample into the delay buffer
        delayBuff.setSample(0, delBuffWriter, curSineValue);
        
        //find the current modulator sine wave value. Adjust using lfo depth and make sure it has a [0, mLfoDepth] range rather than [-mLfoDepth, mLfoDepth]
//        curDelLfoValue = (mLfoDepth * sinf( updateModPhase() ) + mLfoDepth ) / 2;
        
        //find the modulator sine wave phase and value
        float curModPhase = updateModPhase();
        float curModSample = (mLfoDepth * sinf(curModPhase) + mLfoDepth) / 2.0;
        //account for LFO depth transitions
        if (trnLfoDepth)
        {
            //linear fade from previous depth to new depth over the course of this callback iteration
            float curPrevModSample = (mPrevLfoDepth * sinf(curModPhase) + mPrevLfoDepth) / 2.0;
            float curScale = (buffer.getNumSamples() - trSamplesLeft) / buffer.getNumSamples();
            float prevScale = trSamplesLeft / buffer.getNumSamples();
            
            curModSample = curScale * curModSample + prevScale * curPrevModSample;
            --trSamplesLeft;
        }
        //get the "delayed" index, i.e. where in the delay buffer to actually go to retrieve the current output sample value. Here, we delay (i.e. subtract) the modulator's current value from the value of the buffer writer. We mod this by the total size of the delay buffer so that we don't go out of bounds of the delay buffer (in either direction)
        //wonky modulo method since I can't find a cleaner way to guarantee a positive modulo result right now
        delBuffReader = ((delBuffWriter - static_cast<long>(curModSample)) % delBuffSize + delBuffSize) % delBuffSize;
        
        //        delBuffReader = static_cast<long>(
//            std::fmod(delBuffWriter - curModSample,delBuffSize) );
        
        
        //write the properly delayBuff sample value into the output buffer
        //*******TO DO: *********
        //  Make this a fractional delay for a smooth(er) transition between delay frequency values
        float pctLaterSample = curModSample - floorf(curModSample);
        long laterSampleIx = (delBuffReader + 1) % delBuffSize;
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            buffer.setSample(ch, i,
                (delayBuff.getSample(0, delBuffReader) * (1.0 - pctLaterSample) +
                delayBuff.getSample(0, laterSampleIx) * pctLaterSample));
        }
    
        //increment the writer, being careful to not exceed the buffer bounds
        delBuffWriter = (delBuffWriter + 1) % delBuffSize;
        
    }
    
    //apply smooth gain ramp
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        buffer.applyGainRamp(ch, 0, buffer.getNumSamples(), mPrevOutputGain, mOutputGain);
    }
    
    //update previous gain so that we don't keep ramping during every callback
    mPrevOutputGain = mOutputGain;
    
    //update lfo depth members appropriately
    if (trnLfoDepth)
    {
        mPrevLfoDepth = mLfoDepth;
        trnLfoDepth = false;
    }
    
    
}

//==============================================================================
bool VariableDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VariableDelayAudioProcessor::createEditor()
{
    return new VariableDelayAudioProcessorEditor (*this);
}

//==============================================================================
void VariableDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VariableDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

float VariableDelayAudioProcessor::updateCarPhase()
{
    mCarrierPhase = std::fmod( mCarrierPhase +      2 * M_PI * mCarrierFreq / mSampleRate,      2 * M_PI);
    
    return mCarrierPhase;
}

float VariableDelayAudioProcessor::updateModPhase()
{
    mLfoPhase = std::fmod( mLfoPhase + 2 * M_PI * mLfoDelFreq / mSampleRate, 2 * M_PI);
    
    return mLfoPhase;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VariableDelayAudioProcessor();
}
