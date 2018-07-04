#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QVector>
#include <QLabel>
#include <QTimer>

class FolderPane;
class QGridLayout;
class QHBoxLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QStack<FolderPane*> panePool;
    QVector<FolderPane*> panes;

    int focusedPane;
    QHBoxLayout *hbox;

    QLabel *currentDirLabel;

    QTimer timer;

    FolderPane* addFolderPane(const QString &path);
    void highlightChanged(FolderPane *folderPane, const QModelIndex &current, const QModelIndex &previous);
    void focusNextPane(FolderPane *sender);
    void focusPreviousPane(FolderPane *sender);
    void removePanesAfter(int index);

    void focusPane(int index);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
