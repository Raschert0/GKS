#include "paintingwindow.h"
#include <QPainter>
#include <QMap>
#include <algorithm>
#include <QDebug>

PaintingWindow::PaintingWindow(QWidget *parent) : QWidget(parent)
{
    move(250, 40);
    resize(450, 700);
    setMouseTracking(true);
}

void PaintingWindow::initialize(QVector<ProductModule> &targets)
{
    this->targets = targets;
    QVector<Module*> all_modules;
    QMap<Module*, short> inputs;
    QMap<Module*, short> outputs;

    for(auto &it : targets){
        for(const auto &module : it.modules){ //Формування вектору всіх модулів
            if(all_modules.contains(module)){
                continue;
            }
            all_modules.append(module);
        }

        if(inputs.contains(it.modules.first())){ //Знаходження модуля з найб. кількістю входів: крок 1
            inputs[it.modules.first()]++;
        }else{
            inputs.insert(it.modules.first(), 1);
        }

        if(outputs.contains(it.modules.last())){ //Знаходження модуля з найб. кількістю виходів: крок 1
            outputs[it.modules.last()]++;
        }else{
            outputs.insert(it.modules.last(), 1);
        }
    }

    short max_val{0};
    Module *max_mod{nullptr};
    QMap<Module*, short>::iterator it;
    for(it = inputs.begin(); it != inputs.end(); it++){ //Знаходження модуля з найб. кількустю входів: крок 2
        if(it.value() > max_val){
            max_mod = it.key();
            max_val = it.value();
        }
    }
    first_module = max_mod;
    max_val = 0;
    max_mod = nullptr;
    for(it = outputs.begin(); it != outputs.end(); it++){ //Знаходження модуля з найб. кількустю виходів: крок 2
        if(it.key() == first_module){
            continue;
        }
        if(it.value() > max_val){
            max_mod = it.key();
            max_val = it.value();
        }
    }
    last_module = max_mod;

    QVector<Module*> modules_to_shuffle(all_modules);
    modules_to_shuffle.removeOne(first_module);
    modules_to_shuffle.removeOne(last_module);

    cached_modules = all_modules;

    if(modules_to_shuffle.length()){
        QVector<int> a_ind;
        QVector<int> empty_vect;
        for(int i{1}; i < all_modules.length() - 1; i++){
            a_ind.append(i);
        }
        shuffleModules(a_ind, empty_vect); //Оптимізація схеми
    }
}

void PaintingWindow::shuffleModules(QVector<int> &available_indexes, QVector<int> &selected_indexes)
{
    if(available_indexes.length() > 1){
        for(int it : available_indexes){
            QVector<int> new_avail(available_indexes);
            QVector<int> new_sel(selected_indexes);
            new_avail.removeOne(it);
            new_sel.append(it);
            shuffleModules(new_avail, new_sel);
        }
    }else{
        QVector<int> new_sel(selected_indexes);
        new_sel.append(available_indexes[0]);
        checkShuffleSequence(new_sel);
    }
}

void PaintingWindow::checkShuffleSequence(QVector<int> &selected_indexes)
{
    QVector<Module *> to_shuffle(cached_modules);
    ModuleShuffleScore new_score;
    to_shuffle.removeOne(first_module);
    to_shuffle.removeOne(last_module);
    new_score.modules.append(first_module);
    for(int mod_ind : selected_indexes){
        new_score.modules.append(to_shuffle[mod_ind - 1]);
    }
    new_score.modules.append(last_module);

    uint reverse_count{0};
    for(ProductModule &it : targets){
        int last_i{0};
        for(Module* &mod : it.modules){
            int cur_i{new_score.modules.indexOf(mod)};
            if(cur_i < last_i){
                reverse_count++;
            }
            last_i = cur_i;
        }
    }
    new_score.score = reverse_count;

    if(new_score.score < best_score.score){
        best_score = new_score;
    }
}

struct DrawedLine
{
    DrawedLine(){}
    DrawedLine(Module *f, Module *t):from{f}, to{t}{}
    Module *from{nullptr}, *to{nullptr};
    QString label;
    bool operator==(const DrawedLine& DL)const{
        return from == DL.from && to == DL.to;
    }
};

void PaintingWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::BrushStyle::NoBrush);
    painter.setPen(QColor(Qt::GlobalColor::black));
    painter.fillRect(0, 0, width(), height(), Qt::GlobalColor::white);

    double module_width_per_operation{18 * factor};
    double module_height_per_product{13 * factor};
    double padding_interlines{12 * factor};
    double padding_intermodules{17 * factor};
    double inputs_labels_offset(-20 * factor);

    QVector<Module*> drawed_modules;
    QVector<QPointF> module_starts;
    QVector<DrawedLine> lines_to_draw;
    QVector<Module*> to_draw{best_score.modules};
    QMap<Module*, int> available_right_connections;

    double mod_width{50 * factor};
    for(Module* &it : to_draw){
        mod_width = std::max(mod_width, it->operations.length() * module_width_per_operation);
    }

    double mod_height{(targets.length() + 1) * module_height_per_product};
    int line_count{1};
    QPoint draw_start(35 + temp_rel.x(), 20 + temp_rel.y());


/*
 * Modules drawing
 */

    for(Module* &it : to_draw){
        QPoint draw_point;
        if(module_starts.length()){
            draw_point.setX(module_starts.last().x());
            draw_point.setY(module_starts.last().y() + mod_height + padding_intermodules);
        }else{
            draw_point = draw_start;
        }
        module_starts.append(draw_point);

        painter.drawRect(draw_point.x(), draw_point.y(), mod_width, mod_height);
        QString draw_string;
        for(auto &operation : it->operations){
            draw_string.append(operation);
            draw_string.append(" ");
        }
        painter.drawText(draw_point.x() + 5, draw_point.y() + mod_height * 0.5 + 5, draw_string);

        drawed_modules.append(it);
    }

/*
 * Drawing inputs/ouputs
 */

    for(ProductModule &it : targets){
        QPointF inp_mod_pos{module_starts[drawed_modules.indexOf(it.modules.first())]};
        QPointF out_mod_pos{module_starts[drawed_modules.indexOf(it.modules.last())]};

        QPointF my_inp_pos(inp_mod_pos.x() + inputs_labels_offset, inp_mod_pos.y() + it.product.id * module_height_per_product);
        QPointF my_out_pos(out_mod_pos.x() + inputs_labels_offset, out_mod_pos.y() + it.product.id * module_height_per_product);

        painter.drawText(my_inp_pos, QString::number(it.product.id));
        painter.drawText(my_out_pos, QString::number(it.product.id));

        painter.drawLine(my_inp_pos.x(), my_inp_pos.y() + 1, my_inp_pos.x() - inputs_labels_offset, my_inp_pos.y() + 1);
        painter.drawLine(my_out_pos.x() - 15, my_out_pos.y() + 1, my_out_pos.x() - inputs_labels_offset, my_out_pos.y() + 1);

        painter.drawLine(my_inp_pos.x() - inputs_labels_offset, my_inp_pos.y() + 1, my_inp_pos.x() - inputs_labels_offset - 7, my_inp_pos.y() + 1 - module_height_per_product * 0.4);
        painter.drawLine(my_inp_pos.x() - inputs_labels_offset, my_inp_pos.y() + 1, my_inp_pos.x() - inputs_labels_offset - 7, my_inp_pos.y() + 1 + module_height_per_product * 0.4);
        painter.drawLine(my_out_pos.x() - 15, my_out_pos.y() + 1, my_out_pos.x() - 8, my_out_pos.y() + 1 - module_height_per_product * 0.4);
        painter.drawLine(my_out_pos.x() - 15, my_out_pos.y() + 1, my_out_pos.x() - 8, my_out_pos.y() + 1 + module_height_per_product * 0.4);
    }

/*
 * Preparing intermodules lines
 */

    for(ProductModule &it : targets){
        for(int i{1}; i < it.modules.length(); i++){
            Module *cur_mod{it.modules[i]};
            Module *prev_mod{it.modules[i-1]};
            DrawedLine new_line(prev_mod, cur_mod);
            if(lines_to_draw.contains(new_line)){
                lines_to_draw[lines_to_draw.indexOf(new_line)].label.append(QString(" " + QString::number(it.product.id)));
            }else{
                new_line.label = QString::number(it.product.id);
                lines_to_draw.push_back(new_line);
            }
        }
    }

/*
 * Drawing intermodules lines
 */

    for(DrawedLine &it : lines_to_draw){
        int pos_diff{drawed_modules.indexOf(it.from) - drawed_modules.indexOf(it.to)}; //-1 - desc, 1 - asc, else - right side
        if(pos_diff == 1){
            for(DrawedLine &iit : lines_to_draw){
                if(it == iit){
                    continue;
                }
                if(it.from == iit.to && it.to == iit.from){
                    pos_diff = 2; //draw at right side
                    break;
                }
            }
        }

        if(pos_diff == -1){
            QPointF from_mod_pos{module_starts[drawed_modules.indexOf(it.from)]};
            QPointF to_mod_pos{module_starts[drawed_modules.indexOf(it.to)]};

            QPointF draw_start(from_mod_pos.x() + 10, from_mod_pos.y() + mod_height);
            QPointF draw_end(to_mod_pos.x() + 10, to_mod_pos.y());

            painter.drawLine(draw_start, draw_end);
            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() - module_width_per_operation * 0.4, draw_end.y() - 7);
            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() + module_width_per_operation * 0.4, draw_end.y() - 7);

            painter.drawText(draw_end.x() + 2, draw_start.y() + (draw_end.y() - draw_start.y()) * 0.5 + 5, it.label);
        }else if(pos_diff == 1){
            QPointF from_mod_pos{module_starts[drawed_modules.indexOf(it.from)]};
            QPointF to_mod_pos{module_starts[drawed_modules.indexOf(it.to)]};

            QPointF draw_start(from_mod_pos.x() + 10, from_mod_pos.y());
            QPointF draw_end(to_mod_pos.x() + 10, to_mod_pos.y() + mod_height);

            painter.drawLine(draw_start, draw_end);
            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() - module_width_per_operation * 0.4, draw_end.y() + 7);
            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() + module_width_per_operation * 0.4, draw_end.y() + 7);

            painter.drawText(draw_end.x() + 2, draw_end.y() + (draw_start.y() - draw_end.y()) * 0.5 + 5, it.label);
        }else{
            QPointF from_mod_pos{module_starts[drawed_modules.indexOf(it.from)]};
            QPointF to_mod_pos{module_starts[drawed_modules.indexOf(it.to)]};

            int avail_conn_start{1};
            if(available_right_connections.count(it.from)){
                avail_conn_start = available_right_connections[it.from];
            }else{
                available_right_connections.insert(it.from, 1);
            }
            available_right_connections[it.from]++;

            int avail_conn_end{1};
            if(available_right_connections.count(it.to)){
                avail_conn_end = available_right_connections[it.to];
            }else{
                available_right_connections.insert(it.to, 1);
            }
            available_right_connections[it.to]++;

            QPoint draw_start(from_mod_pos.x() + mod_width, from_mod_pos.y() + avail_conn_start * module_height_per_product);
            QPoint draw_end(to_mod_pos.x() + mod_width, to_mod_pos.y() + avail_conn_end * module_height_per_product);

            painter.setPen(QColor(Qt::GlobalColor::gray));
            painter.drawLine(draw_start.x(), draw_start.y(), draw_start.x() + (lines_to_draw.length() + 1) * padding_interlines, draw_start.y());
            painter.setPen(QColor(Qt::GlobalColor::black));
            painter.drawLine(draw_start.x(), draw_start.y(), draw_start.x() + line_count * padding_interlines, draw_start.y());
            painter.drawLine(draw_start.x() + line_count * padding_interlines, draw_start.y(), draw_end.x() + line_count * padding_interlines, draw_end.y());
            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() + line_count * padding_interlines, draw_end.y());
            line_count++;

            painter.drawText(draw_start.x() + (lines_to_draw.length() + 1) * padding_interlines + 2, draw_start.y() - 1, it.label);

            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() + 7, draw_end.y() + module_height_per_product * 0.4);
            painter.drawLine(draw_end.x(), draw_end.y(), draw_end.x() + 7, draw_end.y() - module_height_per_product * 0.4);
        }
    }
}

void PaintingWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){
        drag_start = event->pos();
    }
}

void PaintingWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!(event->buttons() & Qt::LeftButton))
           return;

    temp_rel = rel + (event->pos() - drag_start);
    update();
}

void PaintingWindow::mouseReleaseEvent(QMouseEvent *event)
{
    rel = temp_rel;
    drag_start.setX(0);
    drag_start.setY(0);
    update();
}

void PaintingWindow::wheelEvent(QWheelEvent *event)
{
    double new_factor{factor};
    if(event->angleDelta().y() > 0){
        new_factor *= 6.0/5.0;
    }else if(event->angleDelta().y() < 0){
        new_factor *= 5.0/6.0;
    }else{
        return;
    }

    if(new_factor > 10 || new_factor < 0.2){
        return;
    }
    //factor = new_factor;
    //update();
}
