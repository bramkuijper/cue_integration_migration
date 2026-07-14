#ifndef _SIMULATION_HPP_
#define _SIMULATION_HPP_

// class that sets up the simulation

#include <random>
#include <fstream>
#include "parameters.hpp"
#include "site.hpp"

class Simulation
{
    private:

        // a random device needed to initialize
        // the random number sequence (otherwise always
        // the same random number sequence among replicates)
        std::random_device rd;

        // store the seed used to initialize the random
        // number sequence, so that we can 'replay' the 
        // same simulation for debugging, visualisation purposes
        unsigned int seed;

        // the random number generator itself
        std::mt19937 rng_r;

        // object with all the parameters of this simulation
        Parameters par;

        // the file to write statistics to 
        std::ofstream data_file;
        std::ofstream data_file_migration;

        // uniform distribution
        std::uniform_real_distribution<double> uniform;

        std::vector <Site> sites{};

        std::vector <double> average_group_size_per_site{};
        std::vector <double> average_pr_fly_per_site{};

        // current generation
        unsigned int generation{0};

        unsigned int ecological_time_idx{0};

        void reproduce();

        void reproduce_from_site(Site const &site);

        void write_data();
        void write_data_migration();
        void write_parameters();
        void write_data_headers();

        void initialise_sites();

        void ready_to_migrate();

        void move_between_sites();

        bool is_extinct();

        double group_size_flight_survival(
                double const resources,
                unsigned int const group_size); 

    public:
        // initialize the simulation
        Simulation(Parameters const &parms);

        // run the thing
        void run();
};


#endif
