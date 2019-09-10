//
// Created by weiqin xu on 03/09/2019.
//

#ifndef TRIPLE_STORE_JOINRESULTS_HPP
#define TRIPLE_STORE_JOINRESULTS_HPP

#include <vector>
#include <map>
#include <string>
#include "Constant.hpp"

using namespace std;

class JoinResults {
private:
    vector<vector<ID_TYPE>> result_table;
    map<string, int> variable2index;
    void add(vector<ID_TYPE> line);
public:
    JoinResults();
    JoinResults(vector<vector<ID_TYPE>> table_b, vector<string> variables);
    void join(vector<vector<ID_TYPE>> table_b, vector<string> variables);
    void join(JoinResults table_b);
    void merge_join(vector<vector<ID_TYPE>> table_b, vector<string> variables);
    void print_ele();
    vector<vector<ID_TYPE>> get_result_table();
    map<string, int> get_variable2index();
    vector<string> get_vector_variables();
};


#endif //TRIPLE_STORE_JOINRESULTS_HPP
