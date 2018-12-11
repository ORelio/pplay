//
// Created by cpasjuste on 08/12/18.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "status_box.h"

using namespace c2d;

StatusBox::StatusBox(Main *m, const c2d::Vector2f &position) : Rectangle({m->getSize().x - 64, 48}) {

    main = m;

    setPosition(position);

    icon = new C2DTexture(main->getIo()->getDataReadPath() + "skin/wait.png");
    icon->setOrigin(Origin::Center);
    icon->setPosition(icon->getSize().x / 2 + 8, getSize().y / 2);
    icon->setFillColor(COLOR_FONT);
    icon->setAlpha(200);
    icon->add(new TweenRotation(0, 360, 2, TweenLoop::Loop));
    add(icon);

    titleText = new Text("Please Wait...", FONT_SIZE, main->getFont());
    titleText->setFillColor(COLOR_RED);
    titleText->setPosition(icon->getSize().x + 16, 4);
    add(titleText);

    messageText = new Text("Doing something in background, please wait", 16, main->getFont());
    messageText->setFillColor(COLOR_FONT);
    messageText->setPosition(icon->getSize().x + 16, titleText->getPosition().y + FONT_SIZE + 4);
    messageText->setWidth(getSize().x - icon->getSize().x - 16);
    add(messageText);

    clock = new C2DClock();
    mutex = SDL_CreateMutex();

    tween = new TweenAlpha(0, 255, 0.5f);
    add(tween);
    setVisibility(Visibility::Hidden);
}

StatusBox::~StatusBox() {
    delete (clock);
    SDL_DestroyMutex(mutex);
}

void StatusBox::show(const std::string &title, const std::string &message, bool infinite, bool drawNow) {

    SDL_LockMutex(mutex);
    titleText->setString(title);
    messageText->setString(message);
    SDL_UnlockMutex(mutex);

    this->infinite = infinite;
    clock->restart();
    setVisibility(Visibility::Visible, true);
    if (drawNow) {
        for (int i = 0; i < 30; i++) {
            main->flip();
        }
    }
}

void StatusBox::hide() {
    clock->restart();
    infinite = false;
}

void StatusBox::onDraw(c2d::Transform &transform) {

    if (isVisible() && !infinite && clock->getElapsedTime().asSeconds() > 2) {
        setVisibility(Visibility::Hidden, true);
    }

    FloatRect bounds = main->getMenuMain()->getGlobalBounds();
    setPosition(bounds.left + bounds.width + 10, getPosition().y);

    SDL_LockMutex(mutex);
    C2DObject::onDraw(transform);
    SDL_UnlockMutex(mutex);
}

void StatusBox::setAlpha(uint8_t alpha, bool recursive) {
    C2DObject::setAlpha(alpha, recursive);
}
