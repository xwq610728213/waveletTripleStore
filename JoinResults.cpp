//
// Created by weiqin xu on 03/09/2019.
//

#include "JoinResults.hpp"
#include <sstream>
#include <iostream>
#include <map>

JoinResults::JoinResults() {}

JoinResults::JoinResults(vector<vector<ID_TYPE>> table_b, vector<string> variables) {
    for(int i = 0; i < variables.size(); ++i){
        variable2index[variables[i]] = i;
    }
    result_table = table_b;
}


void JoinResults::join(vector<vector<ID_TYPE>> table_b, vector<string> variables){
    vector<vector<ID_TYPE>> new_result_table;
    map<string, int> new_variable2index;
    // result_table not empty.
    if(result_table.size() == 0){
        cerr << "0 lines!" << endl;
        return;
    }else{
        int num_join = 0;
        for(auto i: variables){
            if(variable2index.count(i) > 0){
                ++num_join;
            }
        }
        if(num_join == 1 && variable2index.count(variables[0]) > 0 && variable2index[variables[0]] == 0){
            this->merge_join(table_b, variables);
            return;
        }else{
            new_variable2index = variable2index;
            int size_table = result_table[0].size();
            vector<string> join_col, not_join_col;
            map<string, int> variable2index_b;
            for(auto i = 0; i < variables.size(); ++i){
                if(new_variable2index.count(variables[i]) == 0){
                    new_variable2index[variables[i]] = size_table++;
                    not_join_col.push_back(variables[i]);
                }else{
                    join_col.push_back(variables[i]);
                }
                variable2index_b[variables[i]] = i;
            }
            // NLjoin
            for(auto table_a_ele:result_table){
                for(auto table_b_ele:table_b){
                    int trig = 0;
                    for(auto k: join_col){
                        if(table_a_ele[variable2index[k]] != table_b_ele[variable2index_b[k]]){
                            trig = 1;
                        }
                    }
                    if(trig == 0){
                        vector<ID_TYPE> tmp(table_a_ele);
                        for(auto i : not_join_col){
                            tmp.push_back(table_b_ele[variable2index_b[i]]);
                        }
                        new_result_table.push_back(tmp);
                    }
                }
            }
            result_table = new_result_table;
            variable2index = new_variable2index;
            return;
        }
    }
}

// Merge join can be applied when the two join variable are in order (index 0)
void JoinResults::merge_join(vector<vector<ID_TYPE>> table_b, vector<string> variables) {
    unsigned long long index_a = 0, index_b = 0;
    vector<vector<ID_TYPE>> new_result_table;
    int size_table = result_table[0].size();
    vector<string> not_join_col;
    map<string, int> variable2index_b;
    map<string, int> new_variable2index;
    new_variable2index = variable2index;

    for(int i = 1; i < variables.size(); ++i){
        new_variable2index[variables[i]] = size_table++;
        not_join_col.push_back(variables[i]);
        variable2index_b[variables[i]] = i;
    }

    while(index_a < result_table.size() && index_b < table_b.size() ){
        while(result_table[index_a][0] < table_b[index_b][0]){
            ++index_a;
            if(index_a >= result_table.size()){
                break;
            }
        }
        if(index_a >= result_table.size()){
            break;
        }
        while(result_table[index_a][0] > table_b[index_b][0]){
            ++index_b;
            if(index_b >= table_b.size()){
                break;
            }
        }
        if(index_b >= table_b.size()){
            break;
        }
        if(result_table[index_a][0] == table_b[index_b][0]){
            ID_TYPE key = result_table[index_a][0];
            unsigned long long len_a = 0, len_b = 0;
            while(result_table[index_a + len_a][0] == key){
                ++len_a;
                if(index_a + len_a >= result_table.size())
                    break;
            }
            while(table_b[index_b + len_b][0] == key){
                ++len_b;
                if(index_b + len_b >= table_b.size())
                    break;
            }
            for(auto i = index_a; i < index_a + len_a; ++i){
                for(auto j = index_b; j < index_b + len_b; ++j){
                    vector<ID_TYPE> tmp(result_table[i]);
                    for(auto k = 1; k < variables.size(); ++k){
                        tmp.push_back(table_b[j][k]);
                    }
                    new_result_table.push_back(tmp);
                }
            }

            index_a += len_a;
            index_b += len_b;
        }
    }
    result_table = new_result_table;
    variable2index = new_variable2index;
}

void JoinResults::print_ele() {
    for(auto iter = variable2index.begin(); iter != variable2index.end(); ++iter){
        cout << iter->first << " " ;
    }
    cout << endl;
    for(int i = 0; i < result_table.size(); ++i){
        for(auto iter = variable2index.begin(); iter != variable2index.end(); ++iter){
            cout << result_table[i][iter->second] << " " ;
        }
        cout << endl;
    }
}

void JoinResults::join(JoinResults results_b) {
    vector<vector<ID_TYPE>> table_b = results_b.get_result_table();
    auto variables_b = results_b.get_vector_variables();
    this -> join(table_b, variables_b);
}

vector<vector<ID_TYPE>> JoinResults::get_result_table() {
    return result_table;
}

map<string, int> JoinResults::get_variable2index() {
    return variable2index;
}

vector<string> JoinResults::get_vector_variables() {
    vector<string> variables_vector;
    for(auto i = 0; i < variable2index.size(); ++i){
        for(auto j = variable2index.begin(); j != variable2index.end(); ++j){
            if(j->second == i){
                variables_vector.push_back(j->first);
                break;
            }
        }
    }
    return variables_vector;
}