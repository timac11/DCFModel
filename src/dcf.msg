//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
enum FrameKinds {
    KIND_DATA = 100;
    KIND_ACK = 101;
    KIND_QUEUE = 102;
}
packet Frame {
    simtime_t frameDuration;
    simtime_t collisionTime = 0;
    int retransmittedTrying = 0;
    int sendId;
    int receiveId;
    simtime_t generationTime;   	//	time of generation
    simtime_t queueTime;	//	statistic
    simtime_t endTransmitTime;	    //	statistic
    bool retransmitted = false;
}
packet DataFrame extends Frame {
    kind = KIND_DATA;
}
packet AckFrame extends Frame {
    kind = KIND_ACK;
}
packet QueueFrame extends Frame {
	kind = KIND_QUEUE;
}