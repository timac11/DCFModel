//////////////////////////////////////////////////////////////////////
// This module is included for statistic collection //////////////////
//////////////////////////////////////////////////////////////////////
#include <omnetpp.h>
#include "dcf_m.h"
#include <list>
using namespace omnetpp;

class statisticModule : public omnetpp::cSimpleModule {
    public:
        virtual ~statisticModule()
        {
            //cancelAndDelete(timeout);
        }
    private:
        omnetpp::simtime_t allTime;         // simulation time;
        omnetpp::simtime_t loadTime;        // loading time, while some_one transmit messages
        omnetpp::simtime_t freeTime;        // noone transmit messages
        omnetpp::simtime_t collisionTime;   // time is spent for collision transmission
        omnetpp::simtime_t successfulTime;  // time is spent for succesful transmission
        /////////////////////////////////////////////////////////////////////////////////////////
        int queueLength;                             // length of the queue during time
        std::list<omnetpp::simtime_t> missTime;     // from generation to the begining of transmission
        std::list<omnetpp::simtime_t> delayTime;    // from generation to the end of transmission
        std::list<omnetpp::simtime_t> packetTime;   // from getting to end of transmission
    protected:

};
Define_Module(statisticModule);
