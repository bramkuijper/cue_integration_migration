#ifndef _INDIVIDUAL_HPP_
#define _INDIVIDUAL_HPP_

#include <random>
#include "parameters.hpp"

class Individual
{
    public:

        bool is_female{true};

        // diploid loci 
        //
        // reaction norm to population density on the ground
        double an[2]{0.0,0.0};
        double bn[2]{0.0,0.0};

        // reaction norm to an individual's resources
        double ax[2]{0.0,0.0};
        double bx[2]{0.0,0.0};

        // reaction norm to time of day
        double at[2]{0.0,0.0};
        double bt[2]{0.0,0.0};

        // reaction norm to predator presence
        double ap[2]{0.0,0.0};
        double bp[2]{0.0,0.0};

        // reaction norm to continue flying
        // dependent on number of individuals
        // already in the air
        double anu[2]{0.0,0.0};
        double bnu[2]{0.0,0.0};
        

        double resources{0.0};
        // default constructor, used when we initialize
        // the simulation at t = 0
        Individual(Parameters const &parms, bool const is_female);

        // copy constructor
        Individual(Individual const &other);

        // birth constructor
        Individual(Individual const &mother,
                Individual const &father,
                std::mt19937 &rng_r,
                Parameters const &parms
                );

        void operator=(Individual const &other);

        // start flying yes or no
        double pr_fly(
                double const n, // density
                double const x, // resources
                double const t, // time
                double const p // predator y/n
                );

        // once flying actually leave
        double pr_leave_given_fly(
                double const nflying
                );
};


#endif
