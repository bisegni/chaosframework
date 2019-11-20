#ifndef ChaosBaseDatasetAttributeUI_H
#define ChaosBaseDatasetAttributeUI_H

#include "ChaosBaseDatasetUI.h"

class ChaosBaseDatasetAttributeUI:
        public ChaosBaseDatasetUI {
    Q_OBJECT
    Q_PROPERTY(QString attributeName READ attributeName WRITE setAttributeName)
public:
    ChaosBaseDatasetAttributeUI(QWidget *parent = nullptr);

    QString attributeName() const;
    virtual void setAttributeName(const QString& new_attribute_name);
    void dragEnterEvent (QDragEnterEvent* event);
    void dragMoveEvent (QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    QJsonObject serialize();
    void deserialize(QJsonObject& serialized_data);
protected:
    void updateEditInformation();
    void execContextualMenuForDatasetSelection(QPoint pos);
public slots:
    void updateData(int dataset_type,
                    QString attribute_name,
                    QVariant attribute_value);
    // widget if is a input one need to clear his state to show as normal
    virtual void changeSetCommitted();
    virtual void changeSetRollback();
private slots:
    void  datasetTriggerAction(bool checked = false);
signals:
    void attributeChangeSetUpdated(QString device_id,
                                   QString attribute_name,
                                   QVariant attribute_value);
    void attributeChangeSetClear(QString device_id,
                                 QString attribute_name);
private:
    QString p_attribute_name;
};

#endif // ChaosBaseDatasetAttributeUI_H
