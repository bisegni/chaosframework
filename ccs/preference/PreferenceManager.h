#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include "preference_types.h"

#include <QStringList>

#include <QString>

class PreferenceManager:
public chaos::common::utility::Singleton<PreferenceManager>{
    friend class chaos::common::utility::Singleton<PreferenceManager>;
public:
    bool activeNetworkConfiguration(const QString &configuration_name);
    QStringList getNetworkConfigurationNames();
    QString  getActiveNetworkConfigurationName();
private:
    PreferenceManager();
    ~PreferenceManager();
};

#endif // PREFERENCEMANAGER_H
