#ifndef LISTDELEGATE_H
#define LISTDELEGATE_H

#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif

#include <QPainter>
#include <QItemDelegate>
#include <QSvgRenderer>

#define ROLE_ID           Qt::UserRole + 0
#define ROLE_DESCRIPTION  Qt::UserRole + 1
#define ROLE_DESCRIPTION2 Qt::UserRole + 2
#define ROLE_STATE        Qt::UserRole + 3
#define ROLE_DISPLAYMODE  Qt::UserRole + 4
#define ROLE_ICON         Qt::UserRole + 5
#define ROLE_ICON2        Qt::UserRole + 6
#define ROLE_STYLE        Qt::UserRole + 7


#define ROLE_DM_ITEM  0
#define ROLE_DM_GROUP 1
#define ROLE_DM_MENU  2

class ListDelegate : public QItemDelegate
{
public:
    ListDelegate(QObject *parent = 0);

    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize    sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    bool     editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );
    QWidget *createEditor( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    void	 updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    void     setEditorData ( QWidget * editor, const QModelIndex & index ) const;
    void     setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
    virtual ~ListDelegate();
private:
    void drawSVG(QPainter *painter, const QString &file, int x, int y, int w, int h) const;
    void drawSVG(QPainter *painter, QSvgRenderer &renderer, int x, int y, int w, int h) const;


};
#endif // LISTDELEGATE_H
