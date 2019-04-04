#ifndef ATTRIBUTEVALUERANGEDIALOG_H
#define ATTRIBUTEVALUERANGEDIALOG_H

#include <QDialog>

namespace Ui {
class AttributeValueRangeDialog;
}

class AttributeValueRangeDialog : public QDialog
{
    Q_OBJECT
    int current_row;
    const QString attribute_name;
    const QString attribute_default_value;
    const QString attribute_default_max_value;
    const QString attribute_default_min_value;
public:
    AttributeValueRangeDialog(QWidget *parent = 0);
    AttributeValueRangeDialog(int _current_row,
                              const QString& _attribute_name,
                              const QString& _attribute_default_value,
                              const QString& _attribute_default_max_value,
                              const QString& _attribute_default_min_value,
                              QWidget *parent = 0);
    ~AttributeValueRangeDialog();
private slots:
    void endWork();

signals:
    void newAttributeDescription(const QString& attribute_name,
                                 const QString& attribute_default_value,
                                 const QString& attribute_default_max_value,
                                 const QString& attribute_default_min_value);
    void updateAttributeDescription(int current_row,
                                    const QString& attribute_name,
                                    const QString& attribute_default_value,
                                    const QString& attribute_default_max_value,
                                    const QString& attribute_default_min_value);

private:
    Ui::AttributeValueRangeDialog *ui;
};

#endif // ATTRIBUTEVALUERANGEDIALOG_H
