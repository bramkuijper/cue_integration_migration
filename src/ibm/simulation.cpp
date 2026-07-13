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
    average_group_size_per_site(par.n_sites,0.0) // vector for group flight stats
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

    initialise_sites();

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

            write_data_migration();
        }

        // replace the current generation
        reproduce();

    } // end for unsigned int generation
   
    write_parameters();
        
} // end run_simulation()

// check whether simulation is extinct
bool Simulation::is_extinct()
{
    for (auto site_iterator{sites.begin()};
            site_iterator != sites.end();
            ++site_iterator)
    {
        if (site_iterator->has_positive_density())
        {
            return false;
        }
    }
    
    return true;
}

// produce new generation from picked site (the furthest)
void Simulation::reproduce_from_site(Site const &site)
{
    std::vector <double> male_resource_distribution{};
    std::vector <double> female_resource_distribution{};

    sites[0].juvenile_males.clear();
    sites[0].juvenile_females.clear();

    // get resource distribution among all the females
    // with sites towards the back having enormous advantage
    // over sites at the front
    for (auto female_iter{site.females.begin()};
            female_iter != site.females.end();
            ++female_iter)
    {
        female_resource_distribution.push_back(female_iter->resources);
    }

    // get resource distribution among all the males
    for (auto male_iter{site.males.begin()};
            male_iter != site.males.end();
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

    sites[0].juvenile_females.clear();
    sites[0].juvenile_males.clear();

    for (unsigned int newborn_idx{0};
            newborn_idx < par.N;
            ++newborn_idx)
    {
        male_idx = male_sampler(rng_r);
        female_idx = female_sampler(rng_r);

        Individual offspring(
                site.females[female_idx],
                site.males[male_idx],
                rng_r,
                par);

        if (offspring.is_female)
        {
            sites[0].juvenile_females.push_back(offspring);
        }
        else
        {
            sites[0].juvenile_males.push_back(offspring);
        }
    }
}

// reproduction
// only individuals in the final site 
// contribute to fitness
// with reproduction sexual and dependent
// on the remaining resources of each individual
void Simulation::reproduce()
{
    if (is_extinct())
    {
        write_data();
        write_parameters();
        exit(1);
    }

    // go backwards and the furthest possible site is allowed to reproduce
    for (unsigned site_idx{par.n_sites - 1};
            site_idx >= 0;
            --site_idx)
    {
        // remotest possible site positive density?
        // then reproduce from this site and break out of the loop
        if (sites[site_idx].has_positive_density())
        {
            reproduce_from_site(sites[site_idx]);
            break;
        }
    }

    assert(static_cast<unsigned>(sites[0].juvenile_males.size() +
            sites[0].juvenile_females.size()) == par.N);

    sites[0].females = sites[0].juvenile_females;
    sites[0].males = sites[0].juvenile_males;

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
            else 
            {
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
                female_iter->resources -= par.f;
                ++n_airborne;
                resources_airborne += female_iter->resources;
                female_iter->is_airborne = true;
            } 
            else
            {
                female_iter->resources += par.g;
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
    if (focal_resources < par.min_resources)
    {
        return(0);
    }

    double pr_survive{
        par.pr_base_flight_survive 
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

    for (unsigned site_idx{0};
            site_idx < par.n_sites;
            ++site_idx)
    {
        average_group_size_per_site[site_idx] = 0.0;
    }


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

        average_group_size_per_site[site_idx - 1] += flight_group_size;

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

                ++sites[site_idx - 1].n_mortality;
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
                
                ++sites[site_idx - 1].n_mortality;
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
            << average_group_size_per_site[site_idx] << ";" 
            << sites[site_idx].n_mortality << ";" 
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
            << average_group_size_per_site[site_idx] << ";" 
            << sites[site_idx].n_mortality << ";" 
            << resources << ";"
            << std::endl;
    }
} // write_data_migration

void Simulation::write_data()
{
    unsigned n_female{0};

    double mean_resources{0.0};
    double ss_resources{0.0};
    
    double mean_an{0.0};
    double mean_bn{0.0};
    double mean_ax{0.0};
    double mean_bx{0.0};
    double mean_at{0.0};
    double mean_bt{0.0};
    double mean_ap{0.0};
    double mean_bp{0.0};
    double mean_anu{0.0};
    double mean_bnu{0.0};
    double mean_axo{0.0};
    double mean_bxo{0.0};

    double ss_an{0.0};
    double ss_bn{0.0};
    double ss_ax{0.0};
    double ss_bx{0.0};
    double ss_at{0.0};
    double ss_bt{0.0};
    double ss_ap{0.0};
    double ss_bp{0.0};
    double ss_anu{0.0};
    double ss_bnu{0.0};
    double ss_axo{0.0};
    double ss_bxo{0.0};

    double x;

    unsigned n{0};

    for (auto site_iter{sites.begin()};
            site_iter != sites.end();
            ++site_iter)
    {
        n_female += static_cast<unsigned>(
                site_iter->females.size());

        n+= static_cast<unsigned>(
                site_iter->females.size() +
                site_iter->males.size()
                );

        for (auto female_iter{site_iter->females.begin()};
                female_iter != site_iter->females.end();
                ++female_iter)
        {
            x = female_iter->resources;
            mean_resources += x;
            ss_resources += x*x;
            
            x = 0.5 * (female_iter->an[0] + female_iter->an[1]);
            mean_an += x;
            ss_an += x*x;
            
            x = 0.5 * (female_iter->bn[0] + female_iter->bn[1]);
            mean_bn += x;
            ss_bn += x*x;
            
            x = 0.5 * (female_iter->ax[0] + female_iter->ax[1]);
            mean_ax += x;
            ss_ax += x*x;
            
            x = 0.5 * (female_iter->bx[0] + female_iter->bx[1]);
            mean_bx += x;
            ss_bx += x*x;
            
            x = 0.5 * (female_iter->at[0] + female_iter->at[1]);
            mean_at += x;
            ss_at += x*x;
            
            x = 0.5 * (female_iter->bt[0] + female_iter->bt[1]);
            mean_bt += x;
            ss_bt += x*x;
            
            x = 0.5 * (female_iter->ap[0] + female_iter->ap[1]);
            mean_ap += x;
            ss_ap += x*x;
            
            x = 0.5 * (female_iter->bp[0] + female_iter->bp[1]);
            mean_bp += x;
            ss_bp += x*x;
            
            x = 0.5 * (female_iter->anu[0] + female_iter->anu[1]);
            mean_anu += x;
            ss_anu += x*x;
            
            x = 0.5 * (female_iter->bnu[0] + female_iter->bnu[1]);
            mean_bnu += x;
            ss_bnu += x*x;
            
            x = 0.5 * (female_iter->axo[0] + female_iter->axo[1]);
            mean_axo += x;
            ss_axo += x*x;
            
            x = 0.5 * (female_iter->bxo[0] + female_iter->bxo[1]);
            mean_bxo += x;
            ss_bxo += x*x;
        }

        for (auto male_iter{site_iter->females.begin()};
                male_iter != site_iter->females.end();
                ++male_iter)
        {
            x = male_iter->resources;
            mean_resources += x;
            ss_resources += x*x;
            
            x = 0.5 * (male_iter->an[0] + male_iter->an[1]);
            mean_an += x;
            ss_an += x*x;
            
            x = 0.5 * (male_iter->bn[0] + male_iter->bn[1]);
            mean_bn += x;
            ss_bn += x*x;
            
            x = 0.5 * (male_iter->ax[0] + male_iter->ax[1]);
            mean_ax += x;
            ss_ax += x*x;
            
            x = 0.5 * (male_iter->bx[0] + male_iter->bx[1]);
            mean_bx += x;
            ss_bx += x*x;
            
            x = 0.5 * (male_iter->at[0] + male_iter->at[1]);
            mean_at += x;
            ss_at += x*x;
            
            x = 0.5 * (male_iter->bt[0] + male_iter->bt[1]);
            mean_bt += x;
            ss_bt += x*x;
            
            x = 0.5 * (male_iter->ap[0] + male_iter->ap[1]);
            mean_ap += x;
            ss_ap += x*x;
            
            x = 0.5 * (male_iter->bp[0] + male_iter->bp[1]);
            mean_bp += x;
            ss_bp += x*x;
            
            x = 0.5 * (male_iter->anu[0] + male_iter->anu[1]);
            mean_anu += x;
            ss_anu += x*x;
            
            x = 0.5 * (male_iter->bnu[0] + male_iter->bnu[1]);
            mean_bnu += x;
            ss_bnu += x*x;
            
            x = 0.5 * (male_iter->axo[0] + male_iter->axo[1]);
            mean_axo += x;
            ss_axo += x*x;
            
            x = 0.5 * (male_iter->bxo[0] + male_iter->bxo[1]);
            mean_bxo += x;
            ss_bxo += x*x;
        }
    } // end for site iter
    mean_an /= n;
    mean_bn /= n;
    mean_ax /= n;
    mean_bx /= n;
    mean_at /= n;
    mean_bt /= n;
    mean_ap /= n;
    mean_bp /= n;
    mean_anu /= n;
    mean_bnu /= n;
    mean_axo /= n;
    mean_bxo /= n;
    mean_resources /= n;
    
    double var_an = ss_an/n - mean_an * mean_an; 
    double var_bn = ss_bn/n - mean_bn * mean_bn; 
    double var_ax = ss_ax/n - mean_ax * mean_ax; 
    double var_bx = ss_bx/n - mean_bx * mean_bx; 
    double var_at = ss_at/n - mean_at * mean_at; 
    double var_bt = ss_bt/n - mean_bt * mean_bt; 
    double var_ap = ss_ap/n - mean_ap * mean_ap; 
    double var_bp = ss_bp/n - mean_bp * mean_bp; 
    double var_anu = ss_anu/n - mean_anu * mean_anu; 
    double var_bnu = ss_bnu/n - mean_bnu * mean_bnu; 
    double var_axo = ss_axo/n - mean_axo * mean_axo; 
    double var_bxo = ss_bxo/n - mean_bxo * mean_bxo; 
    double var_resources = ss_resources / n - 
        mean_resources * mean_resources;

    data_file << generation << ";"
        << mean_an << ";" 
        << mean_bn << ";" 
        << mean_ax << ";" 
        << mean_bx << ";" 
        << mean_at << ";" 
        << mean_bt << ";" 
        << mean_ap << ";" 
        << mean_bp << ";" 
        << mean_anu << ";" 
        << mean_bnu << ";" 
        << mean_axo << ";" 
        << mean_bxo << ";" 
        << mean_resources << ";" 
        << var_an << ";" 
        << var_bn << ";" 
        << var_ax << ";" 
        << var_bx << ";" 
        << var_at << ";" 
        << var_bt << ";" 
        << var_ap << ";" 
        << var_bp << ";" 
        << var_anu << ";" 
        << var_bnu << ";" 
        << var_axo << ";" 
        << var_bxo << ";" 
        << var_resources << ";" 
        << n << ";" 
        << n_female << ";" 
        << (n - n_female) << ";" 
        << std::endl;
} // end write_data

void Simulation::write_data_headers()
{
    data_file << "generation" << ";"
        << "mean_an" << ";" 
        << "mean_bn" << ";" 
        << "mean_ax" << ";" 
        << "mean_bx" << ";" 
        << "mean_at" << ";" 
        << "mean_bt" << ";" 
        << "mean_ap" << ";" 
        << "mean_bp" << ";" 
        << "mean_anu" << ";" 
        << "mean_bnu" << ";" 
        << "mean_axo" << ";" 
        << "mean_bxo" << ";" 
        << "mean_resources" << ";" 
        << "var_an" << ";" 
        << "var_bn" << ";" 
        << "var_ax" << ";" 
        << "var_bx" << ";" 
        << "var_at" << ";" 
        << "var_bt" << ";" 
        << "var_ap" << ";" 
        << "var_bp" << ";" 
        << "var_anu" << ";" 
        << "var_bnu" << ";" 
        << "var_axo" << ";" 
        << "var_bxo" << ";" 
        << "var_resources" << ";" 
        << "n" << ";"
        << "n_female" << ";"
        << "n_male" << ";"
        << std::endl;

    data_file_migration << "generation;ecological_time;sex;site;n;resources;" << std::endl;
} // end write_data_headers()

void Simulation::write_parameters()
{
    data_file << std::endl << std::endl
        << "seed;" << seed << ";" << std::endl
        << "N;" << par.N << ";" << std::endl
        << "max_generation;" << par.max_generation << ";" << std::endl
        << "max_season_time_steps;" << par.max_season_time_steps << ";" << std::endl
        << "n_sites;" << par.n_sites << ";" << std::endl
        << "data_print_interval;" << par.data_print_interval << ";" << std::endl
        << "init_resources_site;" << par.init_resources_site << ";" << std::endl
        << "init_predator_density;" << par.init_predator_density << ";" << std::endl
        << "mu_an;" << par.mu_an << ";" << std::endl
        << "an_init;" << par.an_init << ";" << std::endl
        << "mu_bn;" << par.mu_bn << ";" << std::endl
        << "bn_init;" << par.bn_init << ";" << std::endl
       
        << "mu_ax;" << par.mu_ax << ";" << std::endl
        << "ax_init;" << par.ax_init << ";" << std::endl
        << "mu_bx;" << par.mu_bx << ";" << std::endl
        << "bx_init;" << par.bx_init << ";" << std::endl
        
        << "mu_at;" << par.mu_at << ";" << std::endl
        << "at_init;" << par.at_init << ";" << std::endl
        << "mu_bt;" << par.mu_bt << ";" << std::endl
        << "bt_init;" << par.bt_init << ";" << std::endl

        << "mu_ap;" << par.mu_ap << ";" << std::endl
        << "ap_init;" << par.ap_init << ";" << std::endl
        << "mu_bp;" << par.mu_bp << ";" << std::endl
        << "bp_init;" << par.bp_init << ";" << std::endl
       
        << "mu_anu;" << par.mu_anu << ";" << std::endl
        << "anu_init;" << par.anu_init << ";" << std::endl
        << "mu_bnu;" << par.mu_bnu << ";" << std::endl
        << "bnu_init;" << par.bnu_init << ";" << std::endl

        << "mu_axo;" << par.mu_axo << ";" << std::endl
        << "axo_init;" << par.axo_init << ";" << std::endl
        << "mu_bxo;" << par.mu_bxo << ";" << std::endl
        << "bxo_init;" << par.bxo_init << ";" << std::endl
        
        << "sdmu;" << par.sdmu << ";" << std::endl
        << "pr_base_flight_survive;" << par.pr_base_flight_survive << ";" << std::endl
        << "flight_survive_scale;" << par.flight_survive_scale << ";" << std::endl
        << "flight_survive_power;" << par.flight_survive_power << ";" << std::endl
        << "flight_survive_max_size;" << par.flight_survive_max_size << ";" << std::endl
        << "min_resources;" << par.min_resources << ";" << std::endl
        << "f;" << par.f << ";" << std::endl
        << "g;" << par.g << ";" << std::endl;
}
