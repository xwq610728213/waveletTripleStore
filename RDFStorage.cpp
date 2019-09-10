//
// Created by weiqin xu on 26/08/2019.
//

#include "RDFStorage.hpp"
#include "RankBaseElement.hpp"
#include <sstream>
#include "Util.hpp"
#include "Constant.hpp"
#include "quickSortTS.hpp"
#include "JoinResults.hpp"


using namespace std;

RDFStorage::RDFStorage(){}

RDFStorage::RDFStorage(ifstream& abox_file, TripleStoreType ts_type){
    load_facts(abox_file, ts_type);
}

void RDFStorage::load_facts(ifstream& abox_file, TripleStoreType ts_type) {

    //dict_properties.add("<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>", 0);

    ID_TYPE last_instance_id = 0;
    ID_TYPE last_property_id = 0;
    //ID_TYPE last_concept_id = 0;

    vector<ID_TYPE> s_vector;
    vector<ID_TYPE> p_vector;
    vector<ID_TYPE> o_vector;

    string str;
    vector<RankBaseElement<3>> triple_vector;

    while(getline(abox_file, str, '\n')){
        stringstream input(str);
        string subject, predicate, object;
        input >> subject >> predicate >> object;
        ID_TYPE id_s, id_p, id_o;

        // Update dictionary
        try {
            id_s = dict_instances.look_up_instance(subject);
        }catch(DictException& excep){
            ++last_instance_id;
            dict_instances.add(subject, last_instance_id);
            id_s = last_instance_id;
        }

        // Can be optimized with LiteMat
        if(predicate != RDF_TYPE) {
            try {
                id_p = dict_properties.look_up_instance(predicate);
            } catch (DictException &excep) {
                ++last_property_id;
                dict_properties.add(predicate, last_property_id);
                id_p = last_property_id;
            }
        }


        // If predicate == rdf:type, object is a concept, otherwise object is an instance.
        if(predicate != RDF_TYPE){
            try {
                id_o = dict_instances.look_up_instance(object);
            }catch(DictException& excep){
                ++last_instance_id;
                dict_instances.add(object, last_instance_id);
                id_o = last_instance_id;
            }
            // In order of PSO or POS
            ID_TYPE tmp[3];
            if(ts_type == PSO){
                tmp[0] = id_p;
                tmp[1] = id_s;
                tmp[2] = id_o;
            }else if(ts_type == POS){
                tmp[0] = id_p;
                tmp[1] = id_o;
                tmp[2] = id_s;
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }


            //cout << "new triple: " << id_s << " " << id_p << " " << id_o << endl;
            triple_vector.push_back(RankBaseElement<3>(tmp));
        }else{
            try {
                id_o = dict_concepts.look_up_instance(object);
                try {
                    if (!type_store.contains(id_s, id_o)) {
                        type_store.add(id_s, id_o);
                    }
                }catch(RDFTypeException& except){
                    cout << "Is " << object << " the most precise concept of " << subject << "? Y/N" << endl;
                    char answer;
                    cin >> answer;
                    while(answer != 'Y' && answer != 'N' && answer != 'y' && answer != 'n'){
                        cout << "Please type Y/N!" << endl;
                        cin >> answer;
                    }
                    if(answer == 'Y' || answer == 'y'){
                        type_store.add(id_s, id_o, true);
                    }
                }
            }catch(DictException& excep){
                cerr << "Concept doesn't exist! Check the TBOX!" << endl;
            }
        }

    }

    if(triple_vector.size() <= 0){
        cerr << "Triple vector size is 0!\n";
        exit(1);
    }
    quickSortTS(triple_vector, 0, triple_vector.size() - 1);

    // In order of PSO or POS
    if(ts_type == PSO){
        for(unsigned long i = 0; i < triple_vector.size(); ++i){
            p_vector.push_back(triple_vector[i][0]);
            s_vector.push_back(triple_vector[i][1]);
            o_vector.push_back(triple_vector[i][2]);
        }
        triple_store = TripleStore(p_vector, s_vector, o_vector, ts_type);
    }else if(ts_type == POS){
        for(unsigned long i = 0; i < triple_vector.size(); ++i){
            p_vector.push_back(triple_vector[i][0]);
            o_vector.push_back(triple_vector[i][1]);
            s_vector.push_back(triple_vector[i][2]);
        }
        triple_store = TripleStore(p_vector, o_vector, s_vector, ts_type);
    }else{
        cerr << "Triple store type not defined!" << endl;
        exit(1);
    }

}

tuple<vector<string>, vector<vector<ID_TYPE>>> RDFStorage::query_triple_pattern(string s, string p, string o) {
    vector<vector<ID_TYPE>> results;
    vector<string> variables;
    if(p != RDF_TYPE) {

        if (s[0] == '?' && p[0] == '?' && o[0] == '?') {
            results = triple_store.look_up_ele012();

            //cout << "s" << " " << "p" << " " << "o" << "\n";
            if(triple_store.get_ts_type() == PSO) {
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id(i[1]) << " "
                         << dict_properties.look_up_id(i[0])
                         << " " << dict_instances.look_up_id(i[2]) << "\n";
                }*/
                variables.push_back(p);
                variables.push_back(s);
                variables.push_back(o);
            }else if(triple_store.get_ts_type() == POS){
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id(i[2]) << " "
                         << dict_properties.look_up_id(i[0])
                         << " " << dict_instances.look_up_id(i[1]) << "\n";
                }*/
                variables.push_back(p);
                variables.push_back(o);
                variables.push_back(s);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }

            //cout << '\n' << "\n";
        } else if (s[0] == '?' && p[0] == '?') {
            // In order of P S O
            //cout << "s" << " " << "p" << "\n";


            if(triple_store.get_ts_type() == PSO) {
                try {
                    results = triple_store.look_up_ele01(dict_instances.look_up_instance(o));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[1] ) << " "
                         << dict_properties.look_up_id( i[0] )
                         << "\n";
                }*/
                variables.push_back(p);
                variables.push_back(s);
            }else if(triple_store.get_ts_type() == POS){
                try {
                    results = triple_store.look_up_ele02(dict_instances.look_up_instance(o));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[1] ) << " "
                         << dict_properties.look_up_id( i[0] )
                         << "\n";
                }*/
                variables.push_back(p);
                variables.push_back(s);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }
            //cout << '\n' << "\n";
        } else if (s[0] == '?' && o[0] == '?') {
            // In order of P S O
            // Consider rdf:type
            //cout << "s" << " " << "o" << "\n";
            if(triple_store.get_ts_type() == PSO) {
                try {
                    results = triple_store.look_up_ele12(dict_properties.look_up_instance(p));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[0] ) << " "
                         << dict_instances.look_up_id( i[1] )
                         << "\n";
                }*/
                variables.push_back(s);
                variables.push_back(o);
            }else if(triple_store.get_ts_type() == POS){
                try {
                    results = triple_store.look_up_ele12(dict_properties.look_up_instance(p));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[1] ) << " "
                         << dict_instances.look_up_id( i[0] )
                         << "\n";
                }*/
                variables.push_back(o);
                variables.push_back(s);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }
            //cout << '\n' << "\n";
        } else if (p[0] == '?' && o[0] == '?') {
            // In order of P S O
            //cout << "p" << " " << "o" << "\n";
            if(triple_store.get_ts_type() == PSO) {
                try {
                    results = triple_store.look_up_ele02(dict_instances.look_up_instance(s));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_properties.look_up_id( i[0] ) << " "
                         << dict_instances.look_up_id( i[1] )
                         << "\n";
                }*/
                variables.push_back(p);
                variables.push_back(o);
            }else if(triple_store.get_ts_type() == POS){
                try {
                    results = triple_store.look_up_ele01(dict_instances.look_up_instance(s));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_properties.look_up_id( i[0] ) << " "
                         << dict_instances.look_up_id( i[1] )
                         << "\n";
                }*/
                variables.push_back(p);
                variables.push_back(o);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }
            //cout << '\n' << "\n";
        } else if (p[0] == '?') {
            // In order of P S O
            // Consider rdf:type
            //cout << "p" << "\n";
            if(triple_store.get_ts_type() == PSO) {
                try {
                    results = triple_store.look_up_ele0(dict_instances.look_up_instance(s),
                                                        dict_instances.look_up_instance(o));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }/*
                for (auto i:results) {
                    cout << dict_properties.look_up_id( i[0] ) << endl;
                }*/
                variables.push_back(p);
            }else if(triple_store.get_ts_type() == POS){
                try {
                    results = triple_store.look_up_ele0(dict_instances.look_up_instance(o),
                                                        dict_instances.look_up_instance(s));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_properties.look_up_id( i[0] ) << endl;
                }*/
                variables.push_back(p);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }
            //cout << '\n' << "\n";
        } else if (s[0] == '?') {
            // In order of P S O
            // Consider rdf:type
            //cout << "s" << "\n";
            if(triple_store.get_ts_type() == PSO) {
                try {
                    results = triple_store.look_up_ele1(dict_properties.look_up_instance(p), dict_instances.look_up_instance(o));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[0] ) << endl;
                }*/
                variables.push_back(s);
            }else if(triple_store.get_ts_type() == POS){
                try {
                    results = triple_store.look_up_ele2(dict_properties.look_up_instance(p), dict_instances.look_up_instance(o));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }/*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[0] ) << endl;
                }*/
                variables.push_back(s);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }
            //cout << '\n' << "\n";
        } else if (o[0] == '?') {
            // In order of P S O
            // Consider rdf:type
            //cout << "o" << "\n";
            if(triple_store.get_ts_type() == PSO) {
                try {
                    results = triple_store.look_up_ele2(dict_properties.look_up_instance(p), dict_instances.look_up_instance(s));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[0] ) << endl;
                }*/
                variables.push_back(o);
            }else if(triple_store.get_ts_type() == POS){
                try {
                    results = triple_store.look_up_ele1(dict_properties.look_up_instance(p), dict_instances.look_up_instance(s));
                } catch (DictException &except) {
                    return make_tuple(variables, results);
                }
                /*
                for (auto i:results) {
                    cout << dict_instances.look_up_id( i[0] ) << endl;
                }*/
                variables.push_back(o);
            }else{
                cerr << "Triple store type not defined!" << endl;
                exit(1);
            }
            //cout << '\n' << "\n";
        } else {
            cerr << "Wrong question!\n";
        }
        return make_tuple(variables, results);
    }else{
        cerr << "Reasoner doesn't exist!" << endl;
        return make_tuple(variables, results);
    }
}

tuple<vector<string>, vector<vector<ID_TYPE>>> RDFStorage::query_graph_pattern(vector<string> triple_string_vector) {
    vector<vector<string>> query_table;
    for(string triple_pattern:triple_string_vector){
        stringstream input(triple_pattern);
        string query_subject, query_predicate, query_object;
        input >> query_subject >> query_predicate >> query_object;
        vector<string> tmp;
        tmp.push_back(query_subject);
        tmp.push_back(query_predicate);
        tmp.push_back(query_object);
        query_table.push_back(tmp);
    }

    JoinResults res;
    // Naive join order
    for(int i = 0; i < query_table.size(); ++i){
        vector<vector<ID_TYPE>> tmp_table;
        vector<string> tmp_varibales;
        clock_t start, end;
        start = clock();
        tie(tmp_varibales, tmp_table) = this -> query_triple_pattern(query_table[i][0], query_table[i][1], query_table[i][2]);
        end = clock();
        cout << "Triple patter search time: " << (double)(end - start)/CLOCKS_PER_SEC*1000 << " ms" << endl;
        if(i == 0){
            res = JoinResults(tmp_table, tmp_varibales);
        }else{
            res.join(tmp_table, tmp_varibales);
        }
    }
    return make_tuple(res.get_vector_variables(), res.get_result_table());
}