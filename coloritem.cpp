#include "coloritem.h"
#include <QtGui>

ColorItem::ColorItem(string mask, QColor clr,int id)
    :Tile(mask), color(clr), Id(id),active(false)

{
    setToolTip(QString("QColor(%1, %2, %3)\n%4")
               .arg(color.red()).arg(color.green()).arg(color.blue())
               .arg("Click and drag this color onto the robot!"));
    setCursor(Qt::OpenHandCursor);
}

QRectF ColorItem::boundingRect() const
{
    //return QRectF(-15.5, -15.5, 34, 34);
    //cout << "getbrect" << getHeight()<< " " << getWidth() <<"\n";
    return QRectF(0,0,getWidth()*20,getHeight()*20);
}

void ColorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(color));
    int h,s,v;
    color.getHsv(&h,&s,&v);
    QColor clr = color;
    if (!active)
    {
        clr.setHsv(h,s/2,v);
    }
    for(int i=0;i<getHeight();++i)
    {
        for(int j=0;j<getWidth();++j)
        {
            if (Tile::data[i][j]!='0') painter->fillRect(j*20+1,i*20+1,18,18,clr);
        }
    }
}

void ColorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!active) return;
    switch(event->button()) {
    case(Qt::LeftButton):
        setCursor(Qt::ClosedHandCursor);
        break;
    case(Qt::RightButton):
        prepareGeometryChange();
        rotateTile();
        update();
        break;
    default:
        event->ignore();
        break;
    }
}
void ColorItem::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
    if (!active) return;
    Q_UNUSED(event);
    //    if (event->button()!=Qt::RightButton) return;
    reflectTile();
    update();
}

void ColorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!active) return;
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);

    mime->setColorData(color);
    mime->setText(QString::number(Id)+QString(':')+QString(getAsText().c_str()));

    QPixmap pixmap(getWidth()*20, getHeight()*20);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    //painter.translate(15, 15);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, 0, 0);
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(5, 5));

    drag->exec();
    setCursor(Qt::OpenHandCursor);
}

void ColorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::OpenHandCursor);
}

void ColorItem::activate()
{
    active=true;
}
void ColorItem::deactivate()
{
    active=false;
}

#ifdef DEBUG_DESTRUCTORS
ColorItem::~ColorItem()
{
    cerr << "colorItem " << Id << color.name().toStdString() << " destroyed" << endl;
}
#endif
