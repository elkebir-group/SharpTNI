/*
 * ilpsolver.cpp
 *
 *  Created on: 9-nov-2018
 *      Author: P. Sashittal
 */

#include "dimacs.h"

Dimacs::Dimacs(const BaseTree& T,
               const int rootLabel,
               int maxk,
               std::string psol,
               std::string varlist)
    : Sankoff(T, rootLabel)
    , _Rv(_T.tree())
    , _Re(_T.tree())
    , _Cm(_T.tree())
    , _C()
    , _maxk(maxk)
    , _solFile(psol.c_str())
    , _varFile(varlist.c_str())
{
}

bool Dimacs::solveDimacs()
{
    // check if root label is out of bound
    // check if root host is possible
    if (_rootLabel >= _T.getNrHost() || _rootLabel < -1)
    {
        std::cerr << "root label is out of bounds" << std::endl;
        return false;
    }
    
    if (_rootLabel > -1)
    {
        if (_T.getEntTime(_rootLabel) > _T.getTime(root()) || _T.getRemTime(_rootLabel) < _T.getTime(root()))
        {
            std::cerr << "value of root label is " << _rootLabel + 1 << std::endl;
            std::cerr << "not a feasible root" << std::endl;
            return false;
        }
    }
    
    // check if leaf labels are feasible
    for (NodeIt vi(_T.tree()); vi != lemon::INVALID; ++vi)
    {
        if (lemon::countOutArcs(_T.tree(), vi) == 0)
        {
            int lhost = _T.getHostLabel(vi);
            if (_T.getEntTime(lhost) > _T.getTime(vi) || _T.getRemTime(lhost) < _T.getTime(vi))
            {
                std::cerr << "label of leaf " << _T.getName(vi) << " is wrong" << std::endl;
                return false;
            }
        }
    }
    

    if (countSankoff() == 0) {
        return false;
    }
    
    initVariables();
    
    writeVariableList();

    Node root = _T.root();
    int nrInfectedHosts = _T.getNrHost();
    
    BoolVector rootFeasibleLabels(nrInfectedHosts, false);
    
    if (_rootLabel == -1)
    {
        uint64_t min_cost = *std::min_element(_M[root].begin(), _M[root].end());
        
        std::cout << "feasible roots are: ";
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            if (_M[root][s] == min_cost)
            {
                std::cout << s + 1 << ", ";
                rootFeasibleLabels[s] = true;
            }
        }
    }
    else
    {
        if (_M[root][_rootLabel] == std::numeric_limits<uint64_t>::max())
        {
            std::cerr << "no solutions" << std::endl;
            return false;
        }
        else
        {
            rootFeasibleLabels[_rootLabel] = true;
        }
    }
    
    // write independent support
    writeIndependentSupport();
    std::cout << "independent support written" << std::endl;
    
    // vertex label constraints
    writeOneHotConstraints(root, rootFeasibleLabels);
    std::cout<< "root label constraints written" << std::endl;
    
    writeVertexConstraints(root, rootFeasibleLabels);
    std::cout<< "vertex constraints written" << std::endl;
  
  
    // infection edge constraints
    writeInfectionEdgeConstraints(root);
    std::cout<< "Infection edge constraints written" << std::endl;
    
    writeNonInfectionEdgeConstraints(root);
    std::cout<< "non-infection edge constraints written" << std::endl;
    
    // clique constraints
    writeFeasibleCliqueConstraints();
    std::cout<< "feasible clique constraints written" << std::endl;
    
    writeNotFeasibleCliqueConstraints();
    std::cout<< "non feasible clique constraints written" << std::endl;
    
    writeNotFeasibleInfectionConstraints(root);
    std::cout<< "not feasible infection constraints written" << std::endl;
    
    writeInfectionCliqueConstraints();
    std::cout<< "infection clique constraints written" << std::endl;
    //writeInfectionTimeConstraints();
    
    writeNonEmptyCliqueConstraints();
    std::cout<< "non empty clique constraints written" << std::endl;
    
    writeMinimalPartitionConstraints();
    std::cout<< "minimal partition constraints written" << std::endl;
  
    writeCliqueMasterConstraints();
    std::cout<< "clique master constraints written" << std::endl;
    
    writeCliqueOrderingConstraints();
    std::cout << "clique ordering constraints written" << std::endl;
    
    _solFile.close();
    
    return true;
}

void Dimacs::initVariables()
{
    if (!_solFile.good())
    {
        std::cerr << "error: failed opening solution file" << std::endl;
    }
    if (!_varFile.good())
    {
        std::cerr << "error: failed opening solution file" << std::endl;
    }
    
    const Digraph& G = tree();
    const int nrInfectedHosts = _T.getNrHost();
    
    // fix maximum k value (different from co-infection value by 1)
    if (_maxk == -1)
    {
        _maxk = nrInfectedHosts - 2;
    }
    
    // init vertex variables
    for (NodeIt vi(G); vi != lemon::INVALID; ++vi)
    {
        _Rv[vi] = UInt64Vector(nrInfectedHosts);
    }
    
    int rank = 1;
    
    for (NodeIt vi(G); vi != lemon::INVALID; ++vi)
    {
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            if (_W[vi][s] > 0)
            {
                _Rv[vi][s] = rank;
                ++rank;
            }
            else
            {
                _Rv[vi][s] = 0;
            }
        }
    }
  

    // init edge variables
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        Node vi = G.source(eij);
        Node vj = G.target(eij);

        bool feasibleInfection = false;
        
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            for (int t = 0; t < nrInfectedHosts; ++t)
            {
                if (s != t && _Rv[vi][s] > 0 && _Rv[vj][t] > 0)
                {
                    feasibleInfection = true;
                }
            }
        }
        
        if (feasibleInfection)
        {
            _Re[eij] = UInt64Vector(_maxk + 1);
            
            for (int k = 0; k <= _maxk; ++k)
            {
                _Re[eij][k] = rank;
                ++rank;
            }
        }
    }
    
    //init edge-pair variables
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        for (ArcIt ekl(G); ekl != lemon::INVALID; ++ekl)
        {
            if (arc_index(eij) < arc_index(ekl))
            {
                bool feasiblepair = false;
                
                Node vi = G.source(eij);
                Node vj = G.target(eij);
                Node vk = G.source(ekl);
                Node vl = G.target(ekl);
                
                double ti = time(G.source(eij));
                double tj = time(G.target(eij));
                double tk = time(G.source(ekl));
                double tl = time(G.target(ekl));
                
                if (ti <= tl && tj >= tk)
                {
                    for (int s = 0; s < nrInfectedHosts; ++s)
                    {
                        for (int t = 0; t < nrInfectedHosts; ++t)
                        {
                            if (s!=t && _Rv[vi][s] > 0 && _Rv[vk][s] > 0 && _Rv[vj][t] > 0 && _Rv[vl][t] > 0)
                            {
                                feasiblepair = true;
                            }
                        }
                    }
                }
                
                if (feasiblepair)
                {
                    _C.push_back(std::make_pair(rank, std::make_pair(arc_index(eij), arc_index(ekl))));
                    
                    ++rank;
                }
            }
        }
    }
    
    // init clique master variables
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        if (_Re[eij].size() > 0)
        {
            _Cm[eij] = rank;
            ++rank;
        }
    }

    _numVar = rank;
    
}

void Dimacs::writeVariableList()
{
    const Digraph& G = tree();
    const int nrInfectedHosts = _T.getNrHost();
    
    int rank = 1;
    
    for (NodeIt vi(G); vi != lemon::INVALID; ++vi)
    {
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            if (_Rv[vi][s] > 0)
            {
                _varFile << _Rv[vi][s] << " " << _T.getName(vi) << " " << s << " (v, s)"<< std::endl;
                ++rank;
            }
        }
    }
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        if (_Re[eij].size() > 0)
        {
            for (int k = 0; k <= _maxk; ++k)
            {
                _varFile << rank << " " << _T.getArcIndex(eij) << " " << k << " (e, k)"<< std::endl;
                ++rank;
            }
        }
    }
    
    int nCVariables = _C.size();
    
    for (int i = 0; i < nCVariables; ++i)
    {
        _varFile << _C[i].first << " " << _C[i].second.first << " " << _C[i].second.second << " (e, e)" << std::endl;
        ++rank;
    }
    
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        if (_Re[eij].size() > 0)
        {
            _varFile << rank << " " << _T.getArcIndex(eij) << " (e)" << std::endl;
            ++rank;
        }
    }
    
    
    _varFile.close();
}

void Dimacs::writeIndependentSupport()
{
    const Digraph& G = tree();
    const int nrInfectedHosts = _T.getNrHost();
    
    int rank = 1;
    
    _solFile << "c ind ";
    
    for (NodeIt vi(G); vi != lemon::INVALID; ++vi)
    {
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            if (_Rv[vi][s] > 0)
            {
                _solFile << getVarIndex(vi, s) << " ";
                ++rank;
                
                if ((rank - 1)%10 == 0)
                {
                    _solFile << "0" << std::endl;
                    _solFile << "c ind ";
                }
            }
        }
    }
    
    _solFile << " 0" << std::endl;
}


void Dimacs::writeVertexConstraints(Node u, BoolVector feasibleLabels)
{
    const Digraph& G = tree();
    const int nrInfectedHosts = _T.getNrHost();
    
    assert(feasibleLabels.size() == nrInfectedHosts);

    if (lemon::countOutArcs(G, u) == 0)
    {
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            if (feasibleLabels[s])
            {
                _solFile << getVarIndex(u, s) << " 0" << std::endl;
            }
        }
    }
    
    for (Digraph::OutArcIt a(G,u); a != lemon::INVALID; ++a)
    {
        Node v = G.target(a);

        BoolVector childFeasibleLabels(nrInfectedHosts, false);
    
        for (int s = 0; s < nrInfectedHosts; ++s)
        {
            if (feasibleLabels[s])
            {
                uint64_t minimum_cost = computeMinCost(v, s);
                
                IntVector childPaths;
                
                for (int t = 0; t < nrInfectedHosts; ++t)
                {
                    uint64_t cost_vt = computeCost(v, t, s);
                    
                    if (cost_vt == minimum_cost)
                    {
                        // make this label feasible for child v
                        childFeasibleLabels[t] = true;
                        
                        // add this label to child path
                        childPaths.push_back(t);
                    }
                }
                
                int npaths = childPaths.size();
                
                // write in file
                for (int i = 0; i < npaths; ++i)
                {
                    for (int j = 0; j < i; ++j)
                    {
                        _solFile << -getVarIndex(u, s) << " " << -getVarIndex(v, childPaths[i]) << " " << -getVarIndex(v, childPaths[j]) << " 0" << std::endl;
                    }
                }
                
                _solFile << -getVarIndex(u, s);
                for (int i = 0; i < npaths; ++i)
                {
                    _solFile << " " << getVarIndex(v, childPaths[i]);
                }
                _solFile << " 0" << std::endl;
            }
        }
        
        writeVertexConstraints(v, childFeasibleLabels);
    }
    
}

void Dimacs::writeOneHotConstraints(Node u, BoolVector feasbileLabels)
{
    const int nrInfectedHosts = _T.getNrHost();
    assert(feasbileLabels.size() == nrInfectedHosts);
    
    IntVector feasiblePaths;
    
    for (int s = 0; s < nrInfectedHosts; ++s)
    {
        if (feasbileLabels[s])
        {
            feasiblePaths.push_back(s);
        }
    }
    
    int nfeasible = feasiblePaths.size();
    
    for (int i = 0; i < nfeasible; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            _solFile << -getVarIndex(u, feasiblePaths[i]) << " " << -getVarIndex(u, feasiblePaths[j]) << " 0" << std::endl;
        }
    }
    
    
    _solFile << getVarIndex(u, feasiblePaths[0]);
    for (int i = 1; i < nfeasible; ++i)
    {
        _solFile << " " << getVarIndex(u, feasiblePaths[i]);
    }
    _solFile << " 0" << std::endl;
}

void Dimacs::writeInfectionEdgeConstraints(Node u)
{
    const Digraph& G = tree();

    IntVector feasibleSources = getFeasibleLables(u);
    
    int nsources = feasibleSources.size();
    
    for (Digraph::OutArcIt a(G,u); a != lemon::INVALID; ++a)
    {
        Node v = G.target(a);
        
        IntVector feasibleTargets = getFeasibleLables(v);
        
        int ntargets = feasibleTargets.size();
        
        for (int i = 0; i < nsources; ++i)
        {
            for (int j =0; j < ntargets; ++j)
            {
                if (feasibleSources[i] != feasibleTargets[j])
                {
                    for (int k = 0; k <= _maxk; ++k)
                    {
                        for (int l = 0; l < k; ++l)
                        {
                            _solFile << -getVarIndex(u, feasibleSources[i]) << " " << -getVarIndex(v, feasibleTargets[j]) << " " << -getVarIndex(a, k) << " " << -getVarIndex(a, l) << " 0" << std::endl;
                        }
                    }
                    
                    _solFile << -getVarIndex(u, feasibleSources[i]) << " " << -getVarIndex(v, feasibleTargets[j]);
                    for (int k = 0; k <= _maxk; ++k)
                    {
                        _solFile << " " << getVarIndex(a, k);
                    }
                    _solFile << " 0" << std::endl;
                }
            }
        }
        writeInfectionEdgeConstraints(v);
    }
}

void Dimacs::writeNonInfectionEdgeConstraints(Node u)
{
    const Digraph& G = tree();
    
    IntVector feasibleSources = getFeasibleLables(u);
    
    int nsources = feasibleSources.size();
    
    for (Digraph::OutArcIt a(G,u); a != lemon::INVALID; ++a)
    {
        Node v = G.target(a);
        
        if (_Re[a].size() > 0)
        {
            IntVector feasibleTargets = getFeasibleLables(v);
            
            int ntargets = feasibleTargets.size();
            
            for (int i = 0; i < nsources; ++i)
            {
                for (int j =0; j < ntargets; ++j)
                {
                    if (feasibleSources[i] == feasibleTargets[j])
                    {
                        for (int k = 0; k <= _maxk; ++k)
                        {
                            _solFile << -getVarIndex(u, feasibleSources[i]) << " " << -getVarIndex(v, feasibleTargets[j]) << " " << -getVarIndex(a, k) << " 0" << std::endl;
                        }
                        
                        _solFile << -getVarIndex(u, feasibleSources[i]) << " " << -getVarIndex(v, feasibleTargets[j]) << " " << -getVarIndex(a) << " 0" << std::endl;
                    }
                }
            }
        }
        writeNonInfectionEdgeConstraints(v);
    }
}

void Dimacs::writeNotFeasibleInfectionConstraints(Node u)
{
    const Digraph& G = tree();
    
    IntVector feasibleSources = getFeasibleLables(u);
    
    int nsources = feasibleSources.size();
    
    for (Digraph::OutArcIt a(G,u); a != lemon::INVALID; ++a)
    {
        Node v = G.target(a);
        
        IntVector feasibleTargets = getFeasibleLables(v);
        
        int ntargets = feasibleTargets.size();
        
        for (int i = 0; i < nsources; ++i)
        {
            for (int j = 0; j < ntargets; ++j)
            {
                if (feasibleSources[i] == feasibleTargets[j])
                {
                    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
                    {
                        if (getVarIndex(a, eij) != 0)
                        {
                            _solFile << -getVarIndex(u, feasibleSources[i]) << " " << -getVarIndex(v, feasibleTargets[j]) << " " << -getVarIndex(a, eij) << " 0" << std::endl;
                        }
                    }
                }
            }
        }
        
        writeNotFeasibleInfectionConstraints(v);
    }
}

void Dimacs::writeInfectionTimeConstraints()
{
    const Digraph& G = tree();

    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        Node vi = G.source(eij);
        Node vj = G.target(eij);
        
        for (ArcIt ekl(G); ekl != lemon::INVALID; ++ekl)
        {
            Node vk = G.source(ekl);
            Node vl = G.target(ekl);
            
            if (arc_index(eij) < arc_index(ekl))
            {
                double ti = time(vi);
                double tj = time(vj);
                double tk = time(vk);
                double tl = time(vl);
                
                if ((ti > tl) || (tj < tk))
                {
                    for (int k = 0; k <= _maxk; ++k)
                    {
                        _solFile << -getVarIndex(eij, k) << " " << -getVarIndex(ekl, k) << " 0" << std::endl;
                    }
                }
            }
        }
    }
}

void Dimacs::writeInfectionCliqueConstraints()
{
    const Digraph& G = tree();
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        for (ArcIt ekl(G); ekl != lemon::INVALID; ++ekl)
        {
            if (arc_index(eij) < arc_index(ekl) && _Re[eij].size() > 0 && _Re[ekl].size() > 0)
            {
                if (getVarIndex(eij, ekl) != 0)
                {
                    for (int k = 0; k <= _maxk; ++k)
                    {
                        _solFile << getVarIndex(eij, ekl) << " " << -getVarIndex(eij, k) << " " << -getVarIndex(ekl, k) << " 0" << std::endl;
                    }
                    
                }
                else
                {
                    for (int k = 0; k <= _maxk; ++k)
                    {
                        _solFile << -getVarIndex(eij, k) << " " << -getVarIndex(ekl, k) << " 0" << std::endl;
                    }
                }
                /*
                if (ti <= tl && tj >= tk)
                {
                    for (int s = 0; s < nrInfectedHosts; ++s)
                    {
                        for (int t = 0; t < nrInfectedHosts; ++t)
                        {
                            if (s != t)
                            {
                                if (_Rv[vi][s] > 0 && _Rv[vk][t] > 0)
                                {
                                    // write something
                                    for (int k = 0; k <= _maxk; ++k)
                                    {
                                        _solFile << -getVarIndex(vi, s) << " " << -getVarIndex(vk, t) << " " << -getVarIndex(eij, k) << " " << -getVarIndex(ekl, k) << std::endl;
                                    }
                                }
                                
                                if (_Rv[vj][s] > 0 && _Rv[vl][t] > 0)
                                {
                                    // write something
                                    for (int k = 0; k <= _maxk; ++k)
                                    {
                                        _solFile << -getVarIndex(vj, s) << " " << -getVarIndex(vl, t) << " " << -getVarIndex(eij, k) << " " << -getVarIndex(ekl, k) << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
                */
            }
        }
    }
}

void Dimacs::writeNonEmptyCliqueConstraints()
{
    const Digraph& G = tree();
    
    for (int k = 0; k <= _maxk; ++k)
    {
        for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
        {
            if (_Re[eij].size() > 0)
            {
                _solFile << getVarIndex(eij, k) << " ";
            }
        }
        _solFile << " 0" << std::endl;
    }
}


void Dimacs::writeNotFeasibleCliqueConstraints()
{
    const Digraph& G = tree();
    const int nrInfectedHosts = _T.getNrHost();
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        for (ArcIt ekl(G); ekl != lemon::INVALID; ++ekl)
        {
            if (arc_index(eij) < arc_index(ekl))
            {
                if (getVarIndex(eij, ekl) != 0)
                {
                    Node vi = G.source(eij);
                    Node vj = G.target(eij);
                    
                    Node vk = G.source(ekl);
                    Node vl = G.target(ekl);
                    
                    for (int s = 0; s < nrInfectedHosts; ++s)
                    {
                        for (int t = 0; t < nrInfectedHosts; ++t)
                        {
                            if (s != t)
                            {
                                if (_Rv[vi][s] > 0 && _Rv[vk][t] > 0)
                                {
                                    _solFile << -getVarIndex(vi, s) << " " << -getVarIndex(vk, t) << " " << -getVarIndex(eij, ekl) << " 0" << std::endl;
                                }
                                
                                if (_Rv[vj][s] > 0 && _Rv[vl][t] > 0)
                                {
                                    _solFile << -getVarIndex(vj, s) << " " << -getVarIndex(vl, t) << " " << -getVarIndex(eij, ekl) << " 0" << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Dimacs::writeFeasibleCliqueConstraints()
{
    const Digraph& G = tree();
    const int nrInfectedHosts = _T.getNrHost();
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        for (ArcIt ekl(G); ekl != lemon::INVALID; ++ekl)
        {
            if (arc_index(eij) < arc_index(ekl))
            {
                if (getVarIndex(eij, ekl) != 0)
                {
                    Node vi = G.source(eij);
                    Node vj = G.target(eij);
                    
                    Node vk = G.source(ekl);
                    Node vl = G.target(ekl);
                    
                    for (int s = 0; s < nrInfectedHosts; ++s)
                    {
                        for (int t = 0; t < nrInfectedHosts; ++t)
                        {
                            if (s != t)
                            {
                                if (_Rv[vi][s] > 0 && _Rv[vj][t] > 0 && _Rv[vk][s] > 0 && _Rv[vl][t] > 0)
                                {
                                    _solFile << -getVarIndex(vi, s) << " " << -getVarIndex(vj, t) << " " << -getVarIndex(vk, s) << " " << -getVarIndex(vl, t) << " " << getVarIndex(eij, ekl) << " 0" << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Dimacs::writeMinimalPartitionConstraints()
{
    const Digraph& G = tree();
    DoubleArcPairVector ArcList;
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        if (_Re[eij].size() > 0)
        {
            Node vj = G.target(eij);
            ArcList.push_back(std::make_pair(_T.getTime(vj), eij));
        }
    }
    
    sort(ArcList.begin(), ArcList.end());
    
    int nList = ArcList.size();
    
    for (int i = 0; i < nList; ++i)
    {
        Arc a = ArcList[i].second;
        
        for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
        {
            if (getVarIndex(eij, a) != 0 && _Re[eij].size() > 0 && _Re[a].size() > 0)
            {
                for (int k = 0; k <= _maxk; ++k)
                {
                    for (int j = 0; j < i; ++j)
                    {
                        Arc b = ArcList[j].second;
                        
                        if (getVarIndex(eij, b) != 0)
                        {
                            _solFile << getVarIndex(eij, b) << " ";
                        }
                    }
                    _solFile << -getVarIndex(eij, a) << " " << -getVarIndex(a, k) << " " << getVarIndex(eij, k) << " 0" << std::endl;
                }
            }
        }
    }
}

void Dimacs::writeCliqueMasterConstraints()
{
    const Digraph& G = tree();
    DoubleArcPairVector ArcList;
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        if (_Re[eij].size() > 0)
        {
            Node vj = G.target(eij);
            ArcList.push_back(std::make_pair(_T.getTime(vj), eij));
        }
    }
    
    sort(ArcList.begin(), ArcList.end());
    
    int nList = ArcList.size();
    
    // find out if an edge is a clique master
    for (int k = 0; k <= _maxk; ++k)
    {
        for (int j = 0; j < nList; ++j)
        {
            Arc a = ArcList[j].second;
            
            if (_Re[a].size() > 0)
            {
                for (int i = 0; i < j; ++i)
                {
                    Arc b = ArcList[i].second;
                    
                    if (_Re[b].size() > 0)
                    {
                        _solFile << getVarIndex(b, k) << " ";
                    }
                }
                _solFile << -getVarIndex(a, k) << " " << getVarIndex(a) << " 0" << std::endl;
            }
        }
    }
    
    // find out if an edge is not a clique master
    for (int k = 0; k <= _maxk; ++k)
    {
        for (int j = 0; j < nList; ++j)
        {
            Arc a = ArcList[j].second;
            
            if (_Re[a].size() > 0)
            {
                for (int i = 0; i < j; ++i)
                {
                    Arc b = ArcList[i].second;
                    
                    if (_Re[b].size() > 0)
                    {
                        _solFile << -getVarIndex(b, k) << " " << -getVarIndex(a, k) << " " << -getVarIndex(a) << " 0" << std::endl;
                    }
                }
            }
        }
    }
    
    /*
    // no infection is not a clique (already done)
    */
}

void Dimacs::writeCliqueOrderingConstraints()
{
    const Digraph&G = tree();
    DoubleArcPairVector ArcList;
    
    for (ArcIt eij(G); eij != lemon::INVALID; ++eij)
    {
        if (_Re[eij].size() > 0)
        {
            Node vj = G.target(eij);
            ArcList.push_back(std::make_pair(_T.getTime(vj), eij));
        }
    }
    
    sort(ArcList.begin(), ArcList.end());
    
    int nList = ArcList.size();

    // clique ordering
    for (int k = 0; k <= _maxk; ++k)
    {
        for (int l = 0; l < k; ++l)
        {
            for (int i = 0; i < nList; ++i)
            {
                Arc a = ArcList[i].second;
                
                if (_Re[a].size() > 0)
                {
                    for (int j = i + 1; j < nList; ++j)
                    {
                        Arc b = ArcList[j].second;
                        
                        if (_Re[b].size() > 0)
                        {
                            _solFile << -getVarIndex(a, k) << " " << -getVarIndex(a) << " " << -getVarIndex(b) << " " << -getVarIndex(b, l) << " 0" << std::endl;
                        }
                        
                    }
                }
            }
        }
    }
}
