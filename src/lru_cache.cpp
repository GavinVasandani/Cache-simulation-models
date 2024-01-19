#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <utility>
#include <vector>
#include <type_traits>

using namespace std;

//creating custom hash for typename key
template <typename key>
struct hash {
    constexpr auto operator() (const key& k) -> size_t {
        return k<<1;
    }
};

template <typename key, typename value>
class lru_cache {
private:
    using kvpair = pair<key, value>;
    using ItType = typename list<kvpair>::iterator;
    unordered_map<key, ItType> mp;
    list<kvpair> lst; //used to implement LRU policy
    size_t cache_sz;
    mutex m;

public:
    //constructor - using default values makes this constructor replace default constructor
    lru_cache(size_t _cache_sz = 3) : cache_sz(_cache_sz) {}

    //get
    value get(const key& index) {
        //add lock
        m.lock();
        auto map_it = mp.find(index); //iterator to kv pair at position index
        if (map_it == mp.end()) throw range_error("No such key in the cache");

        //else as it exists, remove from position in list and put in front (indicating max priority)
        ItType& lst_it = map_it->second;
        kvpair return_pair = *lst_it;

        //to update priority so update pos of element in list and the iterator to the element in list in mp
        lst.erase(lst_it);
        lst.push_front(return_pair);
        lst_it = lst.begin();

        cout<<"[GET] Found key: "<<index<<endl;

        //unlock
        m.unlock();
        return return_pair.second;
    }

    //put
    void put(const key& index, const value& val) {
        //add lock
        m.lock();
        //if index exists in mp then just update val and make high priority
        auto map_it = mp.find(index);
        if (map_it != mp.end()) {
            lst.erase(map_it->second);
        }
        else if (mp.size() == cache_sz) {
            //need to do replacement policy - remove last element from lst
            kvpair lru_pair = *lst.rbegin();
            lst.pop_back();
            mp.erase(lru_pair.first);
        }
        lst.emplace_front(index, val);
        mp[index] = lst.begin();
        cout<<"[PUT] Added key: "<<index<<" Value is: "<<val<<endl;
        //unlock
        m.unlock();
        return;
    }

    //outputs contents and priority of cache
    void display() {
        //add lock
        m.lock();
        int i = 0;
        for (auto [index, val]: lst) {
            cout<<"Key is: "<<index<<", Value is: "<<val<<", Priority is: "<<++i<<endl;
        }
        //unlock
        m.unlock();
    }

    static auto test() -> void {
        cout<<"Static - obj of class isn't needed!"<<endl;
    }

};

int main() {

    //lru_cache<int, char>::test();

    //shared mem
    lru_cache<int, char> c1;

    auto run_put = [&c1] (auto k, auto v) {
        c1.put(k, v);
    };

    auto run_get = [&c1] (auto k) {
        c1.get(k);
    };

    //create vector of threads
    auto t1 = thread(run_put, 1, 'A');
    auto t2 = thread(run_put, 2, 'B');
    auto t3 = thread(run_put, 3, 'C');
    auto t4 = thread(run_get, 2);
    auto t5 = thread(run_put, 4, 'G');
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    c1.display();

    
    //auto t1 = jthread(c1.put(), 1, 'A');
    //auto t3 = jthread(c1.display());
    //t1.join();
    //t3.join();
    //use join after initializing all threads such that main thread waits for all threads to finish their execution, if no join then main thread finishes and terminates before smaller threads can even
    //execute their function call in their thread, and if main thread goes out of scope then all threads die
    

    return 0;
}