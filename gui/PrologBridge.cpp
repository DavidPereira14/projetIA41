#include "PrologBridge.h"
#include <SWI-cpp.h>
#include <iostream>

PrologBridge::PrologBridge()
{
    // Initialisation SWI-Prolog
    char *argv[] = { (char*)"swipl", (char*)"-q", nullptr };
    PL_initialise(2, argv);

    try {
        // Charger le fichier Prolog
        PlCall("consult('../../moteur/vide.pl')");
    } catch(const PlException &ex) {
        std::cerr << "Erreur Prolog: " << (char*)ex << std::endl;
    }
}

PrologBridge::~PrologBridge()
{
    PL_halt(0);
}

std::vector<std::string> PrologBridge::getBoard()
{
    std::vector<std::string> result;

    try {
        PlTermv args(1);
        PlQuery q("plateau_actuel", args);

        while (q.next_solution())
        {
            char *str;
            if (PL_get_chars(args[0], &str, CVT_ALL | CVT_WRITE))
            {
                result.push_back(std::string(str));
            }
        }
    } catch(const PlException &ex) {
        std::cerr << "Erreur Prolog: " << (char*)ex << std::endl;
    }

    return result;
}


