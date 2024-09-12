#ifndef SPRITE_H
#define SPRITE_H
#include <QPixmap>
class GameScene;
class Sprite
{
public:
    Sprite();
    void setPosition(float x, float y);
    QPointF position() const;
    void setPixmap(QPixmap pixmap);
    void draw(GameScene* gs);
    float height() const;
    float width() const;
    QPixmap pixmap() const;
    void move(float offsetX, float offsetY);
private:
    QPointF m_position;
    QPixmap m_pixmap;
};

#endif // SPRITE_H
