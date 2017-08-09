/*
 * trafficGenerator.cpp
 *
 *  Created on: 14 θών. 2017 γ.
 *      Author: aser 2014
 */
#include <omnetpp.h>
#include "dcf_m.h"
#include <queue>
using namespace omnetpp;

class trafficGenerator : public omnetpp::cSimpleModule {
    public:
        virtual ~trafficGenerator()
        {
            cancelAndDelete(timeout);
        }
    private:
        const double MAX_PACKET_SIZE = 0.001; // data from Bianchi article
        const double MAX_GENERATION_DELAY = 0.1;
        bool isPassive = false; // variable for recognize regime of traffic work
        int stationsNumber;
        int currentStation;
        omnetpp::cMessage* currentFrame = nullptr;
        std::queue<DataFrame*> traffic; //empty queue
        omnetpp::cMessage *timeout = nullptr;
    protected:
        virtual void initialize()
        {
            currentStation = getParentModule()->getSubmodule("dcfModule")->getId();
            EV<< *getParentModule()<<endl;
            EV<< currentStation<<endl;
            stationsNumber = 10;
            timeout = new omnetpp::cMessage("generate new package!");
            getNextTimeSendPacketToQueue();//new omnetpp::cMessage("new Packet is sent from queue!");
        }

        virtual void handleMessage(omnetpp::cMessage *msg)
        {
            if(msg == timeout)
            {
                generateNewPacket();
                send(currentFrame->dup(), "out$o");
                delete currentFrame;
                getNextTimeSendPacketToQueue();
            }
        }

        virtual void getNextTimeSendPacketToQueue()
        {
            double time = uniform(0, MAX_GENERATION_DELAY); //time before generation new packet
            scheduleAt(simTime() + time, timeout);
            EV <<"current time:"<< time<< endl;
            EV << simTime()<<endl;
        }

        virtual void generateNewPacket()
        {
            DataFrame* frame = new DataFrame();
            frame->setFrameDuration(uniform(0, MAX_PACKET_SIZE));
            frame->setKind(KIND_QUEUE);
            frame->setSendId(currentStation);
            frame->setGenerationTime(simTime());
            int receiveStation = 2*intuniform(6,15); // DELETE HARDCODE!!!
            while (receiveStation == currentStation)
            {
                receiveStation = 2*intuniform(6,15); // DELETE HARDCODE!!!
            }
            frame->setReceiveId(receiveStation);
            currentFrame = frame;
            //return frame;
        }

        virtual omnetpp::cMessage* getCurrentFrame() {
            return currentFrame;
        }
////////////////////That is used for load regime/////////////////////////////////////////////////////

};
Define_Module(trafficGenerator);
