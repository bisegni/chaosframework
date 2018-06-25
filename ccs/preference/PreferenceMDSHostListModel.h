#ifndef PREFERENCEMDSHOSTLISTMODEL_H
#define PREFERENCEMDSHOSTLISTMODEL_H

#include <QStringListModel>

class PreferenceMDSHostListModel :
        public QStringListModel {
    Q_OBJECT
public:
    explicit PreferenceMDSHostListModel(QObject *parent = 0);
    void setStringList(const QStringList& host_port_splitted);
    bool addMDSServer(const QString& hostname_port);
    bool setData(const QModelIndex & index,
                 const QVariant & value,
                 int role = Qt::EditRole);

signals:

public slots:
private:
     QStringList server_list;
};

#endif // PREFERENCEMDSHOSTLISTMODEL_H
