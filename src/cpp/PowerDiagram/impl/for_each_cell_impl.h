#pragma once

#include <vfs/support/ThreadPool.h>
#include "CellImpl.h"
#include "../Cell.h"

/// b_dirs, b_offs => boundaries
/// beg_index_... => beg of cut indices for each category (power_diagram, regular boundary, infinite boundary)
template<class WeightedPointSet>
void for_each_cell_impl( const WeightedPointSet &weighted_point_set, const auto &b_dirs, const auto &b_offs, auto &&func, PI beg_index_wps, PI beg_index_bnd, PI beg_index_inf ) {
    constexpr auto nb_dims = WeightedPointSet::nb_dims;
    using Scalar = WeightedPointSet::Scalar;
    using Weight = WeightedPointSet::Weight;
    using Point = WeightedPointSet::Point;
    using Box = WeightedPointSet::Box;
    using Lc = CellImpl<Scalar,Point,Weight,nb_dims>;

    thread_pool.execute( weighted_point_set.leaves.size(), [&]( PI nb_threads, const auto &cb ) {
        Vec<Vec<Box *>> init_boxes_to_tests( FromSize(), nb_threads );
        Vec<Vec<Box *>> boxes_to_tests( FromSize(), nb_threads );
        Vec<Lc> lcs( FromSize(), nb_threads );
        func( nb_threads, [&]( const std::function<void( Cell &lc, PI num_thread )> &f ) {
            cb( [&]( PI num_leaf, PI num_thread ) {
                Vec<Box *> &init_boxes_to_test = init_boxes_to_tests[ num_thread ];
                Vec<Box *> &boxes_to_test = boxes_to_tests[ num_thread ];
                Box *leaf = weighted_point_set.leaves[ num_leaf ];

                Lc &lc = lcs[ num_thread ];
                Cell cell{ FromPointerOnBorrowed(), &lc };

                // init_boxes_to_test
                init_boxes_to_test.clear();
                for( Box *p = leaf; p; p = p->parent )
                    for( Box *s = p->sibling; s != p; s = s->sibling )
                        init_boxes_to_test.push_back( s );
                std::reverse( init_boxes_to_test.begin(), init_boxes_to_test.end() );

                // make a cell for each point of the leaf
                leaf->for_each_point( [&]( const Point &o_point, const Weight o_weight, PI o_index ) {
                    // (re)init cell
                    auto radius = weighted_point_set.root->radius();
                    if ( ! radius )
                        radius = 1e10; // TODO: something more robust
                    lc.init( beg_index_wps + o_index, beg_index_inf, weighted_point_set.root->center(), weighted_point_set.init_radius_mul * radius );
                    lc.orig_weight = &o_weight;
                    lc.orig_point = &o_point;

                    // local test (leaf box)
                    leaf->for_each_point( [&]( const Point &n_point, const Weight &n_weight, PI n_index ) {
                        if ( n_index == o_index )
                            return;
                        Point dir = n_point - o_point;
                        auto n = norm_2_p2( dir );
                        auto so = sp( dir, o_point );
                        auto sn = sp( dir, n_point );
                        auto off = so + ( 1 + 1 * ( o_weight - n_weight ) / n ) / 2 * ( sn - so );
                        lc.cut( beg_index_wps + n_index, dir, off );
                    } );

                    // test the neighbor boxes
                    boxes_to_test = init_boxes_to_test;
                    while ( boxes_to_test.size() ) {
                        Box *box = boxes_to_test.pop_back_val();

                        // exit if no potential intersection

                        // children boxes ?
                        if ( box->children.size() ) {
                            for( Box *ch : box->children )
                                boxes_to_test.push_back( ch );
                            continue;
                        }

                        // else, make the cuts from points of `box`
                        box->for_each_point( [&]( const Point &n_point, const Weight &n_weight, PI n_index ) {
                            Point dir = n_point - o_point;
                            auto n = norm_2_p2( dir );
                            auto so = sp( dir, o_point );
                            auto sn = sp( dir, n_point );
                            auto off = so + ( 1 + 1 * ( o_weight - n_weight ) / n ) / 2 * ( sn - so );
                            lc.cut( beg_index_wps + n_index, dir, off );
                        } );
                    }

                    // boundaries
                    if constexpr ( ! std::is_same_v< decltype( b_dirs.size() ), CtInt<0>> )
                        for( PI i = 0; i < b_dirs.size(); ++i )
                            lc.cut( beg_index_bnd + i, b_dirs[ i ], b_offs[ i ] );

                    // call
                    f( cell, num_thread );
                } );
            } );
        } );
    } );
}

void for_each_cell_impl( const auto &weighted_point_set, const auto &b_dirs, const auto &b_offs, auto &&func ) {
    PI ws = weighted_point_set.size();
    for_each_cell_impl( weighted_point_set, b_dirs, b_offs, func, 0, ws, ws + b_dirs.size() );
}
