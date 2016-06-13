#include "GlobalServices.h"

GlobalServices::GlobalServices() {

}

CommandPresenter *GlobalServices::presenter() {
    return current_presenter;
}

void GlobalServices::setPresenter(CommandPresenter *presenter) {
    current_presenter = presenter;
}
