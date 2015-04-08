#include "CommandPresenter.h"
#include "PresenterWidget.h"
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
    pwidget->presenter_instance = this;
    pwidget->setParent(mdi_area_editors);
    QRect mdi_rec = mdi_area_editors->geometry();
    QMdiSubWindow *sub_win = mdi_area_editors->addSubWindow(pwidget);
    int x = (mdi_rec.width()-sub_win->width()) / 2;
    int y = (mdi_rec.height()-sub_win->height()) / 2;
    sub_win->move(x, y);
    pwidget->setSubWindow(sub_win);
    sub_win->show();
    pwidget->initUI();
}
