#ifndef FOLDERPANE_H
#define FOLDERPANE_H

#include <QModelIndex>
#include <QListView>
#include <QFileSystemModel>

class FolderPane : public QListView
{
    Q_OBJECT

    QString m_path;
    QFileSystemModel *fs_model;

    QModelIndex m_rootIndex;

    QColor defaultBg;
    QColor focusBg;

    QColor defaultHighlight;
    QColor dimmedHighlight;

public:
    explicit FolderPane(QWidget *parent = nullptr, const QString &path = "/");

    const QString & path();

    void reload();
    void changePath(const QString &path);

    QFileInfo fileInfo(const QModelIndex &index);

    void selectUp();
    void selectDown();

    void selectFirstEntry();

    void keyPressEvent(QKeyEvent *event);

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

signals:
    void focusPreviousPane(FolderPane *pane);
    void focusNextPane(FolderPane *pane);

public slots:

};

#endif // FOLDERPANE_H
