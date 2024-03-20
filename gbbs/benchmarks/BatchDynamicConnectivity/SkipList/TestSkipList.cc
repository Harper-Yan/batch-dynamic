#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "benchmarks/BatchDynamicConnectivity/SkipList/SkipList.h"
#include "single_include/catch2/catch.hpp"

TEST_CASE( "Equality operator == matches two empty skiplists", "[skiplist][equality]" )
{
    gbbs::SkipList default_constructed_list;
    gbbs::SkipList zero_initialised_list(0);

    REQUIRE( default_constructed_list == zero_initialised_list );
}

TEST_CASE( "Equality operator == failes with just one empty skiplist", "[skiplist][equality]" )
{
    gbbs::SkipList default_constructed_list;
    gbbs::SkipList zero_initialised_list(42);

    REQUIRE( default_constructed_list == zero_initialised_list );
}

