#ifndef COMMANDPRESENTER_H
#define COMMANDPRESENTER_H

#include "PresenterWidget.h"

#include <QObject>
#include <QMdiArea>

//! the helper class to present a result of a command pannel
/*!
 * \brief Usually every command panel need to rpesent the result f his operation
 * these operation are shown into the default tabbed pane
 */

class CommandPresenter : public QObject
{
    Q_OBJECT
    QMdiArea *mdi_area_editors;
public:
    explicit CommandPresenter(QObject *parent = 0,  QMdiArea *_mdi_area_editors = NULL);
    ~CommandPresenter();

    //! show the command presenter
    void showCommandPresenter(PresenterWidget *pwidget);
signals:

public slots:
};

#endif // COMMANDPRESENTER_H
