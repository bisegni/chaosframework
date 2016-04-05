#ifndef CNODESTATELABEL_H
#define CNODESTATELABEL_H

#include "ChaosMonitorWidgetCompanion.h"

#include <QLabel>
#include <QString>
class CNodeStateLabel:
        public QLabel,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    Q_OBJECT
public:
    explicit CNodeStateLabel(QWidget *parent = 0);
    ~CNodeStateLabel();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();

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

    void handlerHasBeenRegistered(const std::string& node_uid,
                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_state);
signals:

private slots:
    void updateStateUI();
    void updateErrorUI();
private:
    void	clear();
    void	setMovie(QMovie * movie);
    void	setNum(int num);
    void	setNum(double num);
    void	setPicture(const QPicture & picture);
    void	setPixmap(const QPixmap &pixmap);
    QString internal_state;
    chaos::metadata_service_client::node_monitor::OnlineState online_state;
    chaos::metadata_service_client::node_monitor::ErrorInformation error_information;

};

#endif // CNODESTATELABEL_H
