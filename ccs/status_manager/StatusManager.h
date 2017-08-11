#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include <chaos/common/utility/Singleton.h>

#include <QStatusBar>

class StatusManager:
        public chaos::common::utility::Singleton<StatusManager> {
public:
    ~StatusManager();
private:
    QStatusBar *app_status_bar;
    StatusManager();

};

#endif // STATUSMANAGER_H
