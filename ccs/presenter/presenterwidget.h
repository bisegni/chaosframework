#ifndef PRESENTERWIDGET_H
#define PRESENTERWIDGET_H

#include <QWidget>
#include <QMdiSubWindow>
class PresenterWidget:
        public QWidget
{
    Q_OBJECT
    QMdiSubWindow *editor_subwindow;

public:
    explicit PresenterWidget(QWidget *parent = NULL);
    ~PresenterWidget();

    void setThisTabIndex(int index);
    void setSubWindow(QMdiSubWindow *_editor_subwindow);

    virtual bool canClose() = 0;
    virtual void updateUI() = 0;

protected:
    void setTabTitle(const QString& title);
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
signals:
};

#endif // PRESENTERWIDGET_H
