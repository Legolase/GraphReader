#ifndef PANEL_H
#define PANEL_H

#include <QMainWindow>
#include <QWidget>
#include <QStatusBar>
#include "qcustomplot.h"

class Panel : public QMainWindow {
public:
    Panel(QWidget* parent = nullptr);

private:
    QCustomPlot* graf;
    QCPItemTracer* tracer;

public slots:
    void setgraph();
    void checkBar(const QString&);
    //void pressTracer(QMouseEvent* event);
    void moveTracer(QMouseEvent* event);

private:
    //functions
    bool right_line(const QString& line);
    QStringList get_lines(const QString& path);
    QVector<QVector<double>> Parser(const QStringList& lines);
    QVector<double> getY(const QVector<QVector<double>>& dates, const bool mode = 0);
    QVector<double> getX(const QVector<QVector<double>>& dates);
    double maxpoint(const QVector<double>& y);
};

#endif // PANEL_H
