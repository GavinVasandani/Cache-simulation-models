//Builder design pattern
//Create a director that takes in builder obj and has preset function calls on builder
//Create builder abstract class which has methods and member var but result is abstract
//Create carBuilder and CarManualBuilder which inherits from builder and adds extra member var

//create car class and manual class (maybe inherit due to sharing some common things)
//create interfaces to set member variable values in car and manual classes (get, set funcs)
//builder is just interface so just declare function prototypes (abstract classes, which we have to implement in carbuilder and man builder - have to do this as don't have car, manual member var)
//create carbuilder and carmanualbuilder to implement these funcs and assign values to car and manual member vars.

#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <algorithm>

using namespace std;

enum engineType {_4_cyl, _6_cyl, _8_cyl};

//unnecessary to make vars in private as anyways car obj in builder will be in private and only way to create class obj is via builder
class car {
public:
    int seats;
    engineType engine;
    bool tripComp;
    bool gps;
    
    car(int _seats = 0, engineType _engine = _4_cyl, bool _tripComp = false, bool _gps = false) : 
    seats(_seats), engine(_engine), tripComp(_tripComp), gps(_gps) {} //def constructor - not sure if even needed
};

class manual {
public:
    int seats;
    engineType engine;
    bool tripComp;
    bool gps;
    
    manual(int _seats = 0, engineType _engine = _4_cyl, bool _tripComp = false, bool _gps = false) : 
    seats(_seats), engine(_engine), tripComp(_tripComp), gps(_gps) {} //def constructor - not sure if even needed
};

//create builder interface
class builder {
public:
    //no member var so just use def constructor
    builder() {}

    //interface:
    virtual void setSeats(int num) = 0;
    virtual void setEngine(engineType eng) = 0;
    virtual void setTripComp() = 0;
    virtual void setGPS() = 0;
    virtual void reset() = 0;
};

class carBuilder: public builder {
public:
    //constructor
    carBuilder() { //builder def constructor is automatically called
        Car = new car();
        val = 5;
    }

    //virtual func implementations
    virtual void setSeats(int num) override {
        Car->seats = num;
    }
    virtual void setEngine(engineType eng) override {
        Car->engine = eng;
    }
    virtual void setTripComp() override {
        Car->tripComp = true;
    }
    virtual void setGPS() override {
        Car->gps = true;
        cout<<"Val: "<<val<<endl;
    }
    virtual void reset() override {
        Car = new car();
    }

    void test() {
        cout<<"Testing!"<<endl;
    }

    car* getResult() {
        string engStr;
        switch (Car->engine) {
            case _4_cyl:
                engStr = "4_cyl";
                break;
            case _6_cyl:
                engStr = "6_cyl";
                break;
            case _8_cyl:
                engStr = "8_cyl";
                break;
            default:
                engStr = "unknown";
                break;
        }
        cout<<"Car has: Seats: "<<Car->seats<<", Engine: "<<engStr<<", TripComp: "<<Car->tripComp<<", GPS: "<<Car->gps<<endl;
        return Car;
    }

    ~carBuilder() {
        delete Car;
    }

private:
    car* Car;
    int val;
};

class carManualBuilder: public builder {
public:
    //constructor
    carManualBuilder() { //builder def constructor is automatically called
        Manual = new manual();
    }

    //virtual func implementations
    virtual void setSeats(int num) override {
        Manual->seats = num;
    }
    virtual void setEngine(engineType eng) override {
        Manual->engine = eng;
    }
    virtual void setTripComp() override {
        Manual->tripComp = true;
    }
    virtual void setGPS() override {
        Manual->gps = true;
    }
    virtual void reset() override {
        Manual = new manual();
    }

    manual* getResult() {
        return Manual;
    }

    ~carManualBuilder() {
        delete Manual;
    }

private:
    manual* Manual;
};

class director {
private:
    static director* instance;
    director() {}

public:
    static director* getInstance() {
        if (instance == nullptr) {
            instance = new director();
        }
        return instance;
    }

    void makeSUV(builder* obj) {
        obj->setSeats(4);
        obj->setEngine(_4_cyl);
        obj->setTripComp();
        obj->setGPS();
    }

    void makeSportsCar(builder* obj) {
        obj->setSeats(2);
        obj->setEngine(_8_cyl);
        obj->setTripComp();
        obj->setGPS();
    }
};

director* director::instance = nullptr;

int main() {

    director* dir = director::getInstance();
    carBuilder* carObj = new carBuilder();
    dir->makeSportsCar(carObj);
    car* c = carObj->getResult();

    carBuilder inst;
    inst.getResult();

    builder& cst = inst;

    cst.setGPS();

    carBuilder testInst;
    builder& testObj = testInst;
    testObj.setGPS();

    builder* testPntr = new carBuilder();
    testPntr->setGPS();

    /*
    cst.setSeats(5);
    inst.getResult();
    */

    return 0;
}