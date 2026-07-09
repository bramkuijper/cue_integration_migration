#ifndef _SITE_HPP_
#define _SITE_HPP_

#include <vector>
#include "individual.hpp"
#include "parameters.hpp"

class Site
{
    public:
        std::vector<Individual> females;
        std::vector<Individual> males;
        std::vector<Individual> departing_females;
        std::vector<Individual> departing_males;

        double resources{0.0};
        double predator_density{0.0};

        // initiation constructor
        Site(
                unsigned const nmales, 
                unsigned const nfemales, 
                Parameters const &params
                );

        // 
        Site(Site const &other);

        void operator=(Site const &other);
};

#endif
