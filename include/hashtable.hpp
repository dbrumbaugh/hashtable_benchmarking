#include <cstdlib>
#include <cstdio>

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

        element_t<TKey, TValue> *operate(TKey key, Operation op)
        {
            int idx = hash(key);
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
                free(temp);

                count--;
            }

            return empty;
        }

        element_t<TKey, TValue> *operate(TKey key, Operation op, TValue value) 
        {
            int idx = hash(key);
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
                    return new_elem;
                } else {
                    return temp;
                }
            }

            return nullptr;
        }


        int hash(TKey key)
        {

            return key % size;

            /*
            u_int64_t hval = 0;
            char *temp = (char *) &key;

            for (u_int32_t i=0; i<sizeof(TKey); i++) {
                hval += *(temp + i);
            }

            hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
            hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
            hval = (hval >> 16) ^ hval;

            return (int) (hval * size) >> 32;
            */
        }


    public:
        HashTable<TKey, TValue>(int init_size=1024) {
            buckets = (bucket_t<TKey, TValue> *) malloc(init_size * sizeof(bucket_t<TKey, TValue>));
            for (int i=0; i<init_size; i++) {
                buckets[i] = bucket_t<TKey, TValue> {0, nullptr};
            }

            size = init_size;
            count = 0;
        }


        element_t<TKey, TValue> *insert(TKey key, TValue value) {
            return operate(key, O_INSERT, value);
        }


        element_t<TKey, TValue> *access(TKey key) {
            return operate(key, O_ACCESS);
        }


        void remove(TKey key) {
            operate(key, O_DELETE);
        }

        int length() { return count; }
};
