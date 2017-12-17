#include "mainwindow.h"
#include <QMessageBox>
#include "paintingwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    /*QSharedPointer<QLineEdit> input_line = QSharedPointer<QLineEdit>(new QLineEdit(this));
    input_line->setGeometry(10,20,150,20);
    input_rows.push_back(input_line);*/
    text_input.setGeometry(margin,margin*2,200,220);
    do_work_button.setGeometry(margin+text_input.x()+text_input.width(),text_input.y(),130,20);
    do_work_button.setText("Do work");
    close_childs_button.setGeometry(do_work_button.x(), do_work_button.y() + do_work_button.height() + margin, do_work_button.width(), do_work_button.height());
    close_childs_button.setText(QString::fromLocal8Bit("Закрити вікна-таблиці"));
    close_childs_button.hide();
    instructions.setText(QString::fromLocal8Bit("Інструкція:\n - У поле, що знаходиться вище, необхідно увести\n\tоперації, необхідні для виготовлення виробів\n - Один рядок - один виріб\n - Допускаються абсолютно пусті рядки\n - У якості розділювача можна використовувати\n\tбудь-який символ, що не є літерою латинского алфавіту\n\tабо цифрою\n - Ввід є чутливим до регістру\n - Для здійснення обрахунків і виведення таблиць\n\tпісля закінчення вводу необхідно\n\tнатиснути на кнопку Do work"));
    instructions.setFixedHeight(180);
    instructions.move(text_input.x(), text_input.y()+text_input.height()+margin);
    connect(&do_work_button, &QPushButton::clicked, this, &MainWindow::startWorking);
    connect(&close_childs_button, &QPushButton::clicked, this, &MainWindow::closeChildrens);
    setFixedSize(do_work_button.x()+do_work_button.width()+margin,instructions.y()+instructions.height()+margin*2);
    instructions.setFixedWidth(width() - margin*2);
    move(20,20);
}

MainWindow::~MainWindow()
{
    if(fmw)
        delete fmw;
    if(smw)
        delete smw;
    if(tmw)
        delete tmw;
    if(ffmw)
        delete ffmw;
    if(fvmw)
        delete fvmw;
    if(pw)
        delete pw;
}

void MainWindow::parseInput(QString plain_input)
{
    QList<QString> first_list{plain_input.split('\n', QString::SkipEmptyParts)};
    working_set.clear();
    for(int i{0}; i < first_list.size(); i++){
        Product c_prod(i+1);
        c_prod.operations = first_list.at(i).split(QRegExp("\\W+"), QString::SkipEmptyParts);
        working_set.push_back(c_prod);
    }
}

void MainWindow::drawSchema(QVector<ProductModule> &targets)
{
    pw = new PaintingWindow();
    pw->initialize(targets);
    pw->setWindowTitle(QString::fromLocal8Bit("Оптимізована структурно-технологічна схема"));
    pw->show();
    pw->repaint();
}

void MainWindow::startWorking()
{
    closeChildrens();
    parseInput(text_input.toPlainText());
    if(!working_set.size()){
        return;
    }
    QList<QString> cols_names = gks.makeUniquesList(working_set);
    QVector<QVector<bool>> fmatrix = gks.createFirstMatrix();
    if(fmw){
        delete fmw;
    }
    fmw = new FirstMatrixWindow();
    fmw->setWindowTitle(QString::fromLocal8Bit("Таблиця 1. Операції за об'єктами"));
    fmw->show();
    QList<QString> rows_names;
    for(auto &it : working_set){
        QString str;
        str += QString::number(it.id) + ": ";
        for(auto &sit : it.operations){
            str += sit + " ";
        }
        rows_names.push_back(str);
    }
    fmw->setRows(rows_names);
    fmw->setCols(cols_names);
    fmw->setContent(fmatrix);

    QVector<QVector<unsigned int>> smw_data = gks.createSimilarityMatrix();
    if(smw)
        delete smw;
    smw = new FirstMatrixWindow();
    smw->setWindowTitle(QString::fromLocal8Bit("Таблиця 2. Матриця подібності"));
    QList<QString>cols_rows_names;
    for(auto &it : working_set){
        cols_rows_names.push_back(QString::number(it.id));
    }
    smw->setCols(cols_rows_names);
    smw->setRows(cols_rows_names);
    smw->setContent(smw_data);
    smw->show();

    if(tmw)
        delete tmw;
    tmw = new FirstMatrixWindow();
    tmw->setWindowTitle(QString::fromLocal8Bit("Таблиця 3. Розподіл на групи за матрицею подібності"));
    QVector<QVector<Product>> product_groups = gks.splitByGroupsP1();
    QVector<QVector<QString>> operations_by_groups = gks.countOperationsByGroups();
    QList<QString> second_col;
    rows_names.clear();
    for(int i{0}; i < product_groups.size(); i++){
        QString str;
        str += "Group #" + QString::number(i+1)+": ";
        for(auto &it : product_groups.at(i)){
            str += QString::number(it.id) + " ";
        }
        rows_names.push_back(str);
        str.clear();
        for(auto &it : operations_by_groups.at(i)){
            str += it + " ";
        }
        second_col.push_back(str);
    }
    tmw->setColFromStart(0,rows_names);
    tmw->setColFromStart(1,second_col);
    tmw->show();

    if(ffmw)
        delete ffmw;
    ffmw = new FirstMatrixWindow();
    ffmw->setWindowTitle(QString::fromLocal8Bit("Таблиця 4. Остаточний розподіл на групи"));
    gks.sortGroupsByOperationsCount();
    gks.splitByGroupsP2();
    product_groups = gks.getGroups();
    operations_by_groups = gks.getOperationsByGroups();
    rows_names.clear();
    second_col.clear();
    for(int i{0}; i < product_groups.size(); i++){
        QString str;
        str += "Group #" + QString::number(i+1)+": ";
        for(auto &it : product_groups.at(i)){
            str += QString::number(it.id) + " ";
        }
        rows_names.push_back(str);
        str.clear();
        for(auto &it : operations_by_groups.at(i)){
            str += it + " ";
        }
        second_col.push_back(str);
    }
    ffmw->setColFromStart(0,rows_names);
    ffmw->setColFromStart(1,second_col);
    ffmw->show();

    gks.createGraphs();
    gks.graphsDecomposition();
    QVector<Module*> created_modules = gks.createModules();
    if(fvmw){
        delete fvmw;
    }
    fvmw = new FirstMatrixWindow();
    fvmw->setWindowTitle(QString::fromLocal8Bit("Таблиця 5. Модулі"));
    rows_names.clear();
    second_col.clear();
    for(int i{0}; i < created_modules.size(); i++){
        QString str;
        str += "Module #" + QString::number(i+1);
        rows_names.push_back(str);
        str.clear();
        for(auto &operation : created_modules[i]->operations){
            str += operation + " ";
        }
        second_col.push_back(str);
    }
    fvmw->setColFromStart(0,rows_names);
    fvmw->setColFromStart(1,second_col);
    fvmw->show();

    QVector<ProductModule> prod_with_modules = gks.getProductsWithModules();
    drawSchema(prod_with_modules);

    close_childs_button.show();
}

void MainWindow::closeChildrens()
{
    if(fmw){
        if(fmw->isVisible()){
            fmw->close();
        }
        delete fmw;
        fmw = nullptr;
    }
    if(smw){
        if(smw->isVisible()){
            smw->close();
        }
        delete smw;
        smw = nullptr;
    }
    if(tmw){
        if(tmw->isVisible()){
            tmw->close();
        }
        delete tmw;
        tmw = nullptr;
    }
    if(ffmw){
        if(ffmw->isVisible()){
            ffmw->close();
        }
        delete ffmw;
        ffmw = nullptr;
    }
    if(fvmw){
        if(fvmw->isVisible()){
            fvmw->close();
        }
        delete fvmw;
        fvmw = nullptr;
    }
    if(pw){
        if(pw->isVisible()){
            pw->close();
        }
        delete pw;
        pw = nullptr;
    }
    close_childs_button.hide();
}
