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
    unsigned dens;

    unsigned n_airborne{0};

    for (unsigned site_idx{0};
            site_idx < (par.n_sites - 1); // final site is site of arrival
            ++site_idx)
    {
        dens = sites[site_idx].females.size() +
                sites[site_idx].males.size(); 

        // reset things
        n_airborne = 0;

        // 1.go over all individuals in a site
        // and calculate prob of leaving
        for (auto male_iter{sites[site_idx].males.begin()};
                male_iter != sites[site_idx].males.end();
                ++male_iter)
        {
            if (uniform(rng_r) < 
                    male_iter->pr_fly(
                        dens,
                        male_iter->resources,
                        ecological_time_idx,
                        sites[site_idx].predator_density))
            {
                ++n_airborne;
                male_iter->is_airborne = true;
            }
        } // end male_iter
        
        for (auto female_iter{sites[site_idx].females.begin()};
                female_iter != sites[site_idx].females.end();
                ++female_iter)
        {
            if (uniform(rng_r) < 
                    female_iter->pr_fly(
                        dens,
                        female_iter->resources,
                        ecological_time_idx,
                        sites[site_idx].predator_density))
            {
                ++n_airborne;
                female_iter->is_airborne = true;
            }
        } // end female_iter
    
        // 2. then let them fly in the air
        for (auto male_iter{sites[site_idx].males.begin()};
                male_iter != sites[site_idx].males.end();
                ++male_iter)
        {
            departing_males[site_idx].:w
        }
        


        // 3. then move them around
    } // end for site idx
} // end migrate()



void Simulation::write_data()
{

}

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
