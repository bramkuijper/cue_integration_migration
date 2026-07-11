#include <cassert>
#include <random>
#include <iostream>
#include <cmath>
#include "simulation.hpp"
#include "site.hpp"


Simulation::Simulation(Parameters const &params) :
    rd{}, // initialize random device (with which to choose a random seed)
    seed{rd()}, // get the random seed 
    rng_r{seed}, // now use the random seed to initialize a random number generator
    par{params}, // copy over the parameters
    data_file{par.file_name}, // initialize the data file to write output to
    data_file_migration{par.file_name + "_migration"}, // initialize the data file to write output to
    uniform{0.0,1.0}, // initialize the uniform distribution 
    sites(par.n_sites,Site(0,0,params)), // initialize all the sites
    stats_sites(2, std::vector <unsigned> (par.n_sites, 0)) // initialize the site statistics
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
            ready_to_migrate();
            move_between_sites();
        }

        // replace the current generation
        reproduce();

    } // end for unsigned int generation
   
    write_parameters();
        
} // end run_simulation()

// TODO: dealing with resources

// reproduction
// only individuals in the final site 
// contribute to fitness
// with reproduction sexual and dependent
// on the remaining resources of each individual
void Simulation::reproduce()
{
    std::vector <double> male_resource_distribution{};
    std::vector <double> female_resource_distribution{};
    
    // get resource distribution among all the females
    for (auto female_iter{sites[par.n_sites - 1].females.begin()};
            female_iter != sites[par.n_sites - 1].females.end();
            ++female_iter)
    {
        female_resource_distribution.push_back(female_iter->resources);
    }

    // get resource distribution among all the males
    for (auto male_iter{sites[par.n_sites - 1].males.begin()};
            male_iter != sites[par.n_sites - 1].males.end();
            ++male_iter)
    {
        male_resource_distribution.push_back(male_iter->resources);
    }

    std::discrete_distribution <unsigned> female_sampler(
            female_resource_distribution.begin(),
            female_resource_distribution.end());

    std::discrete_distribution <unsigned> male_sampler(
            male_resource_distribution.begin(),
            male_resource_distribution.end());

    // aux variables to store father and mother
    unsigned male_idx, female_idx;

    // clear all females and males in the first patch
    assert(par.n_sites > 1);

    sites[0].females.clear();
    sites[0].males.clear();

    for (unsigned int newborn_idx{0};
            newborn_idx < par.N;
            ++newborn_idx)
    {
        male_idx = male_sampler(rng_r);
        female_idx = female_sampler(rng_r);

        Individual offspring(
                sites[par.n_sites - 1].females[female_idx],
                sites[par.n_sites - 1].males[male_idx],
                rng_r,
                par);

        if (offspring.is_female)
        {
            sites[0].females.push_back(offspring);
        }
        else
        {
            sites[0].males.push_back(offspring);
        }
    }
    // now remove all the other females and males from the other sites
    for (unsigned site_idx{1};
            site_idx < par.n_sites;
            ++site_idx)
    {
        sites[site_idx].females.clear();
        sites[site_idx].males.clear();
    }
} // end reproduce()

// evaluate an individual's readiness to 
// migrate to a different site
void Simulation::ready_to_migrate()
{
    // aux variable reflecting the densit
    // before take-off of a site
    unsigned dens;

    // aux variable tracking
    // current number airborne in a site
    unsigned n_airborne;

    double resources_airborne;

    for (unsigned site_idx{0};
            site_idx < par.n_sites - 1; // final site is site of arrival
            ++site_idx)
    {
        dens = static_cast<unsigned>(sites[site_idx].females.size() +
                sites[site_idx].males.size()); 

        // check whether departing individuals have 
        // indeed departed
        assert(sites[site_idx].departing_males.size() == 0);
        assert(sites[site_idx].departing_females.size() == 0);

        // reset things
        n_airborne = 0;
        resources_airborne = 0.0;

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
                male_iter->resources -= par.f;
                ++n_airborne;
                resources_airborne += male_iter->resources;
                male_iter->is_airborne = true;
            }
            else {
                male_iter->resources += par.g;
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
                resources_airborne += female_iter->resources;
                female_iter->is_airborne = true;
            }
        } // end female_iter
  
        // make average o
        resources_airborne /= n_airborne;

        // 2. while airborne see
        // whether they indeed leave
        for (auto male_iter{sites[site_idx].males.begin()};
                male_iter != sites[site_idx].males.end();
                )
        {
            if (uniform(rng_r) < 
                    male_iter->pr_depart(
                        n_airborne,
                        resources_airborne
                        ))
            {
                sites[site_idx].departing_males.push_back(
                    *male_iter
                        );

                std::swap(*male_iter, 
                        sites[site_idx].males.back());
                sites[site_idx].males.pop_back();
            } 
            else 
            {
                ++male_iter;
            }
        } // end for male iter()
        
        for (auto female_iter{sites[site_idx].females.begin()};
                female_iter != sites[site_idx].females.end();
                )
        {
            if (uniform(rng_r) < 
                    female_iter->pr_depart(
                        n_airborne,
                        resources_airborne
                        ))
            {
                sites[site_idx].departing_females.push_back(
                    *female_iter
                        );

                std::swap(*female_iter, 
                        sites[site_idx].females.back());
                sites[site_idx].females.pop_back();
            } 
            else 
            {
                ++female_iter;
            }
        } // end for male iter()
    } // end for site idx
} // end migrate()

// survival during flight
double Simulation::group_size_flight_survival(
        double const focal_resources,
        unsigned int const group_size)
{
    double pr_survive{
        par.pr_base_flight_survive * focal_resources / par.max_resources
            + 
            par.flight_survive_scale * 
                std::pow(static_cast<double>(group_size) /
                            par.flight_survive_max_size, 
                                par.flight_survive_power)};

    return(pr_survive);
} // end group_size_flight_survival

// movement from one site to another and paying costs
void Simulation::move_between_sites()
{
    // aux variable to store
    // current flight group size
    unsigned flight_group_size;

    // now we need to go backwards across sites
    // to move individuals over and calculate their
    // survival probabilities
    for (unsigned site_idx{par.n_sites - 1};
            site_idx > 0; 
            --site_idx)
    {
        flight_group_size = 
            static_cast<unsigned>(sites[site_idx - 1].departing_females.size()
                    +
                    sites[site_idx - 1].departing_males.size()
                    );

        // have females in a flight survive or not
        for (auto flying_female_iterator{
                sites[site_idx - 1].departing_females.begin()};
                flying_female_iterator != 
                    sites[site_idx - 1].departing_females.end();
                    // addition to iterator within loop
                )
        {
            // calculate mortality (1 - pr_survive)
            // hence we now need to like at numbers
            // sampled from uniform distributions that
            // are larger than
            if (uniform(rng_r) > 
                    group_size_flight_survival(
                        flying_female_iterator->resources,
                        flight_group_size
                        ))
            {
                // ok delete this female by 
                // swapping with the one at the end of stack
                std::swap(*flying_female_iterator, 
                        sites[site_idx - 1].departing_females.back());
                sites[site_idx - 1].departing_females.pop_back();
            }
            else
            {
                ++flying_female_iterator;
            }
        }
        
        // now have males on a flight surviving or not
        for (auto flying_male_iterator{
                sites[site_idx - 1].departing_males.begin()};
                flying_male_iterator != 
                    sites[site_idx - 1].departing_males.end();
                    // addition to iterator within loop
                )
        {
            // calculate mortality (1 - pr_survive)
            // hence we now need to like at numbers
            // sampled from uniform distributions that
            // are larger than
            if (uniform(rng_r) > 
                    group_size_flight_survival(
                        flying_male_iterator->resources,
                        flight_group_size
                        ))
            {
                // ok delete this female by 
                // swapping with the one at the end of stack
                std::swap(*flying_male_iterator, 
                        sites[site_idx - 1].departing_males.back());
                sites[site_idx - 1].departing_males.pop_back();
            }
            else
            {
                ++flying_male_iterator;
            }
        }

        // append surviving departing females 
        // from site at position
        // site_idx - 1 to females 
        // at position site_idx
        sites[site_idx].females.insert(
                sites[site_idx].females.end(),
                sites[site_idx - 1].departing_females.begin(),
                sites[site_idx - 1].departing_females.end());

        sites[site_idx - 1].departing_females.clear();

        sites[site_idx].males.insert(
                sites[site_idx].males.end(),
                sites[site_idx - 1].departing_males.begin(),
                sites[site_idx - 1].departing_males.end());
        
        sites[site_idx - 1].departing_males.clear();
    }
} // end move_between_sites()


void Simulation::calculate_stats_sites()
{
    // first set all stats to 0
    for (unsigned sex_idx{0};
            sex_idx < 2;
            ++sex_idx)
    {
        std::fill(
                stats_sites[sex_idx].begin(),
                stats_sites[sex_idx].end(),0);
    }

    for (unsigned site_idx{0};
            site_idx < par.n_sites;
            ++site_idx)
    {
        stats_sites[0][site_idx] = 
            sites[site_idx].males.size();
        
        stats_sites[0][site_idx] = 
            sites[site_idx].females.size();
    }

}

// write all the migration data
void Simulation::write_data_migration()
{
    // aux variable to keep track of resources
    double resources;
    for (unsigned site_idx{0}; site_idx < par.n_sites; ++site_idx)
    {
        for (auto male_iter{sites[site_idx].males.begin()};
                    male_iter != sites[site_idx].males.end();
                    ++male_iter)
        {
            resources += male_iter->resources;
        }

        data_file_migration << generation << ";"
            << ecological_time_idx << ";"
            << "male;" 
            << site_idx << ";"
            << sites[site_idx].males.size() << ";" 
            << resources << ";"
            << std::endl;
        
        resources = 0;
        
        for (auto female_iter{sites[site_idx].females.begin()};
                    female_iter != sites[site_idx].females.end();
                    ++female_iter)
        {
            resources += female_iter->resources;
        }

        
        data_file_migration << generation << ";"
            << ecological_time_idx << ";"
            << "female;" 
            << site_idx << ";"
            << sites[site_idx].females.size() << ";" 
            << resources << ";"
            << std::endl;
    }
} // write_data_migration

void Simulation::write_data()
{
    unsigned n_female{0};

    data_file << generation;





    data_file << std::endl;
}

void Simulation::write_data_headers()
{
    data_file << std::endl;

    data_file_migration << "generation;ecological_time;sex;site;n;resources;" << std::endl;
} // end write_data_headers()

void Simulation::write_parameters()
{
    data_file << std::endl << std::endl
        << "seed;" << seed << ";" << std::endl
        << "N;" << par.N << ";" << std::endl
        << "max_generation;" << par.max_generation << ";" << std::endl;
}
