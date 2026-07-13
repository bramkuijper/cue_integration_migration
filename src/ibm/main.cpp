#include <string>
#include <cassert>
#include "simulation.hpp"

int main(int argc, char **argv)
{
    Parameters params{}; // start parameters with all the default settings

    // then override defaults with command-line args
    params.file_name = argv[1]; 

    params.n_sites = static_cast<unsigned>(
            std::stoul(argv[2])); 
    
    assert(params.n_sites > 1);

    params.max_generation = static_cast<unsigned>(
            std::stoul(argv[3]));
    
    params.max_season_time_steps = static_cast<unsigned>(
            std::stoul(argv[4]));

    params.mu_an = std::stod(argv[5]);
    params.mu_bn = std::stod(argv[6]);

    params.mu_ax = std::stod(argv[7]);
    params.mu_bx = std::stod(argv[8]);
    
    params.mu_at = std::stod(argv[9]);
    params.mu_bt = std::stod(argv[10]);
    
    params.mu_ap = std::stod(argv[11]);
    params.mu_bp = std::stod(argv[12]);
    
    params.mu_anu = std::stod(argv[13]);
    params.mu_bnu = std::stod(argv[14]);
    
    params.mu_axo = std::stod(argv[15]);
    params.mu_bxo = std::stod(argv[16]);

    params.pr_base_flight_survive = std::stod(argv[17]);
    params.flight_survive_scale = std::stod(argv[18]);
    params.flight_survive_power = std::stod(argv[19]);
    params.f = std::stod(argv[20]);
    params.g = std::stod(argv[21]);

    Simulation sim{params};

    sim.run();

    return 0;
}
