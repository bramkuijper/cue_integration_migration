#ifndef _PARAMETERS_HPP_
#define _PARAMETERS_HPP_

#include <string>

// parameters and their default values
class Parameters
{
    public:
        // population size, which should be even number
        unsigned int N{5000};

        // duration of the simulation
        unsigned int max_generation{100};
        
        unsigned int n_sites{10};

        // time steps in season
        unsigned int max_season_time_steps{50};

        // print data each data_print_interval generation
        // to prevent getting massive data files
        unsigned int data_print_interval{10};

        std::string file_name{"sim_migration_cues"};

        double init_resources_site{10.0};
        double init_predator_density{10.0};


        double mu_an{0.02};
        double an_init{0.0};

        double mu_bn{0.02};
        double bn_init{0.0};
        
        double mu_ax{0.02};
        double ax_init{0.0};

        double mu_bx{0.02};
        double bx_init{0.0};
        
        double mu_at{0.02};
        double at_init{0.0};

        double mu_bt{0.02};
        double bt_init{0.0};
        
        double mu_ap{0.02};
        double ap_init{0.0};

        double mu_bp{0.02};
        double bp_init{0.0};

        double mu_anu{0.02};
        double anu_init{0.0};

        double mu_bnu{0.02};
        double bnu_init{0.0};
        
        double mu_axo{0.02};
        double axo_init{0.0};

        double mu_bxo{0.02};
        double bxo_init{0.0};

        double sdmu{0.02};

        // flight parameters
        double pr_base_flight_survive{0.1};
        double flight_survive_scale{0.7};
        double flight_survive_power{1.0};
        double flight_survive_max_size{50.0};

        double min_resources{0.0};

        double f{3.0}; // cost of flying
        double g{1.0}; // gain of foraging
};

#endif
