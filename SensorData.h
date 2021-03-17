#pragma once
#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

class SensorData{
public:
    QVector<double> seconds, gasValue, clearGasValue, humidity, temperature, height, latitude, longitude;
    float start_height;

    SensorData(): seconds(), gasValue(), clearGasValue(), humidity(), temperature(), height(), latitude(), longitude(), start_height(0.0) {}
    void setData(const QString& path, bool* ok = nullptr);
    void clearData();
    int timeToIndex(const int& s);
private:
    bool rightStr(const QString& line);
    void convertToClear();
};

#endif //SENSORDATA_H
