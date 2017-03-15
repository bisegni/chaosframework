#ifndef JSONEDITOR_H
#define JSONEDITOR_H

#include "../data/json/JsonTableModel.h"
#include "../presenter/PresenterWidget.h"

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class JsonEditor;
}

class JsonEditor :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit JsonEditor(QWidget *parent = 0);
    ~JsonEditor();
signals:
    void acceptedJSONDocument(const QString& json_document);
public slots:
    void loadJSONDocument(const QString& json_document);
protected:
    void initUI();
    bool isClosing();
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
private slots:
    void on_pushButtonSaveAction_clicked();
    void selectionChanged(const QItemSelection& selected,
                          const QItemSelection& deselected);
private:
    Ui::JsonEditor *ui;
    JsonTableModel json_tb;
};

#endif // JSONEDITOR_H
