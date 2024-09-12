#include "sprite.h"
#include "gamescene.h"

Sprite::Sprite() {}

void Sprite::setPosition(float x, float y)
{
    m_position.setX(x);
    m_position.setY(y);
}

QPointF Sprite::position() const
{
    return m_position;
}

void Sprite::setPixmap(QPixmap pixmap)
{
    m_pixmap = pixmap;
}

void Sprite::draw(GameScene *gs)
{
    QGraphicsPixmapItem *pItem = new QGraphicsPixmapItem();
    pItem->setPixmap(m_pixmap);
    pItem->setPos(m_position);
    gs->addItem(pItem);
}

float Sprite::height() const
{
    return m_pixmap.height();
}

float Sprite::width() const
{
    return m_pixmap.width();
}

QPixmap Sprite::pixmap() const
{
    return m_pixmap;
}

void Sprite::move(float offsetX, float offsetY)
{
    setPosition(position().x() + offsetX, position().y() + offsetY);
}
