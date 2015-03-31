#include "CommandPresenter.h"
#include <QDebug>
CommandPresenter::CommandPresenter(QObject *parent,
                                   QMdiArea *_mdi_area_editors) :
    QObject(parent),
    mdi_area_editors(_mdi_area_editors)
{}

CommandPresenter::~CommandPresenter()
{}

void CommandPresenter::showCommandPresenter(PresenterWidget *pwidget) {
    if(pwidget == NULL) return;
    pwidget->setParent(mdi_area_editors);
    QMdiSubWindow *sub_win = mdi_area_editors->addSubWindow(pwidget);
    pwidget->setSubWindow(sub_win);
    sub_win->show();
    pwidget->updateUI();
}
