package dcfmodel.simulations;
//
// This module for station//////////////////////////////////////
//
simple dcfModule
{
    parameters:
        double difs = 0.000050;
        double sifs = 0.000010;
        double slot = 0.000050;
        double txop = 0.1;
        int CWmin = 4;
        int CWmax = 10;
        int numberStations = 10; // for initialization
        @signal[arrival](type="long");
        @signal[beginTransmitSignal](type="simtime_t");
        @signal[endTransmitSignal](type="simtime_t");
        @signal[dropTransmitSignal](type="simtime_t");
        @display("i=block/source");
        @statistic[beginTransmitSignal](title="average miss time"; source="beginTransmitSignal"; record=vector,stats; interpolationmode=none);
    	@statistic[endTransmitSignal](title="average transmission time"; source="endTransmitSignal"; record=vector,stats; interpolationmode=none);
    	@statistic[dropTransmitSignal](title="average life time"; source="dropTransmitSignal"; record=vector,stats; interpolationmode=none);
    gates:
        input in @directIn;
        inout inn;
}
//
// This module for traffic generator of station ///////////////////
//
simple trafficGenerator
{
    parameters:
        int numberStations = 10;// for initialization
        @display("i=block/queue");
    gates:
        inout out;
}
//
// Let's merge them! //////////////////////////////////////////////
//
module dcfSimple
{
    @display("i=device/accesspoint");
    submodules:
        dcfModule: dcfModule;
        trafficGenerator: trafficGenerator;
    connections:
        trafficGenerator.out <--> dcfModule.inn;
}
// And describe Wi-Fi network!
network Network
{
    submodules:
        station[10]: dcfSimple {
            @display("p=100,100,ri,100,100");
        }
}

        //{
        	//@display("p=100,100,ri,100,100");
        	//@statistic[n_errors](title="errors number"; source="arrival"; record=vector,stats; interpolationmode=none);
        //}
//st[10]: dcfModule{
        //	@display("p=100,100,ri,100,100");
        //	@statistic[n_errors](title="errors number"; source="arrival"; record=vector,stats; interpolationmode=none);
        //}
        //queue[10]: trafficGenerator{
        //	@display("p=0,0,ri,200,200");
        //}  
        //st1: dcfModule;
        //st2: dcfModule;
        //st3: dcfModule {
        //    @display("p=350,86");
        //}
        //st4: dcfModule {
        //    @display("p=403,177");
        //}
        //st5: dcfModule;
        //st6: dcfModule {
        //    @display("p=112,242");
       // }
       // st7: dcfModule;
       // st8: dcfModule {
        //    @display("p=73,138");
        //}
        //st9: dcfModule;
        //st10: dcfModule;
