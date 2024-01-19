#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;

//create cache interface
template <typename T>
class cache_base {
public:
    virtual T load(int ind, uint64_t tag) = 0;
    virtual void store(int ind, uint64_t tag, T val) = 0;
};

template <typename T>
class lru_cache : public cache_base<T> {
private:
    using elem = tuple<int, uint64_t, T>; //so lst elem stores cache ind, tag, data
    using it = typename list<elem>::iterator;
    using cache_set = unordered_map<uint64_t, it>;
    unordered_map<int, cache_set> cache;
    vector<list<elem>> vec_lst;
    size_t set_limit; //so number of cache sets
    size_t blk_limit; //number of cache blks per set

public:
    lru_cache(size_t n, size_t k) : blk_limit(n), set_limit(k), vec_lst(k) {}

    T load(int ind, uint64_t tag) override { //load val from cache given cache set index and tag
        if (ind >= set_limit) {
            cout<<"Cache set index is out of range!"<<endl;
            return T{0};
        }

        //check if block exists in cache set ind
        cache_set& cs = cache[ind];
        auto cs_it = cs.find(tag);

        if (cs_it == cs.end()) {
            cout<<"Block not in cache!"<<endl;
            return T{0};
        }

        auto obj = *(cs_it->second);
        T data = get<2>(obj);

        //adjust priority on LRU list
        //so get correct LRU list for the current cache set
        auto& lst = vec_lst[ind];
        lst.erase(cs_it->second);
        lst.push_front(obj);
        cs[tag] = lst.begin();

        return data;
    }

    void store(int ind, uint64_t tag, T val) override { //want to store value at a certain cache set given by index ind
        //check if ind is in range
        if (ind >= set_limit) {
            cout<<"Cache set index is out of range!"<<endl;
            return;
        }

        cache_set& cs = cache[ind];
        auto& lst = vec_lst[ind];

        //check if tag exists in cache - so we're just updating value
        auto cs_it = cs.find(tag);
        if (cs_it != cs.end()) {
            lst.erase(cs_it->second);
        }
        else if (cs.size() == blk_limit) {
            //so must evict LRU
            auto lru = lst.back();
            cs.erase(get<1>(lru));
            lst.pop_back();
        }
        //if we have space and doesn't exist then just add new value
        lst.emplace_front(ind, tag, val);
        cs[tag] = lst.begin();
        return;
    }

    void display() {
        //display cache contents
        for (auto& [key, cs] : cache) {
            cout<<"Cache set ind: "<<key<<endl;
            for (auto& [tag, it] : cs) {
                cout<<"Cache tag is: "<<tag<<", Data stored is: "<<get<2>(*it)<<endl;
            }
        }
    }
};

int main() {

    lru_cache<string> c1 (3, 4);
    c1.store(0, 123123, "str1");
    c1.store(0, 232, "str2");
    c1.store(1, 94, "str3");
    c1.store(2, 414, "str4");
    cout<<c1.load(1, 94)<<endl;

    c1.display();

    return 0;
}