/*
 * dimacs.h
 *
 *  Created on: 9-nov-2018
 *      Author: P. Sashittal
 */

#include "sankoff.h"

class Dimacs : public Sankoff
{
public:
    Dimacs(const BaseTree& T, const int rootLabel, int maxk, std::string psol, std::string varlist);
    
    /// initialize the exporter
    void initVariables();
    
    /// solver for dimacs
    virtual bool solveDimacs();
    
    /// variable list writing
    void writeVariableList();
    
    /// vertex labeling constraints
    void writeVertexConstraints(Node u, BoolVector feasibleLabels);
    
    /// edge constraints
    void writeEdgeConstraints();
    
    /// write oneHot constraints (node)
    void writeOneHotConstraints(Node u, BoolVector feasibleLabels);
    
    /// write oneHot constraints (edge)
    void writeOneHotConstraints(int k);
    
    /// write Infection edge constraints
    void writeInfectionEdgeConstraints(Node u);
    
    /// write non-infection edge constraints
    void writeNonInfectionEdgeConstraints(Node u);
    
    /// write clique constraints
    void writeInfectionCliqueConstraints();
    
    /// write time constraints
    void writeInfectionTimeConstraints();
    
    /// write non-empty clique constraints
    void writeNonEmptyCliqueConstraints();
    
    /// write minimal partition constraints
    void writeMinimalPartitionConstraints();
    
    /// write feasible clique constraints
    void writeFeasibleCliqueConstraints();
    
    /// write not feasible clique constraints
    void writeNotFeasibleCliqueConstraints();
    
    /// write not feasible clique and not infection constraint
    void writeNotFeasibleInfectionConstraints(Node u);
    
    /// write clique master constraints
    void writeCliqueMasterConstraints();
    
    /// write clique ordering constraints
    void writeCliqueOrderingConstraints();
    
    /// write independent support
    void writeIndependentSupport();
    
    /// get variable index
    int getVarIndex(Node u, int label)
    {
        assert(_Rv[u][label] > 0);
        return _Rv[u][label];
    }
    
    /// get variable index
    int getVarIndex(Arc eij, int k)
    {
        assert(_Re[eij].size() > 0);
        return _Re[eij][k];
    }
    
    /// get variable index
    int getVarIndex(Arc eij, Arc ekl)
    {
        int nCVariables = _C.size();
        
        int minIndex = std::min(arc_index(eij), arc_index(ekl));
        int maxIndex = std::max(arc_index(eij), arc_index(ekl));
        
        if (minIndex == maxIndex)
        {
            return 0;
        }
        
        for (int i = 0; i < nCVariables; ++i)
        {
            if (_C[i].second.first == minIndex && _C[i].second.second == maxIndex)
            {
                return _C[i].first;
            }
        }
        
        return 0;
    }
  
    /// get variable index
    int getVarIndex(Arc eij)
    {
        assert(_Re[eij].size() > 0);
        return _Cm[eij];
    }
    
    /// get feasible labels
    IntVector getFeasibleLables(Node u)
    {
        IntVector feasibleLabels;
        
        for (int s = 0; s < _T.getNrHost(); ++s)
        {
            if (_Rv[u][s] > 0)
            {
                feasibleLabels.push_back(s);
            }
        }
        
        return feasibleLabels;
    }
    
protected:
    // vaiable rank in the list of
    // variables for SAT problem (vertices and edges)
    UInt64VectorNodeMap _Rv;
    UInt64VectorArcMap _Re;
    UInt64ArcMap _Cm;
    IntTripleVector _C;
    // maximum number of co-infections
    int _maxk;
    // number of boolean variables
    int _numVar;
    // output DIMACS file
    std::ofstream _solFile;
    // output variable file
    std::ofstream _varFile;
};
