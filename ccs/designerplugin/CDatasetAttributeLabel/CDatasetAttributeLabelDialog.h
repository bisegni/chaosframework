#ifndef CDATASETATTRIBUTELABELDIALOG_H
#define CDATASETATTRIBUTELABELDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
QT_END_NAMESPACE
class CDatasetAttributeLabel;

//! [0]
class CDatasetAttributeLabelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CDatasetAttributeLabelDialog(CDatasetAttributeLabel *_label = nullptr, QWidget *parent = nullptr);

    QSize sizeHint() const override;

private slots:
    void resetState();
    void saveState();

private:
    CDatasetAttributeLabel *editor;
    CDatasetAttributeLabel *label;
    QDialogButtonBox *buttonBox;
};

#endif

