//cache with randomized replacement policy
#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <random>
#include <vector>

using namespace std;

template <typename key, typename value>

class rr_cache {
private:
    using kvpair = pair<key, value>;
    using ItType = typename list<kvpair>::iterator;
    unordered_map<key, ItType> mp;
    list<kvpair> lst;
    vector<ItType> vec; //stores vector of lst nodes' iterators
    size_t cache_sz;

public:
    rr_cache(size_t _cache_sz = 3) : cache_sz(_cache_sz) {}

    value get(const key& index) {
        //check if index exists in cache
        typename unordered_map<key, ItType>::iterator map_it = mp.find(index);
        if (map_it == mp.end()) throw range_error("No such key in the cache");
        else {
            return map_it->second->second;
        }
    }

    void put(const key& index, const value& val) {
        /* check if index exists in cache - if so then update val in place
        if not then check if cache_sz is reached - if so then get randomized iterator from vec and delete it
        if less than cache_sz then add new kvpair normally
        */
        typename unordered_map<key, ItType>::iterator map_it = mp.find(index);
        if (map_it != mp.end()) {
            map_it->second->second = val;
            return;
        }
        else if (lst.size() == cache_sz) {
            //get random iterator to node to evict
            ItType evict_it = rand_it();
            cout<<"Evicting key: "<<evict_it->first<<endl;
            mp.erase(evict_it->first);
            lst.erase(evict_it);
        }
        //add new element
        lst.emplace_front(index, val);
        vec.push_back(lst.begin());
        mp[index] = lst.begin();
        return;
    }

    ItType rand_it() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distribution(0, vec.size() - 1);
        int randomIndex = distribution(gen);
        return vec[randomIndex];
    }

    void display() {
        for (auto [k, v]: lst) {
            cout<<"Key is: "<<k<<", Value is: "<<v<<endl;
        }
    }
};

int main() {

    rr_cache<int, char> c1;

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
    cout<<"Final display"<<endl;
    c1.display();

    return 0;
}