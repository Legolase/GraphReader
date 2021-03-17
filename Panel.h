#ifndef PANEL_H
#define PANEL_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QStatusBar>
#include <QVector>
#include "qcustomplot.h"
#include "SensorData.h"

class Panel : public QMainWindow {
public:
    Panel(QWidget* parent = nullptr);

private:
    QCustomPlot* graf;
    QCPItemTracer* tracer;
    SensorData sensor;
    QLabel* gases, *heit;
    QLineEdit* ln;

public slots:
    void setgraph();
    //void pressTracer(QMouseEvent* event);
    void moveTracer(QMouseEvent* event);
    void pressMouse(QMouseEvent* event);

private:
    //functions
    double maxpoint(const QVector<double>& y);
};

#endif // PANEL_H
