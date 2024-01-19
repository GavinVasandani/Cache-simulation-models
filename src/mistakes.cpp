//unnecessary to make vars in private as anyways car obj in builder will be in private and only way to create class obj is via builder
class car {
public:
    car(int _seats = 0, engineType _engine = 4_cyl, bool _tripComp = false, bool _gps = false) : 
    seats(_seats), engine(_engine), tripComp(_tripComp), gps(_gps) {} //def constructor - not sure if even needed
    //get, set funcs:
    int getSeats() { return seats; }
    engineType getEngine() { return engine; }
    bool getTripComp() { return tripComp; }
    bool getGps() { return gps; }

    //set
    int& getSeats() { return seats; }
    engineType& getEngine() { return engine; }
    bool& getTripComp() { return tripComp; }
    bool& getGps() { return gps; }

private:
    int seats;
    engineType engine;
    bool tripComp;
    bool gps;
};
