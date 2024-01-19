#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <algorithm>
#include <chrono>

using namespace std;

//lst node element
template <typename key, typename value>
class elem {
private:
    key k;
    value v;
    chrono::time_point<chrono::steady_clock> curr_time;
public:
    elem(const key& _k, const value& _v) : k(_k), v(_v) {
        curr_time = chrono::steady_clock::now();
    }
    //get funcs
    key getKey() const { return k; }
    value getVal() const { return v; }
    auto getTime() const { return curr_time; }
    //set funcs
    key& setKey() { return k; }
    value& setVal() { return v; }
    auto& setTime() { return curr_time; }
};

template <typename key, typename value>
class ttl_cache {
private:
    using ItType = typename list<elem<key, value>>::iterator;
    list<elem<key, value>> lst;
    unordered_map<key, ItType> mp;
    size_t cache_sz;
    double stale_time; //if cache is older than this then its considered stale and is removed

public:
    ttl_cache(size_t _cache_sz = 3, double _stale_time = 50000) : cache_sz(_cache_sz), stale_time(_stale_time) {}

    value get(const key& index) {
        //check if index exists in cache
        auto map_it = mp.find(index);
        if (map_it == mp.end()) throw out_of_range("Index doesn't exist in cache.");
        else {
            //return value, also update timestamp (curr_time) and push to front of lst
            elem<key, value> e = *(map_it->second);
            lst.erase(map_it->second);
            e.setTime() = chrono::steady_clock::now();
            lst.push_front(e);
            mp[index] = lst.begin();
            return e.getVal();
        }
    }

    void put(const key& index, const value& val) {
        /* if exists in cache then update val and time and push to front of lst and update mp iterator
        if cache_sz limit reached then check if last val time diff > stale time, if true then start erasing all values that exceed range
        if not true then just evict last val in lst (lru)
        if cache_sz limit not reached then just add to lst normally
        */
        auto now_time = chrono::steady_clock::now();
        auto map_it = mp.find(index);
        if (map_it != mp.end()) {
            lst.erase(map_it->second);
        }
        else if (lst.size() == cache_sz) {
            elem<key, value> e = *lst.rbegin();
            auto time_diff = chrono::duration_cast<chrono::nanoseconds>(now_time - e.getTime());
            if (time_diff.count() >= stale_time) {
                remove_stale(now_time);
            }
            else {
                //just do lru
                mp.erase(lst.rbegin()->getKey());
                lst.pop_back();
            }
        }
        //add new elem to map and lst and put timestamp
        lst.push_front(elem<key, value> (index, val));
        mp[index] = lst.begin();
        return;
    }

    void remove_stale(chrono::time_point<chrono::steady_clock> now_time) {
        //from end of lst to begin, checks if time diff is > stale_time, if it is then erase from mp and lst
        for (auto it = lst.rbegin(); it != lst.rend(); it--) {
            auto time_diff = chrono::duration_cast<chrono::nanoseconds>(now_time - it->getTime());
            if (time_diff.count() >= stale_time) {
                cout<<"EVICT. Diff is: "<<time_diff.count()<<endl;
                mp.erase(it->getKey());
                lst.pop_back();
            }
            else {
                return;
            }
        }
    }

    void display() {
        for (const auto& e: lst) {
            auto now_time = chrono::steady_clock::now();
            auto time_val = chrono::duration_cast<chrono::nanoseconds>(now_time-e.getTime());
            cout<<"Key is: "<<e.getKey()<<", Value is: "<<e.getVal()<<", Time is: "<<time_val.count()<<endl;
        }
    }
};

int main() {

    ttl_cache<int, char> c1;

    c1.put(1, 'G');
    c1.put(2, 'B');
    c1.put(3, 'C');
    c1.get(2);
    c1.display();
    c1.put(6, 'Z');
    cout<<"After put(6, 'Z')"<<endl;
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