#ifndef PAINTINGWINDOW_H
#define PAINTINGWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include "struct_headers.h"

class PaintingWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PaintingWindow(QWidget *parent = nullptr);
    void initialize(QVector<ProductModule> &targets);
    void shuffleModules(QVector<int> &available_indexes, QVector<int> &selected_indexes);
    void checkShuffleSequence(QVector<int> &selected_indexes);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QVector<ProductModule> targets;
    QVector<Module*> cached_modules;
    Module *first_module;
    Module *last_module;
    double factor{1};
    ModuleShuffleScore best_score;

    QPoint drag_start{0,0};
    QPoint rel{0,0}, temp_rel{0,0};
signals:

public slots:
};

#endif // PAINTINGWINDOW_H
