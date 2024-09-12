#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QElapsedTimer>
#include <QTimer>
#include <QImage>
#include <QGraphicsSimpleTextItem>
#include <QImage>
#include <QPixmap>
#include <QVector>
#include <QMap>
#include "sprite.h"

struct KeyStatus
{
    bool m_pressed = false;
    bool m_held = false;
    bool m_released = false;
};

struct MouseStatus
{
    float m_x = 0.0f;
    float m_y = 0.0f;
    bool m_released = false;
    bool m_pressed = false;
};

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);
    ~GameScene();
private slots:
    void loop();

private:
    void onUserCreate();
    void renderScene();
    void handlePlayerInput();
    void resetStatus();
    void update(const float dt);
    void draw();
    KeyStatus* m_keys[256];
    MouseStatus* m_mouse;
    const int FPS = 60;
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    float m_deltaTime = 0.0f, m_loopTime = 0.0f;
    const float m_loopSpeed = int(1000.0f/FPS);
    QImage m_image;
    QPixmap m_spritesheet;

    Sprite m_ground, m_groundBack;
    Sprite m_dino;


    float m_frameGround;
    float m_frameDino;

    float m_gravity;
    float m_velocity;
    float m_jump;

    float m_frameDown;
    bool m_crouch;
    bool m_isCrouching;

    QVector<QRect> m_objs;
    QVector<Sprite> m_sprites;

    int m_count;
    float m_frameBird;

    Sprite m_hi;
    float m_hiStartX;
    int m_numHi;
    float m_spacing = 20.0f;
    int m_total = 5;

    QMap<QChar, QRect> m_digits;

    float m_start_x;
    float m_start_y;

    QRectF m_dinoRect;


    QVector<QRectF> m_rectObjs;
    bool m_gameover;
    Sprite m_spriteGameover;
    Sprite m_iconRestart;

    int m_color;
    bool m_day, m_changeDay;

    Sprite m_sun, m_star1, m_star2;
    QVector<Sprite> m_clouds;

    float m_speed;
    void runGame(const float dt);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};


#endif // GAMESCENE_H
