#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <algorithm>

using namespace std;

template <typename key, typename value>
class elem {
private:
    key k;
    value v;
    int freq;

public:
    elem(key _k, value _v, int _freq) : k(_k), v(_v), freq(_freq) {}
    //get funcs
    key getKey() const { return k; }
    value getVal() const { return v; }
    int getFreq() const { return freq; }
    //set funcs
    key& setKey() { return k; }
    value& setVal() { return v; }
    int& setFreq() { return freq; }
};

template <typename key, typename value>
class lfu_cache {
private:
    using ItType = typename list<elem<key, value>>::iterator;
    unordered_map<key, ItType> mp;
    list<elem<key, value>> lst;
    size_t cache_sz;

public:
    lfu_cache(size_t _cache_sz = 3) : cache_sz(_cache_sz) {}

    value get(key index) {
        /* check if index in cache
        if not throw error
        if yes then increment freq_mp and return
        */
        auto map_it = mp.find(index);
        if (map_it == mp.end()) throw range_error("No such key in the cache");
        else {
            elem<key, value>& obj = *(map_it->second);
            obj.setFreq()++;
            return obj.getVal();
        }
    }

    void put(key index, value val) {
        /* check if index in cache - if so then update (and iterate freq)
        if not then: 
        check if cache is max size - if so then run replacement policy so find min freq
        lst node and remove
        add new tup with freq 1
        if not max cache size then just add new val normally
        */
        auto map_it = mp.find(index);
        if (map_it != mp.end()) {
            elem<key, value>& obj = *(map_it->second);
            obj.setVal() = val;
            obj.setFreq()++;
        }
        else if (lst.size() == cache_sz) {
            auto min_func = [] (const elem<key, value>& curr, const elem<key, value>& smallest) {
                return curr.getFreq() < smallest.getFreq();
            };
            //iterator of min node in lst
            auto min_it = min_element(lst.begin(), lst.end(), min_func);
            mp.erase(min_it->getKey());
            lst.erase(min_it);
            lst.push_front(elem<key, value>(index, val, 1));
            mp[index] = lst.begin();
        }
        else {
            lst.push_front(elem<key, value>(index, val, 1));
            mp[index] = lst.begin();
        }
    }

    void display() {
        //iterate through list and output kv pair and freq
        for (const auto& obj: lst) {
            cout<<"Key is: "<<obj.getKey()<<", Value is: "<<obj.getVal()<<", Freq is: "<<obj.getFreq()<<endl;
        }
    }
};

int main() {

    lfu_cache<int, char> c1;

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