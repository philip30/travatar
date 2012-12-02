#ifndef TEST_TUNE_H__
#define TEST_TUNE_H__

#include "test-base.h"
#include <travatar/tune-greedy-mert.h>

namespace travatar {

class TestTune : public TestBase {

public:

    TestTune() : examps1(3), examps2(3) {
        valid = Dict::WID("val");
        slopeid = Dict::WID("slope");
        // Create the examples
        examps1[0].first[valid] = 1; examps1[0].first[slopeid] = -1; examps1[0].second = 0.2;
        examps1[1].first[valid] = 3; examps1[1].first[slopeid] = 1;  examps1[1].second = 0.1;
        examps1[2].first[valid] = 1; examps1[2].first[slopeid] = 1;  examps1[2].second = 0.3;
        examps2[0].first[valid] = 7; examps2[0].first[slopeid] = -1; examps2[0].second = 0.1;
        examps2[1].first[valid] = 3; examps2[1].first[slopeid] = 1;  examps2[1].second = 0.2;
        examps2[2].first[valid] = 6; examps2[2].first[slopeid] = 0;  examps2[2].second = 0.3;
        examp_set.push_back(examps1); examp_set.push_back(examps2);
        weights[valid] = 1;
        gradient[slopeid] = 1; 
    }
    ~TestTune() { }

    int TestCalculatePotentialGain() {
        TuneGreedyMert mert;
        // Create the weights
        SparseMap weights; weights[Dict::WID("c")] = 1;
        // Create the examples
        vector<TuneGreedyMert::ExamplePair> examps;
        SparseMap x1; x1[Dict::WID("a")] = 1; x1[Dict::WID("b")] = 1;
        examps.push_back(make_pair(x1, 1.0));
        SparseMap x2; x2[Dict::WID("a")] = 1; x2[Dict::WID("c")] = 1;
        examps.push_back(make_pair(x2, 0.5));
        SparseMap x3; x3[Dict::WID("a")] = 1; x3[Dict::WID("d")] = 1;
        examps.push_back(make_pair(x3, 0.0));
        // Find the expected and actual potential gain
        SparseMap gain_act = mert.CalculatePotentialGain(examps, weights);
        // Both b and c are different between x1 and x2, and should be active
        SparseMap gain_exp;
        gain_exp[Dict::WID("b")] = 0.5; gain_exp[Dict::WID("c")] = 0.5;
        return CheckMap(gain_exp, gain_act);
    }

    int TestCalculateConvexHull() {
        TuneGreedyMert mert;
        // Here lines 0 and 1 should form the convex hull with an intersection at -1
        vector<TuneGreedyMert::ScoredSpan> hull1_exp, hull1_act;
        hull1_act = mert.CalculateConvexHull(examps1, weights, gradient);
        hull1_exp.push_back(make_pair(make_pair(-DBL_MAX, -1.0), 0.2));
        hull1_exp.push_back(make_pair(make_pair(-1.0, DBL_MAX), 0.1));  
        vector<TuneGreedyMert::ScoredSpan> hull2_exp, hull2_act;
        hull2_act = mert.CalculateConvexHull(examps2, weights, gradient);
        hull2_exp.push_back(make_pair(make_pair(-DBL_MAX, 1.0), 0.1));
        hull2_exp.push_back(make_pair(make_pair(1.0, 3.0), 0.3));
        hull2_exp.push_back(make_pair(make_pair(3.0, DBL_MAX), 0.2));  
        return CheckVector(hull1_exp, hull1_act) && CheckVector(hull2_exp, hull2_act);
    }

    int TestLineSearch() {
        TuneGreedyMert mert;
        // Here lines 0 and 1 should form the convex hull with an intersection at -1
        pair<double,double> exp_score1(2.0,0.2);
        pair<double,double> act_score1 = mert.LineSearch(examp_set, weights, gradient);
        pair<double,double> exp_score2(-2.0,0.1);
        pair<double,double> act_score2 = mert.LineSearch(examp_set, weights, gradient, make_pair(-DBL_MAX, 0.0));
        pair<double,double> exp_score3(-3.0,0.1);
        pair<double,double> act_score3 = mert.LineSearch(examp_set, weights, gradient, make_pair(-DBL_MAX, -3.0));
        return 
            CheckAlmost(exp_score1.first, act_score1.first) &&
            CheckAlmost(exp_score1.second, act_score1.second) &&
            CheckAlmost(exp_score3.first, act_score3.first) &&
            CheckAlmost(exp_score3.second, act_score3.second) &&
            CheckAlmost(exp_score2.first, act_score2.first) &&
            CheckAlmost(exp_score2.second, act_score2.second);
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestCalculatePotentialGain()" << endl; if(TestCalculatePotentialGain()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestCalculateConvexHull()" << endl; if(TestCalculateConvexHull()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestLineSearch()" << endl; if(TestLineSearch()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestTune Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

    int valid, slopeid;
    vector<TuneGreedyMert::ExamplePair> examps1, examps2;
    vector<vector<TuneGreedyMert::ExamplePair> > examp_set;
    SparseMap weights, gradient;

};

}

#endif