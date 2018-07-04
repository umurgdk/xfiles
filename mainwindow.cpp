#include "mainwindow.h"

#include <QtDebug>
#include <QGridLayout>
#include <QFrame>
#include <QListView>
#include <QAbstractItemView>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSignalMapper>
#include "folderpane.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget();
    this->setCentralWidget(central);

    QVBoxLayout *vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    central->setLayout(vbox);

    QWidget *panesWidget = new QWidget();

    hbox = new QHBoxLayout(panesWidget);
    hbox->setContentsMargins(0, 0, 0, 0);
    panesWidget->setLayout(hbox);

    currentDirLabel = new QLabel(central);
    QPalette labelPalette = currentDirLabel->palette();
    labelPalette.setColor(currentDirLabel->backgroundRole(), labelPalette.color(currentDirLabel->backgroundRole()).darker(130));
    currentDirLabel->setAutoFillBackground(true);
    currentDirLabel->setPalette(labelPalette);
    currentDirLabel->setMargin(5);

    vbox->addWidget(currentDirLabel);
    vbox->addWidget(panesWidget);

    addFolderPane(QDir::homePath());
    focusPane(0);
}

MainWindow::~MainWindow()
{

}

FolderPane * MainWindow::addFolderPane(const QString &path)
{
    FolderPane *pane;
    if (panePool.size() > 0) {
        pane = panePool.takeFirst();
        pane->selectionModel()->clear();
        pane->changePath(path);
        pane->selectFirstEntry();
    } else {
        pane = new FolderPane(this, path);
        pane->setFocusPolicy(Qt::NoFocus);
        pane->reload();

        connect(pane->selectionModel(), &QItemSelectionModel::currentChanged,
                 this, [this,pane](auto current, auto previous){ highlightChanged(pane, current, previous); });
        connect(pane, &FolderPane::focusNextPane,
            this, [this,pane](){ focusNextPane(pane); });

        connect(pane, &FolderPane::focusPreviousPane,
            this, [this,pane](){ focusPreviousPane(pane); });

    }

    hbox->addWidget(pane);
    pane->show();

    panes.push_back(pane);

    return pane;
}

void MainWindow::highlightChanged(FolderPane *folderPane, const QModelIndex &current, const QModelIndex &previous)
{
    if (panes.indexOf(folderPane) != focusedPane)
        return;

    QFileInfo file = folderPane->fileInfo(current);
    currentDirLabel->setText(file.absoluteFilePath());

    if (!file.isDir())
    {
        removePanesAfter(panes.indexOf(folderPane));
        return;
    }

    if (panes.last() == folderPane) {
        addFolderPane(file.absoluteFilePath());
        return;
    }

    int index = panes.indexOf(folderPane);
    removePanesAfter(index+1);
    panes[index+1]->changePath(file.absoluteFilePath());
    panes[index+1]->clearSelection();
    panes[index+1]->selectFirstEntry();
}

void MainWindow::focusNextPane(FolderPane *sender)
{
    if (panes[focusedPane] != sender)
        return;

    if (focusedPane < panes.size() - 1) {
        focusPane(focusedPane + 1);

        auto selectedIndex = panes[focusedPane]->selectionModel()->currentIndex();
        auto selectionFileInfo = panes[focusedPane]->fileInfo(selectedIndex);
        if (panes[focusedPane] == panes.last() && selectionFileInfo.isDir()){
            addFolderPane(selectionFileInfo.absoluteFilePath());
        } else if (selectionFileInfo.isDir()) {
            panes[focusedPane+1]->changePath(selectionFileInfo.absoluteFilePath());
        }
    }
}

void MainWindow::focusPreviousPane(FolderPane *sender)
{
    if (panes[focusedPane] != sender)
        return;

    if (focusedPane > 0)
        focusPane(focusedPane - 1);
}

// TODO: removePanesAfter causing a problem. the pane we changed it's folder on highlightChanged slot
// is probably deleted by this method.
void MainWindow::removePanesAfter(int index)
{
    int npanes = panes.size();
    for (int i = index + 1; i < npanes; i++) {
        auto pane = panes.takeLast();
        pane->hide();
        hbox->removeWidget(pane);
        panePool.push_back(pane);
    }
}

void MainWindow::focusPane(int index)
{
    focusedPane = index;
    panes[focusedPane]->setFocus();
    auto file = panes[focusedPane]->fileInfo(panes[focusedPane]->selectionModel()->currentIndex());
    currentDirLabel->setText(file.absoluteFilePath());

    auto selectedIndex = panes[focusedPane]->selectionModel()->currentIndex();
    if (!selectedIndex.isValid()) {
        panes[focusedPane]->selectFirstEntry();
        selectedIndex = panes[focusedPane]->selectionModel()->currentIndex();
    }
}
