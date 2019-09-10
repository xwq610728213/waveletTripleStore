//
// Created by weiqin xu on 26/08/2019.
//

#include "Dictionary.hpp"
#include <iostream>
#include "Util.hpp"


Dictionary::Dictionary() {}

void Dictionary::add(string instance, ID_TYPE id) {
    instance2id[instance] = id;
    id2instance[id] = instance;
}

ID_TYPE Dictionary::look_up_instance(string instance) {
    if(instance2id.count(instance) > 0){
        return instance2id[instance];
    }else{
        throw DictException("Research instance doesn't exist!");
    }
}

string Dictionary::look_up_id(ID_TYPE id) {
    if(id2instance.count(id) > 0){
        return id2instance[id];
    }else{
        throw DictException("Research id doesn't exist!");
    }
}

void Dictionary::remove(string instance) {
    if(instance2id.count(instance) > 0){
        ID_TYPE id = instance2id[instance];
        instance2id.erase(instance);
        id2instance.erase(id);
    }else{
        throw DictException("Instance doesn't exist!");
    }
}

void Dictionary::remove(ID_TYPE id) {
    if(id2instance.count(id) > 0){
        string instance = id2instance[id];
        instance2id.erase(instance);
        id2instance.erase(id);
    }else{
        throw DictException("Id doesn't exist!");
    }
}


