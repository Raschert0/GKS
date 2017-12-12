#ifndef GKS_HEADER_H
#define GKS_HEADER_H
#include "struct_headers.h"
#include <QMap>

class GKS
{
public:
    GKS(){}
    ~GKS();
    QList<QString> makeUniquesList(QList<Product> input_set);
    QVector<QVector<bool> > createFirstMatrix();
    QVector<QVector<unsigned int> > createSimilarityMatrix();
    QVector<QVector<Product> > splitByGroupsP1();
    QVector<QVector<QString>> countOperationsByGroups();
    void sortGroupsByOperationsCount();
    QVector<QVector<QString>> getOperationsByGroups();
    QVector<QVector<Product> > getGroups();
    void splitByGroupsP2();
    void createGraphs();
    void graphsDecomposition();
    QVector<Module *> createModules();
    QVector<ProductModule> getProductsWithModules();

private:
    QList<QString> uniques_list;
    QList<Product> products_cached;
    QVector<QVector<bool>> first_matrix_cached;
    QVector<QVector<unsigned int>> similarity_matrix_cached;
    QVector<QVector<Product>> groups_cached;
    QVector<QVector<QString>> operations_by_groups_cached;
    QVector<Graph> graphs_cached;
    QVector<Module*> modules_cached;
    QVector<ProductModule> product_module_cached;
    QMap<QString, Module*> modules_by_operation;


    QVector<GNode*> checkClosedLoop(GNode *node, int depth_left);
    QVector<GNode*> checkChain(GNode *node, GNode *head_node, int depth_left);
    void squahNodes(QVector<GNode*> &nodes, Graph &target_graph);
    void populateModByOper();

};

#endif // GKS_HEADER_H
