#ifndef PREFERENCEMDSHOSTLISTMODEL_H
#define PREFERENCEMDSHOSTLISTMODEL_H

#include <QRegExp>
#include <QStringListModel>

class PreferenceMDSHostListModel :
        public QStringListModel {
    Q_OBJECT
public:
    explicit PreferenceMDSHostListModel(QObject *parent = 0);

protected:
    bool setData(const QModelIndex & index,
                 const QVariant & value,
                 int role = Qt::EditRole);

signals:

public slots:
};

#endif // PREFERENCEMDSHOSTLISTMODEL_H
