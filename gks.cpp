#include "gks_header.h"

GKS::~GKS()
{
    for(int i{0}; i < graphs_cached.size(); i++){
        for(int j{graphs_cached[i].nodes.size() - 1}; j >= 0; j--){
            GNode *d_node = graphs_cached[i].nodes[j];
            delete d_node;
        }
    }

    for(int i{0}; i < modules_cached.size(); i++){
        Module *mod = modules_cached[i];
        delete mod;
    }

    /*
    for(auto &modpr : product_module_cached){
        for(auto &mod : modpr.modules){
            delete mod;
        }
    }*/

}

QList<QString> GKS::makeUniquesList(QList<Product> input_set)
{
    uniques_list.clear();
    products_cached = input_set;
    for(auto &it : input_set){
        for(auto &sit : it.operations){
            if(!uniques_list.count(sit)){
                uniques_list.push_back(sit);
            }
        }
    }
    return uniques_list;
}

QVector<QVector<bool>> GKS::createFirstMatrix()
{
    first_matrix_cached.clear();
    if(!uniques_list.size()){
        return first_matrix_cached;
    }
    first_matrix_cached.resize(products_cached.size());
    for(auto &uit : first_matrix_cached){
        uit.resize(uniques_list.size());
    }
    for(int i{0}; i < products_cached.size(); i++){
        for(int j{0}; j < uniques_list.size(); j++){
            if(products_cached.at(i).operations.contains(uniques_list.at(j))){
                first_matrix_cached[i][j] = true;
            }else{
                first_matrix_cached[i][j] = false;
            }
        }
    }
    return first_matrix_cached;
}

QVector<QVector<unsigned int> > GKS::createSimilarityMatrix()
{
    similarity_matrix_cached.clear();
    if(!first_matrix_cached.size()){
        return similarity_matrix_cached;
    }
    similarity_matrix_cached.resize(products_cached.size());
    for(auto &it : similarity_matrix_cached){
        it.resize(products_cached.size());
    }
    for(int i{0}; i < products_cached.size(); i++){
        similarity_matrix_cached[i][i] = 0;
    }
    for(int i{0}; i < products_cached.size() - 1; i++){
        for(int j{i+1}; j < products_cached.size(); j++){
            similarity_matrix_cached[i][j] = 0;
            similarity_matrix_cached[j][i] = 0;
            for(int k{0}; k < uniques_list.size(); k++){
                if(first_matrix_cached.at(i).at(k) == first_matrix_cached.at(j).at(k)){
                    similarity_matrix_cached[i][j]++;
                    similarity_matrix_cached[j][i]++;
                }
            }
        }
    }
    return similarity_matrix_cached;
}

QVector<QVector<Product> > GKS::splitByGroupsP1()
{
    groups_cached.clear();
    if(similarity_matrix_cached.size() <= 1){
        return groups_cached;
    }
    unsigned int max;
    QVector<QVector<unsigned int>> similarity_matrix_cached_copy = similarity_matrix_cached;
    QVector<Product> products_in_groups;
    do{
        QVector<Product> c_group;
        max = 0;
        for(int i{0}; i < products_cached.size() - 1; i++){
            for(int j{i + 1}; j < products_cached.size(); j++){
                if(similarity_matrix_cached_copy.at(i).at(j) > max){
                    max = similarity_matrix_cached_copy.at(i).at(j);
                }
            }
        }
        for(int i{0}; i < products_cached.size() - 1; i++){
            for(int j{i + 1}; j < products_cached.size(); j++){
                if(similarity_matrix_cached_copy.at(i).at(j) == max){
                    similarity_matrix_cached_copy[i][j] = 0;
                    Product c_product_f(products_cached.at(i)), c_product_s(products_cached.at(j));
                    if(!products_in_groups.contains(c_product_f)){
                        if(c_group.contains(c_product_s) || (!c_group.size() && (products_cached.size() == products_in_groups.size() + 1 || !products_in_groups.contains(c_product_s)))){
                            c_group.push_back(c_product_f);
                            products_in_groups.push_back(c_product_f);
                        }
                    }
                    if(!products_in_groups.contains(c_product_s)){
                        if(c_group.contains(c_product_f) || (!c_group.size() && (products_cached.size() == products_in_groups.size() + 1 || !products_in_groups.contains(c_product_f)))){
                            c_group.push_back(c_product_s);
                            products_in_groups.push_back(c_product_s);
                        }
                    }
                }
            }
        }
        if(c_group.size()){
            groups_cached.push_back(c_group);
        }
    }while(products_in_groups.size() != products_cached.size());
    return groups_cached;
}

QVector<QVector<QString> > GKS::countOperationsByGroups()
{
    operations_by_groups_cached.clear();
    if(!groups_cached.size()){
        return operations_by_groups_cached;
    }
    QVector<QVector<QString>> operations_by_groups;
    for(int i{0}; i < groups_cached.size(); i++){
        QVector<QString> c_group_operations;
        for(auto &pit : groups_cached.at(i)){
            for(auto &sit : pit.operations){
                if(!c_group_operations.contains(sit)){
                    c_group_operations.append(sit);
                }
            }
        }
        operations_by_groups.append(c_group_operations);
    }
    operations_by_groups_cached = operations_by_groups;
    return operations_by_groups;
}

void GKS::sortGroupsByOperationsCount()
{
    if(!operations_by_groups_cached.size()){
        return;
    }
    for(int i{operations_by_groups_cached.size() - 1}; i > 0 ; i--){
        for(int j{i - 1}; j >= 0; j--){
            if(operations_by_groups_cached.at(i).size() > operations_by_groups_cached.at(j).size()){
                QVector<QString> t_vec = operations_by_groups_cached.at(i);
                operations_by_groups_cached[i] = operations_by_groups_cached.at(j);
                operations_by_groups_cached[j] = t_vec;
                QVector<Product> t_list = groups_cached.at(i);
                groups_cached[i] = groups_cached.at(j);
                groups_cached[j] = t_list;
            }
        }
    }
}

QVector<QVector<QString> > GKS::getOperationsByGroups()
{
    return operations_by_groups_cached;
}

QVector<QVector<Product> > GKS::getGroups()
{
    return groups_cached;
}

void GKS::splitByGroupsP2()
{
    QList<Product> products_to_check = products_cached;
    int i{0};
    while(products_to_check.size()){
        for(auto &it : groups_cached.at(i)){
            products_to_check.removeOne(it);
        }
        for(int z{products_to_check.size() - 1}; z >= 0; z--){
            bool is_same{true};
            for(auto &op : products_to_check.at(z).operations){
                if(!operations_by_groups_cached.at(i).contains(op)){
                    is_same = false;
                    break;
                }
            }
            if(is_same){
                for(int j{i + 1}; j < groups_cached.size(); j++){
                    if(groups_cached[j].removeOne(products_to_check.at(z))){
                        QVector<QString> t_vec;
                        for(auto &pit : groups_cached.at(j)){
                            for(auto &sit : pit.operations){
                                if(!t_vec.contains(sit)){
                                    t_vec.append(sit);
                                }
                            }
                        }
                        operations_by_groups_cached[j] = t_vec;
                        break;
                    }
                }
                groups_cached[i].append(products_to_check.takeAt(z));
            }
        }
        i++;
    }
    for(int j{groups_cached.size() - 1}; j > 0; j--){
        if(!groups_cached.at(j).size()){
            groups_cached.removeAt(j);
            operations_by_groups_cached.removeAt(j);
        }
    }
}

void GKS::createGraphs()
{
    if(!operations_by_groups_cached.size()){
        return;
    }
    for(int i{0}; i < graphs_cached.size(); i++){
        for(int j{graphs_cached[i].nodes.size() - 1}; j >= 0; j--){
            GNode *d_node = graphs_cached[i].nodes[j];
            delete d_node;
        }
    }
    graphs_cached.clear();
    for(auto &group : groups_cached){
        Graph c_graph;
        for(auto &current_product : group){
            for(int i{0}; i < current_product.operations.size(); i++){
                QString c_operation = current_product.operations.at(i);
                if(!c_graph.operations_cached.contains(c_operation)){
                    GNode *new_node = new GNode();
                    new_node->operations.append(c_operation);
                    if(i){
                        QString prev_operation = current_product.operations.at(i - 1);
                        for(auto &old_node : c_graph.nodes){
                            if(!old_node->operations.contains(prev_operation)){
                                continue;
                            }
                            old_node->outputs.append(new_node);
                            new_node->inputs.append(old_node);
                            break;
                        }
                    }
                    c_graph.nodes.append(new_node);
                    c_graph.operations_cached.append(c_operation);
                }else{
                    if(!i){
                        continue;
                    }
                    for(auto &find_node : c_graph.nodes){
                        if(!find_node->operations.contains(c_operation)){
                            continue;
                        }
                        QString prev_operation = current_product.operations.at(i - 1);
                        for(auto &prev_node : c_graph.nodes){
                            if(!prev_node->operations.contains(prev_operation)){
                                continue;
                            }
                            if(prev_node->outputs.contains(find_node)){
                                break;
                            }
                            prev_node->outputs.append(find_node);
                            find_node->inputs.append(prev_node);
                            break;
                        }
                        break;
                    }
                }
            }
        }
        graphs_cached.push_back(c_graph);
    }
}

void GKS::graphsDecomposition()
{
    if(!graphs_cached.size()){
        return;
    }
    for(int i{0}; i < graphs_cached.size(); i++){
        Graph c_graph = graphs_cached.at(i);
        bool finished = false;
        QVector<GNode*> skipped; //Вершини лише зі входами та лише з виходами;
        while(!finished){
            bool start_from_scratch = false;
            QVector<GNode*> reverse_checked;
            for(int j{0}; j < c_graph.nodes.size(); j++){
                GNode *c_node = c_graph.nodes[j];
                if(skipped.contains(c_node)){
                    continue;
                }
                /*if(c_node->haveOnlyInputs() || c_node->haveOnlyOutputs()){
                    skipped.append(c_node);
                    continue;
                }*/
                if(c_node->operations.length() > 4){
                    continue;
                }
                for(auto &another_node : c_graph.nodes){ //Перевірка наявності зворотніх зв'язків
                    if(skipped.contains(another_node) || reverse_checked.contains(another_node)){ //Вершина не може мати зворотній зв'язок або вже була перевірена
                        continue;
                    }
                    if(another_node->operations.length() > 4 || !c_node->haveReverseWith(another_node)){
                        continue;
                    }
                    QVector<GNode*> to_squash;
                    to_squash.append(c_node);
                    to_squash.append(another_node);
                    squahNodes(to_squash, c_graph);
                    start_from_scratch = true;
                    break;
                }
                if(start_from_scratch){
                    break;
                }
                reverse_checked.append(c_node);

                QVector<GNode*> loop_vect = checkClosedLoop(c_node, 5);
                if(loop_vect.size()){ //Перевірка наявності контуру
                    int total_ops{0};
                    for(GNode* &it : loop_vect){
                        total_ops += it->operations.length();
                    }
                    if(total_ops <= 5){
                        squahNodes(loop_vect, c_graph);
                        start_from_scratch = true;
                        break;
                    }
                }

                QVector<GNode*> chain_vect = checkChain(c_node, c_node, 5);
                if(chain_vect.size()){ //Перевірка наявності контуру
                    int total_ops{0};
                    for(GNode* &it : chain_vect){
                        total_ops += it->operations.length();
                    }
                    if(total_ops <= 5){
                        squahNodes(chain_vect, c_graph);
                        start_from_scratch = true;
                        break;
                    }
                }
            }
            if(!start_from_scratch){
                finished = true;
            }
        }
        graphs_cached[i] = c_graph;
    }
}

QVector<Module*> GKS::createModules()
{
    if(!graphs_cached.size()){
        return modules_cached;
    }
    for(int i{0}; i < modules_cached.size(); i++){
        Module *mod = modules_cached[i];
        delete mod;
    }
    modules_cached.clear();
    QVector<GNode*> all_nodes;
    for(auto &c_graph : graphs_cached){
        for(auto &c_node : c_graph.nodes){
           bool already_included = false;
           for(auto &a_node : all_nodes){
               if(a_node->operations.size() != c_node->operations.size()){
                   continue;
               }
               bool not_same = false;
               for(auto &a_operation : a_node->operations){
                   if(!c_node->operations.contains(a_operation)){
                       not_same = true;
                       break;
                   }
               }
               if(!not_same){
                   already_included = true;
                   break;
               }else{
                   continue;
               }
           }
           if(!already_included){
               all_nodes.append(c_node);
           }
        }
    }

    for(int i{0}; i < all_nodes.size() - 1; i++){
        for(int j{i + 1}; j < all_nodes.size(); j++){
            if(all_nodes[i]->operations.size() > all_nodes[j]->operations.size()){
                GNode *c_node = all_nodes[i];
                all_nodes[i] = all_nodes[j];
                all_nodes[j] = c_node;
            }
        }
    }

    QVector<int> to_del;
    for(int i{0}; i < all_nodes.size() - 1; i++){
        QVector<QString> c_operations = all_nodes[i]->operations;
        QVector<QString> founded;
        for(int j{i + 1}; j < all_nodes.size(); j++){
            for(auto a_operation : all_nodes[j]->operations){
                if(founded.contains(a_operation)){
                    continue;
                }
                if(c_operations.contains(a_operation)){
                    founded.append(a_operation);
                    if(founded.size() == c_operations.size()){
                        break;
                    }
                }
            }
            if(founded.size() == c_operations.size()){
                break;
            }
        }
        if(founded.size() == c_operations.size()){
            to_del.append(i);
        }
    }

    for(int i{to_del.size() - 1}; i >= 0; i--){
        all_nodes.removeAt(to_del[i]);
    }

    int mod_id{1};
    for(auto &node : all_nodes){
        Module *new_module = new Module(mod_id++);
        new_module->operations.append(node->operations);
        modules_cached.append(new_module);
    }
    return modules_cached;
}

QVector<ProductModule> GKS::getProductsWithModules()
{
    if(!modules_cached.length() || !products_cached.size()){
        return product_module_cached;
    }
    populateModByOper();
    product_module_cached.clear();
    for(auto &c_product : products_cached){
        ProductModule new_prodmod;
        new_prodmod.product = c_product;
        for(int i{0}; i < c_product.operations.size(); i++){
            if(new_prodmod.modules.size()){
                Module *c_mod = new_prodmod.modules.last();
                if(c_mod->operations.contains(c_product.operations[i])){
                    continue;
                }
            }
            Module *new_mod = modules_by_operation.value(c_product.operations[i]);
            new_prodmod.modules.append(new_mod);
        }
        product_module_cached.append(new_prodmod);
    }
    return product_module_cached;
}

QVector<GNode*> GKS::checkClosedLoop(GNode *node, int depth_left)
{
    QVector<GNode*> ret_vect;
    if(!depth_left){
        node->check_in_progres = false;
        return ret_vect;
    }
    if(node->check_in_progres){
        node->check_in_progres = false;
        ret_vect.append(node);
        return ret_vect;
    }
    node->check_in_progres = true;
    for(auto f_node : node->outputs){
        ret_vect = checkClosedLoop(f_node, depth_left - 1);
        if(!ret_vect.size()){
            continue;
        }
        if(ret_vect.count(f_node) > 1){ //Вийшли з контуру
            break;
        }
        ret_vect.append(node);
        break;
    }
    node->check_in_progres = false;
    return ret_vect;
}

QVector<GNode *> GKS::checkChain(GNode *node, GNode *head_node, int depth_left)
{
    QVector<GNode*> ret_vect;
    if(!depth_left || node->check_in_progres){
        node->check_in_progres = false;
        return ret_vect;
    }
    node->check_in_progres = true;
    if(node != head_node){
        if(node->inputs.size() > 1){
            if(node->inputs.contains(head_node)){
                ret_vect.append(node);
            }
            node->check_in_progres = false;
            return ret_vect;
        }
        if(node->outputs.size() != 1){
            node->check_in_progres = false;
            return ret_vect;
        }
    }
    for(auto f_node : node->outputs){
        ret_vect = checkChain(f_node, head_node, depth_left - 1);
        if(!ret_vect.size()){
            continue;
        }
        if(node == head_node && ret_vect.size() == 1){
            ret_vect.clear();
            continue;
        }
        ret_vect.append(node);
    }
    node->check_in_progres = false;
    return ret_vect;
}

void GKS::squahNodes(QVector<GNode*> &nodes, Graph &target_graph)
{
    GNode *new_node = new GNode;
    for(auto &nod : nodes){ //Видалення внутрішніх зв'язків
        for(auto &nod2 : nodes){
            if(nod->inputs.contains(nod2)){
                nod->inputs.removeAll(nod2);
            }
            if(nod->outputs.contains(nod2)){
                nod->outputs.removeAll(nod2);
            }
        }
    }
    for(int i{0}; i < nodes.size(); i++){
        for(auto &inp : nodes[i]->inputs){
            /*bool skip = false;
            for(auto &node : nodes){
                if(node == inp){
                    skip = true;
                    break;
                }
            }
            if(skip){
                continue;
            }*/
            new_node->inputs.append(inp);
            inp->outputs.removeAll(nodes[i]);
            if(!inp->inputs.contains(new_node)){
                inp->outputs.append(new_node);
            }
        }
        for(auto &outp : nodes[i]->outputs){
            /*bool skip = false;
            for(auto &node : nodes){
                if(node == inp){
                    skip = true;
                    break;
                }
            }
            if(skip){
                continue;
            }*/
            new_node->outputs.append(outp);
            outp->inputs.removeAll(nodes[i]);
            if(!outp->inputs.contains(new_node)){
                outp->inputs.append(new_node);
            }
        }
        new_node->operations.append(nodes[i]->operations);
        /*for(auto &r_node : nodes[i]->outputs){
            r_node->inputs.removeAll(nodes[i]);
            if(!r_node->inputs.contains(new_node)){
                r_node->inputs.append(new_node);
            }
        }
        for(auto &r_node : nodes[i]->inputs){
            r_node->outputs.removeAll(nodes[i]);
            if(!r_node->outputs.contains(new_node)){
                r_node->outputs.append(new_node);
            }
        }*/
        target_graph.nodes.removeAll(nodes[i]);
        delete nodes[i];
    }
    target_graph.nodes.append(new_node);
}

void GKS::populateModByOper()
{
    if(!modules_cached.size()){
        return;
    }
    modules_by_operation.clear();
    for(auto &module : modules_cached){
        for(auto &mod_oper : module->operations){
            modules_by_operation.insert(mod_oper, module);
        }
    }
}


