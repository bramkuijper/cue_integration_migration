#include <string>
#include <cassert>
#include "simulation.hpp"

int main(int argc, char **argv)
{
    Parameters params{}; // start parameters with all the default settings

    params.file_name = argv[1]; // then override defaults with command-line args
    params.n_sites = std::stoul(argv[2]); // then override defaults with command-line args

    assert(params.n_sites > 1);

    Simulation sim{params};

    sim.run();

    return 0;
}
