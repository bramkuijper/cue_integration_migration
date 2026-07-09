#include <random>
#include "parameters.hpp"
#include "individual.hpp"

Individual::Individual(Parameters const &parms, bool const is_female) :
    is_female{is_female},
    an{0.5 * parms.an_init, 0.5 * parms.an_init},
    bn{0.5 * parms.bn_init, 0.5 * parms.bn_init},
    ax{0.5 * parms.ax_init, 0.5 * parms.ax_init},
    bx{0.5 * parms.bx_init, 0.5 * parms.bx_init},
    at{0.5 * parms.at_init, 0.5 * parms.at_init},
    bt{0.5 * parms.bt_init, 0.5 * parms.bt_init},
    anu{0.5 * parms.anu_init, 0.5 * parms.anu_init},
    bnu{0.5 * parms.bnu_init, 0.5 * parms.bnu_init}
{}

Individual::Individual(Individual const &other) :
    is_female{other.is_female},
    an{other.an[0],other.an[1]},
    bn{other.bn[0],other.bn[1]},
    ax{other.ax[0],other.ax[1]},
    bx{other.bx[0],other.bx[1]},
    at{other.at[0],other.at[1]},
    bt{other.bt[0],other.bt[1]},
    anu{other.anu[0],other.anu[1]},
    bnu{other.bnu[0],other.bnu[1]},
    resources{other.resources}
{}

// birth constructor
Individual::Individual(
        Individual const &mother,
        Individual const &father,
        std::mt19937 &rng_r,
        Parameters const &params) 
{
    // set up bernoulli distribution to 
    // randomly sample parental alleles
    std::bernoulli_distribution random_segregator{0.5};
    std::uniform_real_distribution <double> unif{0.0,1.0};
    std::normal_distribution <double> normal{0.0,1.0};

    is_female = unif(rng_r) < 0.5;

    an[0] = mother.an[random_segregator(rng_r)];
    an[1] = father.an[random_segregator(rng_r)];
    bn[0] = mother.bn[random_segregator(rng_r)];
    bn[1] = father.bn[random_segregator(rng_r)];

    ax[0] = mother.ax[random_segregator(rng_r)];
    ax[1] = father.ax[random_segregator(rng_r)];
    bx[0] = mother.bx[random_segregator(rng_r)];
    bx[1] = father.bx[random_segregator(rng_r)];
    
    at[0] = mother.at[random_segregator(rng_r)];
    at[1] = father.at[random_segregator(rng_r)];
    bt[0] = mother.bt[random_segregator(rng_r)];
    bt[1] = father.bt[random_segregator(rng_r)];
    
    ap[0] = mother.ap[random_segregator(rng_r)];
    ap[1] = father.ap[random_segregator(rng_r)];
    bp[0] = mother.bp[random_segregator(rng_r)];
    bp[1] = father.bp[random_segregator(rng_r)];
    
    anu[0] = mother.anu[random_segregator(rng_r)];
    anu[1] = father.anu[random_segregator(rng_r)];
    bnu[0] = mother.bnu[random_segregator(rng_r)];
    bnu[1] = father.bnu[random_segregator(rng_r)];

    // cycle through alleles
    // and mutate them
    for (unsigned int allele_idx{0}; allele_idx < 2; 
            ++allele_idx)
    {
        if (unif(rng_r) < params.mu_an) 
        {
            an[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_bn) 
        {
            bn[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_ax) 
        {
            ax[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_bx) 
        {
            bx[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_at) 
        {
            at[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_bt) 
        {
            bt[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_ap) 
        {
            ap[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_bp) 
        {
            bp[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_anu) 
        {
            anu[allele_idx] = normal(rng_r) * params.sdmu;
        }
        
        if (unif(rng_r) < params.mu_bnu) 
        {
            bnu[allele_idx] = normal(rng_r) * params.sdmu;
        }
    } // end for allele_idx

} // end birth constructor

double Individual::pr_fly(
                double const n, // density
                double const x, // resources
                double const t, // time
                double const p) // predator y/n
{
    double pr_fly_val_exp{0.0};

    pr_fly_val_exp += 0.5 * (1);

    return(1.0 / (1.0 + std::exp(-pr_fly_val_exp)));

} // end 

// overload the assignment operator
void Individual::operator=(Individual const &other)
{
    for (unsigned int allele_idx{0};
            allele_idx < 2;
            ++allele_idx)
    {
        an[allele_idx] = other.an[allele_idx];
        bn[allele_idx] = other.bn[allele_idx];
        ax[allele_idx] = other.ax[allele_idx];
        bx[allele_idx] = other.bx[allele_idx];
        at[allele_idx] = other.at[allele_idx];
        bt[allele_idx] = other.bt[allele_idx];
        anu[allele_idx] = other.anu[allele_idx];
        bnu[allele_idx] = other.bnu[allele_idx];
    }
    resources = other.resources;
    is_female = other.is_female;
} // end overloaded = operator
