#pragma once

#include "DisplayItem.h"

BEG_METIL_NAMESPACE

/***/
class DisplayPtrId {
public:
    /**/        DisplayPtrId  ( const auto &ptr ) : DisplayPtrId( ptr ? std::to_string( std::size_t( &*ptr ) ) : Str{} ) {}
    /**/        DisplayPtrId  ( const Str &id ) : id( id ) {}

    explicit    operator bool () const;
    bool        operator<     ( const DisplayPtrId &that ) const;

    Str         graphviz_label() const;
    void        write         ( const std::function<void( std::string_view str )> &func, DisplayWriteContext &ctx ) const;

    std::string id;
};

END_METIL_NAMESPACE
