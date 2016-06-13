#ifndef GLOBALSERVICES_H
#define GLOBALSERVICES_H

#include "presenter/CommandPresenter.h"

#include <chaos/common/utility/Singleton.h>

class GlobalServices:
        public chaos::common::utility::Singleton<GlobalServices> {
    friend class Singleton<GlobalServices>;
    friend class MainWindow;
public:
    CommandPresenter *presenter();
private:
    GlobalServices();

    void setPresenter(CommandPresenter *presenter);

    CommandPresenter *current_presenter;
};

#endif // GLOBALSERVICES_H
