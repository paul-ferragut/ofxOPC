//
//  ofxOPC.cpp
//  openPixelControl
//
//  Created by David Haylock on 25/05/2014.
//
#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxNeoPixels.h"



//------------------------------------------------------------------------------
typedef struct OPCPacket_Header {
    // Standard OPC-packet header
    uint8_t channel;
    uint8_t command;
    uint16_t data_length;
} OPCPacket_Header_t;
//------------------------------------------------------------------------------
typedef struct OPCPacket_SPCData {
    // OPC "Set Pixel Colours" data structure
    unsigned char r;
    unsigned char g;
    unsigned char b;
} *OPCPacket_SPCData_t;
//------------------------------------------------------------------------------
typedef struct OPCPacket_SEData {
    // OPC "System Exlcusive" data structure
    uint16_t system_id;
    uint16_t command_id;
    unsigned char payload[];
} *OPCPacket_SEData_t;
//------------------------------------------------------------------------------
typedef struct OPCPacket {
    // An amalgamation of a header and a data-section
    OPCPacket_Header_t header;
    unsigned char data[];
} *OPCPacket_t;

//--------------------------------------------------------------
class ofxOPC  {
    
    public:
        void setup(string address,int port,int fadeCandyNumber);
        void update();
        void draw();
    
        void cleanup();
        void close();
        bool isConnected();
        void tryConnecting();
        void retryConnecting();
        void sendFirmwareConfigPacket(); // Not used
    
        // For writing custom channels
        void writeChannel(uint8_t channel, vector <ofColor> pix);

        string _address;
        int _port;
        int _w,_h;

    private:
    
        void connect();
        void disconnect();
    
        int channelNumber;
    
        // For sending our data packets out to the Server
        ofxTCPClient client;
    
        // Reconnection Stuff
        float timer;
        bool tryReconnecting;
        float startTime;
        float endTime;
        int connectionAttempts;
    
        // Data Packets
        OPCPacket_t OPC_SPC_packet;
        size_t OPC_SPC_packet_length;
        OPCPacket_SPCData_t OPC_SPC_packet_data;
    

};