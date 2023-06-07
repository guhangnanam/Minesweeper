//
// Created by Guhan on 4/18/2023.
//

#include "Buttons.h"


Buttons::Buttons(sf::Texture _texture, int x, int y) {
    this->texture = _texture;
    this->x_cord = x;
    this->y_cord = y;
}

sf::Sprite Buttons::open() {
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition(x_cord, y_cord);
    return sprite;
}

void Buttons::Set_Texture(sf::Texture &_texture) {
    texture = _texture;
}