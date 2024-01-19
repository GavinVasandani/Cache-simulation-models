//Create a base class animal with certain member vars, constructor to assign member vars, constructor that takes in base class obj, pure virtual clone func - so base class is an interface (not concrete)
//Create derived classes with member vars, constructor, constructor that takes in derived class obj, clone func that runs copy constructor - concrete class
//Create an array of animals, create a copy array of animals without knowing actual elements' member vars
//only use virtual on the function we want to override its implementation in a derived class
//in the function in the derived class that overrides the virtual func we just use override keyword

#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <algorithm>
#include <vector>

using namespace std;

class loc {
public:
    virtual void location() = 0;
    virtual ~loc() = default;
};

class africa : public loc {
    //no member var so no explicit constructor needed
public:
    void location() override {
        cout<<"I am in Africa!"<<endl;
    }
};

class asia : public loc {
public:
    void location() override {
        cout<<" I am in Asia!"<<endl;
    }
};

class animal {
private:
    int age;
    bool g;
    string colour;

public:
    //constructor 1
    animal(int _age, bool _g, string _colour) : age(_age), g(_g), colour(_colour) {}

    //copy constructor
    animal(animal* source) {
        age = source->age;
        g = source->g;
        colour = source->colour;
    }

    //pure virtual clone func
    virtual animal* clone() = 0;

    void printAnim() {
        string gen = (g == 0) ? "Male" : "Female";
        cout<<"Age is: "<<age<<", Gender: "<<gen<<", Colour: "<<colour<<endl;
    }

    virtual void printVals() = 0;

    virtual void wherefound() = 0;

    virtual ~animal() = default;
};

class lion : public animal {
private:
    int paw_sz;
    int teeth_sz;
    loc* obj; //so independent on whether africa or asia and we inject the dependency

public:
    //constructor 1
    lion(int _age, bool _g, string _colour, int _paw_sz, int _teeth_sz, loc* _obj) : animal(_age, _g, _colour), paw_sz(_paw_sz), teeth_sz(_teeth_sz), obj(_obj) {}

    //copy constructor
    lion(lion* source) : animal(source) {
        paw_sz = source->paw_sz;
        teeth_sz = source->teeth_sz;
        obj = source->obj;
    }

    //clone func implementation
    animal* clone() override {
        return new lion(this);
    }

    void printVals() override {
        printAnim();
        cout<<"Paw_sz: "<<paw_sz<<", Teeth_sz: "<<teeth_sz<<endl;
    }

    void wherefound() override {
        obj->location();
    }

    ~lion() override {
        delete obj; //calls destructor of loc type (which is not virtual) so actual loc destructor called
    }
};

class bear: public animal {
private:
    int feet_sz;
    int ear_sz;
    loc* obj;

public:
    //constructor 1
    bear(int _age, bool _g, string _colour, int _feet_sz, int _ear_sz, loc* _obj) : animal(_age, _g, _colour), feet_sz(_feet_sz), ear_sz(_ear_sz), obj(_obj) {}

    //copy constructor
    bear(bear* source) : animal(source) {
        feet_sz = source->feet_sz;
        ear_sz = source->ear_sz;
        obj = source->obj;
    }

    //clone func implementation
    animal* clone() override {
        return new bear(this);
    }

    void printVals() override {
        printAnim();
        cout<<"Feet_sz: "<<feet_sz<<", Ear_sz: "<<ear_sz<<endl;
    }

    void wherefound() override {
        obj->location();
    }

    ~bear() override {
        delete obj;
    }
};

int main() {

    //Create an array of animals pntrs, create a copy array of animals pntr without knowing actual elements' member vars
    vector<animal*> vec;
    vec.push_back(new lion(5, 0, "brown", 2, 3, new africa()));
    vec.push_back(new bear(2, 1, "black", 22, 31, new africa()));
    vec.push_back(new bear(11, 0, "white", 19, 28, new asia()));
    vec.push_back(new lion(8, 0, "orange", 3, 4, new asia()));
    
    int lim = vec.size();
    for (int i=0; i<lim; i++) {
        vec.push_back(vec[i]->clone());
    }

    for (auto& obj: vec) {
        obj->printVals();
        obj->wherefound();
    }

    /*
    can't do: 
    animal obj = vec[0] as can't make animal object as animal is abstract class, so can only use as a type

    also this will run infinitely long as i<vec.size() is re-eval after every iteration but every iter vec.size() grows
    for (int i=0; i<vec.size(); i++) {
        vec.push_back(vec[i]->clone());
    }
    */

    return 0;
}