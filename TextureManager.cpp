//
// Created by Guhan on 4/17/2023.
//

#include "TextureManager.h"

void TextureManager::LoadTexture(string textureName) {
    string path = "files/images/";
    path += textureName + ".png";

    textures[textureName].loadFromFile(path);
}

sf::Texture &TextureManager::GetTexture(string textureName) {

    if (textures.find(textureName) == textures.end()){
        LoadTexture(textureName);
    }

    return textures[textureName];
}

void TextureManager::Clear() {
    textures.clear();
}