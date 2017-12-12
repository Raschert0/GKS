#include "firstmatrixwindow.h"
#include <QHeaderView>

FirstMatrixWindow::FirstMatrixWindow(QWidget *parent) : QWidget(parent)
{
    tabw.move(10,10);
    tabw.setColumnCount(1);
    tabw.setRowCount(1);
    tabw.setFixedSize(600,300);
    tabw.setHorizontalHeaderLabels(QStringList(""));
    tabw.setVerticalHeaderLabels(QStringList(""));
    setMinimumSize(tabw.width()+20,tabw.height()+20);
}

void FirstMatrixWindow::setRows(QList<QString> t_list)
{
    tabw.setRowCount(t_list.size()+1);
    QStringList n_list;
    for(int i{0}; i < tabw.rowCount(); i++){
        n_list.push_back("");
    }
    tabw.setVerticalHeaderLabels(n_list);
    for(int i{0}; i < t_list.size(); i++){
        tabw.setItem(i+1,0,new QTableWidgetItem(t_list[i]));
    }
}

void FirstMatrixWindow::setCols(QList<QString> t_list)
{
    tabw.setColumnCount(t_list.size()+1);
    QStringList n_list;
    for(int i{0}; i < tabw.columnCount(); i++){
        n_list.push_back("");
    }
    tabw.setHorizontalHeaderLabels(n_list);
    for(int i{0}; i < t_list.size(); i++){
        tabw.setItem(0,i+1,new QTableWidgetItem(t_list.at(i)));
    }
    tabw.resizeColumnsToContents();
}

void FirstMatrixWindow::setContent(QVector<QVector<bool> > t_list)
{
    for(int i{0}; i < t_list.size(); i++){
        for(int j{0}; j < t_list.at(i).size(); j++){
            if(t_list.at(i).at(j)){
                tabw.setItem(i+1,j+1,new QTableWidgetItem("1"));
            }else{
                tabw.setItem(i+1,j+1,new QTableWidgetItem("0"));
            }
        }
    }
    tabw.resizeColumnsToContents();
}

void FirstMatrixWindow::setContent(QVector<QVector<unsigned int> > t_list)
{
    for(int i{0}; i < t_list.size(); i++){
        for(int j{0}; j < t_list.at(i).size(); j++){
            tabw.setItem(i+1,j+1,new QTableWidgetItem(QString::number(t_list.at(i).at(j))));
        }
    }
    tabw.resizeColumnsToContents();
}

void FirstMatrixWindow::setColFromStart(int col_id, QList<QString> t_list)
{
    if(col_id + 1> tabw.columnCount()){
        tabw.setColumnCount(col_id+1);
    }
    if(t_list.size() + 1 > tabw.rowCount()){
        tabw.setRowCount(t_list.size());
    }
    QStringList n_list;
    for(int i{0}; i < tabw.rowCount(); i++){
        n_list.push_back("");
    }
    tabw.setVerticalHeaderLabels(n_list);
    QStringList nn_list;
    for(int i{0}; i < tabw.columnCount(); i++){
        nn_list.push_back("");
    }
    tabw.setHorizontalHeaderLabels(nn_list);
    for(int i{0}; i < t_list.size(); i++){
        tabw.setItem(i,col_id,new QTableWidgetItem(t_list[i]));
    }
    tabw.resizeColumnsToContents();
}
