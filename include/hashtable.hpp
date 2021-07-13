#include <cassert>
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

typedef bool hashmode_t;
#define hash0 false
#define hash1 true


template <typename TKey, typename TValue>
class HashTable {
    private:
        size_t count;
        size_t size;
        size_t partial_sz;
        element_t<TKey, TValue> *elements_0;
        element_t<TKey, TValue> *elements_1;

        u_int32_t (*h0)(TKey);
        u_int32_t (*h1)(TKey);
        size_t max_itr;


        element_t<TKey, TValue> *operate(TKey key, Operation op)
        {
            auto idx0 = hash(key, hash0);
            auto idx1 = hash(key, hash1);
            element_t<TKey, TValue> *temp0 = &(elements_0[idx0]);
            element_t<TKey, TValue> *temp1 = &(elements_1[idx1]);
            element_t<TKey, TValue> *element = nullptr;

            if (temp0->key == key && temp0->status == ES_POPULATED) element = temp0;
            if (temp1->key == key && temp1->status == ES_POPULATED) element = temp1;

            if (op == O_ACCESS) return element;

            if (op == O_DELETE && element) {
                element->status = ES_DELETED;
                return nullptr;
            }

            return element;
        }


        element_t<TKey, TValue> *operate(TKey key, Operation op, TValue value)
        {
            auto idx0 = hash(key, hash0);
            auto idx1 = hash(key, hash1);
            element_t<TKey, TValue> *temp0 = &(elements_0[idx0]);
            element_t<TKey, TValue> *temp1 = &(elements_1[idx1]);
            element_t<TKey, TValue> *element = nullptr;

            if (temp0->key == key && temp0->status == ES_POPULATED) element = temp0;
            if (temp1->key == key && temp1->status == ES_POPULATED) element = temp1;

            if (!element && op == O_INSERT) {
                // if either of the possible spots are free, then all is good!
                // just stick the new value into there. "Free" means either
                // empty, or deleted
                if (temp0->status != ES_POPULATED) {
                    temp0->key = key;
                    temp0->value = value;
                    temp0->status = ES_POPULATED;
                    count++;
                    return temp0;
                }
                else if (temp1->status != ES_POPULATED) {
                    temp1->key = key;
                    temp1->value = value;
                    temp1->status = ES_POPULATED;
                    count++;
                    return temp1;
                }

                // if not, evict temp1 back to the first array, evicting elements
                // to their alternate array as needed until the insert succeeds,
                // or the iteration limit is reached.

            }
        }


        u_int32_t hash(TKey key, hashmode_t mode)
        {
            u_int64_t hashed = mode ? h1(key) : h0(key);

            return (hashed * size) >> 32;
        }


        void initialize(size_t init_size, u_int32_t (*hfunc0)(TKey),
                u_int32_t (*hfunc1)(TKey), size_t maxitr)
        {
            size = init_size;
            partial_sz = init_size / 2;
            h0 = hfunc0;
            h1 = hfunc1;
            max_itr = maxitr;

            elements_0 = new element_t<TKey, TValue>[partial_sz];
            elements_1 = new element_t<TKey, TValue>[partial_sz];
            for (size_t i=0; i<partial_sz; i++) {
                elements_0[i] = element_t<TKey, TValue> {0, 0, ES_EMPTY};
                elements_1[i] = element_t<TKey, TValue> {0, 0, ES_EMPTY};
            }

            count = 0;
        }


        void resize()
        {
            count = 0;

            auto old_array0 = elements_0;
            auto old_array1 = elements_1;

            elements_0 = new element_t<TKey, TValue>[size];
            elements_1 = new element_t<TKey, TValue>[size];


            for (size_t i=0; i<size; i++) {
                elements_0[i] = element_t<TKey, TValue> {0, 0, ES_EMPTY};
                elements_1[i] = element_t<TKey, TValue> {0, 0, ES_EMPTY};
            }

            // yes, it's silly. I need to put a little more thought
            // into this. The compiler will do strength reduction automatically
            // on these anyway, right? Is it even worth it to store the half
            // size somewhere, compared to just doing size / 2?
            size_t old_sz = size;
            size = size * 2;
            partial_sz = size / 2;

            for (size_t i=0; i<old_sz; i++) {
                if (old_array0[i].status == ES_POPULATED)
                    operate(old_array0[i].key, O_INSERT, old_array0[i].value);

                if (old_array1[i].status == ES_POPULATED)
                        operate(old_array1[i].key, O_INSERT, old_array1[i].value);
            }


            delete old_array0;
            delete old_array1;
        }


    public:
        HashTable<TKey, TValue>(u_int32_t (*hfunc)(TKey))
        {
            initialize(1024, hfunc, .75);
        }


        HashTable<TKey, TValue>(size_t init_size, u_int32_t (*hfunc)(TKey))
        {
            initialize(init_size, hfunc, .75);
        }



        HashTable<TKey, TValue>(size_t init_size, u_int32_t (*hfunc)(TKey), size_t maxitr)
        {
            initialize(init_size, hfunc, maxitr);
        }


        HashTable<TKey, TValue>(size_t init_size)
        {
            initialize(init_size, nullptr, .75);
        }


        HashTable<TKey, TValue>()
        {
            initialize(1024, nullptr, .75);
        }


        element_t<TKey, TValue> *insert(TKey key, TValue value)
        {
            if (load_factor() > max_itr) resize();

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


        size_t length() { return count; }


        double load_factor() {return (double) count / (double) size; }


        ~HashTable()
        {
            delete elements_0;
            delete elements_1;
        }
};
