#pragma once
#include "hashtable.hpp"
#include <stdexcept>
#include "element.hpp"
#include "hashmode.hpp"



template <typename TKey, typename TValue>
struct cuckoo_stackelem_t {
    element_t<TKey, TValue> *elem;
    hashmode_t hmode;
    size_t idx;
};


template <typename TKey, typename TValue>
class ArrayStack {
    private:
        size_t len;
        size_t maxlen;
        cuckoo_stackelem_t<TKey, TValue> *data;
    public:
        ArrayStack(size_t size)
        {
            len = 0;
            maxlen = size;

            data = new cuckoo_stackelem_t<TKey, TValue>[size];
        }

        cuckoo_stackelem_t<TKey, TValue> pop()
        {
            if (len > 0) {
                auto pval = data[--len];
                return pval;
            }

            throw std::out_of_range("Pop from empty stack.");
        }


        void push(element_t<TKey, TValue> *elem, hashmode_t hmode, size_t idx)
        {
            if (len == maxlen) throw std::out_of_range("Push to full stack!");
            data[len].elem = elem;
            data[len].hmode = hmode;
            data[len].idx = idx;
            len++;
        }


        bool has_space()
        {
            return len < maxlen;
        }


        bool is_empty()
        {
            if (len == 0) return true;
            return false;
        }


        size_t length()
        {
            return len;
        }


        ~ArrayStack()
        {
            delete data;
        }

};
