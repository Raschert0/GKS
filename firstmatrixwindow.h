#ifndef FIRSTMATRIXWINDOW_H
#define FIRSTMATRIXWINDOW_H

#include <QWidget>
#include <QTableWidget>

class FirstMatrixWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FirstMatrixWindow(QWidget *parent = nullptr);
    void setRows(QList<QString> t_list);
    void setCols(QList<QString> t_list);
    void setContent(QVector<QVector<bool>> t_list);
    void setContent(QVector<QVector<unsigned int>> t_list);
    void setColFromStart(int col_id, QList<QString> t_list);

private:
    QTableWidget tabw{this};

signals:

public slots:
};

#endif // FIRSTMATRIXWINDOW_H
