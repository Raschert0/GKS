#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QSharedPointer>
#include <QPlainTextEdit>
#include <QPushButton>
#include "gks_header.h"
#include "firstmatrixwindow.h"
#include "paintingwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void parseInput(QString plain_input);
    void drawSchema(QVector<ProductModule> &targets);

    QPlainTextEdit text_input{this};
    QPushButton do_work_button{this};
    QPushButton close_childs_button{this};
    QPushButton load_from_json_button{this};
    const int margin{10};
    QList<Product> working_set;
    GKS gks;
    QLabel instructions{this};
    FirstMatrixWindow *fmw{nullptr}, *smw{nullptr}, *tmw{nullptr}, *ffmw{nullptr}, *fvmw{nullptr};
    PaintingWindow *pw{nullptr};

public slots:
    void startWorking();
    void closeChildrens();
    void loadFromJson();
};

#endif // MAINWINDOW_H
