#pragma once

#include <utility>
#include "Opt.h"

BEG_METIL_NAMESPACE

#define DTP template<class T>
#define UTP Opt<T>

DTP const T &UTP::value() const {
    return *reinterpret_cast<const T *>( bytes );
}

DTP T &UTP::value() {
    return *reinterpret_cast<T *>( bytes );
}

DTP UTP::Opt( auto &&head, auto &&...tail ) {
    new ( bytes ) T( FORWARD( head ), FORWARD( tail )... );
    ok = true;
}

DTP UTP::Opt( const Opt<T> &that ) {
    if ( that.ok ) {
        new ( bytes ) T( that.value() );
        ok = true;
    } else
        ok = false;
}

DTP UTP::Opt( Opt<T> &&that ) {
    if ( that.ok ) {
        new ( bytes ) T( std::move( that.value() ) );
        that.value().~T();
        that.ok = false;
        ok = true;
    } else
        ok = false;
}

DTP UTP::Opt() : ok( false ) {
}

DTP UTP::~Opt() {
    if ( ok )
        value().~T();
}

DTP auto *UTP::display( DisplayItemFactory &ds ) const {
    if ( ok )
        return ds.new_display_item( value() );
    return ds.new_symbol( CtType<Opt>(), "none" );
}

DTP UTP &UTP::operator=( const Opt<T> &that ) {
    if ( that.ok ) {
        if ( ok ) {
            value() = that.value();
        } else {
            new ( bytes ) T( that.value() );
            ok = true;
        }
    } else {
        if ( ok ) {
            value().~T();
            ok = false;
        }
    }
    return *this;
}

DTP UTP &UTP::operator=( Opt<T> &&that ) {
    if ( that.ok ) {
        if ( ok ) {
            value() = std::move( that.value() );
        } else {
            new ( bytes ) T( std::move( that.value() ) );
            ok = true;
        }
        that.value().~T();
        that.ok = false;
    } else {
        if ( ok ) {
            value().~T();
            ok = false;
        }
    }
    return *this;
}

DTP UTP &UTP::operator=( const T &that ) {
    if ( ok ) {
        value() = that;
    } else {
        new ( bytes ) T( that );
        ok = true;
    }
    return *this;
}

DTP UTP &UTP::operator=( T &&that ) {
    if ( ok ) {
        value() = std::move( that );
    } else {
        new ( bytes ) T( std::move( that ) );
        ok = true;
    }
    return *this;
}

DTP UTP::operator bool() const {
    return ok;
}

DTP const T *UTP::operator->() const {
    return &value();
}

DTP const T &UTP::operator*() const {
    return value();
}

DTP void UTP::clear() {
    if ( ok ) {
        value().~T();
        ok = false;
    }
}

#undef DTP
#undef UTP


END_METIL_NAMESPACE
