#pragma once
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
