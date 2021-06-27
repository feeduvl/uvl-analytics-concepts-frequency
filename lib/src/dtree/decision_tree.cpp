#include "decision_tree.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <utility>
#include "../util/str_util.h"
#include "../util/math_util.h"

// --------------------- DecisionTreeNode Class ---------------------------

DecisionTreeNode::DecisionTreeNode() {
    ;
}

void DecisionTreeNode::setAttrName(const std::string& attr_name) {
    this -> attr_name = attr_name;
}

std::string DecisionTreeNode::getAttrName() {
    return attr_name;
}


// --------------------- Instance Class ---------------------------


Instance::Instance(std::vector<bool>& attr_vals) : attribute_values(attr_vals) {
}


// --------------------- Example Class ---------------------------


Example::Example(std::vector<bool> &attr_vals, const bool &target_class)
        : Instance(attr_vals) {
    this -> attribute_values = attr_vals;
    this -> target_class = target_class;
}

bool Example::getTargetClass() const {
    return target_class;
}

bool Example::get_attribute_value(unsigned int index) const {
    return this->attribute_values[index];
}

// --------------------- DecisionTree Class ---------------------------


void DecisionTree::build(std::vector<Example>& train_data) {
    std::vector<int> all_indices;
    int i = 0;
    for(auto & s : this->attribute_names){
        all_indices.push_back(i++);
    }
    root = new DecisionTreeNode();
    build(train_data, root, all_indices);
}



void DecisionTree::build(std::vector<Example> & train_data,
                         DecisionTreeNode* p, std::vector<int> & check_attr) {

    depth++;
    /*
     * Establish recursion base cases - either all have same label, or no more attributes to check for
     */
    bool data_empty = train_data.empty();
    unsigned int pos_label = 0, neg_label = 0;

    if(!data_empty){
        get_label_counts(train_data, pos_label, neg_label);
    } else {
        //  parent recursion already checked for this case and set the classification
        p->is_leaf = true;
        return;
    }

    if(check_attr.empty()){  //  nothing left to split on - node is a leaf with majority vote
        p->setAttrName(std::to_string(pos_label >= neg_label));
        p->is_leaf = true;
        return;
    }
    if(pos_label == 0 || neg_label==0){
        p->is_leaf = true;
        p->setAttrName(std::to_string(train_data[0].getTargetClass()));
        return;
    }

    p->is_leaf = false;
    double max_gain = -1;
    int max_index = 0;
    double entropy_1 = calcEntropy(pos_label, neg_label);

    int del_index = 0;
    // find which attribute should be at the node
    for (int i = 0; i < check_attr.size(); i++) {
        double cand_gain = discInfoGain(entropy_1, train_data, check_attr[i]);
        if (cand_gain > max_gain) {
            max_gain = cand_gain;
            max_index = check_attr[i];
            del_index = i;
        }
    }
    // now, the attribute to be placed has been found
    std::string attr_name = this->attribute_names[max_index];

    //std::cout << "Max info gain attribute: " << attr_name << " with index in current attr: " << del_index << " and overall index: " << max_index << std::endl;
    //std::cout << "Information gain from split: " << max_gain << std::endl;

    check_attr.erase(check_attr.begin() + del_index);

    p->setAttrName(attr_name);


    {
        auto pos_child = new DecisionTreeNode;
        p->true_child = pos_child;
        pos_child->branch_on = true;

        auto neg_child = new DecisionTreeNode;
        p->false_child = neg_child;
        neg_child->branch_on = false;

        std::vector<Example> pos_train_data;
        std::vector<Example> neg_train_data;

        for (auto & j : train_data) {
            if (j.get_attribute_value(max_index)) {
                pos_train_data.push_back(j);
            } else {
                neg_train_data.push_back(j);
            }
        }
        unsigned int other_pos = 0, other_neg = 0;

        if(pos_train_data.empty()){
            get_label_counts(neg_train_data, other_pos, other_neg);
            pos_child->setAttrName(std::to_string(other_pos >= other_neg));
        }
        if(neg_train_data.empty()){
            get_label_counts(pos_train_data, other_pos, other_neg);
            neg_child->setAttrName(std::to_string(other_pos >= other_neg));
        }
        build(pos_train_data, pos_child, check_attr);
        build(neg_train_data, neg_child, check_attr);
    }
}



void DecisionTree::print() {
    print(root, root->getAttrName());
}

void DecisionTree::print(DecisionTreeNode *p, const std::string& parent_path) {

    if(!p->is_leaf){
        print(p->true_child, parent_path + " (" + std::to_string(p->true_child->branch_on) + ")" + " -> " + p->true_child->getAttrName());
        print(p->false_child, parent_path + " (" + std::to_string(p->false_child->branch_on) + ")" + " -> " + p->false_child->getAttrName());
    } else {
        std::cout << parent_path << std::endl;
    }
}

double DecisionTree::discInfoGain(const double entropy_total, std::vector<Example> els, const unsigned int & attr_index){

    unsigned int pos_pos = 0, pos_neg = 0, neg_neg = 0, neg_pos = 0;  //  first label, then attribute

    for(int i=0; i<els.size(); i++){
        Example temp =els[i];
        if(temp.getTargetClass()){
            if(temp.get_attribute_value(attr_index)){
                pos_pos++;
            } else {
                pos_neg++;
            }
        } else {
            if(temp.get_attribute_value(attr_index)){
                neg_pos++;
            } else {
                neg_neg++;
            }
        }
    }

    double sum = els.size();

    double entropy_2 = 0.0;

    unsigned int pos_attr = pos_pos + neg_pos;
    unsigned int neg_attr = pos_neg + neg_neg;

    entropy_2 += (pos_attr / sum) * calcEntropy(pos_pos, neg_pos);
    entropy_2 += (neg_attr / sum) * calcEntropy(pos_neg, neg_neg);

    return (entropy_total-entropy_2);
}

double DecisionTree::calcEntropy(unsigned int num_pos, unsigned int num_neg){
    double sum,ans=0.0;
    sum = num_pos + num_neg;
    if(num_pos > 0){
        ans += (num_pos/sum)*(log2(num_pos/sum));
    }
    if(num_neg > 0){
        ans += (num_neg/sum)*(log2(num_neg/sum));
    }
    ans*=-1.00;
    return ans;
}

std::vector<std::string> DecisionTree::tree_to_vec() {
    std::vector<std::string> tree_out = std::vector<std::string>();
    tree_to_vec(root, 0, 1, tree_out);
    return tree_out;
}

void DecisionTree::tree_to_vec(DecisionTreeNode *node, unsigned int current_depth, const unsigned int index, vector<std::string> &out_vec) {

    out_vec.push_back(std::to_string(index));
    out_vec.push_back(node->getAttrName());

    if (!node->is_leaf) {
        unsigned int next_index_true = 2*index;
        unsigned int next_index_false = next_index_true +1;

        tree_to_vec(node->true_child, current_depth +1, next_index_true, out_vec);
        tree_to_vec(node->false_child, current_depth +1, next_index_false, out_vec);
    }
}

void DecisionTree::get_label_counts(std::vector<Example> & train_data, unsigned int & pos_count , unsigned int & neg_count){
    for(auto & temp : train_data){
        if(temp.getTargetClass()){
            pos_count++;
        } else {
            neg_count++;
        }
    }
}

// -------------------- Data Extraction --------------------------------------
std::vector<std::vector<std::string> > Reader::readData(std::string fileloc){
    std::ifstream fin(fileloc,std::ios::in);
    std::vector<std::vector<std::string> > data;
    while(!fin.eof()){
        std::string s;
        std::vector<std::string> att;
        fin>>s;
        std::stringstream str(s);
        while (str) {
            std::string temp;
            std::getline(str,temp,',');
            att.push_back(temp);
        }
        data.push_back(att);
    }
    return data;
}

std::set<std::string> Reader::readTargetVal(std::string fileloc, int n){
    std::ifstream fin(fileloc,std::ios::in);
    std::set<std::string> att;
    while(!fin.eof()){
        std::string s;
        fin>>s;
        std::stringstream str(s);
        for(int i=0;i<n;i++){
            std::string temp;
            std::getline(str,temp,',');
            if(i==n-1)
                att.insert(temp);
        }
    }
    return att;
}