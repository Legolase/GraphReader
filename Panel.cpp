#include "Panel.h"
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QIcon>
#include <QFile>
#include <QGridLayout>

Panel::Panel(QWidget* parent) : QMainWindow(parent) {
    QPixmap loadPix(":resorse\\Image\\binoculars.png");
    QPixmap exitPix(":resorse\\Image\\exit.png");

    QAction *load = new QAction(loadPix, "&Load", this);
    QAction *exitt = new QAction(exitPix, "&Exit", this);

    connect(load, &QAction::triggered, this, &Panel::setgraph);
    connect(exitt, &QAction::triggered, qApp, &QApplication::quit);

    QMenu* file;
    file = menuBar()->addMenu("&File");

    file->addAction(load);
    file->addSeparator();
    file->addAction(exitt);

    graf = new QCustomPlot(this);
    graf->setMinimumSize(QSize(600, 360));
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();

    tracer = new QCPItemTracer(graf);
    tracer->setGraph(graf->graph(4));
    QPen a;
    a.setWidthF(1.0);
    a.setColor(Qt::black);
    tracer->setPen(a);

    QGridLayout* gr = new QGridLayout(this);
    gr->setHorizontalSpacing(10);
    gr->setVerticalSpacing(10);
    gr->addWidget(graf);

    QWidget* Window = new QWidget();
    Window->setLayout(gr);

    setCentralWidget(Window);

    ln = new QLineEdit(this);
    ln->setReadOnly(true);
    ln->setFixedWidth(210);

    gases = new QLabel(this);
    gases->setMinimumWidth(220);
    heit = new QLabel(this);
    heit->setFixedWidth(115);

    statusBar()->addWidget(gases);
    statusBar()->addWidget(ln);
    statusBar()->addWidget(heit);

    gases->setText("seconds: ; Gas Value ");
    heit->setText(" Height:");

    connect(graf, &QCustomPlot::mousePress, this, &Panel::pressMouse);
    connect(graf, &QCustomPlot::mouseMove, this, &Panel::moveTracer);
}

double Panel::maxpoint(const QVector<double>& y){
    if (y.size()){
        double max = 0;

        for (int i = 0; i < y.size(); ++i)
            if (y[i] > max)
                max= y[i];
        return max;
    }
    return 1;
}

void Panel::setgraph(){
    QString way = QFileDialog::getOpenFileName();

    if (way == ""){
        graf->clearGraphs();
        graf->addGraph();
        graf->addGraph();
        graf->addGraph();
        graf->addGraph();
        graf->addGraph();
        graf->replot();
        sensor.clearData();
        statusBar()->showMessage("\"File not found\" © Nekit", 6000);
        return;
    }

    bool is_ok = true;
    sensor.setData(way, &is_ok);

    if (!is_ok){
        graf->clearGraphs();
        graf->addGraph();
        graf->addGraph();
        graf->addGraph();
        graf->addGraph();
        graf->addGraph();
        graf->replot();
        statusBar()->showMessage("Error reading file. Check it's exsisting and type", 3000);
        return;
    }

    int maxpty = std::max(maxpoint(sensor.gasValue), maxpoint(sensor.clearGasValue)) + 5;

    graf->clearGraphs();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();


    tracer->setGraph(graf->graph(4));
    tracer->setVisible(false);

    QPen MyPen;

    graf->graph(0)->addData(QVector<double>{0, double(sensor.seconds[sensor.seconds.size() - 1])}, QVector<double>{(double)maxpty, (double)maxpty});
    graf->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 26)));
    graf->graph(0)->setPen(QPen(Qt::red));
    graf->graph(0)->setName("Завышенный");

    graf->graph(1)->addData(QVector<double>{0, double(sensor.seconds[sensor.seconds.size() - 1])}, QVector<double>{200.0, 200.0});
    graf->graph(1)->setBrush(QBrush(QColor(255, 255, 0, 60)));
    graf->graph(1)->setPen(QPen(Qt::yellow));
    graf->graph(1)->setName("Удовлетворительный");

    graf->graph(2)->addData(QVector<double>{0, double(sensor.seconds[sensor.seconds.size() - 1])}, QVector<double>{50.0, 50.0});
    graf->graph(2)->setBrush(QBrush(QColor(0, 200, 0, 70)));
    graf->graph(2)->setPen(QPen(Qt::green));
    graf->graph(2)->setName("Хороший");

    MyPen.setWidthF(2.0);
    MyPen.setColor(QColor(0, 156, 0, 255));
    graf->graph(3)->addData(sensor.seconds, sensor.gasValue);
    graf->graph(3)->setPen(MyPen);
    graf->graph(3)->setName("Без учёта");

    MyPen.setWidthF(2.4);
    MyPen.setColor(QColor(238, 0, 157, 255));
    graf->graph(4)->addData(sensor.seconds, sensor.clearGasValue);
    graf->graph(4)->setPen(MyPen);
    graf->graph(4)->setName("С учётом");

    tracer->setVisible(true);

    graf->xAxis->setLabel("seconds");
    graf->yAxis->setLabel("ppm");

    graf->xAxis->setRange(0, sensor.seconds[sensor.seconds.size() - 1]);
    graf->yAxis->setRange(0, maxpty);

    graf->legend->setVisible(true);
    graf->legend->setFont(QFont("Helvetica", 9));

    graf->replot();

    statusBar()->showMessage("\"Graph created\" © Vovan and Nekit", 3000);

}

void Panel::moveTracer(QMouseEvent* event){
    if(QApplication::mouseButtons()){
        auto conteiner = graf->graph(4)->data();
        auto coordX = graf->xAxis->pixelToCoord(event->pos().x());

        tracer->setGraphKey(coordX);

        double x = tracer->position->key(), y = tracer->position->value();
        int index = sensor.timeToIndex(x);
        gases->setText("seconds: " + QString().setNum(x)
                       + " ; Gas Value: " + QString().setNum(y));
        QString sheight = " Height: ";
        if ((index != -1) && (sensor.latitude[index] != 0))
            sheight += (sensor.height[index] >= 0) ? QString().setNum(sensor.height[index]) : "0.0";

        ln->setText(QString().setNum(sensor.latitude[index], 'g', 15) + " " + QString().setNum(sensor.longitude[index], 'g', 15));
        heit->setText(sheight);

        graf->replot();
    }
}

void Panel::pressMouse(QMouseEvent* event){
    if(QApplication::mouseButtons()) moveTracer(event);
}
//void Panel::moveTracer(QMouseEvent* event){
  //  /*if (QApplication::mouseButtons())*/ pressTracer(event);
//}
