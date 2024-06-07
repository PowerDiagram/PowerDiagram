#pragma once

#include "metil_namespace.h" // IWYU pragma: export
#include <utility> // IWYU pragma: export

// shortcuts for template<...>
#define     TAUV                          template<class... A,class U,class V>
#define     TUVW                          template<class U,class V,class W>
#define     TTij                          template<class T,int i,int j>
#define     TUVi                          template<class U,class V,int i>
#define     Tijs                          template<int i,int... j>
#define     TTY                           template<template<typename...> class Y>
#define     TTA                           template<class T,class... A>
#define     TTI                           template<class T,std::size_t i>
#define     TTi                           template<class T,int i>
#define     TiT                           template<int i,class T>
#define     TUV                           template<class U,class V>
#define     TiA                           template<int i,class... A>
#define     Tij                           template<int i,int j>
#define     TSs                           template<CtStringValue... S>
#define     Tis                           template<int... i>
#define     TS                            template<CtStringValue S>
#define     TA                            template<class... A>
#define     TT                            template<class T>
#define     TU                            template<class U>
#define     Ti                            template<int i>

// type handling
#define     VALUE_IN_DECAYED_TYPE_OF( v ) std::decay_t<decltype( v )>::value
#define     CT_DECAYED_TYPE_OF( v )       METIL_NAMESPACE::CtType<std::decay_t<decltype( v )>>()
#define     STORAGE_TYPE_OF( v )          typename StorageTypeFor<decltype( v )>::value
#define     DECAYED_TYPE_OF( v )          std::decay_t<decltype( v )>
#define     IS_BASE_OF( A, V )            std::is_base_of_v<A,std::decay_t<V>>
#define     FORWARD( v )                  std::forward<decltype( v )>( v )

#define     FIRST_TYPE_IS_A_SPECIALIZATION_OF( TYPE_LIST, P ) ( []( auto &t, const auto &... ){ return CtInt<requires { t.~P(); }>(); } )( TYPE_LIST(), ... )
#define     IS_A_SPECIALIZATION_OF( S, P )                    requires ( S &t ) { t.~P(); }

//
#define     NUA                           [[no_unique_address]]
