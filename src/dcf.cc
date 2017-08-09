#include <omnetpp.h>
#include "dcf_m.h"
#include <list>

using namespace omnetpp;

class dcfModule : public omnetpp::cSimpleModule {
    public:
        virtual ~dcfModule()
        {
            cancelAndDelete(timeout);
        }

    protected:
        virtual void initialize()
        {
            //arrivalSignal = registerSignal("arrival");
            beginTransmitSignal = registerSignal("beginTransmitSignal");
            endTransmitSignal = registerSignal("endTransmitSignal");
            dropTransmitSignal = registerSignal("dropTransmitSignal");
            difs = par("difs").doubleValue();
            slot = par("slot").doubleValue();
            globalTxopTime = par("txop").doubleValue();
            cw_min = static_cast<int>(par("CWmin").longValue());
            cw_max = static_cast<int>(par("CWmax").longValue());
            sifs = par("sifs").doubleValue();
            timeout = new omnetpp::cMessage("timeout!");
            cw = cw_min;
            //scheduleAt(simTime() + difs, timeout); // during regime with queue is not unnecessary
            state = IDLE; //in the begining chanel is idle
            n_errors = 0;
            data_frame = nullptr;
            queueLength.setName("Queue length");
            queueLength.setRangeAutoUpper(0, 100, 3);
            generationBegin.setName("Miss time in queue");
            generationBegin.setRangeAutoUpper(0, 100, 2);
            generationEnd.setName("Time in system");
            generationEnd.setRangeAutoUpper(0, 100, 2);
            queueEnd.setName("Get from queue to end");
            queueEnd.setRangeAutoUpper(0, 100, 2);
            //EDCA////////////////////////////////////////////////////////
            currentTxopTime = 0;
            sendPacketsTxop = 0;
        }

        virtual void handleMessage(omnetpp::cMessage *msg)
        {
            if (msg == timeout)
            {  //queueLength.collect(10);

                if (state == DIFS)
                {
                    backoff = intuniform(0, pow(2, cw) - 1);
                    if (backoff == 0)
                    {
                        state = DATA;
                        if (!dynamic_cast<DataFrame*>(data_frame)->getRetransmitted())
                        {
                            emit(beginTransmitSignal, simTime() - dynamic_cast<DataFrame*>(data_frame)->getGenerationTime()); // emit signal for begin transmission
                            dynamic_cast<DataFrame*>(data_frame)->setRetransmitted(true);
                        }
                        sendData(data_frame);
                        //sendEdcaData(data_frame);
                    }
                    else
                    {
                        state = BACKOFF;
                        chanelFree += 1;
                        scheduleAt(simTime() + slot, timeout);
                    }
                }
                else if (state == BACKOFF)
                {
                    backoff--;
                    if (backoff <= 0)
                    {
                        state = DATA;
                        //emit(beginTransmitSignal, simTime() - dynamic_cast<DataFrame*>(data_frame)->getGenerationTime()); // emit signal for begin transmission
                        if (!dynamic_cast<DataFrame*>(data_frame)->getRetransmitted())
                        {
                            emit(beginTransmitSignal, simTime() - dynamic_cast<DataFrame*>(data_frame)->getGenerationTime()); // emit signal for begin transmission
                            generationBegin.collect(simTime() - dynamic_cast<DataFrame*>(data_frame)->getGenerationTime());
                            dynamic_cast<DataFrame*>(data_frame)->setRetransmitted(true);
                        }
                        sendData(data_frame);
                        //sendEdcaData(data_frame);
                    }
                    else
                    {
                        scheduleAt(simTime() + slot, timeout);
                    }
                }
                else if (state == DATA)
                {
                    /* handle data transmission finished, start waiting SIFS + eps */
                    state = SIFS;
                    scheduleAt(simTime() + sifs * 1.1, timeout);
                }
                else if (state == SIFS)
                {
                    /* no ACK received, failed to send data */
                    ///////////statistic//////////////////////////////////////////////////////////////
                    queueLength.collect(traffic.size());
                    dynamic_cast<DataFrame*>(data_frame)->setCollisionTime(simTime() - dynamic_cast<DataFrame*>(data_frame)->getQueueTime());
                    //    collisionTime += simTime() - dynamic_cast<DataFrame*>(data_frame)->getQueueTime();

                    //////////////////////////////////////////////////////////////////////////////////
                    state = DIFS;
                    n_errors++;
                    getParentModule()->bubble("collision");
                    traffic.push_front(data_frame);
                    this->increaseCw();
                    scheduleAt(simTime() + difs, timeout);
                    /* keep current frame, so no call to getNextDataFrame() here */
                }
                else if (state == ACK)
                {
                    ///////////////////////////////STATISTIC////////////////////////////////////////////////////////
                    emit(endTransmitSignal, simTime() - dynamic_cast<DataFrame*>(data_frame)->getGenerationTime());
                    generationEnd.collect(simTime() - dynamic_cast<DataFrame*>(data_frame)->getGenerationTime());
                    emit(dropTransmitSignal, simTime() - dynamic_cast<DataFrame*>(data_frame)->getQueueTime());
                    queueEnd.collect(simTime() - dynamic_cast<DataFrame*>(data_frame)->getQueueTime());
                    successTime += simTime() - dynamic_cast<DataFrame*>(data_frame)->getQueueTime();
                    collisionTime += dynamic_cast<DataFrame*>(data_frame)->getCollisionTime();
                    ///////////////////EDCA IMPLEMENTATION//////////////////////////////////////////////////////////
                    if(currentTxopTime < globalTxopTime)
                    {
                        if(!traffic.empty())
                        {
                            currentTxopTime += 0.0003 + sifs*1.1;
                            state = DATA;
                            data_frame = this->getCurrentDataFrame();
                            sendEdcaData(data_frame);
                        }
                        else
                        {
                            currentTxopTime = 0;
                            state = IDLE;
                            cw = cw_min;
                        }
                    }
                    else
                    {
                    ///////////////////DCF IMPLEMENTATION////////////////////////////////////////////////////////////

                    if(!traffic.empty())
                    {
                        currentTxopTime = 0;
                        state = DIFS;
                        data_frame = this->getCurrentDataFrame();
                        scheduleAt(simTime() + difs, timeout);
                    }
                    else
                    {
                        state = IDLE; // without timeouts, station is empty and states alone :(
                    }
                    cw = cw_min;
                    ////////////////////////////////////////////////DELETE this bracket |
                    }
                }
                else if(state == IDLE)
                {
                    cancelEvent(timeout);
                }
                else if (state == BACKOFF_WAIT_RECEIVE || state == DIFS_WAIT_RECEIVE || state == IDLE_WAIT_RECEIVE)
                {
                    if(receive_data_frame!=nullptr)
                    if(getReceiveDataId(receive_data_frame) == getId())
                    {
                        AckFrame *ack = new AckFrame();
                        ack->setFrameDuration(0.00002);
                        ackTime += ack->getFrameDuration();
                        sendDirect(ack, getSimulation()->getModule(getSendDataId(receive_data_frame->dup())), "in");
                        receive_data_frame = nullptr;
                    }

                    if(state == BACKOFF_WAIT_RECEIVE)
                    {
                        state = BACKOFF;
                        scheduleAt(simTime() + slot, timeout);
                    }
                    else if(state == DIFS_WAIT_RECEIVE)
                    {
                        state = DIFS;
                        scheduleAt(simTime() + difs, timeout);
                    }
                    else
                    {
                        if(traffic.empty())
                        {
                            state = IDLE;
                            scheduleAt(simTime() + slot, timeout);
                        }
                        else
                        {
                            state = DIFS;
                            scheduleAt(simTime() + difs, timeout);
                        }
                    }
                }
                else
                {
                    throw omnetpp::cRuntimeError("unexpected timeout in state = %d", state);
                }
            }
            else
            {
                /* If here, some foreign message came in! */
                if (isDataFrame(msg))
                {
                    if (state == BACKOFF || state == DIFS || state == IDLE)
                    {
                        if(timeout->isScheduled())
                        {
                            cancelEvent(timeout); // no timeout now!
                        }
                        if(state == BACKOFF) state = BACKOFF_WAIT_RECEIVE;
                        if(state == DIFS) state = DIFS_WAIT_RECEIVE;
                        if(state == IDLE) state = IDLE_WAIT_RECEIVE;
                    }
                    else if (state == SIFS || state == ACK)
                    {
                        /* this is strange, but... treat this as a collision */
                        cancelEvent(timeout);
                        state = DIFS_WAIT_RECEIVE;
                        n_errors += 1;
                        collisionTime += simTime() - dynamic_cast<DataFrame*>(data_frame)->getQueueTime();
                        //emit(arrivalSignal, n_errors);
                        getParentModule()->bubble("collision");
                        //EV << n_errors << endl;
                    }
                    if(state != DATA)
                    {
                        //cancelEvent(timeout);
                        if(!timeout->isScheduled())
                        {
                            scheduleAt(simTime() + getDataDuration(msg), timeout); // for EDCA all stations schedule at txop
                            if(getReceiveDataId(msg)==getId())
                            {
                                receive_data_frame = msg;
                                EV << receive_data_frame << endl;
                            }
                            else
                            {
                                delete msg;
                            }
                        }
                    }
                }
                else if (isAckFrame(msg))
                {
                    if (state == SIFS)
                    {
                        cancelEvent(timeout);
                        scheduleAt(simTime(), timeout);
                        state = ACK;
                    }
                    else
                    {
                        delete msg;
                    }
                }
                else if (isQueueFrame(msg))
                {
                    msg->setKind(KIND_DATA); //change kind of packet after getting from generator
                    traffic.push_back(msg);
                    if (state == IDLE)
                    {
                        state = DIFS;
                        if(!timeout->isScheduled())
                        {
                            scheduleAt(simTime() + difs, timeout);
                        }
                        data_frame = getCurrentDataFrame();
                    }
                    else if (state == IDLE_WAIT_RECEIVE)
                    {
                        state = DIFS_WAIT_RECEIVE;
                        data_frame = getCurrentDataFrame();
                    }
                    queueLength.collect(traffic.size());
                }
            }
        }

        void sendData(omnetpp::cMessage *data_frame)
        {
            ASSERT2(!timeout->isScheduled(), "Sending data while timeout is already scheduled");
            if (isDataFrame(data_frame))
            {
                state = DATA;
            }
            cModule * parModule = getParentModule()->getParentModule();
            EV<< *parModule <<endl;
            for (cModule::SubmoduleIterator it(parModule); !it.end(); it++)
            {
              cModule *submodule = *it;
              //EV<< *submodule <<endl;
              cModule *dcfSimple = submodule->getSubmodule("dcfModule");
              //EV<< *dcfSimple <<endl;
              if (dcfSimple->getId() != getId())
              sendDirect(data_frame->dup(), dcfSimple, "in");
            }
            scheduleAt(simTime() + getDataDuration(data_frame), timeout);
            attemptsToSend += 1;
        }

        void sendEdcaData(omnetpp::cMessage *data_frame)
        {
            if(currentTxopTime < globalTxopTime)
            {
                sendData(data_frame);
                currentTxopTime += getDataDuration(data_frame);
            }
            else
            {
                currentTxopTime = 0;
                state = DIFS;
                scheduleAt(simTime() + difs, timeout);
                cw = cw_min;
            }
        }

        virtual omnetpp::simtime_t getDataDuration(omnetpp::cMessage *msg)
        {
            return dynamic_cast<Frame*>(msg)->getFrameDuration();
        }

        virtual void increaseCw()
        {
            if(cw < cw_max)
                cw++;
        }

        virtual int getSendDataId(omnetpp::cMessage *msg)
        {
            return dynamic_cast<Frame*>(msg)->getSendId();
        }

        virtual int getReceiveDataId(omnetpp::cMessage *msg)
        {
            return dynamic_cast<Frame*>(msg)->getReceiveId();
        }

        omnetpp::cMessage *getCurrentDataFrame()
        {
            DataFrame *msg;
            msg = dynamic_cast<DataFrame*>(traffic.front()->dup());
            msg->setQueueTime(simTime());
            traffic.pop_front();
            queueLength.collect(traffic.size());
            return msg;
        }

        bool isDataFrame(omnetpp::cMessage *msg)
        {
            return msg->getKind() == KIND_DATA;
        }

        bool isAckFrame(omnetpp::cMessage *msg)
        {
            return msg->getKind() == KIND_ACK;
        }

        bool isQueueFrame(omnetpp::cMessage *msg)
        {
            return msg->getKind() == KIND_QUEUE;
        }

        int getNetworkStations()
        {
            int number = 0;
            cModule * parModule = getParentModule();
            for (cModule::SubmoduleIterator it(parModule); !it.end(); it++)
            {
                number++;
            }
            return number;
        }

    virtual void finish(){
        EV<<"Collision probability= "<< static_cast<double>(n_errors)/static_cast<double>(attemptsToSend)<<endl;
        EV<<"Simulation time= " << simTime() << endl;
        EV<<"Channel is free during= "<<chanelFree*slot<< endl;
        EV<<"Collision time= "<< collisionTime<< endl;
        EV<<"Success time= "<< successTime<< endl;
        EV<<"ACK time= "<< ackTime<< endl;
        EV<<"##########################" <<endl;
    }
    private:
        omnetpp::simtime_t difs;
        omnetpp::simtime_t sifs;
        omnetpp::simtime_t slot;
        int backoff = 0;
        int cw_min = 0;
        int cw_max = 0;
        int cw = 0;
        int n_errors = 0;
        int srcAdress = 0;
        int chanelFree = 0;
        omnetpp::cMessage *timeout = nullptr;
        omnetpp::cMessage *data_frame = nullptr;
        omnetpp::cMessage *receive_data_frame = nullptr;
        //simsignal_t arrivalSignal;
        //trafficGenerator* generator;
        std::list<omnetpp::cMessage*> traffic;  //empty queue
        int attemptsToSend = 0;                 // according this variables probability of collision is determined
        enum State {
            IDLE,
            BUSY,
            DIFS,
            BACKOFF,
            DATA,
            SIFS,
            ACK,
            BACKOFF_WAIT_RECEIVE,
            DIFS_WAIT_RECEIVE,
            IDLE_WAIT_RECEIVE
        };
        State state = IDLE;
        /////////STATISTIC VARIABLES///////
        omnetpp::simtime_t collisionTime;
        omnetpp::simtime_t successTime;
        omnetpp::simtime_t ackTime;
        simsignal_t beginTransmitSignal;
        simsignal_t endTransmitSignal;
        simsignal_t dropTransmitSignal;
        cLongHistogram queueLength;
        cDoubleHistogram generationBegin;
        cDoubleHistogram generationEnd;
        cDoubleHistogram queueEnd;
        /////////PARAMETERS FOR EDCA///////
        omnetpp::simtime_t currentTxopTime;
        omnetpp::simtime_t globalTxopTime;
        int sendPacketsTxop;
};
Define_Module(dcfModule);

