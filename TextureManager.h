//
// Created by Guhan on 4/17/2023.
//

#ifndef PROJECT3_TEXTUREMANAGER_H
#define PROJECT3_TEXTUREMANAGER_H
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>
using namespace std;
using std::unordered_map;
using std::string;

class TextureManager {
private:
    static unordered_map<string, sf::Texture> textures;
public:

    static void LoadTexture(string textureName);
    static sf::Texture& GetTexture(string textureName);
    static void Clear();


};


#endif //PROJECT3_TEXTUREMANAGER_H
