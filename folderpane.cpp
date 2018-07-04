#include "folderpane.h"

#include <QtDebug>
#include <QListView>
#include <QEvent>
#include <QKeyEvent>
#include <QScrollBar>

FolderPane::FolderPane(QWidget *parent, const QString &path) : QListView(parent), m_path(path)
{
    fs_model = new QFileSystemModel(this);
    setModel(fs_model);
    setFrameStyle(QFrame::NoFrame);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setSelectionMode(QAbstractItemView::SingleSelection);

    defaultBg = viewport()->palette().color(backgroundRole());
    focusBg = defaultBg.lighter(120);

    horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");
}

const QString &FolderPane::path()
{
    return this->m_path;
}

void FolderPane::reload() {
    m_rootIndex = fs_model->setRootPath(m_path);
    setRootIndex(m_rootIndex);
}

void FolderPane::changePath(const QString &path)
{
    m_path = path;
    reload();
}

QFileInfo FolderPane::fileInfo(const QModelIndex &index)
{
    return fs_model->fileInfo(index);
}

void FolderPane::keyPressEvent(QKeyEvent *keyEvent)
{
    switch (keyEvent->key()) {
    case Qt::Key_H:
        emit focusPreviousPane(this);
        break;
    case Qt::Key_L:
        emit focusNextPane(this);
        break;
    case Qt::Key_K:
        selectUp();
        break;
    case Qt::Key_J:
        selectDown();
        break;
    default:
        break;
    }
}

void FolderPane::focusInEvent(QFocusEvent *event)
{
    QPalette p = viewport()->palette();
    p.setColor(viewport()->backgroundRole(), focusBg);
    viewport()->setPalette(p);
}

void FolderPane::focusOutEvent(QFocusEvent *event)
{
    QPalette p = viewport()->palette();
    p.setColor(viewport()->backgroundRole(), defaultBg);
    viewport()->setPalette(p);
}

void FolderPane::selectUp()
{
    QModelIndex index = selectionModel()->currentIndex();
    if (index.row() > 0) {
        auto selectionIndex = index.sibling(index.row() - 1, 0);
        setCurrentIndex(selectionIndex);
        selectionModel()->select(selectionIndex, QItemSelectionModel::SelectCurrent);
    }
}

void FolderPane::selectDown()
{
    QModelIndex index = selectionModel()->currentIndex();
    if (index.row() < (model()->rowCount(index.parent()) - 1)) {
        auto selectionIndex = index.sibling(index.row() + 1, 0);
        setCurrentIndex(selectionIndex);
        selectionModel()->select(selectionIndex, QItemSelectionModel::SelectCurrent);
    }
}

void FolderPane::selectFirstEntry()
{
    selectionModel()->setCurrentIndex(fs_model->index(0, 0, m_rootIndex), QItemSelectionModel::SelectCurrent);
}
