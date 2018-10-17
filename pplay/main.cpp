//
// Created by cpasjuste on 02/10/18.
//

#include "main.h"

using namespace c2d;

Main::Main() {

    // create main renderer
    renderer = new C2DRenderer(Vector2f(1280, 720));
    renderer->setClearColor(COLOR_GRAY_LIGHT);

    // create a font
    font = new Font();
    font->load();

    // create io
    io = new C2DIo();

    // create input
    input = new C2DInput();
    input->setJoystickMapping(0, C2D_DEFAULT_JOY_KEYS, 0);
    input->setKeyboardMapping(C2D_DEFAULT_KB_KEYS);

    // create a timer
    timer = new C2DClock();

    // create a rect
    mainRect = new C2DRectangle({renderer->getSize().x - 8, renderer->getSize().y - 8});
    mainRect->setFillColor(Color::Transparent);

    filer = new HttpFiler(io, "http://divers.klikissi.fr/telechargements/", *font, FONT_SIZE,
                          {16, 16,
                           (mainRect->getSize().x / 2) - 16, mainRect->getSize().y - 32});
    mainRect->add(filer);

    // add all this crap
    renderer->add(mainRect);

    input->setRepeatEnable(true);
    input->setRepeatDelay(INPUT_DELAY);

    // ffmpeg player
    player = new Player(this);
    player->setVisibility(Visibility::Hidden);
    renderer->add(player);
}

void Main::run() {

    while (true) {

        unsigned int keys = input->update()[0].state;

        player->step(player->isFullscreen() ? keys : 0);

        if (keys > 0) {

            if (((keys & c2d::Input::Key::KEY_START)
                 || (keys & c2d::Input::Key::KEY_COIN)
                 || (keys & EV_QUIT))) {
                if (player->isPlaying()) {
                    player->stop();
                } else {
                    break;
                }
            }

            if (keys & c2d::Input::KEY_FIRE3) {
                if (player->isPlaying()) {
                    if (player->isFullscreen()) {
                        renderer->setClearColor(COLOR_GRAY_LIGHT);
                        mainRect->setVisibility(Visibility::Visible);
                        player->setScale(0.4f, 0.4f);
                        player->setPosition(renderer->getSize().x * 0.55f, renderer->getSize().y * 0.55f);
                        player->setFullscreen(false);
                    } else {
                        renderer->setClearColor(Color::Black);
                        mainRect->setVisibility(Visibility::Hidden);
                        player->setScale(1.0f, 1.0f);
                        player->setPosition(0, 0);
                        player->setFullscreen(true);
                    }
                }
            }

            if (!player->isFullscreen()) {
                if (filer->step(keys)) {
                    Io::File file = filer->getSelection();
                    printf("file: %s, type: %i\n", file.name.c_str(), file.type);
                    // TODO: "if is http filer then..."
                    file.path = filer->getPath() + file.path;
                    if (player->load(file)) {
                        renderer->setClearColor(Color::Black);
                        mainRect->setVisibility(Visibility::Hidden);
                        player->setScale(1.0f, 1.0f);
                        player->setPosition(0, 0);
                        player->setFullscreen(true);
                    }
                }
            }
        }

        renderer->flip();
    }
}

c2d::C2DRenderer *Main::getRenderer() {
    return renderer;
}

c2d::C2DIo *Main::getIo() {
    return io;
}

c2d::C2DFont *Main::getFont() {
    return font;
}

c2d::C2DInput *Main::getInput() {
    return input;
}

Main::~Main() {

    delete (timer);
    delete (input);
    delete (io);
    delete (font);
    // will delete widgets recursively
    delete (renderer);
}

int main() {

#ifdef __SWITCH__
    socketInitializeDefault();
#endif

    Main *main = new Main();
    main->run();
    delete (main);

#ifdef __SWITCH__
    socketExit();
#endif

    return 0;
}
