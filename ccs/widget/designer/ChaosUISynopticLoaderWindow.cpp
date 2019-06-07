#include "ChaosUISynopticLoaderWindow.h"
#include "ui_ChaosUISynopticLoaderWindow.h"

#include <QFile>
#include <QtUiTools>
#include <QFileDialog>
#include <QDebug>

ChaosUISynopticLoaderWindow::ChaosUISynopticLoaderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChaosUISynopticLoaderWindow) {
    ui->setupUi(this);
}

ChaosUISynopticLoaderWindow::~ChaosUISynopticLoaderWindow()
{
    delete ui;
}

void ChaosUISynopticLoaderWindow::on_actionLoad_UI_File_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open ui file"),
                                                    "$HOME",
                                                    tr("Ui Files (*.ui)"));
    if(!fileName.size()) {
        return;
    }
    qDebug() << "load file:" << fileName;
    //we can load ui file

    QWidget *rootUIWidget = loadUiFile(this, fileName);
    setCentralWidget(rootUIWidget);
    qDebug() << "num of widget" << 0;
}

QWidget *ChaosUISynopticLoaderWindow::loadUiFile(QWidget *parent, QString filePath) {
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);

    QUiLoader loader;
    return loader.load(&file, parent);
}
