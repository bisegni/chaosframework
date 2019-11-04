#include "PreferenceManager.h"

#include <QSettings>
#include <QDebug>

using namespace chaos::metadata_service_client;

PreferenceManager::PreferenceManager() {}

PreferenceManager::~PreferenceManager() {}

bool PreferenceManager::activeNetworkConfiguration(const QString &configuration_name) {
    QSettings settings("it.infn", "ccs");
    ChaosMetadataServiceClient::getInstance()->clearServerList();

    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);

    if(settings.childGroups().contains(configuration_name) == false) return false;
    settings.beginGroup(configuration_name);

    qDebug() << "Using mds address:";
    int mds_address_size = settings.beginReadArray("mds_address");
    for (int i = 0; i < mds_address_size; ++i) {
        settings.setArrayIndex(i);
        ChaosMetadataServiceClient::getInstance()->addServerAddress(settings.value("address").toString().toStdString());
        qDebug() << settings.value("address");
    }
    settings.endArray();
    settings.endGroup();
    //check if monitoring is started
    if(mds_address_size &&
            !ChaosMetadataServiceClient::getInstance()->monitoringIsStarted()) {
        //try to start it
        ChaosMetadataServiceClient::getInstance()->enableMonitor();
    }
    ChaosMetadataServiceClient::getInstance()->reconfigureMonitor();

    //set new configuration as active
    settings.setValue("active_configuration", configuration_name);

    settings.endGroup();
    return true;
}

QStringList PreferenceManager::getNetworkConfigurationNames() {
    QSettings settings("it.infn", "ccs");
    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
    const QStringList result = settings.childGroups();
    settings.endGroup();
    return result;
}

QString  PreferenceManager::getActiveNetworkConfigurationName() {
    QSettings settings("it.infn", "ccs");
    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
    const QString current_setting = settings.value("active_configuration").toString();
    settings.endGroup();
    return current_setting;
}
