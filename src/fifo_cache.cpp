#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <deque>

using namespace std;

template <typename key, typename value>

class fifo_cache {
private:
    using kvpair = pair<key, value>;
    using ItType = typename deque<kvpair>::iterator;
    unordered_map<key, ItType> mp;
    deque<kvpair> dq; //used to implement LRU policy
    size_t cache_sz;

public:
    //constructor - using default values makes this constructor replace default constructor
    fifo_cache(size_t _cache_sz = 3) : cache_sz(_cache_sz) {}

    //get
    value get(const key& index) {
        auto map_it = mp.find(index); //iterator to kv pair at position index
        if (map_it == mp.end()) throw range_error("No such key in the cache");

        return map_it->second->second;
    }

    //put
    void put(const key& index, const value& val) {
        //if index exists in mp then just update val and make high priority
        auto map_it = mp.find(index);
        if (map_it != mp.end()) {
            //if index exists in mp then reassign kvpair in dq with new val
            map_it->second->second = val;
            return;
        }
        else if (mp.size() == cache_sz) {
            //need to do replacement policy - remove last element from dq
            mp.erase(dq.rbegin()->first);
            dq.pop_back();
        }
        //add new element
        dq.push_front(make_pair(index, val));
        mp[index] = dq.begin();
        return;
    }

    //outputs contents and priority of cache
    void display() {
        int i = 0;
        for (auto [index, val]: dq) {
            cout<<"Key is: "<<index<<", Value is: "<<val<<", Old is: "<<++i<<endl;
        }
    }
};

int main() {

    //use default constructor
    fifo_cache<int, char> c1;

    c1.put(1, 'G');
    c1.put(2, 'B');
    c1.put(3, 'C');
    c1.display();
    c1.get(2);
    cout<<"After get(2)"<<endl;
    c1.display();
    //c1.get(4);
    c1.put(4, 'D');
    c1.get(4);
    cout<<"After get(4)"<<endl;
    c1.display();
    c1.put(3, 'E');
    c1.put(1, 'A');
    c1.display();

    return 0;
}