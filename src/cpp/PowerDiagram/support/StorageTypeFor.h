#pragma once

#include "metil_namespace.h"
#include <string>

BEG_METIL_NAMESPACE

template<class T>
struct StorageTypeFor {
    using value = T;
};

// reference
template<class T> struct StorageTypeFor<const T *> { using value = StorageTypeFor<T *>::value; };
template<class T> struct StorageTypeFor<const T> { using value = StorageTypeFor<T>::value; };
template<class T> struct StorageTypeFor<T &&> { using value = StorageTypeFor<T>::value; };
template<class T> struct StorageTypeFor<T &> { using value = StorageTypeFor<T>::value; };

// strings
template<std::size_t n> struct StorageTypeFor<char[n]> { using value = std::string; };
template<> struct StorageTypeFor<char *> { using value = std::string; };

END_METIL_NAMESPACE
