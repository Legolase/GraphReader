#include "SensorData.h"

void SensorData::clearData(){
    seconds.clear();
    seconds.shrink_to_fit();
    gasValue.clear();
    gasValue.shrink_to_fit();
    clearGasValue.clear();
    clearGasValue.shrink_to_fit();
    humidity.clear();
    humidity.shrink_to_fit();
    temperature.clear();
    temperature.shrink_to_fit();
    height.clear();
    height.shrink_to_fit();
    latitude.clear();
    latitude.shrink_to_fit();
    longitude.clear();
    longitude.shrink_to_fit();
    start_height = 0.0;
}

bool SensorData::rightStr(const QString& line){
    QStringList dates = line.split(' ');

    if (dates.size() != 7)
        return false;

    bool number = true;
    for (int i = 1; i < dates.size(); ++i){
        dates[i].toDouble(&number);
        if (!number)
            return false;
    }

    dates = dates[0].split(':');

    if (dates.size() != 3)
        return false;

    for (int i = 0; i < dates.size(); ++i){
        dates[i].toInt(&number);
        if (!number)
            return false;
    }

    return number;

}

void SensorData::convertToClear(){
    clearGasValue.clear();
    clearGasValue.shrink_to_fit();

    for (int i = 0; i < gasValue.size(); ++i)
        clearGasValue.push_back((gasValue[i] - (0.00115384)*(humidity[i] - 33))*(0.0004*(pow(temperature[i], 2)) - (0.027*temperature[i]) + 1.3894));
}

void SensorData::setData(const QString& path, bool *ok){
    this->clearData();
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)){
        if (ok)
            *ok = false;
        return;
    }

    QTextStream readerFile(&file);

    while(!readerFile.atEnd()){
        QString line = readerFile.readLine();
        if (!rightStr(line)){
            if (ok)
                *ok = false;
            clearData();
            return;
        }
        QStringList dates = line.split(' ');
        QStringList tm = dates[0].split(':');
        int s = (tm[0].toInt() * 3600) + (tm[1].toInt() * 60) + tm[2].toInt();
        seconds.push_back(s);
        gasValue.push_back(dates[1].toDouble());
        humidity.push_back(dates[2].toDouble());
        temperature.push_back(dates[3].toDouble());
        latitude.push_back(dates[4].toDouble());
        longitude.push_back(dates[5].toDouble());
        height.push_back(dates[6].toDouble() - start_height);
        if ((latitude[latitude.size()-1] != 0) && (start_height == 0)){
            start_height = height[height.size()-1];
            height[height.size() - 1] = dates[6].toDouble() - start_height;
        }
        convertToClear();
    }

    if (seconds.size() < 3){
        if (ok)
            *ok = false;
        clearData();
        return;
    }

    if (ok)
        *ok = true;
    return;
}

int SensorData::timeToIndex(const int &s){
    const int size = this->seconds.size();

    for (int i = 0; i < size; ++i)
        if (this->seconds[i] == s)
            return i;
    return -1;
}
