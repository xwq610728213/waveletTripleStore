//
// Created by weiqin xu on 2019-08-23.
//

#ifndef TRIPLE_STORE_TRIPLESTORE_HPP
#define TRIPLE_STORE_TRIPLESTORE_HPP

#include <sdsl/wavelet_trees.hpp>
#include <string>
#include "Constant.hpp"

using namespace sdsl;
using namespace std;
typedef wt_int<rrr_vector<63>> WT_TYPE;

class TripleStore {
private:
    TripleStoreType triple_store_type = UNKNOWN;
    WT_TYPE wt_ele0;
    WT_TYPE wt_ele1_bit_map;
    WT_TYPE wt_ele1;
    WT_TYPE wt_ele2_bit_map;
    WT_TYPE wt_ele2;

public:
    TripleStore();

    TripleStore(std::vector<ID_TYPE>&  ele0_vector, std::vector<ID_TYPE>& ele1_vector, std::vector<ID_TYPE>&  ele2_vector, TripleStoreType ts_type);

    void load(std::vector<ID_TYPE>&  ele0_vector, std::vector<ID_TYPE>& ele1_vector, std::vector<ID_TYPE>&  ele2_vector, TripleStoreType ts_type);
    std::vector<vector<ID_TYPE>> look_up_ele0(ID_TYPE ele1, ID_TYPE ele2);
    std::vector<vector<ID_TYPE>> look_up_ele1(ID_TYPE ele0, ID_TYPE ele2);
    std::vector<vector<ID_TYPE>> look_up_ele2(ID_TYPE ele0, ID_TYPE ele1);
    std::vector<vector<ID_TYPE>> look_up_ele01(ID_TYPE ele2);
    std::vector<vector<ID_TYPE>> look_up_ele02(ID_TYPE ele1);
    std::vector<vector<ID_TYPE>> look_up_ele12(ID_TYPE ele0);
    std::vector<vector<ID_TYPE>> look_up_ele012();

    // ts_type: PSO or POS
    TripleStoreType get_ts_type();

};


#endif //TRIPLE_STORE_TRIPLESTORE_HPP
