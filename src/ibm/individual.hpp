#ifndef _INDIVIDUAL_HPP_
#define _INDIVIDUAL_HPP_

#include <random>
#include "parameters.hpp"

class Individual
{
    public:

        bool is_female{true}; // sex
        bool is_airborne{false}; // whether individual is airborne or not

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
       
        // reaction norm to continue flying
        // dependent on resource levels of others
        // already in the air
        double axo[2]{0.0,0.0};
        double bxo[2]{0.0,0.0};
        
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

        double pr_depart(
                unsigned const n_airborne,
                double const resources_other);

        // start flying yes or no
        double pr_fly(
                double const fraction_of_total_dens, // density
                double const x, // resources
                double const fraction_of_season, // time
                double const p // predator density
                );
};


#endif
