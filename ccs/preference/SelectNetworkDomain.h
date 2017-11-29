/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef SELECTNETWORKDOMAIN_H
#define SELECTNETWORKDOMAIN_H

#include "preference_types.h"

#include <QDialog>
#include <QSettings>

namespace Ui {
class SelectNetworkDomain;
}

class SelectNetworkDomain : public QDialog
{
    Q_OBJECT

public:
    explicit SelectNetworkDomain(QWidget *parent = 0);
    ~SelectNetworkDomain();
signals:
    void networkDomainSelected(const QString& network_domain_name);
    void selectionAborted();

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonSelection_clicked();

private:
    void loadAllConfiguration();
    QSettings settings;
    Ui::SelectNetworkDomain *ui;
};

#endif // SELECTNETWORKDOMAIN_H
