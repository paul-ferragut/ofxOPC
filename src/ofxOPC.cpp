//
//  ofxOPC.cpp
//  openPixelControl
//
//  Created by David Haylock on 25/05/2014.
//
#include "ofxOPC.h"
//--------------------------------------------------------------
void ofxOPC::setup(string address, int port,int fadeCandyNumber)
{
    // Copy the Address and port to the variables
    _port = port;
    _address = address;
    
    connectionAttempts = 0;
    tryReconnecting = false;
    startTime = ofGetElapsedTimeMillis();  // get the start time
    endTime = 5000;// in milliseconds
    
    // Connect to the Server
    connect();
    
    channelNumber=8*fadeCandyNumber;
    
    // Determine the length of the data section, as a multiple of the SPCData type
    uint16_t data_length = channelNumber * 64 * sizeof(OPCPacket_SPCData_t);
    
    // Add the header-section's length to the data-section's to determine the total packet length; allocate the packet
    OPC_SPC_packet_length = sizeof(OPCPacket_Header_t) + data_length;
    OPC_SPC_packet = (OPCPacket_t)malloc(OPC_SPC_packet_length);

    // Ensure all LEDs are initially off
    memset(&OPC_SPC_packet->data, 0, data_length);
    
    // Fill out the header
    OPC_SPC_packet->header.channel = 0x00;
    OPC_SPC_packet->header.command = 0x00; // Set Pixel Colour
    OPC_SPC_packet->header.data_length = htons(data_length); // Convert the 16bit number into two bytes (High-, then Low-byte)
    
    // Create an alias for code-readability
    OPC_SPC_packet_data = (OPCPacket_SPCData_t)(&OPC_SPC_packet->data);
}
//--------------------------------------------------------------
void ofxOPC::cleanup()
{
    // Clean-up
    free(OPC_SPC_packet);
}
//--------------------------------------------------------------
void ofxOPC::update()
{
    timer = ofGetElapsedTimeMillis() - startTime;
}
//--------------------------------------------------------------
void ofxOPC::writeChannel(uint8_t channel, vector<ofColor>pix)
{
    // Bail early if there's no pixel data
    if(pix.empty())
    {
        return;

    } else if(channel < 1 || channel > channelNumber) {
        // TODO: Emit error
        return;
    }

    uint16_t channel_offset = (channel - 1) * 64;
    
    // Copy the data
    for (int i = 0; i < pix.size(); i++)
    {
        OPC_SPC_packet_data[channel_offset + i].r = pix[i].r;
        OPC_SPC_packet_data[channel_offset + i].g = pix[i].g;
        OPC_SPC_packet_data[channel_offset + i].b = pix[i].b;
    }
    
    // Send the data
    client.sendRawBytes((char *)(OPC_SPC_packet), OPC_SPC_packet_length);
}
//--------------------------------------------------------------
void ofxOPC::writeChannel(uint8_t channel, vector <ofColor> pix1,vector <ofColor> pix2,vector <ofColor> pix3)
{
    pix1.insert(pix1.end(), pix2.begin(),pix2.end());
    pix1.insert(pix1.end(), pix3.begin(),pix3.end());
    
    // Bail early if there's no pixel data
    if(pix1.empty() && pix2.empty() && pix3.empty())
    {
        return;
        
    } else if(channel < 1 || channel > channelNumber) {
        // TODO: Emit error
        return;
    }
    
    uint16_t channel_offset = (channel - 1) * 64;
    
    // Copy the data
    for (int i = 0; i < pix1.size(); i++)
    {
        OPC_SPC_packet_data[channel_offset + i].r = pix1[i].r;
        OPC_SPC_packet_data[channel_offset + i].g = pix1[i].g;
        OPC_SPC_packet_data[channel_offset + i].b = pix1[i].b;
    }
    
    // Send the data
    client.sendRawBytes((char *)(OPC_SPC_packet), OPC_SPC_packet_length);
}
//--------------------------------------------------------------
void ofxOPC::draw()
{
    // Redundant unless color data needs to be drawn into strings
    // Or data about the connected unit
}
//--------------------------------------------------------------
bool ofxOPC::isConnected()
{
    bool _isThere = client.isConnected();
    return _isThere;
}
//--------------------------------------------------------------
void ofxOPC::tryConnecting()
{
    if (!client.isConnected())
    {
        if (connectionAttempts <= 3)
        {
            if(timer >= endTime && !tryReconnecting)
            {
                tryReconnecting = true;
                ofLog(OF_LOG_NOTICE, "Trying to Reconnect: Attempt "+ofToString(connectionAttempts));
                connect();
                startTime = ofGetElapsedTimeMillis();
                connectionAttempts++;
                tryReconnecting = false;
            }
            else
            {
                
            }
        }
    }
}
//--------------------------------------------------------------
void ofxOPC::retryConnecting()
{
    ofLog(OF_LOG_NOTICE, "Will Try Reconnecting");
    connectionAttempts = 0;
}
//--------------------------------------------------------------
void ofxOPC::connect()
{
    client.setup(_address, _port);
    ofLog(OF_LOG_NOTICE, "Opening Connection to Server @"+ofToString(_address)+ ":" + ofToString(_port));
    if (client.isConnected()) {
        ofLog(OF_LOG_NOTICE, "Connected to Server @"+ofToString(_address)+ ":" + ofToString(_port));
    }
}
//--------------------------------------------------------------
void ofxOPC::close()
{
    disconnect();
    ofLog(OF_LOG_NOTICE, "Closing Connection @"+ofToString(_address)+ ":" + ofToString(_port));
}
//--------------------------------------------------------------
void ofxOPC::disconnect()
{
    client.close();
    cleanup();
}
