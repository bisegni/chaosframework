#ifndef TREEGROUPMANAGER_H
#define TREEGROUPMANAGER_H

#include <QWidget>
#include "../presenter/PresenterWidget.h"

namespace Ui {
class TreeGroupManager;
}

class TreeGroupManager :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit TreeGroupManager();
    ~TreeGroupManager();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private:
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
    Ui::TreeGroupManager *ui;
};

#endif // TREEGROUPMANAGER_H
