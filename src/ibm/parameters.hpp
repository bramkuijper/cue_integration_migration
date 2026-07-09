#ifndef _PARAMETERS_HPP_
#define _PARAMETERS_HPP_

#include <string>

class Parameters
{
    public:
        // population size, which should be even number
        unsigned int N{5000};

        // duration of the simulation
        unsigned int max_generation{20000};

        double mu_an{0.02};
        double an_init{0.0};

        double mu_bn{0.02};
        double bn_init{0.02};
        
        double mu_ax{0.02};
        double ax_init{0.0};

        double mu_bx{0.02};
        double bx_init{0.02};
        
        double mu_at{0.02};
        double at_init{0.0};

        double mu_bt{0.02};
        double bt_init{0.02};
        
        double mu_ap{0.02};
        double ap_init{0.0};

        double mu_bp{0.02};
        double bp_init{0.02};

        double mu_anu{0.02};
        double anu_init{0.0};

        double mu_bnu{0.02};
        double bnu_init{0.02};

        double sdmu{0.02};
};

#endif
