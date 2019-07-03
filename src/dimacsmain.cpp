/*
 * dimacsmain.cpp
 *
 *  Created on: 9-nov-2018
 *      Author: P. Sashittal
 */

#include "utils.h"
#include "dimacs.h"
#include "basetree.h"
#include <fstream>
#include <lemon/arg_parser.h>

int main(int argc, char** argv)
{

    typedef Digraph::NodeMap<int> IntNodeMap;
    typedef Digraph::NodeMap<double> DoubleNodeMap;
    typedef Digraph::ArcMap<int> IntArcMap;

    std::string varlist_filename, dimacs_filename, ptree_filename, host_filename;
    int rootLabel = 0;
    int nrUnsampledHosts = 0;
    int CoInf = 0;
    bool transTree = false;

    lemon::ArgParser ap(argc, argv);
    ap.refOption("r", "Root label (default: 0)", rootLabel);
    ap.refOption("u", "Number of unsampled hosts (default: 0)", nrUnsampledHosts);
    ap.refOption("t", "Transmission tree instead of host file", transTree);
    ap.refOption("k", "number of co-infection events (default: m-1)", CoInf);
    ap.other("<host> / <transmission_tree>");
    ap.other("<ptree>");
    ap.other("<output_dimacs_file>");
    ap.other("<output_varlist_file>");
    ap.parse();


    if (ap.files().size() != 4)
    {
        std::cerr << "Error1: expected <host>/<transmission_tree> <ptree> <output_ptree>" << std::endl;
        return 1;
    }

    host_filename = ap.files()[0];
    ptree_filename = ap.files()[1];
    dimacs_filename = ap.files()[2];
    varlist_filename = ap.files()[3];

    std::ifstream host_file(host_filename.c_str());
    if (!host_file.good())
    {
        std::cerr << "Error2: failed opening '" << host_filename << "' for reading" << std::endl;
        return 1;
    }

    std::ifstream ptree_file(ptree_filename.c_str());
    if (!ptree_file.good())
    {
        std::cerr << "Error3: failed opening '" << ptree_filename << "' for reading" << std::endl;
        return 1;
    }

    BaseTree B;
    try
    {
        if (!transTree)
        {
            BaseTree::readInputWithHosts(host_file, ptree_file, nrUnsampledHosts, true, B);
        }
        else
        {
            BaseTree::readInputWithTransmission(host_file, ptree_file, B);
        }
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error4: " << e.what() << std::endl;
        return 1;
    }
    host_file.close();
    ptree_file.close();
    
    Dimacs solver(B, rootLabel - 1, CoInf - 1, dimacs_filename.c_str(), varlist_filename.c_str());
    
    solver.init();
    
    solver.solveDimacs();
    
    return 0;
}
