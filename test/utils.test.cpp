#include "catch.hpp"
#include "utils.hpp"

SCENARIO("normal distribution can be created") {
    std::normal_distribution<double> distribution;

    GIVEN("number of elements in the sequence") {
        int num_elements;

        WHEN("number of elements is 0") {
            num_elements = 0;
            distribution = Utils::generate_normal_distribution(num_elements);

            THEN("mean is -0.5 and stddev is 0") {
                REQUIRE( distribution.mean() == -0.5 );
                REQUIRE( distribution.stddev() == 0.0 );
            }
        }
    }
}

