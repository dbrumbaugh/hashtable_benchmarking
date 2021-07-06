#include <cstdlib>

enum Operation {
    O_INSERT,
    O_ACCESS,
    O_DELETE
};


template <typename TKey, typename TValue>
struct alignas(64) element_t {
    TKey key;
    TValue value;
    struct element_t<TKey, TValue> *next;
};


template <typename TKey, typename TValue>
struct bucket_t {
    int count;
    element_t<TKey, TValue> *first;
};


template <typename TKey, typename TValue>
class HashTable {
    private:
        int count;
        int size;
        bucket_t<TKey, TValue> *buckets;
        element_t<TKey, TValue> *empty;

        u_int32_t (*h0)(TKey);
        double max_lf;


        element_t<TKey, TValue> *operate(TKey key, Operation op)
        {
            auto idx = hash(key);
            element_t<TKey, TValue> *temp=nullptr, *prev=nullptr;

            for (temp=buckets[idx].first; temp; temp=temp->next) {
                if (temp->key == key)
                    break;
                prev = temp;
            }

            if (op == O_ACCESS)
                return temp;

            if (op == O_DELETE && temp) {
                if (prev)
                    prev->next = temp->next;
                else
                    buckets[idx].first = temp->next;
                delete temp;

                buckets[idx].count--;
                count--;
            }

            return empty;
        }


        element_t<TKey, TValue> *operate(TKey key, Operation op, TValue value)
        {
            auto idx = hash(key);
            element_t<TKey, TValue> *temp=nullptr, *prev=nullptr;

            for (temp=buckets[idx].first; temp; temp=temp->next) {
                if (temp->key == key)
                    break;
                prev = temp;
            }

            if (op == O_INSERT) {
                if (!temp) {
                    auto new_elem = new element_t<TKey, TValue> {key, value, nullptr};
                    if (prev) {
                        prev->next = new_elem;
                    } else {
                        buckets[idx].first = new_elem;
                    }

                    count++;
                    buckets[idx].count++;
                    return new_elem;
                } else {
                    return temp;
                }
            }

            return nullptr;
        }


        u_int32_t hash(TKey key)
        {
            u_int64_t hashed;

            if (h0) {
                hashed = h0(key);
            } else {
                hashed = (u_int64_t) key;
            }

            return (hashed * size) >> 32;
        }


        void initialize(int init_size, u_int32_t (*hfunc)(TKey), double max_loadfactor)
        {
            size = init_size;
            h0 = hfunc;
            max_lf = max_loadfactor;

            buckets = new bucket_t<TKey, TValue>[size];
            for (int i=0; i<size; i++) {
                buckets[i] = bucket_t<TKey, TValue> {0, nullptr};
            }

            count = 0;
        }


        void resize()
        {
            int old_size = size;
            size = size * 2;

            count = 0;

            auto old_array = buckets;

            buckets = new bucket_t<TKey, TValue>[size];

            for (int i=0; i<size; i++) {
                buckets[i] = bucket_t<TKey, TValue> {0, nullptr};
            }

            for (int i=0; i<old_size; i++) {
                element_t<TKey, TValue> *prev = nullptr;
                for (auto temp = old_array[i].first; temp; temp = temp->next) {
                    delete prev;
                    operate(temp->key, O_INSERT, temp->value);
                    prev = temp;
                }
            }
        }


    public:
        HashTable<TKey, TValue>(u_int32_t (*hfunc)(TKey))
        {
            initialize(1024, hfunc, .75);

        }


        HashTable<TKey, TValue>(int init_size, u_int32_t (*hfunc)(TKey))
        {
            initialize(init_size, hfunc, .75);
        }


        HashTable<TKey, TValue>(int init_size)
        {
            initialize(init_size, nullptr, .75);
        }


        HashTable<TKey, TValue>()
        {
            initialize(1024, nullptr, .75);
        }


        element_t<TKey, TValue> *insert(TKey key, TValue value)
        {
            if (load_factor() > max_lf) resize();

            return operate(key, O_INSERT, value);
        }


        element_t<TKey, TValue> *access(TKey key)
        {
            return operate(key, O_ACCESS);
        }


        void remove(TKey key)
        {
            operate(key, O_DELETE);
        }


        int length() { return count; }


        double load_factor() {return (double) count / (double) size; }


        double average_chain()
        {
            double total = 0;
            for (int i=0; i<size; i++) {
                total += buckets[i].count;
            }

            return total / (double) size;
        }


        ~HashTable()
        {
            for (int i=0; i<size; i++) {
                element_t<TKey, TValue> *prev = nullptr;
                for (auto temp=buckets[i].first; temp; temp=temp->next) {
                    delete prev;
                    prev = temp;
                }
            }

            delete buckets;
        }
};
