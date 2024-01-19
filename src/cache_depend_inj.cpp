#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;

//want to dependency inject the replacement policy
//create replacement policy interface
//so just do normal direct-mapped cache with integer index/key
template <typename elem, typename value> //lst element so either <key, pair<value, freq> or <key, value
class policy {
public:
    using ItType = typename list<elem>::iterator;
    virtual void updateOnAccess(unordered_map<int, ItType>& cache, list<elem>& lst, int& index) = 0;
    virtual void updateOnEvict(unordered_map<int, ItType>& cache, list<elem>& lst) = 0;
    //adding is specific to policy also (add with freq 1 for LFU, add to front of list for LRU)
    virtual void addData(unordered_map<int, ItType>& cache, list<elem>& lst, int& index, value& val) = 0;
    virtual value loadData(unordered_map<int, ItType>& cache, list<elem>& lst, int& index) = 0; //if elem is <key, value> or <key, <value, freq>> then access to get value is different
    virtual void updateValue(unordered_map<int, ItType>& cache, list<elem>& lst, int& index, value& val) = 0;
    virtual ~policy() = default;
};

template <typename elem, typename value>
class lru : public policy<elem, value> {
public:
    using ItType = typename list<elem>::iterator;
    void updateOnAccess(unordered_map<int, ItType>& cache, list<elem>& lst, int& index) override {
        //update priority
        auto cache_it = cache.find(index);
        ItType it = cache_it->second;
        elem data = *it;

        //remove from current place in lst and put in front
        lst.erase(it);
        lst.push_front(data);
        cache[index] = lst.begin();
    }

    void updateOnEvict(unordered_map<int, ItType>& cache, list<elem>& lst) override {
        //so max cache size reached so need to evict data
        //for lru remove end value
        int key = lst.rbegin()->first;
        lst.pop_back();
        cache.erase(key);
    }

    void addData(unordered_map<int, ItType>& cache, list<elem>& lst, int& index, value& val) override {
        lst.emplace_front(index, val);
        cache[index] = lst.begin();
    }

    value loadData(unordered_map<int, ItType>& cache, list<elem>& lst, int& index) override {
        return cache.find(index)->second->second;
    }

    void updateValue(unordered_map<int, ItType>& cache, list<elem>& lst, int& index, value& val) override {
        //given index, update its value in place
        auto cache_it = cache.find(index);
        cache_it->second->second = val; 
    }
};

template <typename elem, typename value>
class lfu : public policy<elem, value> {
public:
    using ItType = typename list<elem>::iterator;
    void updateOnAccess(unordered_map<int, ItType>& cache, list<elem>& lst, int& index) override {
        //update freq
        auto cache_it = cache.find(index);
        ItType it = cache_it->second;
        elem& data = *it;
        (data.second).second++; //updates freq in place
    }

    void updateOnEvict(unordered_map<int, ItType>& cache, list<elem>& lst) override {
        //so max cache size reached so need to evict data
        //for lfu find min freq value and evict
        auto min_func = [] (elem& curr, elem& small) {
            return curr.second.second < small.second.second;
        };
        auto min_it = min_element(lst.begin(), lst.end(), min_func); 
        elem data = *min_it;
        cache.erase(data.first);
        lst.erase(min_it);
    }

    void addData(unordered_map<int, ItType>& cache, list<elem>& lst, int& index, value& val) override {
        elem data = make_pair(index, make_pair(val, 1));
        lst.push_front(data);
        cache[index] = lst.begin();
    }

    value loadData(unordered_map<int, ItType>& cache, list<elem>& lst, int& index) override {
        return (cache.find(index)->second->second).first;
    }

    void updateValue(unordered_map<int, ItType>& cache, list<elem>& lst, int& index, value& val) override {
        //given index, update its value in place
        auto cache_it = cache.find(index);
        (cache_it->second->second).first = val;
    }
};

//create cache interface
template <typename value> //so elem is lst element either: key,value or key, value, freq
class cache_base {
public:
    virtual value load(int index) = 0;
    virtual void store(int index, value val) = 0;
    virtual ~cache_base() = default;
};

template <typename elem, typename value>
class cache_type : public cache_base<value> {
private:
    using ItType = typename list<elem>::iterator;
    unordered_map<int, ItType> cache;
    list<elem> lst;
    size_t cache_sz;
    policy<elem, value>* rp; //replacement policy dependency injection

public:
    cache_type(size_t capacity, policy<elem, value>* _rp) : cache_sz(capacity), rp(_rp) {}

    value load(int index) override {
        auto cache_it = cache.find(index);
        if (cache_it == cache.end()) {
            cout<<"No such key in cache."<<endl;
            return value{0};
        }
        else {
            cout<<"Found key!"<<endl;
            rp->updateOnAccess(cache, lst, index);
            return rp->loadData(cache, lst, index);
        }
    }

    void store(int index, value val) override {
        auto cache_it = cache.find(index);
        if (cache_it != cache.end()) {
            rp->updateValue(cache, lst, index, val);
            rp->updateOnAccess(cache, lst, index);
            return;
        }
        else if (cache.size() == cache_sz) {
            cout<<"Evicting!"<<endl;
            rp->updateOnEvict(cache, lst);
        }
        cout<<"Adding: "<<val<<", at index: "<<index<<endl;
        rp->addData(cache, lst, index, val);
    }

    ~cache_type() override{
        delete rp;
    }
};

int main() {

    //create lru cache
    //with key (int) and value (string) aka index (int) and data (string)
    lru<pair<int, string>, string>* LRUpntr = new lru<pair<int, string>, string>();
    cache_type<pair<int, string>, string> LRUcache(3, LRUpntr);

    LRUcache.store(1, "str1");
    LRUcache.store(2, "str2");
    LRUcache.store(4, "str4");
    LRUcache.load(2);
    LRUcache.store(5, "str5");
    LRUcache.load(1);

    lfu<pair<int, pair<string, int>>, string>* LFUpntr = new lfu<pair<int, pair<string, int>>, string>();
    cache_type<pair<int, pair<string, int>>, string> LFUcache(3, LFUpntr);

    LFUcache.store(1, "str1");
    LFUcache.store(2, "str2");
    LFUcache.store(4, "str4");
    LFUcache.load(2);
    LFUcache.load(4);
    LFUcache.store(5, "str5");
    LFUcache.load(1);

    return 0;
}