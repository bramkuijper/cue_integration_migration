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
    individuals{par.N, Individual(par)} // initialize all males
{}

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

        // replace the current generation
        reproduce();

    } // end for unsigned int generation
   
    write_parameters();
        
} // end run_simulation()

void Simulation::reproduce()
{
    if (individuals.size() < 1)
    {
        write_data();
        write_parameters();
        exit(1);
    }

} // end reproduce()

void Simulation::write_data()
{
    double mean_t{0.0};
    double ss_t{0.0};
    double mean_p{0.0};
    double ss_p{0.0};
    double sum_pt{0.0};

    double t,p;

    for (auto male_iterator{males.begin()};
            male_iterator != males.end();
            ++male_iterator)
    {
        t = 0.5 * (male_iterator->t[0] + male_iterator->t[1]);
        p = 0.5 * (male_iterator->p[0] + male_iterator->p[1]);
        mean_t += t;
        mean_p += p;
        ss_t += t*t;
        ss_p += p*p;
        sum_pt += p*t;
    }
    
    for (auto female_iterator{females.begin()};
            female_iterator != females.end();
            ++female_iterator)
    {
        t = 0.5 * (female_iterator->t[0] + female_iterator->t[1]);
        p = 0.5 * (female_iterator->p[0] + female_iterator->p[1]);
        mean_t += t;
        mean_p += p;
        ss_t += t*t;
        ss_p += p*p;
        sum_pt += p*t;
    }

    unsigned int n = static_cast<unsigned int>(males.size() + females.size());

    mean_t /= n;
    mean_p /= n;

    double var_t = ss_t / n - mean_t * mean_t;
    double var_p = ss_p / n - mean_p * mean_p;

    double cov_pt = sum_pt / n - mean_p * mean_t;

    data_file << generation << ";" 
        << mean_t << ";" 
        << mean_p << ";" 
        << var_t << ";" 
        << var_p << ";"
        << cov_pt << ";"
        << fraction_females_survive << ";"
        << fraction_males_survive << std::endl;
} // end write_data()

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
        << "max_generation;" << par.max_generation << ";" << std::endl
        << "n_males_sampled;" << par.n_males_sampled << ";" << std::endl
        << "mu_t;" << par.mu_t << ";" << std::endl
        << "mu_p;" << par.mu_p << ";" << std::endl
        << "max_mut_p;" << par.max_mut_p << ";" << std::endl
        << "max_mut_t;" << par.max_mut_t << ";" << std::endl
        << "biast;" << par.biast << ";" << std::endl
        << "a;" << par.a << ";" << std::endl
        << "b;" << par.b << ";" << std::endl
        << "c;" << par.c << ";" << std::endl
        << "only_positive;" << par.only_positive << ";" << std::endl
        << "bias_negative;" << par.bias_negative << ";" << std::endl
        << "init_t;" << par.init_t << ";" << std::endl
        << "init_p;" << par.init_p << ";" << std::endl;
}
