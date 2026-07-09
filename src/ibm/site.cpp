#include "site.hpp"

// group creation constructor
Site::Site(
        unsigned const nfemale, 
        unsigned const nmale, 
        Parameters const &params) :
    females(nfemale, Individual(params, true)),
    males(nmale, Individual(params, false)),
    resources{params.init_resources_site}
{}

// copy constructor
Site::Site(Site const &other) :
    resources(other.resources)
{}

void Site::operator=(Site const &other)
{
    members = other.members;
    resources = other.resources;
}
