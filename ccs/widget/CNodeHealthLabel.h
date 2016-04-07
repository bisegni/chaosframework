#ifndef CNodeHealthLabel_H
#define CNodeHealthLabel_H

#include "ChaosMonitorWidgetCompanion.h"

#include <QLabel>
#include <QString>
class CNodeHealthLabel:
        public QLabel,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    Q_OBJECT

public:

    enum HealthAttribute { HealthTimestamp,
                           HealthUptime,
                           HealthUserTime,
                           HealthSystemTime,
                           HealthSwapTime,
                           HealthOnlineState,
                           HealthOperationalState,
                           HealthCustomAttribute};

    explicit CNodeHealthLabel(QWidget *parent = 0);
    ~CNodeHealthLabel();

    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();

    void setHealthAttribute(CNodeHealthLabel::HealthAttribute health_attribute);
    CNodeHealthLabel::HealthAttribute healthAttribute();

    void setCustomHealthAttribute(const QString& health_attribute);
    QString customHealthAttribute();
protected:
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_state);

    void nodeChangedInternalState(const std::string& node_uid,
                                  const std::string& old_state,
                                  const std::string& new_state);

    void nodeChangedErrorInformation(const std::string& node_uid,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& old_error_information,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& new_error_information);

    void nodeChangedHealthDataset(const std::string& node_uid,
                                  chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& map_health_dataset);
signals:

private slots:
    void updateStateUI();
    void updateErrorUI();
    void updateLabelUI( const QString& label);
private:
    void	clear();
    void	setMovie(QMovie * movie);
    void	setNum(int num);
    void	setNum(double num);
    void	setPicture(const QPicture & picture);
    void	setPixmap(const QPixmap &pixmap);

    Q_PROPERTY(QString custom_health_attribute READ customHealthAttribute WRITE setCustomHealthAttribute)
    QString p_custom_health_attribute;

    Q_ENUMS(HealthAttribute)
    Q_PROPERTY(HealthAttribute healthAttribute READ healthAttribute WRITE setHealthAttribute)
    HealthAttribute p_health_attribute;

    QString internal_state;
    chaos::metadata_service_client::node_monitor::OnlineState online_state;
    chaos::metadata_service_client::node_monitor::ErrorInformation error_information;

};

#endif // CNodeHealthLabel_H
