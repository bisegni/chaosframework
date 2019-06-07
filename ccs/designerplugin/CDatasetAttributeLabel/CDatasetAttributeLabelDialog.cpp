#include "../../widget/designer/CDatasetAttributeLabel/CDatasetAttributeLabel.h"
#include "CDatasetAttributeLabelDialog.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVariant>

//! [0]
CDatasetAttributeLabelDialog::CDatasetAttributeLabelDialog(CDatasetAttributeLabel *_label, QWidget *parent)
    : QDialog(parent)
    , editor(new CDatasetAttributeLabel)
    , label(_label)
    , buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Reset))
{
//    editor->setState(ticTacToe->state());

    connect(buttonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked,
            this, &CDatasetAttributeLabelDialog::resetState);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CDatasetAttributeLabelDialog::saveState);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(editor);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    setWindowTitle(tr("Edit State"));
}
//! [0]

//! [1]
QSize CDatasetAttributeLabelDialog::sizeHint() const
{
    return QSize(250, 250);
}
//! [1]

//! [2]
void CDatasetAttributeLabelDialog::resetState()
{
    //editor->clearBoard();
}
//! [2]

//! [3]
void CDatasetAttributeLabelDialog::saveState()
{
//! [3] //! [4]
    if (QDesignerFormWindowInterface *formWindow
            = QDesignerFormWindowInterface::findFormWindow(label)) {
        formWindow->cursor()->setProperty("state", "editor->state()");
    }
//! [4] //! [5]
    accept();
}
//! [5]
