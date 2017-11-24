#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include "preference_types.h"

#include <QStringList>

#include <QString>

class PreferenceManager:
public chaos::common::utility::Singleton<PreferenceManager>{
    friend class chaos::common::utility::Singleton<PreferenceManager>;
public:
    bool activerNetworkConfiguration(const QString &configuration);
    QStringList getNetowrkConfigurationsNames();
    QString  getActiveConfigurationName();
    void setDefaultNetworkConfiguration(const QString& configuration_name);
    bool activeDefaultNetworkConfiguration();
private:
    PreferenceManager();
    ~PreferenceManager();
};

#endif // PREFERENCEMANAGER_H
