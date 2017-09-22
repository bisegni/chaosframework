#include "PreferenceManager.h"

#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

#include <QSettings>

using namespace chaos::metadata_service_client;

PreferenceManager::PreferenceManager() {

}

PreferenceManager::~PreferenceManager() {

}

bool PreferenceManager::activerNetworkConfiguration(const QString& configuration_name) {
    setDefaultNetworkConfiguration(configuration_name);
    return activeDefaultNetworkConfiguration();
}

void PreferenceManager::setDefaultNetworkConfiguration(const QString& configuration_name) {
    QSettings settings;
    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
    settings.setValue("active_configuration", configuration_name);
    settings.endGroup();
    settings.sync();
}

bool PreferenceManager::activeDefaultNetworkConfiguration() {
    QSettings settings;
    ChaosMetadataServiceClient::getInstance()->clearServerList();
    const QString active_configuration = getActiveConfigurationName();

    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);

    if(settings.childGroups().contains(active_configuration) == false) return false;
    settings.beginGroup(active_configuration);

    int mds_address_size = settings.beginReadArray("mds_address");
    for (int i = 0; i < mds_address_size; ++i) {
        settings.setArrayIndex(i);
        ChaosMetadataServiceClient::getInstance()->addServerAddress(settings.value("address").toString().toStdString());
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
    settings.endGroup();
    return true;
}

QStringList PreferenceManager::getNetowrkConfigurationsNames() {
    QSettings settings;
    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
    const QStringList result = settings.childGroups();
    settings.endGroup();
    return result;
}

QString  PreferenceManager::getActiveConfigurationName() {
    QSettings settings;
    ChaosMetadataServiceClient::getInstance()->clearServerList();
    settings.beginGroup(PREFERENCE_NETWORK_GROUP_NAME);
    const QString current_setting = settings.value("active_configuration").toString();
    settings.endGroup();
    return current_setting;
}
