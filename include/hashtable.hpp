#include <cstdlib>
#include <cstdio>

enum Operation {
    O_INSERT,
    O_ACCESS,
    O_DELETE
};


enum EStatus {
    ES_EMPTY,
    ES_POPULATED,
    ES_DELETED
};


template <typename TKey, typename TValue>
struct alignas(64) element_t {
    TKey key;
    TValue value;
    EStatus status;
};


template <typename TKey, typename TValue>
class HashTable {
    private:
        int count;
        int size;
        int stride;
        element_t<TKey, TValue> *elements;

        u_int32_t (*h0)(TKey);
        double max_lf;


        element_t<TKey, TValue> *operate(TKey key, Operation op)
        {
            auto idx = hash(key);

            element_t<TKey, TValue> *temp = &elements[idx];

            // TODO: This will classify a deleted element as "found" still,
            // but I want to get other stuff working first, before I deal with
            // that edge case
            while (temp->key != key && temp->status != ES_EMPTY) {
                idx = (idx + stride) % size;
                temp = &(elements[idx]);
            }

            if (temp->status == ES_EMPTY || temp->status == ES_DELETED)
                temp = nullptr;

            if (op == O_ACCESS)
                return temp;

            // We need the tombstones to still count for load_factor calculation,
            // so don't decrement the count variable here.
            if (op == O_DELETE && temp) {
                temp->status = ES_DELETED;
            }

            return nullptr;
        }


        element_t<TKey, TValue> *operate(TKey key, Operation op, TValue value)
        {
            auto idx = hash(key);
            element_t<TKey, TValue> *temp = &(elements[idx]);


            while (temp->key != key && temp->status != ES_EMPTY) {
                idx = (idx + stride) % size;
                temp = &(elements[idx]);
            }

            if (op == O_INSERT) {
                if (temp->status == ES_EMPTY) {
                    temp->key = key;
                    temp->value = value;
                    temp->status = ES_POPULATED;
                    count++;
                } 

                return temp;
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


        void initialize(int init_size, u_int32_t (*hfunc)(TKey), double max_loadfactor, int strd=1)
        {
            size = init_size;
            h0 = hfunc;
            max_lf = max_loadfactor;
            stride = strd;

            elements = new element_t<TKey, TValue>[size];
            for (int i=0; i<size; i++) {
                elements[i] = element_t<TKey, TValue> {0, 0, ES_EMPTY};
            }

            count = 0;
        }


        void resize()
        {
            int old_size = size;
            size = size * 2;

            count = 0;

            auto old_array = elements;
            elements = new element_t<TKey, TValue>[size];
            for (int i=0; i<size; i++) {
                elements[i] = element_t<TKey, TValue> {0, 0, ES_EMPTY};
            }

            for (int i=0; i<old_size; i++) {
                if (old_array[i].status == ES_POPULATED) {
                    operate(old_array[i].key, O_INSERT, old_array[i].value);
                }
            }

            delete old_array;
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


        HashTable<TKey, TValue>(int init_size, u_int32_t (*hfunc)(TKey), int strd)
        {
            initialize(init_size, hfunc, .75, strd);
        }


        HashTable<TKey, TValue>(int init_size, u_int32_t (*hfunc)(TKey), int strd, double maxlod)
        {
            initialize(init_size, hfunc, maxlod, strd);
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
                total += elements[i].count;
            }

            return total / (double) size;
        }


        ~HashTable()
        {
            delete elements;
        }
};
