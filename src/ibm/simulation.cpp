#include <cassert>
#include <random>
#include <iostream>
#include "simulation.hpp"


Simulation::Simulation(Parameters const &params) :
    rd{}, // initialize random device (with which to choose a random seed)
    seed{rd()}, // get the random seed 
    rng_r{seed}, // now use the random seed to initialize a random number generator
    par{params}, // copy over the parameters
    data_file{par.file_name}, // initialize the data file to write output to
    uniform{0.0,1.0}, // initialize the uniform distribution 
    sites(par.n_sites,Site(0,0,params)) // initialize the uniform distribution 
{}

void Simulation::initialise_sites()
{
    // fill the first site with the whole population
    // they then depart and move to different sites
    sites[0] = Site(par.N/2, par.N/2, par);
} // end initialise_sites()

// run the actual simulation
void Simulation::run()
{
    write_data_headers();

    // have individuals survive dependent on their 
    // ornaments and preferences
    // then choose mates
    for (generation = 0; 
            generation <= par.max_generation; 
            ++generation)
    {
        // write statistics to file
        if (generation % par.data_print_interval == 0)
        {
            write_data();
        }

        for (ecological_time_idx = 0;
                ecological_time_idx < par.max_season_time_steps;
                ++ecological_time_idx)
        {
            // go around all sites and move individuals around
            migrate();
        }

        // replace the current generation
        reproduce();

    } // end for unsigned int generation
   
    write_parameters();
        
} // end run_simulation()

// reproduction
// only individuals in the final site 
// contribute to fitness
void Simulation::reproduce()
{

} // end reproduce()

// migrate individuals across stop over sites
void Simulation::migrate()
{
    for (unsigned site_idx{0};
            site_idx < par.n_sites;
            ++site_idx)
    {
        // 1.go over all individuals in a site
        // and calculate prob of leaving
        for (auto male_iter{sites[site_idx].males.begin()};
                male_iter != sites[site_idx].males.end();
                ++male_iter)
        {



        }
    



        // 2. then let them fly in the air
        // 3. then move them around
    } // end for site idx
} // end migrate()



void Simulation::write_data()
{}

void Simulation::write_data_headers()
{
    data_file << "generation;mean_t;mean_p;var_t;var_p;cov_pt;frac_female_survive;frac_male_survive"
        << std::endl;
} // end write_data_headers()

void Simulation::write_parameters()
{
    data_file << std::endl << std::endl
        << "seed;" << seed << ";" << std::endl
        << "N;" << par.N << ";" << std::endl
        << "max_generation;" << par.max_generation << ";" << std::endl;
}
