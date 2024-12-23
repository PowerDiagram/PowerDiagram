#include <sdot/DiracVecFromLocallyKnownValues.h>
#include <sdot/HomogeneousWeights.h>
#include <sdot/WeightsWithBounds.h>
#include <sdot/PavingWithDiracs.h>
#include <sdot/RegularGrid.h>
#include <sdot/VtkOutput.h>
#include <sdot/Cell.h>

#include <tl/support/string/to_string.h>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
// #include "pybind11/gil.h"
 
using Array = pybind11::array_t<sdot::TF, pybind11::array::c_style>;
using namespace sdot;

// #define PolyCon_Py CC_DT( PolyCon_py )

// Ti static Array to_Array( const Vec<Scalar,i> &v ) {
//     Vec<PI,1> shape{ v.size() };
//     Array res( shape );
//     for( PI n = 0; n < PI( v.size() ); ++n )
//         res.mutable_at( n ) = v[ n ];
//     return res;
// }

// static Array to_Array( const Vec<Point> &v ) {
//     Vec<PI,2> shape{ v.size(), PI( POLYCON_DIM ) };
//     Array res( shape );
//     for( PI i = 0; i < v.size(); ++i )
//         for( PI d = 0; d < PI( POLYCON_DIM ); ++d )
//             res.mutable_at( i, d ) = v[ i ][ d ];
//     return res;
// }

// static Array to_Array( const Vec<Point> &v, const Vec<Scalar> &s ) {
//     Vec<PI,2> shape{ v.size(), PI( POLYCON_DIM + 1 ) };
//     Array res( shape );
//     for( PI i = 0; i < v.size(); ++i ) {
//         for( PI d = 0; d < PI( POLYCON_DIM ); ++d )
//             res.mutable_at( i, d ) = v[ i ][ d ];
//         res.mutable_at( i, POLYCON_DIM ) = s[ i ];
//     }
//     return res;
// }

// /***/
// struct PolyCon_py {
//     PolyCon_py( Array a_dir, Array a_off, Array b_dir, Array b_off ) : pc(
//             Span<Point>{ reinterpret_cast<Point *>( a_dir.mutable_data() ), PI( a_dir.shape( 0 ) ) },
//             { a_off.mutable_data(), PI( a_off.shape( 0 ) ) },
//             Span<Point>{ reinterpret_cast<Point *>( b_dir.mutable_data() ), PI( b_dir.shape( 0 ) ) },
//             { b_off.mutable_data(), PI( b_off.shape( 0 ) ) }
//     ) { }

//     PolyCon_py( PolyCon<Scalar,POLYCON_DIM> &&pc ) : pc( std::move( pc ) ) {
//     }

//     void write_vtk( const Str &filename ) {
//         VtkOutput vo;
//         pc.display_vtk( vo );
//         vo.save( filename );
//     }

//     std::variant<std::tuple<Scalar,Array>, py::none> value_and_gradient( Array x ) {
//         Point p( FromItemValue(), 0 );
//         for( PI i = 0; i < std::min( PI( POLYCON_DIM ), PI( x.size() ) ); ++i )
//             p[ i ] = x.at( i );
         
//         if ( auto vg = pc.value_and_gradient( p ) )
//             return std::tuple<Scalar,Array>{ std::get<0>( *vg ), to_Array( std::get<1>( *vg ) ) };
//         return py::none();
//     }

//     std::variant<std::tuple<Scalar,std::vector<Array>>, py::none> value_and_gradients( Array x, Scalar probe_size ) {
//         Point p( FromItemValue(), 0 );
//         for( PI i = 0; i < std::min( PI( POLYCON_DIM ), PI( x.size() ) ); ++i )
//             p[ i ] = x.at( i );
         
//         if ( auto vg = pc.value_and_gradients( p, probe_size ) ) {
//             std::vector<Array> gradients;
//             for( const auto &g : std::get<1>( *vg ) )
//                 gradients.push_back( to_Array( g ) );
//             return std::tuple<Scalar,std::vector<Array>>{ std::get<0>( *vg ), gradients };
//         }
//         return py::none();
//     }

//     PolyCon_py legendre_transform() {
//         return pc.legendre_transform();
//     }

//     auto as_fbdo_arrays() -> std::tuple<Array,Array,Array,Array> {
//         return { to_Array( pc.f_dirs ), to_Array( pc.f_offs ), to_Array( pc.b_dirs ), to_Array( pc.b_offs ) };
//     }

//     auto as_fb_arrays() -> std::tuple<Array,Array> {
//         return { to_Array( pc.f_dirs, pc.f_offs ), to_Array( pc.b_dirs, pc.b_offs ) };
//     }

//     struct VertexData {
//         Scalar          height;
//         CountOfCutTypes cct;
//         Point           pos;
//     };

//     auto edge_data( CtInt<1> ) {
//         Vec<Vec<VertexData,2>> res;
//         pc.for_each_cell( [&]( Cell<Scalar,POLYCON_DIM> &cell ) {
//             cell.for_each_edge( [&]( auto num_cuts, const Vertex<Scalar,POLYCON_DIM> &v0, const Vertex<Scalar,POLYCON_DIM> &v1 ) {
//                 const Scalar h0 = cell.height( v0.pos );
//                 const Scalar h1 = cell.height( v1.pos );

//                 CountOfCutTypes c0, c1;
//                 cell.add_cut_types( c0, v0.num_cuts, pc.nb_bnds() );
//                 cell.add_cut_types( c1, v1.num_cuts, pc.nb_bnds() );

//                 res << Vec<VertexData,2> {
//                     VertexData{ h0, c0, v0.pos },
//                     VertexData{ h1, c1, v1.pos },
//                 };
//             } );
//         } );
//         return res;
//     }

//     template<int nd>
//     auto edge_data( CtInt<nd> ) {
//         using NC = Vec<SI,POLYCON_DIM-1>;
//         std::map<NC,Vec<VertexData,2>,Less> map;
//         pc.for_each_cell( [&]( Cell<Scalar,POLYCON_DIM> &cell ) {
//             cell.for_each_edge( [&]( auto num_cuts, const Vertex<Scalar,POLYCON_DIM> &v0, const Vertex<Scalar,POLYCON_DIM> &v1 ) {
//                 std::sort( num_cuts.begin(), num_cuts.end() );
//                 map_item( map, num_cuts, [&]() -> Vec<VertexData,2> {
//                     const Scalar h0 = cell.height( v0.pos );
//                     const Scalar h1 = cell.height( v1.pos );

//                     CountOfCutTypes c0, c1;
//                     cell.add_cut_types( c0, v0.num_cuts, pc.nb_bnds() );
//                     cell.add_cut_types( c1, v1.num_cuts, pc.nb_bnds() );

//                     return {
//                         VertexData{ h0, c0, v0.pos },
//                         VertexData{ h1, c1, v1.pos },
//                     };
//                 } );
//             } );
//         } );

//         Vec<Vec<VertexData,2>> res;
//         for( const auto &p : map )
//             res << p.second;
//         return res;
//     }

//     Array edge_points() {
//         auto map = edge_data( CtInt<POLYCON_DIM>() );

//         Vec<PI> shape{ map.size(), 2, POLYCON_DIM + 4 };
//         Array res( shape );
//         for( PI n = 0; n < map.size(); ++n ) {
//             for( PI i = 0; i < 2; ++i ) {
//                 for( PI d = 0; d < POLYCON_DIM; ++d )
//                     res.mutable_at( n, i, d ) = map[ n ][ i ].pos[ d ];
//                 res.mutable_at( n, i, POLYCON_DIM + 0 ) = map[ n ][ i ].height;
//                 res.mutable_at( n, i, POLYCON_DIM + 1 ) = map[ n ][ i ].cct.nb_ints;
//                 res.mutable_at( n, i, POLYCON_DIM + 2 ) = map[ n ][ i ].cct.nb_bnds;
//                 res.mutable_at( n, i, POLYCON_DIM + 3 ) = map[ n ][ i ].cct.nb_infs;
//             }
//         }
//         return res;
//     }

//     PI ndim() const {
//         return pc.ndim();
//     }

//     PolyCon_py add_polycon( const PolyCon_py &that ) {
//         return pc + that.pc;
//     }

//     PolyCon_py add_scalar( Scalar that ) {
//         Vec<Scalar> new_f_offs = pc.f_offs - that;
//         return PolyCon<Scalar,POLYCON_DIM>( pc.f_dirs, new_f_offs, pc.b_dirs, pc.b_offs );
//     }

//     PolyCon_py mul_scalar( Scalar that ) {
//         Vec<Point>  new_f_dirs = that * pc.f_dirs;
//         Vec<Scalar> new_f_offs = that * pc.f_offs;
//         return PolyCon<Scalar,POLYCON_DIM>( new_f_dirs, new_f_offs, pc.b_dirs, pc.b_offs );
//     }

//     PolyCon_py normalized( POLYCON_SCALAR min_volume = 0 ) {
//         PolyCon<POLYCON_SCALAR,POLYCON_DIM> cp = pc;
//         cp.normalize( min_volume );
//         return cp;
//     }

//     PolyCon<POLYCON_SCALAR,POLYCON_DIM> pc;
// };

Pt Pt_from_Array( const Array &array ) {
    Pt res;
    if ( array.size() < nb_dims )
        throw pybind11::value_error( "array is not large enough" );
    for( PI d = 0; d < nb_dims; ++d )
       res[ d ] = array.at( d );
    return res;
}

Vec<Pt> VecPt_from_Array( const Array &array ) {
    Vec<Pt> res;
    if ( array.shape( 1 ) < nb_dims )
        throw pybind11::value_error( "array is not large enough" );
    res.resize( array.shape( 0 ) );
    for( PI r = 0; r < res.size(); ++r )
        for( PI d = 0; d < nb_dims; ++d )
           res[ r ][ d ] = array.at( r, d );
    return res;
}

PYBIND11_MODULE( SDOT_CONFIG_module_name, m ) { // py::module_local()
    // pybind11::class_<Pt>( m, PD_STR( Pt ) )
    //     .def( pybind11::init( &create_Pt_from_Array ) )
    //     .def( "__repr__", []( const Pt &pt ) { return to_string( pt ); } )
    //     ;
 
    pybind11::class_<VtkOutput>( m, PD_STR( VtkOutput ) )
        .def( pybind11::init<>() )
        .def( "save", []( VtkOutput &vo, Str fn ) { return vo.save( fn ); } )
        ;
 
    pybind11::class_<Cell>( m, PD_STR( Cell ) )
        // base methods
        .def( pybind11::init<>() )
        .def( "__repr__", []( const Cell &cell ) { return to_string( cell ); } )
        
        // properties
        .def_property_readonly( "true_dimensionality", &Cell::true_dimensionality )
        .def_property_readonly( "nb_vertices", &Cell::nb_vertices )
        .def_property_readonly( "nb_cuts", &Cell::nb_cuts )
        .def_property_readonly( "bounded", &Cell::bounded )
        .def_property_readonly( "empty", &Cell::empty )

        // modifications
        .def( "cut_boundary", []( Cell &cell, const Array &p0, TF w0, PI i0 ) { return cell.cut_boundary( Pt_from_Array( p0 ), w0, i0 ); } )

        // output
        .def( "display_vtk", []( const Cell &cell, VtkOutput &vo ) { return cell.display_vtk( vo ); } )
        ;

    pybind11::class_<DiracVec>( m, PD_STR( DiracVec ) )
        .def( "__repr__", []( const DiracVec &a ) { return to_string( a ); } )
        ;

    pybind11::class_<DiracVecFromLocallyKnownValues,DiracVec>( m, PD_STR( DiracVecFromLocallyKnownValues ) )
        .def( pybind11::init( [&]( const Array &pts ) -> DiracVecFromLocallyKnownValues { return VecPt_from_Array( pts ); } ) )
        // .def( "__repr__", []( const DiracVecFromLocallyKnownValues &a ) { return to_string( a ); } )
        ;

    // weights ======================================================================================
    pybind11::class_<WeightsWithBounds>( m, PD_STR( WeightsWithBounds ) )
        .def( "__repr__", []( const WeightsWithBounds &a ) { return to_string( a ); } )
        ;

    pybind11::class_<HomogeneousWeights,WeightsWithBounds>( m, PD_STR( HomogeneousWeights ) )
        .def( pybind11::init<TF>() )
        // .def( "__repr__", []( const HomogeneousWeights &a ) { return to_string( a ); } )
        ;

    // paving ========================================================================================
    pybind11::class_<PavingWithDiracs>( m, PD_STR( PavingWithDiracs ) )
        .def( "__repr__", []( const PavingWithDiracs &a ) { return to_string( a ); } )
        .def( "for_each_cell", []( PavingWithDiracs &paving, const Cell &base_cell, const WeightsWithBounds &wwb, const std::function<void( const Cell &cell )> &f ) { 
            pybind11::gil_scoped_release gsr;
            paving.for_each_cell( base_cell, wwb, [&]( Cell &cell, int ) {
                pybind11::gil_scoped_acquire gsa;
                f( cell );
            } );
        } )
        ;

    pybind11::class_<RegularGrid,PavingWithDiracs>( m, PD_STR( RegularGrid ) )
        .def( pybind11::init( []( const DiracVec &pts ) -> RegularGrid { return { pts, {} }; } ) )
        .def( "__repr__", []( const RegularGrid &a ) { return to_string( a ); } )
        ;

    // utility functions ============================================================================
    m.def( "display_vtk", []( VtkOutput &vo, const Cell &base_cell, PavingWithDiracs &diracs, const WeightsWithBounds &weights ) {
        std::mutex m;
        diracs.for_each_cell( base_cell, weights, [&]( Cell &cell, int ) { 
            m.lock();
            cell.display_vtk( vo );
            m.unlock();
        } );
    } );
    
    m.def( "ndim", []() {
        return sdot::nb_dims;
    } );
}
