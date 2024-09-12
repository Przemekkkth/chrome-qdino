#include "gamescene.h"
#include "utils.h"
#include "spritesheet.h"

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QImage>
#include <QRandomGenerator>
#include <QApplication>

#include <sstream>
#include <string>

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
{
    onUserCreate();
}

GameScene::~GameScene()
{

}

void GameScene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    while(m_loopTime > m_loopSpeed)
    {
        m_loopTime -= m_loopSpeed;
        const float deltaTime = m_loopSpeed / 1000.0f; //Qt use int(ms) so divide by 1000 to use second 1.0f
        handlePlayerInput();
        update(deltaTime);
        draw();
        resetStatus();
    }
}

void GameScene::onUserCreate()
{
    setBackgroundBrush(COLOR_STYLE::BACKGROUND);
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i] = new KeyStatus();
    }
    m_mouse = new MouseStatus();
    setSceneRect(0,0, SCREEN::PHYSICAL_SIZE.width(), SCREEN::PHYSICAL_SIZE.height());
    connect(&m_timer, &QTimer::timeout, this, &GameScene::loop);
    m_timer.start(int(1000.0f/FPS));
    m_elapsedTimer.start();
    m_image.loadFromData(data.data(), static_cast<int>(data.size()), "PNG");
    if (m_image.isNull()) {
        qWarning("Not load a image.");
        QApplication::instance()->quit();
    }

    m_spritesheet = QPixmap::fromImage(m_image);
    m_ground.setPixmap(m_spritesheet.copy(2, 104, 2440, 26));
    m_ground.setPosition(0, 520);
    m_groundBack = m_ground;

    m_dino.setPixmap(m_spritesheet.copy(1678, 2, 88, 94));
    m_dino.setPosition(150, m_ground.position().y() - (m_dino.height() - gap));

    m_frameGround = 0.0f;
    m_frameDino = 0.0f;

    m_gravity = m_ground.position().y() - (m_dino.height() - gap);
    m_velocity = 0.0f;
    m_jump = -20.f;

    m_frameDown = 0.0f;
    m_crouch = false;
    m_isCrouching = false;

    m_objs = {
        QRect(260, 14, 92, 68),
        QRect(446, 2, 68, 70),
        QRect(752, 2, 50, 96),
        QRect(652, 2, 98, 96),
        QRect(848, 2, 104, 98),
    };

    m_count = 0;
    m_frameBird = 0.0f;

    m_hi.setPixmap(m_spritesheet.copy(1494, 2, 38, 21));
    m_hi.setPosition(955, 25);
    m_hiStartX = 1020.0f;
    m_numHi = 0;
    m_spacing = 20.0f;
    m_total = 5;

    m_digits = {
        {'0', QRect(1294, 2, 18, 21)},
        {'1', QRect(1316, 2, 18, 21)},
        {'2', QRect(1334, 2, 18, 21)},
        {'3', QRect(1354, 2, 18, 21)},
        {'4', QRect(1374, 2, 18, 21)},
        {'5', QRect(1394, 2, 18, 21)},
        {'6', QRect(1414, 2, 18, 21)},
        {'7', QRect(1434, 2, 18, 21)},
        {'8', QRect(1454, 2, 18, 21)},
        {'9', QRect(1474, 2, 18, 21)}
    };

    m_start_x = 1140;
    m_start_y = m_hi.position().y();

    m_dinoRect = QRect(0, 0, 40, 70);

    m_rectObjs.clear();
    m_gameover = false;

    m_spriteGameover.setPixmap(m_spritesheet.copy(1295, 29, 380, 21));
    m_spriteGameover.setPosition(SCREEN::PHYSICAL_SIZE.width() / 2.0 - m_spriteGameover.pixmap().width() / 2.0,
                                  SCREEN::PHYSICAL_SIZE.height() / 2.0 - m_spriteGameover.pixmap().height() / 2.0);

    m_iconRestart.setPixmap(m_spritesheet.copy(506, 130, 72, 64));
    m_iconRestart.setPosition(SCREEN::PHYSICAL_SIZE.width() / 2.0 - m_iconRestart.pixmap().width() / 2.0,
                                  SCREEN::PHYSICAL_SIZE.height() / 2.0 - m_iconRestart.pixmap().height() / 2.0 + 70.0f);

    m_color = 255;
    m_day = true;
    m_changeDay = false;

    m_sun.setPixmap(m_spritesheet.copy(1074, 2, 80, 80));
    m_sun.setPosition(SCREEN::PHYSICAL_SIZE.width() / 2.0f - m_sun.pixmap().width() / 2.0,
                      SCREEN::PHYSICAL_SIZE.height() / 2.0f - m_sun.height() / 2.0 - 150.0f);

    Sprite cloud;
    cloud.setPixmap(m_spritesheet.copy(166, 2, 92, 27));

    for (size_t i {}; i < 6; ++i){
        m_clouds.push_back(cloud);
    }
    m_clouds[0].setPosition(100.f, 100.f);
    m_clouds[1].setPosition(300.f, 300.f);
    m_clouds[2].setPosition(1280.f, 200.f);
    m_clouds[3].setPosition(650.f, 60.f);
    m_clouds[4].setPosition(830.f, 150.f);
    m_clouds[5].setPosition(1000.f, 400.f);

    m_star1.setPixmap(m_spritesheet.copy(1274, 39, 18, 17));
    m_star2 = m_star1;
    m_star1.setPosition(200.f, 200.f);
    m_star2.setPosition(800.f, 300.f);

    m_speed = 200.0f;
}

void GameScene::renderScene()
{
    static int index = 0;
    QString fileName = QDir::currentPath() + QDir::separator() + "screen" + QString::number(index++) + ".png";
    QRect rect = sceneRect().toAlignedRect();
    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    render(&painter);
    image.save(fileName);
    qDebug() << "saved " << fileName;
}

void GameScene::handlePlayerInput()
{
    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Z]]->m_released)
    {
        //renderScene();//uncoment if want to make screenshots
    }
    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Space]]->m_pressed || m_keys[KEYBOARD::KeysMapper[Qt::Key_Up]]->m_pressed)
    {
        bool isDinoCollidedWithGround = (m_gravity == m_ground.position().y() - (m_dino.height() - gap));
        if(isDinoCollidedWithGround){
            m_velocity = m_jump;
        }
    }

    if((m_keys[KEYBOARD::KeysMapper[Qt::Key_S]]->m_pressed || m_keys[KEYBOARD::KeysMapper[Qt::Key_S]]->m_held)
        || (m_keys[KEYBOARD::KeysMapper[Qt::Key_Down]]->m_pressed || m_keys[KEYBOARD::KeysMapper[Qt::Key_Down]]->m_held)) {
        bool isDinoCollidedWithGround = (m_gravity == m_ground.position().y() - (m_dino.height() - gap));
        if(isDinoCollidedWithGround) {
            m_crouch = true;
            m_isCrouching = true;
        }
    }

    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_S]]->m_released || m_keys[KEYBOARD::KeysMapper[Qt::Key_Down]]->m_released) {
        m_crouch = false;
        m_isCrouching = false;
    }

    if (m_keys[KEYBOARD::KeysMapper[Qt::Key_R]]->m_released) {
        if (m_gameover) {
            m_count = 0;
            m_speed = 200.f;
            m_rectObjs.clear();
            m_sprites.clear();
            m_gravity = m_ground.position().y() - (m_dino.height() - gap);
            m_velocity = 0.f;
            m_day = true;
            m_color = 255;
            m_changeDay = false;
            m_gameover = false;
        }
    }
}

void GameScene::resetStatus()
{
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_released = false;
    }
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_pressed = false;
    }
    m_mouse->m_released = false;
}

void GameScene::runGame(const float dt)
{
    const float fixedSpeed = m_speed * dt;
    if (!m_gameover){
        int rand_distance = QRandomGenerator::global()->generate() % 2 + 1;
        //if(count % (50 * rand_distance - (int)speed) == 0){ HARD MODE
        if (m_count % ((50 - int(fixedSpeed)) * rand_distance) == 0){
            int rand_obj = QRandomGenerator::global()->generate() % m_objs.size();
            Sprite sprite;
            sprite.setPixmap(m_spritesheet.copy(m_objs.at(rand_obj)));
            if (sprite.height() == 68) {
                sprite.setPosition(SCREEN::PHYSICAL_SIZE.width(), 410);
            }
            else {
                sprite.setPosition(SCREEN::PHYSICAL_SIZE.width(), 520 - sprite.height() + gap);
            }

            QRect rect_obj;
            rect_obj.setX(sprite.position().x());
            rect_obj.setY(sprite.position().y());
            rect_obj.setSize(QSize(sprite.width(), sprite.height()));

            m_sprites.push_back(sprite);
            m_rectObjs.push_back(rect_obj);
        }

        if (!m_crouch){
            m_velocity += 1.f;
            if (m_velocity < m_jump){
                m_velocity = m_jump;
            }
        }

        m_gravity += m_velocity;
        if (m_gravity > m_ground.position().y() - (m_dino.height() - gap)){
            m_gravity = m_ground.position().y() - (m_dino.height() - gap);
            m_velocity = 0.0f;
        }

        m_frameDino += 0.2f;
        if (m_frameDino > 4.f) {
            m_frameDino -= 4.0f;
        }

        m_frameDown += 0.1f;
        if (m_frameDown > 1.5f) {
            m_frameDown -= 1.5f;
        }

        m_frameGround -= 8.0f;
        if (m_frameGround < -m_ground.width()) {
            m_frameGround = 0.0f;
        }

        m_ground.setPosition(m_frameGround - fixedSpeed, m_ground.position().y());
        m_groundBack.setPosition(m_frameGround - fixedSpeed + (m_ground.width() - 40.0f), m_ground.position().y());

        if (m_crouch) {
            m_dino.setPixmap(m_spritesheet.copy(2206 + 118 * static_cast<int>(m_frameDown), 36, 118, 60));
            m_dino.setPosition(m_dino.position().x(), m_gravity + 30);
            m_dinoRect.setSize(QSize(100, 40));
            m_dinoRect.setTopLeft(QPoint(m_dino.position().x() + 20, m_gravity + 40.f));
        }
        else {
            m_dino.setPixmap(m_spritesheet.copy(1678 + 88 * static_cast<int>(m_frameDino), 2, 88, 94));
            m_dino.setPosition(m_dino.position().x(), m_gravity);
            m_dinoRect.setSize(QSize(40, 70));
            m_dinoRect.setTopLeft(QPoint(m_dino.position().x() + 20, m_gravity + 10.f));
        }

        for (int i = 0; i < m_sprites.size(); ++i) {
            if (m_sprites.at(i).height() == 68) {
                m_frameBird += 0.08f;
                if (m_frameBird > 1.5) {
                    m_frameBird -= 1.5;
                }
                m_sprites[i].setPixmap(m_spritesheet.copy(260 + 92 * static_cast<int>(m_frameBird), 14, 92, 68));
            }
            m_sprites[i].move(-10.f - fixedSpeed, 0.0f);
            m_rectObjs[i].setTopLeft(QPoint(m_sprites.at(i).position().x(), m_rectObjs.at(i).y()));
            m_rectObjs[i].setSize(QSizeF(m_sprites.at(i).width(), m_sprites.at(i).height()));

            if (m_dinoRect.intersects(m_rectObjs.at(i))) {
                m_gameover = true;
            }

            if (m_sprites.at(i).position().x() < -m_sprites.at(i).width()) {
                m_sprites.erase(m_sprites.cbegin() + i);
                m_rectObjs.erase(m_rectObjs.cbegin() + i);
            }
        }

        for (int i = 0; i < m_clouds.size(); ++i) {
            m_clouds[i].move(-0.2f, 0.0f);
            if (m_clouds.at(i).position().x() < -m_clouds.at(i).width()) {
                m_clouds[i].setPosition(1280, m_clouds.at(i).position().y());
            }
        }

        if (!m_day) {
            m_sun.setPixmap(m_spritesheet.copy(1034, 2, 40, 80));
        }
        else {
            m_sun.setPixmap(m_spritesheet.copy(1074, 2, 40, 80));
        }

        ++m_count;
        if (m_count >= 99999){
            m_count = 99999;
        }
    } // GAME OVER

    if (m_count % 250 == 0){
        m_speed += 40.f;
    }

    if (m_count % 1000 == 0 && m_count > 1){
        m_changeDay = true;
    }

    if (m_changeDay) {
        if (m_day) {
            m_color -= 3;
            if (m_color <= 30) {
                m_color = 30;
                m_day = false;
                m_changeDay = false;
            }
        }
        else{
            m_color += 3;
            if (m_color >= 255) {
                m_color = 255;
                m_day = true;
                m_changeDay = false;
            }
        }
    }
}

void GameScene::update(const float dt)
{
    runGame(dt);
    if (m_gameover) {
        if (m_count > m_numHi) {
            m_numHi = m_count;
        }
    }
}

void GameScene::draw()
{
    clear();
    setBackgroundBrush(QBrush(QColor(m_color, m_color, m_color)));
    m_sun.draw(this);
    for (int i = 0; i < m_clouds.size(); ++i) {
        m_clouds[i].draw(this);
    }
    m_star1.draw(this);
    m_star2.draw(this);
    m_ground.draw(this);
    m_groundBack.draw(this);
    for(int i = 0; i < m_sprites.size(); ++i) {
#ifndef QT_NO_DEBUG
        QGraphicsRectItem *spriteRect = new QGraphicsRectItem();
        spriteRect->setRect(m_rectObjs[i]);
        spriteRect->setPen(QPen(Qt::red));
        spriteRect->setBrush(QBrush(Qt::red));
        addItem(spriteRect);
#endif
        m_sprites[i].draw(this);
    }

#ifndef QT_NO_DEBUG
    QGraphicsRectItem *dinoRect = new QGraphicsRectItem();
    dinoRect->setRect(m_dinoRect);
    dinoRect->setPen(QPen(Qt::green));
    dinoRect->setBrush(QBrush(Qt::green));
    addItem(dinoRect);
#endif

    m_dino.draw(this);

    std::stringstream ss;
    ss << std::setw(m_total) << std::setfill('0') << m_count;
    std::string count_str = ss.str();
    float x = m_start_x;
    for (char digit : count_str) {
        Sprite sprite;
        sprite.setPixmap(m_spritesheet.copy(m_digits[digit]));
        sprite.setPosition(x, m_start_y);
        sprite.draw(this);
        x += m_spacing;
    }

    m_hi.draw(this);

    std::stringstream hi_ss;
    hi_ss << std::setw(m_total) << std::setfill('0') << m_numHi;
    std::string hi_str = hi_ss.str();
    float pos_x_hi = m_hiStartX;
    for (char digit : hi_str) {
        Sprite sprite;
        sprite.setPixmap(m_spritesheet.copy(m_digits[digit]));
        sprite.setPosition(pos_x_hi, m_start_y);
        sprite.draw(this);
        pos_x_hi += m_spacing;
    }

    if (m_gameover) {
        m_spriteGameover.draw(this);
        m_iconRestart.draw(this);
        m_dino.setPixmap(m_spritesheet.copy(2122, 6, 80, 86));
    }
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
        }
        else
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held    = false;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(!event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_released = true;
        }

    }
    QGraphicsScene::keyReleaseEvent(event);
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = true;
    QGraphicsScene::mousePressEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    QGraphicsScene::mouseMoveEvent(event);
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = false;
    m_mouse->m_released = true;
    QGraphicsScene::mouseReleaseEvent(event);
}
