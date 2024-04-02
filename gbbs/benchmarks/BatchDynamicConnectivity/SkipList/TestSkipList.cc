#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "benchmarks/BatchDynamicConnectivity/SkipList/SkipList.h"
#include "single_include/catch2/catch.hpp"

int max_length_for_bottom_level=10;

void initialize_skiplist_for_tests(gbbs::SkipList & skip_list,parlay::sequence<gbbs::SkipList::SkipListElement> &skip_list_elements){
    //Create ndoes and assign integer values to their values array. 
    for (int i=0; i<max_length_for_bottom_level; i++){
        skip_list_elements[i] = skip_list.create_node(i, nullptr, nullptr, gbbs::default_values(i,i));
        skip_list_elements[i].update_level=0;
        skip_list.update_top_down(skip_list_elements[i].height-1,&skip_list_elements[i]);
    }

    for(int i=0; i<max_length_for_bottom_level-1; i++){
        skip_list.join(&skip_list_elements[i], &skip_list_elements[i+1]);
    }
}

TEST_CASE("Unit tests for update_top_down","SkipList")
{
    auto skip_list = gbbs::SkipList(max_length_for_bottom_level);
    parlay::sequence<gbbs::SkipList::SkipListElement> skip_list_elements = parlay::sequence<gbbs::SkipList::SkipListElement>(max_length_for_bottom_level);
    initialize_skiplist_for_tests(skip_list,skip_list_elements);

    //Test if assigned actual values instead of zeros to the values arraies
    SECTION("Test the correctness of initialization!"){
        for(int i=0; i<max_length_for_bottom_level; i++){
            gbbs::SkipList::values_array expected_value_arr = gbbs::default_values_array(skip_list_elements[i].height,i,i);
            for(int j=1; j<expected_value_arr.size();j++){expected_value_arr[j]=expected_value_arr[0];}
            REQUIRE(skip_list_elements[i].values == expected_value_arr);
        }
    }

    SECTION("Level = 0 || Number of children =0"){
        for(int i=0; i<max_length_for_bottom_level; i++){
            ////The location within the skiplist for this test case, which means all nodes with height = 1
            if(skip_list_elements[i].height==1){
                skip_list_elements[i].values[0]=gbbs::default_values(i+1,i+1);//Make a change to the value of the element being tested
                skip_list.update_top_down(skip_list_elements[i].height-1, &skip_list_elements[i]);
                gbbs::SkipList::values_array expected_value_arr = gbbs::default_values_array(skip_list_elements[i].height,i+1,i+1);
                REQUIRE(skip_list_elements[i].values == expected_value_arr);
            }
        }
    }

    SECTION("Level = 3 which is Max_height-1"){
        //The location within the skiplist for this test case, which is the seventh node in the skiplist
        int loc = 7;
        skip_list_elements[loc].values[0]=gbbs::default_values(loc+1,loc+1);//Make a change to the value of the element being tested
        gbbs::SkipList::values_array expected_value_arr = gbbs::default_values_array(skip_list_elements[loc].height,loc,loc);
        for(int j=1; j<expected_value_arr.size();j++){expected_value_arr[j]=expected_value_arr[0];}
        expected_value_arr[0]=skip_list_elements[loc].values[0];

        SECTION("Number of children = 1"){
            skip_list.split(&skip_list_elements[loc]);//Break the following chain to prevent more children 
            
            SECTION("Recursive spawn: no"){
                skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                REQUIRE(skip_list_elements[loc].values == expected_value_arr);
            }

            SECTION("Recursive spawn: only for the top two levels of the element"){
                skip_list_elements[loc].update_level=skip_list_elements[loc].height-2;//Set ONLY top 2 levels of this element to be updated, which should produce the same result as the case above
                skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                REQUIRE(skip_list_elements[loc].values == expected_value_arr);
            }

            SECTION("Recursive spawn: all over the element"){
                skip_list_elements[loc].update_level=0;//Set all levels of this element to be updated
                skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                for(int j=1; j<expected_value_arr.size();j++){expected_value_arr[j]=expected_value_arr[0];}
                REQUIRE(skip_list_elements[loc].values == expected_value_arr);
            }
        }

        SECTION("Number of children = N"){
            expected_value_arr[3]=gbbs::XOR_function(skip_list_elements[loc].values[2],skip_list_elements[loc+1].values[2]);

            SECTION("Recursive spawn: no"){//Should only update the value for the top level
                skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                REQUIRE(skip_list_elements[loc].values == expected_value_arr);
            }

            SECTION("Recursive spawn: only for the top two levels of the element"){
                skip_list_elements[loc].update_level=skip_list_elements[loc].height-2;//Set ONLY top 2 levels of this element to be updated, which should produce the same result as the case above
                skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                REQUIRE(skip_list_elements[loc].values == expected_value_arr);
            }

            SECTION("Recursive spawn: all over the element"){
                skip_list_elements[loc].update_level=0;//Set all levels of this element to be updated
                skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);

                expected_value_arr[0]=skip_list_elements[loc].values[0];
                for(int j=1; j<expected_value_arr.size()-1;j++){expected_value_arr[j]=expected_value_arr[0];}
                expected_value_arr[3]=gbbs::XOR_function(skip_list_elements[loc].values[2],skip_list_elements[loc+1].values[2]);
                REQUIRE(skip_list_elements[loc].values == expected_value_arr);
            }
        }
    }

    SECTION("Level = 2 which is between 0 and Max_height-1"){
        int loc = 8;
        skip_list_elements[loc].values[0]=gbbs::default_values(loc+1,loc+1);
        gbbs::SkipList::values_array expected_value_arr = gbbs::default_values_array(skip_list_elements[loc].height,loc,loc);

        SECTION("Number of children = N"){
            SECTION("No loop exist in the SkipList"){
                for(int j=1; j<expected_value_arr.size();j++){expected_value_arr[j]=expected_value_arr[0];}
                expected_value_arr[0]=skip_list_elements[loc].values[0];
                expected_value_arr[2]=gbbs::XOR_function(skip_list_elements[loc].values[1],skip_list_elements[loc+1].values[1]);

                SECTION("Recursive spawn: no"){
                    skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                    REQUIRE(skip_list_elements[loc].values == expected_value_arr);
                }

                expected_value_arr[1]=skip_list_elements[loc].values[0];
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[1],skip_list_elements[loc+1].values[1]);

                SECTION("Recursive spawn: only for the top two levels of the element"){
                    skip_list_elements[loc].update_level=skip_list_elements[loc].height-2;//Set ONLY top 2 levels of this element to be updated, which means all levels for this element as the case below
                    skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                    REQUIRE(skip_list_elements[loc].values == expected_value_arr);
                }

                SECTION("Recursive spawn: all over the element"){
                    skip_list_elements[loc].update_level=0;//Set all levels of this element to be updated, which should produce the same result as the case above
                    skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                    REQUIRE(skip_list_elements[loc].values == expected_value_arr);
                }
            }

            SECTION("Loop exist in the SkipList"){
                skip_list.join(&skip_list_elements[max_length_for_bottom_level-1],&skip_list_elements[0]);

                expected_value_arr[0]=skip_list_elements[loc].values[0];
                expected_value_arr[1]=gbbs::default_values(loc,loc);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[1],skip_list_elements[9].values[1]);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[2],skip_list_elements[1].values[1]);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[2],skip_list_elements[4].values[1]);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[2],skip_list_elements[6].values[1]);
    
                SECTION("Recursive spawn: no"){
                    skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                    REQUIRE(skip_list_elements[loc].values == expected_value_arr);
                }

                expected_value_arr[1]=skip_list_elements[loc].values[0];
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[1],skip_list_elements[9].values[1]);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[2],skip_list_elements[1].values[1]);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[2],skip_list_elements[4].values[1]);
                expected_value_arr[2]=gbbs::XOR_function(expected_value_arr[2],skip_list_elements[6].values[1]);

                SECTION("Recursive spawn: only for the top two levels of the element"){
                    skip_list_elements[loc].update_level=skip_list_elements[loc].height-2;//Set ONLY top 2 levels of this element to be updated, which means all levels for this element
                    skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                    REQUIRE(skip_list_elements[loc].values == expected_value_arr);
                }

                SECTION("Recursive spawn: all over the element"){
                    skip_list_elements[loc].update_level=0;//Set all levels of this element to be updated, which should produce the same result as the case above
                    skip_list.update_top_down(skip_list_elements[loc].height-1, &skip_list_elements[loc]);
                    REQUIRE(skip_list_elements[loc].values == expected_value_arr);
                }
            }
        }
    }
}
