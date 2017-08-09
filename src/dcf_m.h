//
// Generated file, do not edit! Created by nedtool 5.1 from dcf.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __DCF_M_H
#define __DCF_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0501
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>dcf.msg:15</tt> by nedtool.
 * <pre>
 * //
 * // This program is free software: you can redistribute it and/or modify
 * // it under the terms of the GNU Lesser General Public License as published by
 * // the Free Software Foundation, either version 3 of the License, or
 * // (at your option) any later version.
 * // 
 * // This program is distributed in the hope that it will be useful,
 * // but WITHOUT ANY WARRANTY; without even the implied warranty of
 * // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * // GNU Lesser General Public License for more details.
 * // 
 * // You should have received a copy of the GNU Lesser General Public License
 * // along with this program.  If not, see http://www.gnu.org/licenses/.
 * //
 * enum FrameKinds
 * {
 * 
 *     KIND_DATA = 100;
 *     KIND_ACK = 101;
 *     KIND_QUEUE = 102;
 * }
 * </pre>
 */
enum FrameKinds {
    KIND_DATA = 100,
    KIND_ACK = 101,
    KIND_QUEUE = 102
};

/**
 * Class generated from <tt>dcf.msg:20</tt> by nedtool.
 * <pre>
 * packet Frame
 * {
 *     simtime_t frameDuration;
 *     simtime_t collisionTime = 0;
 *     int retransmittedTrying = 0;
 *     int sendId;
 *     int receiveId;
 *     simtime_t generationTime;   	//	time of generation
 *     simtime_t queueTime;	//	statistic
 *     simtime_t endTransmitTime;	    //	statistic
 *     bool retransmitted = false;
 * }
 * </pre>
 */
class Frame : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::simtime_t frameDuration;
    ::omnetpp::simtime_t collisionTime;
    int retransmittedTrying;
    int sendId;
    int receiveId;
    ::omnetpp::simtime_t generationTime;
    ::omnetpp::simtime_t queueTime;
    ::omnetpp::simtime_t endTransmitTime;
    bool retransmitted;

  private:
    void copy(const Frame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Frame&);

  public:
    Frame(const char *name=nullptr, short kind=0);
    Frame(const Frame& other);
    virtual ~Frame();
    Frame& operator=(const Frame& other);
    virtual Frame *dup() const override {return new Frame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual ::omnetpp::simtime_t getFrameDuration() const;
    virtual void setFrameDuration(::omnetpp::simtime_t frameDuration);
    virtual ::omnetpp::simtime_t getCollisionTime() const;
    virtual void setCollisionTime(::omnetpp::simtime_t collisionTime);
    virtual int getRetransmittedTrying() const;
    virtual void setRetransmittedTrying(int retransmittedTrying);
    virtual int getSendId() const;
    virtual void setSendId(int sendId);
    virtual int getReceiveId() const;
    virtual void setReceiveId(int receiveId);
    virtual ::omnetpp::simtime_t getGenerationTime() const;
    virtual void setGenerationTime(::omnetpp::simtime_t generationTime);
    virtual ::omnetpp::simtime_t getQueueTime() const;
    virtual void setQueueTime(::omnetpp::simtime_t queueTime);
    virtual ::omnetpp::simtime_t getEndTransmitTime() const;
    virtual void setEndTransmitTime(::omnetpp::simtime_t endTransmitTime);
    virtual bool getRetransmitted() const;
    virtual void setRetransmitted(bool retransmitted);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Frame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Frame& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>dcf.msg:31</tt> by nedtool.
 * <pre>
 * packet DataFrame extends Frame
 * {
 *     kind = KIND_DATA;
 * }
 * </pre>
 */
class DataFrame : public ::Frame
{
  protected:

  private:
    void copy(const DataFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const DataFrame&);

  public:
    DataFrame(const char *name=nullptr, short kind=0);
    DataFrame(const DataFrame& other);
    virtual ~DataFrame();
    DataFrame& operator=(const DataFrame& other);
    virtual DataFrame *dup() const override {return new DataFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const DataFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, DataFrame& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>dcf.msg:34</tt> by nedtool.
 * <pre>
 * packet AckFrame extends Frame
 * {
 *     kind = KIND_ACK;
 * }
 * </pre>
 */
class AckFrame : public ::Frame
{
  protected:

  private:
    void copy(const AckFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const AckFrame&);

  public:
    AckFrame(const char *name=nullptr, short kind=0);
    AckFrame(const AckFrame& other);
    virtual ~AckFrame();
    AckFrame& operator=(const AckFrame& other);
    virtual AckFrame *dup() const override {return new AckFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const AckFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, AckFrame& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>dcf.msg:37</tt> by nedtool.
 * <pre>
 * packet QueueFrame extends Frame
 * {
 *     kind = KIND_QUEUE;
 * }
 * </pre>
 */
class QueueFrame : public ::Frame
{
  protected:

  private:
    void copy(const QueueFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const QueueFrame&);

  public:
    QueueFrame(const char *name=nullptr, short kind=0);
    QueueFrame(const QueueFrame& other);
    virtual ~QueueFrame();
    QueueFrame& operator=(const QueueFrame& other);
    virtual QueueFrame *dup() const override {return new QueueFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const QueueFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, QueueFrame& obj) {obj.parsimUnpack(b);}


#endif // ifndef __DCF_M_H
