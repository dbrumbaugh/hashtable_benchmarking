#pragma once
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "arraystack.hpp"
#include "element.hpp"
#include "hashmode.hpp"


enum Operation {
    O_INSERT,
    O_ACCESS,
    O_DELETE
};


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

            // Check if either of the two elements matches the one we are
            // interested in.
            if (temp0->key == key && temp0->status == ES_POPULATED) element = temp0;
            if (temp1->key == key && temp1->status == ES_POPULATED) element = temp1;

            // if we are inserting, and element is still null, then we must
            // actually perform the insert.
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

                // if not, evict temp1 back to the first array, evicting
                // elements to their alternate array as needed until the insert
                // succeeds, or the iteration limit is reached.
                auto aux_stack = new ArrayStack<TKey, TValue>(max_itr);
                hashmode_t hfunc = hash0;
                element_t<TKey, TValue> *evicted = temp1;

                while (evicted->status == ES_POPULATED && aux_stack->has_space()) {
                    size_t idx = hash(evicted->key, hfunc);
                    aux_stack->push(evicted, hfunc, idx);

                    evicted = (hfunc == hash0) ? &(elements_0[idx]) : &(elements_1[idx]);
                    hfunc = hash_toggle(hfunc);
                }

                // if the stack is full, we have reached out maximum iteration
                // count and so the insert fails. Return null to indicate this.
                if (!aux_stack->has_space()) {
                    return nullptr;
                }

                // perform all of the table evictions
                while (!aux_stack->is_empty()) {
                    auto current = aux_stack->pop();
                    if (current.hmode == hash0) {
                        elements_0[current.idx].key = current.elem->key;
                        elements_0[current.idx].value = current.elem->value;
                        elements_0[current.idx].status = ES_POPULATED;
                    } else {
                        elements_1[current.idx].key = current.elem->key;
                        elements_1[current.idx].value = current.elem->value;
                        elements_1[current.idx].status = ES_POPULATED;
                    }
                }

                // after which, the desired spot should be free to place the
                // inserted element.
                temp1->key = key;
                temp1->value = value;
                temp1->status = ES_POPULATED; // should always be the case, so
                                              // this is likely unneeded
                count++;

                element = temp1;
            }

            return element;
        }


        size_t hash(TKey key, hashmode_t mode)
        {
            u_int64_t hashed = mode ? h1(key) : h0(key);

            return hashed % partial_sz;
//            return (hashed * partial_sz) >> 32;
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
            size_t old_sz = partial_sz;
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
        HashTable<TKey, TValue>(size_t init_size, u_int32_t (*hfunc0)(TKey),
                                u_int32_t (*hfunc1)(TKey), size_t maxitr)
        {
            initialize(init_size, hfunc0, hfunc1, maxitr);
        }


        HashTable<TKey, TValue>(u_int32_t (*hfunc0)(TKey),
                                u_int32_t (*hfunc1)(TKey), size_t maxitr)
        {
            initialize(1024, hfunc0, hfunc1, maxitr);
        }


        HashTable<TKey, TValue>(size_t init_size, u_int32_t (*hfunc0)(TKey),
                                u_int32_t (*hfunc1)(TKey))
        {
            initialize(init_size, hfunc0, hfunc1, 16);
        }


        HashTable<TKey, TValue>(u_int32_t (*hfunc0)(TKey),
                                u_int32_t (*hfunc1)(TKey))
        {
            initialize(1024, hfunc0, hfunc1, 16);
        }


        element_t<TKey, TValue> *insert(TKey key, TValue value)
        {
            element_t<TKey, TValue> *elem;
            do {
                elem = operate(key, O_INSERT, value);
                if (!elem) resize();
            } while (!elem);

            return elem;
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
