#ifndef STRUCT_HEADERS_H
#define STRUCT_HEADERS_H
#include <QList>
#include <QString>

struct Product{
    Product() : id{0}{}
    Product(unsigned int n_id) : id{n_id}{}
    Product(const Product& P) : id{P.id}, operations{P.operations}{}
    unsigned int id;
    QList<QString> operations;
    bool operator==(const Product& P)const{return id == P.id;}
};

struct Module
{
    Module() : id{0} {}
    Module(unsigned int n_id) : id{n_id}{}
    Module(const Module& M): id{M.id}, operations{M.operations}{}
    unsigned int id;
    QVector<QString> operations;
    bool operator==(const Module& M)const{return id == M.id;}
};

struct GNode
{
    QVector<GNode*> inputs, outputs;
    QVector<QString> operations;
    bool check_in_progres{false};
    bool operator==(const GNode& GN)const{
        return operations == GN.operations && inputs == GN.inputs && outputs == GN.outputs;
    }
    bool haveOnlyOutputs(){
        return !inputs.size() && outputs.size();
    }
    bool haveOnlyInputs(){
        return inputs.size() && !outputs.size();
    }
    bool haveReverseWith(GNode* GN){
        return inputs.contains(GN) && outputs.contains(GN);
    }
};

struct Graph
{
    Graph() {}
    Graph(const Graph& G): nodes{G.nodes}, operations_cached{G.operations_cached}{}
    QVector<GNode*> nodes;
    QVector<QString> operations_cached;
};

struct ProductModule
{
    ProductModule() {}
    ProductModule(const ProductModule& PM): product{PM.product}, modules{PM.modules}{}
    Product product;
    QVector<Module*> modules;
};

struct ModuleShuffleScore
{
    QVector<Module*> modules;
    uint score{UINT_MAX}; //bigger == worse
};

#endif // STRUCT_HEADERS_H
