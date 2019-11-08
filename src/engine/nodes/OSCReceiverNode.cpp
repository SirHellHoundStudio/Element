/*
    This file is part of Element
    Copyright (C) 2019  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "engine/nodes/OSCReceiverNode.h"

namespace Element {

OSCReceiverNode::OSCReceiverNode()
    : MidiFilterNode (0)
{
    jassert (metadata.hasType (Tags::node));
    metadata.setProperty (Tags::format, "Element", nullptr);
    metadata.setProperty (Tags::identifier, EL_INTERNAL_ID_OSC_RECEIVER, nullptr);

    oscReceiver.addListener (this);
}

OSCReceiverNode::~OSCReceiverNode()
{
   oscReceiver.removeListener (this);
   oscReceiver.disconnect();
}

/** MIDI */

void OSCReceiverNode::prepareToRender (double sampleRate, int maxBufferSize) {


    if (! outputMidiMessagesInitDone) {
        outputMidiMessages.reset (sampleRate);
        currentSampleRate = sampleRate;
        outputMidiMessagesInitDone = true;
    }

    for (OSCMessage msg : oscMessages)
    {
        outputMidiMessages.addMessageToQueue ( OscProcessor::processOscToMidiMessage( msg ) );
    }

    oscMessages.clear();
}

inline void OSCReceiverNode::createPorts()
{
    if (createdPorts)
        return;

    ports.clearQuick();

    ports.add (PortType::Midi, 0, 0, "midi_in", "MIDI In", true);
    ports.add (PortType::Midi, 1, 0, "midi_out", "MIDI Out", false);
    createdPorts = true;
}


void OSCReceiverNode::render (AudioSampleBuffer& audio, MidiPipe& midi)
{
    auto timestamp = Time::getMillisecondCounterHiRes();
    const auto nframes = audio.getNumSamples();
    if (nframes == 0) {
        return;
    }

    /*

    const auto& midiOut = *midi.getReadBuffer (0);

    Method 1

    outputMidiMessages.removeNextBlockOfMessages (midiOut, nframes);
    midiOut.swapWith(messages);

    // Method 2

    MidiBuffer messages;
    outputMidiMessages.removeNextBlockOfMessages (messages, nframes);

    MidiBuffer::Iterator iter1 (messages);
    MidiMessage msg;
    int frame;

    while (iter1.getNextEvent (msg, frame))
    {
        midiOut.addEvent (msg, frame);
    }

    */
}

/* OSCReceiver real-time callbacks */

void OSCReceiverNode::oscMessageReceived(const OSCMessage& message)
{
    DBG("[EL] Queue OSC message -> MIDI: " << OscProcessor::processOscToMidiMessage(message).getDescription());
    oscMessages.push_back(message);
};

void OSCReceiverNode::oscBundleReceived(const OSCBundle& bundle)
{
};

/** For node editor */

bool OSCReceiverNode::connect (int portNumber)
{
    connected = oscReceiver.connect (portNumber);
    return connected;
}

bool OSCReceiverNode::disconnect ()
{
    connected = false;
    return oscReceiver.disconnect();
}

bool OSCReceiverNode::isConnected ()
{
    return connected;
}

int OSCReceiverNode::getCurrentPortNumber ()
{
    return currentPortNumber;
};

String OSCReceiverNode::getCurrentHostName ()
{
    if (currentHostName == "")
        currentHostName = IPAddress::getLocalAddress().toString();
    return currentHostName;
}


bool OSCReceiverNode::isValidOscPort (int port) const
{
    return port > 0 && port < 65536;
}

/* OSCReceiver message loop callbacks */

void OSCReceiverNode::addMessageLoopListener (OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>* callback)
{
    oscReceiver.addListener (callback);
}

void OSCReceiverNode::removeMessageLoopListener (OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>* callback)
{
    oscReceiver.removeListener (callback);
}

}
