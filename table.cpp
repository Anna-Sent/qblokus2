#include "table.h"
#include "tile.h"
#include <iostream>
#include <QMimeData>
#include <QPainter>
#include <QGraphicsSceneDragDropEvent>

using namespace std;

typedef pair<int, QString> intstr_t;

intstr_t dataparse(QString what)
{
    QStringList parts = what.split(QChar(':'));
    return make_pair(parts[0].toInt(), parts[1]);
}

TableCell::TableCell(QGraphicsItem *parent, int x, int y)
    : QGraphicsItem(parent), xpos(x), ypos(y), color(Qt::lightGray), dragOver(false)
{
    setAcceptDrops(true);
}

void TableCell::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    bool result = false;
    if (event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        intstr_t data = dataparse(text);
        Tile tile(data.second.toStdString());
        Table *table = qgraphicsitem_cast<Table *>(parentItem());
        if (table)
        {
            QColor color = qVariantValue<QColor>(event->mimeData()->hasColor() ? event->mimeData()->colorData() : QColor(0, 0, 0));
            result = table->Accept(xpos, ypos, tile, data.first, false, true, color);
        }
    }

    if (result)
    {
        event->setAccepted(true);
    }
    else
    {
        event->setAccepted(false);
    }
}

void TableCell::dragLeaveEvent(QGraphicsSceneDragDropEvent *)
{
}

void TableCell::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    bool result = false;
    if (event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        intstr_t data = dataparse(text);
        Tile tile(data.second.toStdString());
        Table *table = qgraphicsitem_cast<Table *>(parentItem());
        if (table)
        {
            QColor color = qVariantValue<QColor>(event->mimeData()->hasColor() ? event->mimeData()->colorData() : QColor(0, 0, 0));
            result = table->Accept(xpos, ypos, tile, data.first, true, true, color);
        }
    }

    if (result)
    {
        event->setAccepted(true);
    }
    else
    {
        event->setAccepted(false);
    }
}

void TableCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->fillRect(1, 1, 18, 18, dragOver ? color.light(130) : color);
}

QRectF TableCell::boundingRect() const
{
    return QRectF(0, 0, 20, 20);
}

Table::Table(int w, int h) : width(w), height(h)
{
    cells.resize(h);
    for(int i = 0; i < h; ++i)
    {
        cells[i].resize(w);
        for(int j = 0; j < w; ++j)
        {
            TableCell *ne = new TableCell(this, j, i);
            ne->setPos(j * 20, i * 20);
            cells[i][j] = ne;
        }
    }
}

QRectF Table::boundingRect() const
{
    return QRectF(0, 0, width * 20, height * 20);
}

void Table::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
    painter->fillRect(0, 0, width * 20, height * 20, Qt::darkGray);
}

string readColor(QColor color)
{
    QString res = QString("QColor(%1, %2, %3)\n")
                  .arg(color.red()).arg(color.green()).arg(color.blue());
    return res.toStdString();
}

bool Table::Accept(int x, int y, const Tile &what, int id, bool really, bool local, QColor color)
{
    if (x + what.getWidth() > width)
    {
        return false;
    }

    if (y + what.getHeight() > height)
    {
        return false;
    }

    clearDrags();
    bool inAngle = false;
    bool touchAngles = false;
    bool valid = true;
    for(int i = x; i < x + what.getWidth(); ++i)
    {
        for(int j = y; j < y + what.getHeight(); ++j)
        {
            if (what.data[j - y][i - x] != '0')
            {
                if (really)
                {
                    cells[j][i]->color = color;
                }
                else
                {
                    cells[j][i]->dragOver = true;
                    if (cells[j][i]->color != Qt::lightGray)
                    {
                        return false;
                    }

                    if (j > 0)
                    {
                        if (cells[j - 1][i]->color == color)
                        {
                            valid = false;
                        }
                    }

                    if (j < height - 1)
                    {
                        if (cells[j + 1][i]->color == color)
                        {
                            valid = false;
                        }
                    }

                    if (i > 0)
                    {
                        if (cells[j][i - 1]->color == color)
                        {
                            valid = false;
                        }
                    }

                    if (i < width - 1)
                    {
                        if (cells[j][i + 1]->color == color)
                        {
                            valid = false;
                        }
                    }

                    if (j > 0 && i > 0)
                    {
                        if (cells[j - 1][i - 1]->color == color)
                        {
                            touchAngles = true;
                        }
                    }

                    if (j < height - 1 && i > 0)
                    {
                        if (cells[j + 1][i - 1]->color == color)
                        {
                            touchAngles = true;
                        }
                    }

                    if (j > 0 && i < width - 1)
                    {
                        if (cells[j - 1][i + 1]->color == color)
                        {
                            touchAngles = true;
                        }
                    }

                    if (j < height - 1 && i < width - 1)
                    {
                        if (cells[j + 1][i + 1]->color == color)
                        {
                            touchAngles = true;
                        }
                    }

                    if ((i == 0 && j == 0) || (i == width - 1 && j == height - 1)
                        || (j == 0 && i == width - 1) || (i == 0 && j == height - 1))
                    {
                        inAngle = true;
                    }

                    if (!valid)
                    {
                        return false;
                    }
                }

                cells[j][i]->update();
            }
        }
    }

    if (really && local)
    {
        emit turnComplete(color, what.getAsQString(), id, x, y);
    }

    if (inAngle || touchAngles)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Table::clearDrags()
{
    for(int i = 0; i < width; ++i)
    {
        for(int j = 0; j < height; ++j)
        {
            cells[i][j]->dragOver = false;
            cells[i][j]->update();
        }
    }
}
