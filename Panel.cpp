#include "Panel.h"
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QIcon>
#include <QFile>
#include <QVector>
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
    graf->setMinimumSize(QSize(500, 300));
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

    statusBar()->showMessage("Ready");

    connect(statusBar(), &QStatusBar::messageChanged, this, &Panel::checkBar);
    connect(graf, &QCustomPlot::mouseMove, this, &Panel::moveTracer);
}

bool Panel::right_line(const QString& line){
    QStringList v = line.split(' ');

    if (v.size() != 4)
        return false;

    bool number = true;
    for (int i = 1; i < v.size(); ++i){
        v[i].toDouble(&number);
        if (!number)
            return false;
    }

    v = v[0].split(':');

    if (v.size() != 3)
        return false;

    for (int i = 0; i < v.size(); ++i){
        v[i].toInt(&number);
        if (!number)
            return false;
    }

    return number;
}

QStringList Panel::get_lines(const QString& path){
    QStringList v;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return v;

    QTextStream getter(&f);

    while(!getter.atEnd()){
        QString s = getter.readLine();
        if (s != ""){
            if (!right_line(s))
                return QStringList();
            v.append(s);
        }
    }
    f.close();
    return v;
}

QVector<QVector<double>> Panel::Parser(const QStringList& lines){
    QVector<QVector<double>> stages;

    for (QString line : lines){
        if (line != ""){
            QStringList str_stage = line.split(' ');
            QStringList str_time = str_stage[0].split(':');

            QVector<double> stage = {double((str_time[0].toInt() * 3600) + (str_time[1].toInt() * 60) + (str_time[2].toInt()))};

            for (int i = 1; i < str_stage.size(); ++i)
                stage.append(str_stage[i].toDouble());

            stages.append(stage);
        }
        else
            return QVector<QVector<double>>(0, QVector<double> (0));
    }

    return stages;
}

QVector<double> Panel::getY(const QVector<QVector<double>>& dates, const bool mode){
    QVector<double> res;

    if (mode){
        for (QVector<double> date: dates)
            res.append((date[1] - (0.00115384)*(date[3] - 33))*(0.0004*(pow(date[2], 2)) - (0.027*date[2]) + 1.3894));
    }
    else
        for (QVector<double> date: dates)
            res.append(date[1]);

    return res;
}

QVector<double> Panel::getX(const QVector<QVector<double>>& dates){
    QVector<double> x;

    for(QVector<double> temp : dates)
        x.append(temp[0]);

    return x;
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
        graf->replot();
        statusBar()->showMessage("\"File not found\" © Nekit", 6000);
        return;
    }

    QStringList lines = get_lines(way);

    if (!lines.size()){
        graf->clearGraphs();
        graf->replot();
        statusBar()->showMessage("\"Not right file\'s type\" © Vovan", 6000);
        return;
    }

    QVector<QVector<double>> information = Parser(lines);

    QVector<double> x = getX(information), y_fault = getY(information), y = getY(information, 1);

    int maxpty = std::max(maxpoint(y_fault), maxpoint(y)) + 5;

    graf->clearGraphs();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();
    graf->addGraph();


    tracer->setGraph(graf->graph(4));
    tracer->setVisible(false);

    QPen MyPen;

    graf->graph(0)->addData(QVector<double>{0, double(x[x.size()-1])}, QVector<double>{(double)maxpty, (double)maxpty});
    graf->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 26)));
    graf->graph(0)->setPen(QPen(Qt::red));
    graf->graph(0)->setName("Завышенный");

    graf->graph(1)->addData(QVector<double>{0, double(x[x.size()-1])}, QVector<double>{200.0, 200.0});
    graf->graph(1)->setBrush(QBrush(QColor(255, 255, 0, 60)));
    graf->graph(1)->setPen(QPen(Qt::yellow));
    graf->graph(1)->setName("Удовлетворительный");

    graf->graph(2)->addData(QVector<double>{0, double(x[x.size()-1])}, QVector<double>{50.0, 50.0});
    graf->graph(2)->setBrush(QBrush(QColor(0, 200, 0, 70)));
    graf->graph(2)->setPen(QPen(Qt::green));
    graf->graph(2)->setName("Хороший");

    MyPen.setWidthF(2.0);
    MyPen.setColor(QColor(0, 156, 0, 255));
    graf->graph(3)->addData(x, y_fault);
    graf->graph(3)->setPen(MyPen);
    graf->graph(3)->setName("Без учёта");

    MyPen.setWidthF(2.4);
    MyPen.setColor(QColor(238, 0, 157, 255));
    graf->graph(4)->addData(x, y);
    graf->graph(4)->setPen(MyPen);
    graf->graph(4)->setName("С учётом");

    tracer->setVisible(true);

    graf->xAxis->setLabel("seconds");
    graf->yAxis->setLabel("ppm");

    graf->xAxis->setRange(0, x[x.size()-1]);
    graf->yAxis->setRange(0, maxpty);

    graf->legend->setVisible(true);
    graf->legend->setFont(QFont("Helvetica", 10));

    graf->replot();

    statusBar()->showMessage("\"Graph created\" © Vovan and Nekit", 6000);

}

void Panel::checkBar(const QString& message){
    if (message == ""){
        double x = tracer->position->key(), y = tracer->position->value();
        statusBar()->showMessage("x: " + QString().setNum(x) + " ; y: " + QString().setNum(y), 1);
    }
}

void Panel::moveTracer(QMouseEvent* event){
    auto conteiner = graf->graph(4)->data();
    auto coordX = graf->xAxis->pixelToCoord(event->pos().x());

    QVector<double> x = {coordX, coordX}, y = {0, 800};
    //vertical->setData(x, y);
    tracer->setGraphKey(coordX);
    //QVector<double> x = {100, 100}, y = {0, 1000};
    //vertical->setData(x, y);
    //tracer->setGraphKey(100);

    graf->replot();
}


//void Panel::moveTracer(QMouseEvent* event){
  //  /*if (QApplication::mouseButtons())*/ pressTracer(event);
//}
