class LFUCache {
public:
//for direct access to position in deque that contains key so we can easily delete, use list and iterator
    using it = list<int>::iterator;
    using tp = tuple<int, int, it>;
    unordered_map<int, tp> cache; //key, tuple<val, freq, it>
    map<int, list<int>> lfu; //freq, key
    int cache_sz;

    LFUCache(int capacity) {
        cache_sz = capacity;
    }

    void update_freq(tp& tup, int key) {
        int& val = get<0>(tup);
        int& freq = get<1>(tup);
        auto& iter = get<2>(tup);
        //remove key from current freq list
        lfu[freq].erase(iter);
        if (lfu[freq].empty()) lfu.erase(freq);
        //add to new freq deq at the front
        lfu[++freq].push_front(key);
        //update iter
        iter = lfu[freq].begin();
    }

    int get(int key) {
        auto cache_it = cache.find(key);
        if (cache_it == cache.end()) return -1;
        else {
            update_freq(cache_it->second, key);
            return get<0>(cache_it->second);
        }
    }
    
    void put(int key, int value) {
        auto cache_it = cache.find(key);
        if (cache_it != cache.end()) {
            //update freq
            update_freq(cache_it->second, key);
            //update val
            get<0>(cache_it->second) = value;
            return;
        }
        if (cache.size() == cache_sz) {
            //get begin of map, and remove end of lst
            auto& min_freq_lst = lfu.begin()->second;
            auto min_key = min_freq_lst.back();
            min_freq_lst.pop_back();
            if (min_freq_lst.empty()) lfu.erase(lfu.begin());
            cache.erase(min_key);
        }
        //add new value
        lfu[1].push_front(key);
        cache[key] = make_tuple(value, 1, lfu[1].begin());
    }
};

//could you use an unordered_map<key, pair<value, freq>>
//and map<freq, deque<keys>
//so get normally but update freq and move key from old freq to new freq (add to begin of new freq deq)
//when setting then also update freq, if need to evict then find begin() (min val) remove end (as that's LRU)
//and add new key, val, freq

/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */