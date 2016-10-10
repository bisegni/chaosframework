#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include <usr/local/include/chaos/common/utility/Singleton.h>

#include <QStatusBar>

class StatusManager:
        public chaos::common::utility::Singleton<StatusManager> {
public:

private:
    QStatusBar *app_status_bar;
    StatusManager();
    ~StatusManager();
};

#endif // STATUSMANAGER_H
