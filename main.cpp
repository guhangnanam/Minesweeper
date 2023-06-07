#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <random>
#include "Random.h"
#include "Buttons.h"
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace std;

class Tile{
public:

    sf::Vector2f position;   // used to set the position of the tile
    sf::Sprite reveal_sprite;          // the sprite that is used to represent the tile
    sf::Texture texture;        // string for the texture of the sprite

    sf::Texture mine_texture;
    sf::Sprite mine_sprite;

    sf::Texture flag_texture;
    sf::Sprite flag_sprite;

    sf::Sprite num_sprite;


    bool flagged;
    bool mine;
    bool revealed;
    bool num;

    vector<Tile*> neighbors;  // adjacent tiles

    int neighbor_bombs;
    int value;  // 1 for mine 0 for not a mine




Tile(sf::Vector2f _position, string _texture){
    texture.loadFromFile(_texture);
    reveal_sprite = sf::Sprite(texture);
    reveal_sprite.setPosition(_position.x, _position.y);
    position = _position;

    flagged = false;
    mine = false;
    revealed = false;


}

Tile(){
    texture.loadFromFile("files/images/tile_hidden.png"); // default texture and sprite are set
    reveal_sprite = sf::Sprite(texture);


    flagged = false;
    mine = false;
    revealed = false;
    num = false;
    neighbor_bombs = 0;
    value = 0;


}

void SetMine(){
    mine = true;
    value = 1;

    mine_texture.loadFromFile("files/images/mine.png");
    mine_sprite = sf::Sprite(mine_texture);

}

void SetFlag(){
    flagged = !flagged;
    flag_texture.loadFromFile("files/images/flag.png");
    flag_sprite = sf::Sprite(flag_texture);

}

void SetReveal(sf::Texture& _texture){                  // we need to recursive all the function on itself and reveal a sprite using the
                                                        // same loop type as we did for the adjacent mines
    revealed = true;
    reveal_sprite = sf::Sprite(_texture);

}

void SetSprite(string _texture){
    texture.loadFromFile(_texture);         // set the texture of the sprite
    reveal_sprite = sf::Sprite(texture);
}

void SetPosition(sf::Vector2f _position){
    reveal_sprite.setPosition(_position.x, _position.y);       // set the position of the sprite
}

void SetNumber(sf::Texture& _texture){
    num = true;
    num_sprite = sf::Sprite(_texture);
}

void SetUnrevealed(sf::Texture& _texture){
    revealed = false;
    reveal_sprite = sf::Sprite(_texture);
}
};

class Board{
public:
    int cols;
    int rows;
    int mines;
    vector<vector<Tile>> tiles;
    int numRevealedTiles;
    bool gameWon;
    bool gameOver;


    Board(int _cols, int _rows, int _mines){
        cols = _cols;
        rows = _rows;
        mines = _mines;

        tiles.resize(cols);         // fill vector with tiles which by default are not mines

        for(int i = 0; i < cols; i++){
            tiles[i].resize(rows);
        }

        // place mines in places according to random positions
        int mine_count = 0;

        while (mine_count < mines){
            // generate a random x and y position using Random.h
            int rand_x = Random::Int(0, cols - 1);
            int rand_y = Random::Int(0, rows - 1);

            // add Tile at randomized position if it is not already a mine
            if (!tiles[rand_x][rand_y].mine){
                tiles[rand_x][rand_y].SetMine();
                mine_count++;
            }
        }

        for (int i = 0; i < cols; i++){         // determine all the neighbors for each of the individual tiles
            for (int j = 0; j < rows; j++){
                determine_neighbor_mines(i, j);
            }
        }



        //cout << "END" << endl;
    }

    void determine_neighbors(sf::Texture _reveal,sf::Texture _number,  int x, int y){     // pass in the position of the mine you want to check
        int count = 0;

        // Iterate over neighboring tiles in a 3x3 grid around the current tile
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                // Check if the current neighbor tile has a mine and is within the bounds of the board
                if (i >= 0 && i < cols && j >= 0 && j < rows && !tiles[i][j].mine and !tiles[i][j].flagged and !tiles[i][j].revealed) {
                    if (tiles[i][j].num) {
                        tiles[i][j].SetReveal(_reveal);
                        tiles[i][j].SetNumber(_number);
                    }
                    else{
                        tiles[i][j].SetReveal(_reveal);
                        determine_neighbors(_reveal, _number, i, j);
                    }

                }
            }
        }

    }

    void determine_neighbor_mines(int x, int y){
        int count = 0;

        // Iterate over neighboring tiles in a 3x3 grid around the current tile
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                // Check if the current neighbor tile has a mine and is within the bounds of the board
                if (i >= 0 && i < cols && j >= 0 && j < rows && tiles[i][j].mine) {
                    count++;
                }
            }
        }

        tiles[x][y].neighbor_bombs = count;
    }

    void Print_TestBoard(){
        for (int i = 0; i < cols; i++){         // determine all the neighbors for each of the individual tiles
            for (int j = 0; j < rows; j++){
                cout << tiles[i][j].value;
            }
            cout << endl;
        }
    }

    void Reveal_around(sf::Texture &_texture, unordered_map<int, sf::Texture> &_numbers,  int x, int y){
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                // Check if the current neighbor tile has a mine and is within the bounds of the board
                if (i >= 0 && i < cols && j >= 0 && j < rows && !tiles[i][j].mine and !tiles[i][j].flagged and !tiles[i][j].revealed) {
                    tiles[i][j].SetReveal(_texture);
                    if (tiles[i][j].neighbor_bombs != 0){
                        tiles[i][j].SetNumber(_numbers[tiles[i][j].neighbor_bombs]);
                    }
                    else{
                        Reveal_around(_texture, _numbers, i, j);
                    }
                    }
                }
                }
    }

    void ResetBoard(sf::Texture& _default, int _cols, int _rows, int _mines){
        cols = _cols;
        rows = _rows;
        mines = _mines;

        for (int i = 0; i < cols; i++){
            for(int j = 0; j < rows; j++){
                tiles[i][j].mine = false;
                tiles[i][j].flagged = false;
                tiles[i][j].revealed = false;
                tiles[i][j].num = false;
                tiles[i][j].neighbor_bombs = 0;
                tiles[i][j].reveal_sprite.setTexture(_default);
                tiles[i][j].mine_sprite.setTexture(_default);
                tiles[i][j].num_sprite.setColor(sf::Color::Transparent);
            }
        }

        int mine_count = 0;

        while (mine_count < mines){
            // generate a random x and y position using Random.h
            int rand_x = Random::Int(0, cols - 1);
            int rand_y = Random::Int(0, rows - 1);

            // add Tile at randomized position if it is not already a mine
            if (!tiles[rand_x][rand_y].mine){
                tiles[rand_x][rand_y].SetMine();
                mine_count++;
            }
        }

        for (int i = 0; i < cols; i++){         // determine all the neighbors for each of the individual tiles
            for (int j = 0; j < rows; j++){
                determine_neighbor_mines(i, j);
            }
        }

    }

    bool CheckWin() {
        for (int i = 0; i < tiles.size(); i++) {
            for (int j = 0; j < tiles.at(i).size(); j++) {
                //checking if every tile that can be revealed has been revealed
                if (!tiles[i][j].revealed && !tiles[i][j].mine) {
                    return false;
                }
            }
        }
        return true;
    }
};



void DrawTitle(sf::RenderWindow &window){
    const float height = 600;
    const float width = 800;
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    sf::Text title("WELCOME TO MINESWEEPER", font);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(24);
    sf::FloatRect TitleRect = title.getLocalBounds();
    title.setOrigin(TitleRect.left + TitleRect.width/2.0f, TitleRect.top + TitleRect.height/2.0f);
    title.setPosition(width/2.0f, height/2.0f - 150);
    title.setStyle(sf::Text::Bold);
    title.setStyle(sf::Text::Underlined);
    window.draw(title);
}

void DrawPromptText(sf::RenderWindow &window){
        const float height = 600;
        const float width = 800;
        sf::Font font;
        font.loadFromFile("files/font.ttf");
        sf::Text prompt("Enter your name:", font);
        prompt.setFillColor(sf::Color::White);
        prompt.setCharacterSize(20);
        sf::FloatRect PromptRect = prompt.getLocalBounds();
        prompt.setOrigin(PromptRect.left + PromptRect.width/2.0f, PromptRect.top + PromptRect.height/2.0f);
        prompt.setPosition(width/2.0f, height/2.0f - 75);
        prompt.setStyle(sf::Text::Bold);
        window.draw(prompt);
};

void PrintMine(sf::RenderWindow &window, float x_cord, float y_cord){
    sf::Texture mine_texture;
    mine_texture.loadFromFile("files/images/mine.png");
    sf::Sprite mine(mine_texture);
    mine.setPosition(x_cord, y_cord);
    window.draw(mine);

}

string capitalize_first_letter(string name) {
    if (name.empty()) {
        return name;
    } else {
        name[0] = toupper(name[0]);
        for (std::size_t i = 1; i < name.length(); i++) {
            name[i] = tolower(name[i]);
        }
        return name;
    }
}

string print_leaderboard(map<string, string> &_players){
    string output = "";
    int index = 1;
    auto iter = _players.begin();
    for (auto iter = _players.begin(); iter != _players.end(); iter++){
        output.append(to_string(index) + ". \t" + iter->first + "\t" + iter->second + "\n\n");
        index++;
    }

    return output;
}






int main() {
    bool firsttimearound = true;

    const float height = 600;
    const float width = 800;

    unordered_map<string, sf::Texture> textures;
    textures["mine"].loadFromFile("files/images/mine.png");
    textures["unrevealed"].loadFromFile("files/images/tile_hidden.png");
    textures["revealed"].loadFromFile("files/images/tile_revealed.png");
    textures["flag"].loadFromFile("files/images/flag.png");
    textures["play"].loadFromFile("files/images/play.png");
    textures["pause"].loadFromFile("files/images/pause.png");
    textures["win"].loadFromFile("files/images/face_win.png");
    textures["sad"].loadFromFile("files/images/face_lose.png");
    textures["happy_face"].loadFromFile("files/images/face_happy.png");

    unordered_map<int, sf::Texture> numbers;
    numbers[1].loadFromFile("files/images/number_1.png");
    numbers[2].loadFromFile("files/images/number_2.png");
    numbers[3].loadFromFile("files/images/number_3.png");
    numbers[4].loadFromFile("files/images/number_4.png");
    numbers[5].loadFromFile("files/images/number_5.png");
    numbers[6].loadFromFile("files/images/number_6.png");
    numbers[7].loadFromFile("files/images/number_7.png");
    numbers[8].loadFromFile("files/images/number_8.png");


    unordered_map<int, sf::Texture> clock;
    clock[0].loadFromFile("files/images/digits.png", sf::IntRect(0, 0, 21, 32));
    clock[1].loadFromFile("files/images/digits.png", sf::IntRect(21, 0, 21, 32));
    clock[2].loadFromFile("files/images/digits.png", sf::IntRect(42, 0, 21, 32));
    clock[3].loadFromFile("files/images/digits.png", sf::IntRect(63, 0, 21, 32));
    clock[4].loadFromFile("files/images/digits.png", sf::IntRect(84, 0, 21, 32));
    clock[5].loadFromFile("files/images/digits.png", sf::IntRect(105, 0, 21, 32));
    clock[6].loadFromFile("files/images/digits.png", sf::IntRect(126, 0, 21, 32));
    clock[7].loadFromFile("files/images/digits.png", sf::IntRect(147, 0, 21, 32));
    clock[8].loadFromFile("files/images/digits.png", sf::IntRect(168, 0, 21, 32));
    clock[9].loadFromFile("files/images/digits.png", sf::IntRect(189, 0, 21, 32));
    clock[10].loadFromFile("files/images/digits.png", sf::IntRect(210, 0, 21, 32));







    // Render the welcome screen
    //cout << "Minesweeper Game Starting... Have fun!\n" << endl;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minesweeper", sf::Style::Close | sf::Style::Titlebar);

    // Create User input text
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    sf::Text User_name("", font, 18);
    sf::FloatRect NameRect = User_name.getLocalBounds();
    User_name.setOrigin(NameRect.left + NameRect.width / 2.0f, NameRect.top + NameRect.height / 2.0f);
    User_name.setPosition(width / 2.0f, height / 2.0f - 45);
    User_name.setStyle(sf::Text::Bold);

    // Create the cursor
    sf::Text cursor("|", font, 18);
    sf::FloatRect CursorRect = cursor.getLocalBounds();
    cursor.setOrigin(CursorRect.left + CursorRect.width / 2.0f, CursorRect.top + CursorRect.height / 2.0f);
    cursor.setPosition(width / 2.0f + 5, height / 2.0f - 45);
    cursor.setStyle(sf::Text::Bold);

    // create string that will be added with key inputs
    string input_name;
    string final_name;




    // run the program as long as the window is open
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }


            if (event.type == sf::Event::TextEntered) {      // Take in keyboard input and convert it to text
                if ((event.text.unicode >= 65 && event.text.unicode <= 90) ||
                    (event.text.unicode >= 97 && event.text.unicode <= 122)) {
                    if (input_name.size() < 10) {
                        input_name += static_cast<char>(event.text.unicode);
                        User_name.setString(input_name);
                        sf::FloatRect NameRect = User_name.getLocalBounds();
                        User_name.setOrigin(NameRect.left + NameRect.width / 2.0f,
                                            NameRect.top + NameRect.height / 2.0f);
                        User_name.setPosition(width / 2.0f, height / 2.0f - 45);

                        // recenter and move the cursor
                        cursor.move(6, 0);
                    }
                }
            }


            if (event.type == sf::Event::KeyPressed) {           // If inputted text is deleted
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!User_name.getString().isEmpty()) {
                        input_name.pop_back();
                        User_name.setString(input_name);
                        sf::FloatRect NameRect = User_name.getLocalBounds();
                        User_name.setOrigin(NameRect.left + NameRect.width / 2.0f,
                                            NameRect.top + NameRect.height / 2.0f);
                        User_name.setPosition(width / 2.0f, height / 2.0f - 45);

                        // recenter the cursor

                        cursor.move(-6, 0);
                    }
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    if(!input_name.empty()) {
                        window.close();
                    }

                }
            }


        }

        final_name = capitalize_first_letter(input_name) + "*";
        window.clear(sf::Color::Blue);
        DrawTitle(window);
        DrawPromptText(window);
        window.draw(User_name);
        window.draw(cursor);
        window.display();


    }

    //cout << final_name;
    // get config file
    ifstream file("files/board_config.cfg");

    string config1;
    getline(file, config1);
    int config_cols = stoi(config1);

    string config2;
    getline(file, config2);
    int config_rows = stoi(config2);

    string config3;
    getline(file, config3);
    int config_mines = stoi(config3);

    Board board(config_cols, config_rows, config_mines);
    //board.Print_TestBoard();



    // Render the game window
    int game_width = config_cols * 32;
    int game_height = (config_rows * 32) + 100;

    sf::RenderWindow game(sf::VideoMode(game_width, game_height), "Minesweeper",sf::Style::Close | sf::Style::Titlebar);

    // Render button sprites
    sf::Texture happy_face;
    happy_face.loadFromFile("files/images/face_happy.png");
    Buttons happy(happy_face, (((config_cols) / 2.0) * 32) - 32, 32 * (config_rows + 0.5f));

    sf::Texture debug_texture;
    debug_texture.loadFromFile("files/images/debug.png");
    Buttons debug(debug_texture, (((config_cols) * 32) - 304), 32 * (config_rows + 0.5f));

    sf::Texture pause_button;
    pause_button.loadFromFile("files/images/pause.png");
    Buttons pause(pause_button, (((config_cols) * 32) - 240), 32 * (config_rows + 0.5f));

    sf::Texture leader_texture;
    leader_texture.loadFromFile("files/images/leaderboard.png");
    Buttons leader_button(leader_texture, (((config_cols) * 32) - 176), 32 * (config_rows + 0.5f));

    //Load Timer Sprites
    sf::Texture default_time;
    default_time.loadFromFile("files/images/digits.png", sf::IntRect(0, 0, 21, 32));
    Buttons minute_col1(default_time, (config_cols) * 32 - 97, 32 * ((config_rows) + 0.5f) + 16);
    Buttons minute_col2(default_time, (config_cols) * 32 - 97 + 21, 32 * ((config_rows) + 0.5f) + 16);
    Buttons sec_col1(default_time, (config_cols) * 32 - 54, 32 * ((config_rows) + 0.5f) + 16);
    Buttons sec_col2(default_time, (config_cols) * 32 - 54 + 21, 32 * ((config_rows) + 0.5f) + 16);

    // Load Counter Sprites
    int count_val0;
    int count_val1;
    int count_val2;
    int digi_num;
    if (config_mines >= 0 and config_mines < 10){
        digi_num = 1;
        count_val2 = stoi(config3.substr(0, -1));
        count_val1 = 0;
        count_val0 = 0;
    }
    else if (config_mines >= 10 and config_mines < 100){
        digi_num = 2;
        count_val0 = 0;
        count_val1 = stoi(config3.substr(0, 1));
        count_val2 = stoi(config3.substr(1, -1));
        //cout << count_val1 << endl;

    }
    else if (config_mines >= 100){
        digi_num = 3;
        count_val0 = stoi(config3.substr(0, 1));
        count_val1 = stoi(config3.substr(1, 2));
        count_val2 = stoi(config3.substr(2, -1));


    }

    Buttons count0(clock[count_val0],33, 32 * ((config_rows) + 0.5f) + 16);
    Buttons count1(clock[count_val1],54, 32 * ((config_rows) + 0.5f) + 16);
    Buttons count2(clock[count_val2], 75, 32 * ((config_rows) + 0.5f) + 16);
    Buttons negative(clock[10], 12,32 * ((config_rows) + 0.5f) + 16);






    //
    bool debug_pushed = false;
    bool win = false;
    bool lost = false;
    bool paused = false;
    bool reset = false;
    bool leader = false;
    int flag_count = 0;
    int flags_placed = 0;
    int reveal_count = 0;
    int counter_val;
    int new_val0;
    int new_val1;
    int new_val2;
    bool neg;
    bool render;
    int minutes;
    int seconds;
    bool hasWon = false;

    // Load the leaderboard file
    ifstream leader_file("files/leaderboard.txt");
    string line1;
    string line2;
    string line3;
    string line4;
    string line5;
    getline(leader_file, line1);
    getline(leader_file, line2);
    getline(leader_file, line3);
    getline(leader_file, line4);
    getline(leader_file, line5);

    string name1 = line1.substr(line1.find(",") + 1, -1);
    string time1 = line1.substr(0, line1.find(","));
    //cout << name1;

    string name2 = line2.substr(line2.find(",") + 1, -1);
    string time2 = line2.substr(0, line2.find(","));
    //cout << name2;
    string name3 = line3.substr(line3.find(",") + 1, -1);
    string time3 = line3.substr(0, line3.find(","));
   // cout << name3;
    string name4 = line4.substr(line4.find(",") + 1, -1);
    string time4 = line4.substr(0, line4.find(","));
    //cout << name4;
    string name5 = line5.substr(line5.find(",") + 1, -1);
    string time5 = line5.substr(0, line5.find(","));
    //cout << name5;
    map<string, string> players;
    players.emplace(time1, name1);
    players.emplace(time2, name2);
    players.emplace(time3, name3);
    players.emplace(time4, name4);
    players.emplace(time5, name5);





    // Create the timer
    auto start_time = chrono::high_resolution_clock::now();
    auto pauseTime = chrono::high_resolution_clock::now();
    auto elapsed_paused_time = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - pauseTime).count();

    while (game.isOpen()) {
        auto game_runtime = std::chrono::duration_cast<std::chrono::seconds>(chrono::high_resolution_clock::now() - start_time);
        int total_time = game_runtime.count(); // necessary to subtract elapsed time later because "game_runtime.count()" is const



        // only update time if the game is not paused
        if (!paused) {
            total_time = total_time - elapsed_paused_time; //
            minutes = total_time / 60;
            seconds = total_time % 60;
        }


        // separate time value by index
        int minutes0 = minutes / 10 % 10; //minutes index 0
        int minutes1 = minutes % 10; // minutes index 1
        int seconds0 = seconds / 10 % 10; // seconds index 0
        int seconds1 = seconds % 10; // seconds index 1


        // create a string version of the time
        string minutes_final = to_string(minutes);
        string seconds_final = to_string(seconds);

        if(minutes_final.length() < 2){
            minutes_final = "0" + minutes_final;

        }
        if (seconds_final.length() < 2){
            seconds_final = "0" + seconds_final;
        }

        string final_time = minutes_final + ":" +  seconds_final;

        game.clear(sf::Color::White);


        if(board.CheckWin()){
           win = true;
           hasWon = true;

       }

        for (int i = 0; i < board.cols; i++) {               // draw the board on the screen
            for (int j = 0; j < board.rows; j++) {
                board.tiles[i][j].reveal_sprite.setPosition(32 * i, 32 *j); // draw mine sprite on top of revealed sprite and make reveal mine return a sprite object
                game.draw(board.tiles[i][j].reveal_sprite);
                auto currentTile = board.tiles[i][j];

                if (paused){
                    sf::Sprite temp(textures["revealed"]);
                    temp.setPosition(currentTile.reveal_sprite.getPosition());
                    game.draw(temp);
                }

                if (currentTile.mine == true and currentTile.revealed == true and !paused) {    // can be used for the debug
                    currentTile.mine_sprite.setPosition(32 * i, 32 * j);
                    game.draw(currentTile.mine_sprite);
                } else if (!currentTile.mine and currentTile.revealed and !paused) {
                    currentTile.num_sprite.setPosition(32 * i, 32 * j);
                    game.draw(currentTile.num_sprite);
                }
                if (currentTile.flagged) {
                    currentTile.flag_sprite.setPosition(32 * i, 32 * j);
                    game.draw(currentTile.flag_sprite);
                }
                if (debug_pushed and !win and !lost) {
                    currentTile.mine_sprite.setPosition(32 * i, 32 * j);
                    game.draw(currentTile.mine_sprite);
                }

                if (win) {

                    happy.Set_Texture(textures["win"]); // change face

                    // reset counter
                    //count0.Set_Texture(clock[0]);
                    //count1.Set_Texture(clock[0]);
                    //count2.Set_Texture(clock[0]);

                    //place flags on all the mines
                    if (currentTile.mine and !currentTile.flagged) {
                        currentTile.SetFlag();
                        currentTile.flag_sprite.setPosition(32 * i, 32 * j);
                        game.draw(currentTile.flag_sprite);

                    }


                    if (hasWon and firsttimearound) {
                        hasWon = false;
                        firsttimearound = false;
                        players.emplace(final_time, final_name);

                        // If the player has won we need to write to the leaderboard textfile

                        ofstream leader_file("files/leaderboard.txt", ios_base::out); // output stream file

                        for (auto iter = players.begin(); iter != players.end(); iter++) {
                            if (iter->second.find("*") != string::npos) {
                                players[iter->first] = iter->second.substr(0, iter->second.size());

                            }
                            if (players.size() > 5) {
                                players.erase(--players.end());
                            }

                            leader_file.write((iter->first + "," + iter->second + "\n").c_str(),
                                              (iter->first + "," + iter->second + "\n").length());


                        }

                        sf::RenderWindow leaderboard(sf::VideoMode(16 * config_cols, (16 * config_rows) + 50),
                                                     "Leaderboard", sf::Style::Close | sf::Style::Titlebar);
                        sf::Text Leader_title;
                        Leader_title.setFont(font);
                        Leader_title.setCharacterSize(20);
                        Leader_title.setFillColor(sf::Color::White);
                        Leader_title.setString("LEADERBOARD");
                        Leader_title.setStyle(sf::Text::Bold | sf::Text::Underlined);
                        sf::FloatRect leader_Rect = Leader_title.getLocalBounds();
                        Leader_title.setOrigin(leader_Rect.left + leader_Rect.width / 2.0f,
                                               leader_Rect.top + leader_Rect.height / 2.0f);
                        Leader_title.setPosition(16 * config_cols / 2.0f, 16 * config_rows / 2.0f - 120);


                        sf::Text Player_list;
                        Player_list.setFont(font);
                        Player_list.setCharacterSize(18);
                        Player_list.setStyle(sf::Text::Bold);
                        Player_list.setFillColor(sf::Color::White);
                        Player_list.setString(print_leaderboard(players));
                        sf::FloatRect list_Rect = Player_list.getLocalBounds();
                        Player_list.setOrigin(list_Rect.left + list_Rect.width / 2.0f,
                                              list_Rect.top + list_Rect.height / 2.0f);
                        Player_list.setPosition(16 * config_cols / 2.0f, 16 * config_rows / 2.0f + 20);

                        // while the leaderboard is open
                        while (leaderboard.isOpen()) {

                            sf::Event event2;
                            while (leaderboard.pollEvent(event2)) {
                                if (event2.type == sf::Event::Closed) {
                                    hasWon = false;
                                    leaderboard.close();
                                }
                            }
                            //game.display();
                            leaderboard.clear(sf::Color::Blue);
                            leaderboard.draw(Leader_title);
                            leaderboard.draw(Player_list);
                            leaderboard.display();
                        }



                    }


                }

                else if (lost) {
                    happy.Set_Texture(textures["sad"]);
                    if (currentTile.mine and !currentTile.flagged){
                        currentTile.reveal_sprite.setTexture(textures["revealed"]);
                        currentTile.reveal_sprite.setPosition(32* i, 32* j);
                        currentTile.mine_sprite.setPosition(32 *i , 32* j);
                        game.draw(currentTile.reveal_sprite);
                        game.draw(currentTile.mine_sprite);

                    }

                }

                if (reset) {
                    reset = false;
                    firsttimearound = true;

                    if (win) {
                        win = false;
                        happy.Set_Texture(textures["happy_face"]);
                    } else {
                        lost = false;
                        happy.Set_Texture(textures["happy_face"]);
                    }
                }



            }
        }

        sf::Event event;

        while (game.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                game.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {

                sf::Mouse mouse;
                auto coordinates = mouse.getPosition(game);     // coordinates of click

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {   // if it is a left click
                    //If restart button is clicked
                    auto Happy_Rect = happy.open().getGlobalBounds();
                    if (Happy_Rect.contains(coordinates.x, coordinates.y)) {
                        paused = false;
                        reset = true;
                        happy.Set_Texture(textures["happy_face"]);
                        board.ResetBoard(textures["unrevealed"], config_cols, config_rows, config_mines);

                        flags_placed = 0;
                        flag_count = 0;
                        reveal_count = 0;

                        // Reset timer
                        start_time = chrono::high_resolution_clock::now();

                        // Reset counter
                        neg = false;
                        count0.Set_Texture(clock[count_val0]);
                       count1.Set_Texture(clock[count_val1]);
                        count2.Set_Texture(clock[count_val2]);
                    }
                    // If leaderboard button is clicked
                    auto Leaderboard_Rect = leader_button.open().getGlobalBounds();
                    if (Leaderboard_Rect.contains(coordinates.x, coordinates.y)) {
                        leader = !leader;
                        for (int i = 0; i < board.cols; i++){
                            for (int j = 0; j < board.rows; j++){
                                sf::Sprite temp(textures["revealed"]);
                                temp.setPosition(board.tiles[i][j].reveal_sprite.getPosition());
                                game.draw(temp);
                            }
                        }
                        game.draw(sec_col2.open());
                        game.draw(sec_col1.open());
                        game.draw(minute_col2.open());
                        game.draw(minute_col1.open());

                        if(neg){
                            game.draw(negative.open());
                        }
                        game.draw(happy.open());
                        game.draw(pause.open());
                        game.draw(leader_button.open());
                        game.draw(debug.open());
                        game.draw(count0.open());
                        game.draw(count1.open());
                        game.draw(count2.open());

                        game.display();
                        if (leader) {
                            pauseTime = chrono::high_resolution_clock::now();

                            // render the leaderboard window
                            sf::RenderWindow leaderboard(sf::VideoMode(16 * config_cols, (16 * config_rows) + 50),
                                                         "Leaderboard", sf::Style::Close | sf::Style::Titlebar);
                            sf::Text Leader_title;
                            Leader_title.setFont(font);
                            Leader_title.setCharacterSize(20);
                            Leader_title.setFillColor(sf::Color::White);
                            Leader_title.setString("LEADERBOARD");
                            Leader_title.setStyle(sf::Text::Bold | sf::Text::Underlined);
                            sf::FloatRect leader_Rect = Leader_title.getLocalBounds();
                            Leader_title.setOrigin(leader_Rect.left + leader_Rect.width / 2.0f,
                                                   leader_Rect.top + leader_Rect.height / 2.0f);
                            Leader_title.setPosition(16 * config_cols / 2.0f, 16 * config_rows / 2.0f - 120);


                            sf::Text Player_list;
                            Player_list.setFont(font);
                            Player_list.setCharacterSize(18);
                            Player_list.setStyle(sf::Text::Bold);
                            Player_list.setFillColor(sf::Color::White);
                            Player_list.setString(print_leaderboard(players));
                            sf::FloatRect list_Rect = Player_list.getLocalBounds();
                            Player_list.setOrigin(list_Rect.left + list_Rect.width / 2.0f,
                                                  list_Rect.top + list_Rect.height / 2.0f);
                            Player_list.setPosition(16 * config_cols / 2.0f, 16 * config_rows / 2.0f + 20);

                            // while the leaderboard is open
                            while (leaderboard.isOpen()) {
                                sf::Event event2;
                                while (leaderboard.pollEvent(event2)) {
                                    if (event2.type == sf::Event::Closed) {
                                        leader = false;
                                        leaderboard.close();
                                        auto resumeTime = chrono::high_resolution_clock::now();   // calculate amount of time that leaderboard was open
                                        elapsed_paused_time = elapsed_paused_time += chrono::duration_cast<chrono::seconds>(resumeTime - pauseTime).count();

                                    }
                                }
                                //game.display();
                                leaderboard.clear(sf::Color::Blue);
                                leaderboard.draw(Leader_title);
                                leaderboard.draw(Player_list);
                                leaderboard.display();

                            }

                        }
                    }


                    if (!lost and !win) {
                        // if Pause button is clicked
                        auto Pause_Rect = pause.open().getGlobalBounds();
                        if (Pause_Rect.contains(coordinates.x, coordinates.y)) {
                            paused = !paused;
                            if (paused) {
                                pause.Set_Texture(textures["play"]);
                                // Pause Timer
                                pauseTime = chrono::high_resolution_clock::now();

                            } else {
                                pause.Set_Texture(textures["pause"]);
                                auto resumeTime = chrono::high_resolution_clock::now();   // calculate amount of time that it was paused;
                                elapsed_paused_time = elapsed_paused_time += chrono::duration_cast<chrono::seconds>(resumeTime - pauseTime).count();

                            }

                        }


                        if (!paused) {          // checks if game is paused
                            for (int i = 0; i < board.cols; i++) {
                                for (int j = 0; j < board.rows; j++) {
                                    auto Tile_rect = board.tiles[i][j].reveal_sprite.getGlobalBounds();         // gets global bounds of an individual tile
                                    if (Tile_rect.contains(coordinates.x, coordinates.y)) {
                                        if (board.tiles[i][j].mine and
                                            !board.tiles[i][j].flagged) {            // checks if a tile is a mine and that if it is flagged
                                            board.tiles[i][j].SetReveal(textures["revealed"]);              // reveals the tile
                                            lost = true;

                                        } else if (!board.tiles[i][j].flagged) {                     // checks if a tile is flagged
                                            if (board.tiles[i][j].neighbor_bombs != 0) {
                                                board.tiles[i][j].SetReveal(textures["revealed"]);
                                                board.tiles[i][j].SetNumber(numbers[board.tiles[i][j].neighbor_bombs]);
                                            } else if (board.tiles[i][j].neighbor_bombs == 0) {
                                                board.Reveal_around(textures["revealed"], numbers, i, j);
                                            }

                                        }
                                    }
                                }
                            }

                            // if debug button is clicked
                            auto Debug_Rect = debug.open().getGlobalBounds();
                            if (Debug_Rect.contains(coordinates.x, coordinates.y)) {
                                debug_pushed = !debug_pushed;           // flips the debug boolean value
                            }

                        }
                    }
                } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    if (!win and !lost) {
                        for (int i = 0; i < board.cols; i++) {
                            for (int j = 0; j < board.rows; j++) {
                                auto Flag_rect = board.tiles[i][j].reveal_sprite.getGlobalBounds();
                                if (Flag_rect.contains(coordinates.x, coordinates.y) and !board.tiles[i][j].revealed) {
                                    if (board.tiles[i][j].flagged){
                                        flags_placed--;
                                    }
                                    else{
                                        flags_placed++;
                                    }
                                    board.tiles[i][j].SetFlag();

                                    counter_val = config_mines - flags_placed;
                                    if (counter_val < 0) {
                                        neg = true;
                                        new_val0 = -1 * (counter_val / 100);
                                        new_val1 = -1 * (counter_val / 10);
                                        new_val2 = -1 * (counter_val % 10);
                                    } else {
                                        neg = false;
                                        if (digi_num == 1) {
                                            new_val2 = config_mines - flags_placed;
                                        } else if (digi_num == 2) {
                                            new_val1 = counter_val / 10;
                                            new_val2 = counter_val % 10;

                                        } else if (digi_num == 3) {
                                            new_val0 = counter_val / 100;
                                            new_val1 = (counter_val % 100) / 10;
                                            new_val2 = counter_val % 10;
                                        }

                                    }
                                    // Change the counter texture
                                    count0.Set_Texture(clock[new_val0]);
                                    count1.Set_Texture(clock[new_val1]);
                                    count2.Set_Texture(clock[new_val2]);
                                }
                            }
                        }
                    }
                }
            }
        }


        if (!win and !lost) {
            minute_col1.Set_Texture(clock[minutes0]);
            minute_col2.Set_Texture(clock[minutes1]);
            sec_col1.Set_Texture(clock[seconds0]);
            sec_col2.Set_Texture(clock[seconds1]);
        }

        game.draw(sec_col2.open());
        game.draw(sec_col1.open());
        game.draw(minute_col2.open());
        game.draw(minute_col1.open());

        if(neg){
            game.draw(negative.open());
        }
        game.draw(happy.open());
        game.draw(pause.open());
        game.draw(leader_button.open());
        game.draw(debug.open());
        game.draw(count0.open());
        game.draw(count1.open());
        game.draw(count2.open());
        /**
        game.draw(minute_col1.open());
        game.draw(minute_col2.open());
        game.draw(sec_col1.open());
        game.draw(sec_col2.open());
         **/

        game.display();

    };


};