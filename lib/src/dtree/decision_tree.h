#ifndef DECISION_TREE_H_
#define DECISION_TREE_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <map>

class DecisionTreeNode {
public:
    DecisionTreeNode();
    void setAttrName(const std::string& attr_name);
    std::string getAttrName();

    bool branch_on;  //  either left child or right child of parent node
    bool is_leaf;  // no children
    bool is_root;  //  disregard branch_on in this case (no parent)

    DecisionTreeNode * true_child;
    DecisionTreeNode * false_child;
protected:
    std::string attr_name;
};

class Instance {
public:
    Instance(std::vector<bool>& attr_vals);

protected:
    std::vector<bool> & attribute_values;
};

class Example: public Instance {
public:
    // Precondition: Size of 'attr_names' qual to size of 'attr_vals'
    Example(std::vector<bool> &attr_vals, const bool &target_class);
    bool getTargetClass() const;
    bool get_attribute_value(unsigned int index) const;

private:
    bool target_class;
};

class DecisionTree {
public:
    // 'addAttrInfo' and 'addTargetValues' must be used before calling
    // this function
    void build(std::vector<Example>& train_data);

    void print();

    std::vector<std::string> tree_to_vec();

    std::vector<std::string> attribute_names;
    std::vector<double> information_gain;

private:

    void get_label_counts(std::vector<Example> & train_data, unsigned int & pos_count , unsigned int & neg_count);

    void tree_to_vec(DecisionTreeNode * node, unsigned int current_depth, const unsigned long long int index, std::vector<std::string> & out_vec);
    int depth = -1;  // by convention, the root node has depth 0, incremented on first iteration
    // Used by public 'build'
    void build(std::vector<Example> & train_data, DecisionTreeNode* p,
               std::vector<int> & check_attr);

    // Returns information gain
    // 'attr_name' must be a discrete-valued attribute
    double discInfoGain(const double entropy_total, std::vector<Example> els, const unsigned int & attr_name);

    // Returns entropy given a map
    // (target value, num of occurrences of that target value)
    double calcEntropy(unsigned int num_pos, unsigned int num_neg);

    void print(DecisionTreeNode *p, const std::string& parent_string);

    // key is attribute
    // value is vector of all possible values for that attribute
    // If the attribute is continuous then value will be empty vector
    //std::unordered_map<std::string, std::vector<std::string> > pos_vals;

    DecisionTreeNode *root = nullptr;
};

class Reader {
public:
    // file name and number of attributes
    static std::vector<std::vector<std::string> > readData(std::string fileloc);
    static std::set <std::string> readTargetVal(std::string fileloc, int n);
};


#endif