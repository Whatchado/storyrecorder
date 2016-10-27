#include "listdelegate.h"
#include <QLineEdit>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include "videopreview.h"


//QSvgRenderer icon_interview_bg(QString(":/icons/resources/button_bg.svg"));


ListDelegate::ListDelegate(QObject *parent)
//    :icon_interview_bg(QString(":/icons/resources/button_bg.svg"))

{

}

void ListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QPen fontPen(QColor::fromRgb(51,51,51), 1, Qt::SolidLine);
    QRect r = option.rect;
    int dm = index.data(ROLE_DISPLAYMODE).toInt();
    int style = index.data(ROLE_STYLE).toInt();
    //:/icons/resources/rightarrow-active.svg
    if(style==2)
    {
        QPen fontPen2(QColor::fromRgb(0,0,0), 1, Qt::SolidLine);

        QColor color_bg(228,228,228);
        QColor color_bg_sel(255,255,255);
        QColor color_bgleft(212,212,212);
        QColor color_bgbox(236,236,236);
        QColor color_text(160,160,160);
        QColor color_textsel(254,130,32);
        QColor color_text2(color_text);
        QColor color_text2sel(0,0,0);

        QPen pen_sep(QColor::fromRgb(255,255,255), 1, Qt::SolidLine);

    //background
        painter->setPen(Qt::NoPen);
        if(option.state & QStyle::State_Selected)
        {
            painter->setBrush(color_bg_sel );
            painter->drawRect(r);
            fontPen.setColor(color_textsel);
            fontPen2.setColor(color_text2sel);
        }
        else
        {
            painter->setBrush( color_bg );
            painter->drawRect(r);
            painter->setBrush( color_bgleft );
            QRectF leftbar(r.left()+3,r.top()+3,30,r.bottom()-r.top()-6);
            painter->drawRect(leftbar);
            fontPen.setColor(color_text);
            fontPen2.setColor(color_text2);
        }


        //Color: #C4C4C4
        QPen linePen(QColor::fromRgb(211,211,211), 1, Qt::SolidLine);

        //Color: #005A83
        QPen lineMarkedPen(QColor::fromRgb(200,200,200), 1, Qt::SolidLine);


        //Color: #fff
        //QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);
        //BORDER
        painter->setPen(pen_sep);
        painter->drawLine(r.bottomLeft(),r.bottomRight());

        //painter->setPen(fontMarkedPen);

        QIcon ic(index.data(ROLE_ICON).toString());
        QIcon ic2(index.data(ROLE_ICON2).toString());
        QString title = index.data(Qt::DisplayRole).toString();
        QString description = index.data(ROLE_DESCRIPTION).toString();
        QString description2 = index.data(ROLE_DESCRIPTION2).toString();
        int imageSpace = 10;
        if (!ic.isNull()) {
            ic.paint(painter, r.left()-6,r.top()+40,48,48, Qt::AlignTop|Qt::AlignLeft);
            imageSpace = 44;
        }
        QString iSel("");
        if(option.state & QStyle::State_Selected)
            drawSVG(painter,":/icons/resources/rightarrow-active.svg",r.right()-22,r.top()+2,20,20);
        else
            drawSVG(painter,":/icons/resources/rightarrow-unactive.svg",r.right()-22,r.top()+2,20,20);


        //QIcon iType(":/icons/resources/int_type"+iSel+".svg");
        //QIcon iCount(":/icons/resources/questions_count"+iSel+".svg");
        //iType.paint(painter,r.left()+6+imageSpace, r.top()+27,16,16);
        //iCount.paint(painter,r.left()+6+imageSpace+80, r.top()+27,16,16);
        //TITLE
        //if(option.state & QStyle::State_Selected)

        painter->setPen(fontPen);
        QFont qf1( "Lucida Grande",15, QFont::Bold );
        qf1.setPixelSize(16);
        painter->setFont(qf1);
        QPoint p(r.left()+6+imageSpace, r.top()+18);
        //painter->drawText(p, title);
        QRectF tbounding(r.left()+40,r.top()+8,r.right()-r.left()-65,40);

        painter->drawText(tbounding,Qt::AlignLeft|Qt::TextWordWrap, title);

        //DESCRIPTION
        p.setY(r.top()+68);
        p.setX(r.left()+40);
        painter->setPen(fontPen2);
        painter->setFont( QFont( "Lucida Grande", 9, QFont::Bold ) );
        painter->drawText(p, description);
        p.setX(r.left()+9);
        p.setY(r.top()+32);
        painter->setPen(fontPen);
        QFont qf2( "Lucida Grande", 22, QFont::Bold );
        qf2.setPixelSize(25);
        painter->setFont(qf2 );
        painter->drawText(p, description2);

        if (!ic2.isNull()) {
            ic2.paint(painter, r.right()-(r.bottom()-r.top()),r.top(),r.bottom()-r.top(),r.bottom()-r.top(), Qt::AlignVCenter|Qt::AlignLeft);
        }
        return;
    };
    if(style==1)
    {
        QPen pen_sep(QColor::fromRgb(104,104,104), 1, Qt::SolidLine);
        fontPen.setColor(QColor::fromRgb(228,228,228));

        painter->setBrush( QColor(87,87,87) );
        painter->setPen(Qt::NoPen);
        painter->drawRect(r);

        if(dm==ROLE_DM_GROUP)
        {
            //GET TITLE, DESCRIPTION AND ICON
            QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
            QString title = index.data(Qt::DisplayRole).toString();
            QString description = index.data(ROLE_DESCRIPTION).toString();

            int imageSpace = 10;
            if (!ic.isNull()) {
                ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
                imageSpace = 55;
            }

            //TITLE
            r = option.rect.adjusted(imageSpace, 0, 0, 0);
            painter->setPen(fontPen);
  QFont qf3( "Lucida Grande", 20, QFont::Bold );
            qf3.setPixelSize(25);
            painter->setFont(qf3 );
            QPoint p(r.left(), r.top()+26);
            painter->drawText(p, title);

            return;
        }
        if(dm==ROLE_DM_MENU)
        {
            return; // now with pushbutton
            //GET TITLE, DESCRIPTION AND ICON
            QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
            QString title = index.data(Qt::DisplayRole).toString();
            QString description = index.data(ROLE_DESCRIPTION).toString();
            //QString iconname=
            int imageSpace = 10;
            if (!ic.isNull()) {
                r = option.rect.adjusted(5, 10, -10, -10);
                ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
                imageSpace = 55;
            }

            drawSVG(painter,":/icons/resources/button_bg.svg",r.left()+20, r.top()+2,r.right()-2-20,r.bottom()-r.top()-4);
            QString iconname(index.data(ROLE_ICON).toString());
            drawSVG(painter,iconname,r.left()+26,r.top()+11,14,14);

            //TITLE
            r = option.rect.adjusted(imageSpace, 0, 0, 0);
            painter->setPen(fontPen);

            painter->setFont( QFont( "Lucida Grande", 10, QFont::Bold ) );
            QPoint p(r.left()+36, r.top()+23);
            painter->drawText(p, title);

            return;
        }


        if(option.state & QStyle::State_Selected)
        {
            painter->setBrush( QColor(228,228,228) );
            painter->setPen(Qt::NoPen);
            painter->drawRect(r);
            fontPen.setColor(QColor::fromRgb(51,51,51));
        };

        //Color: #C4C4C4
        QPen linePen(QColor::fromRgb(211,211,211), 1, Qt::SolidLine);

        //Color: #005A83
        QPen lineMarkedPen(QColor::fromRgb(200,200,200), 1, Qt::SolidLine);


        //Color: #fff
        //QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);
        //BORDER
        painter->setPen(pen_sep);
        painter->drawLine(r.bottomLeft(),r.bottomRight());

        //painter->setPen(fontMarkedPen);

        QString title = index.data(Qt::DisplayRole).toString();
        QString description = index.data(ROLE_DESCRIPTION).toString();
        QString description2 = index.data(ROLE_DESCRIPTION2).toString();
        int imageSpace = 10;

        // paint icon 1

        drawSVG(painter,index.data(ROLE_ICON).toString(), r.left(),r.top(),r.bottom()-r.top(),r.bottom()-r.top());

        imageSpace = 44;
        QString iSel("");
        if(option.state & QStyle::State_Selected)
            iSel="_mo";
        drawSVG(painter,":/icons/resources/int_type"+iSel+".svg",r.left()+6+imageSpace, r.top()+27,16,16);
        drawSVG(painter,":/icons/resources/questions_count"+iSel+".svg",r.left()+6+imageSpace+80, r.top()+27,16,16);
        //TITLE
        painter->setPen(fontPen);
        painter->setFont( QFont( "Lucida Grande", 9, QFont::Bold ) );
        QPoint p(r.left()+6+imageSpace, r.top()+19);
        painter->drawText(p, title);

        //DESCRIPTION
        p.setY(p.y()+18);
        p.setX(p.x()+19);
        painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
        painter->drawText(p, description);
        p.setX(p.x()+80);
        painter->setFont( QFont( "Lucida Grande", 8, QFont::Normal ) );
        painter->drawText(p, description2);

        QString ic2name(index.data(ROLE_ICON2).toString());
        if (ic2name.size())
        {
            QIcon ic2(ic2name);
            ic2.paint(painter, r.right()-(r.bottom()-r.top()),r.top(),r.bottom()-r.top(),r.bottom()-r.top(), Qt::AlignVCenter|Qt::AlignLeft);
        }
        return;
    };

    if(dm==ROLE_DM_GROUP)
    {
        painter->setBrush( (index.row() % 2) ? Qt::white : QColor(252,252,252) );
        painter->setPen(Qt::NoPen);
        painter->drawRect(r);
        //GET TITLE, DESCRIPTION AND ICON
        QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
        QString title = index.data(Qt::DisplayRole).toString();
        QString description = index.data(ROLE_DESCRIPTION).toString();

        int imageSpace = 10;
        if (!ic.isNull()) {
            //ICON
            //r = option.rect.adjusted(5, 10, -10, -10);
            ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
            imageSpace = 55;
        }

        //TITLE
        r = option.rect.adjusted(imageSpace, 0, 0, 0);
        painter->setPen(fontPen);

        painter->setFont( QFont( "Lucida Grande", 11, QFont::Bold ) );
        QPoint p(r.left(), r.top()+16);
        painter->drawText(p, title);

        return;
    }
    if(dm==ROLE_DM_MENU)
    {
        painter->setBrush( QColor(40,40,40) );
        painter->setPen(Qt::NoPen);
        painter->drawRect(r);
        //GET TITLE, DESCRIPTION AND ICON
        QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
        QString title = index.data(Qt::DisplayRole).toString();
        QString description = index.data(ROLE_DESCRIPTION).toString();

        int imageSpace = 10;
        if (!ic.isNull()) {
            //ICON
            r = option.rect.adjusted(5, 10, -10, -10);
            ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
            imageSpace = 55;
        }

        //TITLE
        r = option.rect.adjusted(imageSpace, 0, 0, 0);
        QPen pmarked(QColor(254,130,32));
        painter->setPen(pmarked);

        QFont qf1( "Lucida Grande", 19, QFont::Bold );
        qf1.setPixelSize(11);
        painter->setFont( qf1 );
        QPoint p(r.left()+26, r.top()+54);
        painter->drawText(p, title);
        drawSVG(painter,index.data(ROLE_ICON).toString(), r.left(),r.top()+40,22,22);
        QPen pwhite(QColor(255,255,255));
        painter->setPen(pwhite);
        r = option.rect;
        painter->drawLine(r.left()+3,r.top()+3,r.left()+10,r.top()+3);
        painter->drawLine(r.left()+3,r.top()+3,r.left()+3,r.top()+10);

        painter->drawLine(r.right()-3,r.top()+3,r.right()-10,r.top()+3);
        painter->drawLine(r.right()-3,r.top()+3,r.right()-3,r.top()+10);

        painter->drawLine(r.left()+3,r.bottom()-3,r.left()+10,r.bottom()-3);
        painter->drawLine(r.left()+3,r.bottom()-3,r.left()+3,r.bottom()-10);

        painter->drawLine(r.right()-3,r.bottom()-3,r.right()-10,r.bottom()-3);
        painter->drawLine(r.right()-3,r.bottom()-3,r.right()-3,r.bottom()-10);

        return;
    }
    QString title = index.data(Qt::DisplayRole).toString();

    QImage *img=getVideoFrameCached(title.toLocal8Bit(),0.0);
    //Color: #C4C4C4
    QPen linePen(QColor::fromRgb(211,211,211), 1, Qt::SolidLine);

    //Color: #005A83
    QPen lineMarkedPen(QColor::fromRgb(200,200,200), 1, Qt::SolidLine);


    //Color: #fff
    QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);
    painter->setPen(Qt::NoPen);
    if(option.state & QStyle::State_Selected){
       /* QLinearGradient gradientSelected(r.left(),r.top(),r.left(),r.height()+r.top());
        gradientSelected.setColorAt(0.0, QColor::fromRgb(119,213,247));
        gradientSelected.setColorAt(0.9, QColor::fromRgb(27,134,183));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(0,120,174));
        painter->setBrush(gradientSelected);
        painter->drawRect(r);

        //BORDER
        painter->setPen(lineMarkedPen);
        //painter->drawLine(r.topLeft(),r.topRight());
        //painter->drawLine(r.topRight(),r.bottomRight());
        painter->drawLine(r.bottomLeft(),r.bottomRight());
        //painter->drawLine(r.topLeft(),r.bottomLeft());

        painter->setPen(fontMarkedPen);
        //qDebug()<<"sel:"<<r;*/
    } else {
        /*//BACKGROUND
        //ALTERNATING COLORS
        QLinearGradient gradientSelected(r.left(),r.top(),r.left(),r.height()+r.top());
        gradientSelected.setColorAt(0.0, QColor::fromRgb(255,255,255));
        gradientSelected.setColorAt(0.8, QColor::fromRgb(240,240,240));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(255,255,255));
        painter->setBrush(gradientSelected);
        painter->drawRect(r);

        //BORDER
        painter->setPen(linePen);
        //painter->drawLine(r.topLeft(),r.topRight());
        //painter->drawLine(r.topRight(),r.bottomRight());
        painter->drawLine(r.bottomLeft(),r.bottomRight());
        //painter->drawLine(r.topLeft(),r.bottomLeft());

        painter->setPen(fontPen);
        //qDebug()<<"nos:"<<r;*/

    }
    qDebug()<<"img:"<<img;
    if (img)
    {
        qDebug()<<"img2:"<<(*img);
            painter->drawImage(r,*img);
    }
    QBrush b(QColor(0,0,0,80));
    painter->setBrush(b);
    painter->setPen(Qt::NoPen);
    painter->drawRect(r.left(),r.bottom()-44,r.right()-r.left(),44);
    //GET TITLE, DESCRIPTION AND ICON
    //QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    QIcon ic(index.data(ROLE_ICON).toString());
    QIcon ic2(index.data(ROLE_ICON2).toString());
    QString description = index.data(ROLE_DESCRIPTION).toString();
    r=option.rect;
    if (!ic.isNull()) {
        if(!img) ic.paint(painter, r.left()+48,r.top(),110,110, Qt::AlignVCenter|Qt::AlignLeft);
    }

    //TITLE
    //painter->setFont( QFont( "Lucida Grande", 10, QFont::Normal ) );
    //QPoint p(r.left()+6, r.top()+16);
    //painter->drawText(p, title);

    //DESCRIPTION
    QPen pwhite(QColor(255,255,255));
    painter->setPen(pwhite);
    QPoint p(r.left()+6, r.bottom()-16);
    painter->setFont( QFont( "Lucida Grande", 10, QFont::Bold ) );
    painter->drawText(p, description);

    if (!ic2.isNull()) {
       ic2.paint(painter, r.right()-30,r.bottom()-30,22,22, Qt::AlignVCenter|Qt::AlignLeft);
    }
    //if (img) delete img;

}

QSize ListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    int style = index.data(ROLE_STYLE).toInt();
    if(style==1)
    {
        switch(index.data(ROLE_DISPLAYMODE).toInt())
        {
        case ROLE_DM_GROUP:
            return QSize(-1, 32);
        case ROLE_DM_MENU:
            return QSize(-1, 52);
        default:
            return QSize(-1, 50);
        }
    }
    if(style==2)
    {
        switch(index.data(ROLE_DISPLAYMODE).toInt())
        {
        case ROLE_DM_GROUP:
            return QSize(-1, 22);
        case ROLE_DM_MENU:
            return QSize(-1, 36);
        default:
            return QSize(-1, 80);
        }
    }
    switch(index.data(ROLE_DISPLAYMODE).toInt())
    {
    case ROLE_DM_GROUP:
        return QSize(195,110);
    case ROLE_DM_MENU:
        return QSize(195,110);
    default:
        return QSize(195,110);
    }
}

QWidget *ListDelegate::createEditor( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QLineEdit *editor = new QLineEdit(parent);

    return editor;}

bool ListDelegate::editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index )
{
    //qDebug()<<(*event);

    return false;
}
void	ListDelegate::updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    editor->setGeometry(option.rect);
}
void ListDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
    QString title = index.data(Qt::DisplayRole).toString();
    ((QLineEdit *)editor)->setText(title);
}
void ListDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
    QLineEdit *e=(QLineEdit *)editor;
    model->setData(index,e->text(),Qt::DisplayRole);
}

ListDelegate::~ListDelegate()
{

}
void ListDelegate::drawSVG(QPainter *painter,const QString &file,int x,int y,int w,int h) const
{
    if(file.size())
    {
        //QFile f(file);
        //QByteArray data=f.readAll();
        //QSvgRenderer renderer(data);
        QSvgRenderer renderer(file);
        QRectF bounds(x,y,w,h);
        renderer.render(painter,bounds);
    }
}
void ListDelegate::drawSVG(QPainter *painter, QSvgRenderer &renderer,int x,int y,int w,int h) const
{
    QRectF bounds(x,y,w,h);
    renderer.render(painter,bounds);
}
