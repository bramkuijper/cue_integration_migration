#include "site.hpp"

// group creation constructor
Site::Site(
        unsigned const nfemale, 
        unsigned const nmale, 
        Parameters const &params) :
    females(nfemale, Individual(params, true)),
    males(nmale, Individual(params, false)),
    resources{params.init_resources_site},
    predator_density{params.init_predator_density}
{}

// copy constructor
Site::Site(Site const &other) :
    females{other.females},
    males{other.males},
    resources(other.resources),
    predator_density{other.predator_density}
{}

void Site::operator=(Site const &other)
{
    females = other.females;
    males = other.males;
    resources = other.resources;
    predator_density = other.predator_density;
}
