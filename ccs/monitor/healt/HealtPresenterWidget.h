#ifndef HEALTPRESENTERWIDGET_H
#define HEALTPRESENTERWIDGET_H
#include "../handler/healt/healt.h"
#include "../../presenter/CommandPresenter.h"
#include "../../api_async_processor/ApiAsyncProcessor.h"
#include <QFrame>

namespace Ui {
class HealtPresenterWidget;
}

//! Widget that show the healt information about a node
class HealtPresenterWidget :
        public QFrame
{
    Q_OBJECT

protected:
    //! called when there is a new value for the status


    //! called when there is a new value for the heartbeat

public:
    explicit HealtPresenterWidget(CommandPresenter *_global_command_presenter,
                                  const QString& node_to_check,
                                  QWidget *parent = 0);
    ~HealtPresenterWidget();
public slots:

private slots:

    //!Api has ben called successfully
    void asyncApiResult(const QString& tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    //!Api has been give an error
    void asyncApiError(const QString& tag,
                       QSharedPointer<chaos::CException> api_exception);
    void asyncApiTimeout(const QString& tag);
    //!open node editor
    void on_pushButtonOpenNodeEditor_clicked();

private:
    QString type;
    const QString node_uid;
    ApiAsyncProcessor api_processor;
    CommandPresenter *global_command_presenter;
    Ui::HealtPresenterWidget *ui;
};

#endif // HEALTPRESENTERWIDGET_H
