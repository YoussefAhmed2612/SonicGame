#include <SFML/Graphics.hpp>
#include "Menu.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>
#include<iostream>
#include<SFML/Audio.hpp>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<ctime>
#include<random>
#include<iomanip>

using namespace std;
using namespace sf;

struct Leaderboard {
    string name;
    int score;
    float time;
    void initialize() {
        score = 0;
        time = 0;
    }
    void update(string username, int userscore) {
        name = username;
        score = userscore;
        // time = usertime;
    }
};
vector<Leaderboard> leaderboard;
struct Bullet {
    float speed;
    int moveTo;
    Sprite bulletSprite;
    int magazine;
    float cooldownUse;
};
struct player
{
    Sprite sprite;
    float currentframe;
    Vector2f velocity;
    Vector2f acceleration;
    bool onground;
    const float gravity = 700.0f;
    const float moveSpeed = 270.0f;
    const float jumpHeight = 250.0f;
    float groundHeight = 726.0f;
    int last_key_pressed;
    int damage;
    int index;
    int droptype;
    int shootCooldown;
    float speed;
    float addSpeed;
    vector<Bullet>bullet;
    bool canShoot;
    void sp(Texture& sonicTexture)
    {
        sprite.setTexture(sonicTexture);
        currentframe = 0;
        last_key_pressed = 1;
        damage = 0;
        index = -1;
        droptype = -1;
        speed = 1;
        addSpeed = 0.0;
        sprite.setOrigin(15, 0);
    }
    void update(float time, float deltaTime, Sprite block[])
    {
        acceleration = Vector2f(0.0f, 0.0f);

        // Update velocity and acceleration based on player input
        if (Keyboard::isKeyPressed(Keyboard::Space) && onground) {
            velocity.y = -sqrtf(2.0f * jumpHeight * gravity);
            onground = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (velocity.x > 0) {
                while (velocity.x > 0) {
                    velocity.x -= 0.01;
                }
            }
            else
                acceleration.x = -moveSpeed;
            sprite.setScale(-2.3f, 2.3f); // Flip the sprite to face left
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (velocity.x < 0) {
                while (velocity.x < 0) {
                    velocity.x += 0.01;
                }
            }
            else
                acceleration.x = moveSpeed;
            sprite.setScale(2.3f, 2.3f); // Flip the sprite to face right
        }
        else {
            acceleration.x = 0.0f;
            if (onground) {
                if (velocity.x > 0) {
                    while (velocity.x > 0) {
                        velocity.x -= 0.01;
                    }
                }
                else {
                    while (velocity.x < 0) {
                        velocity.x += 0.01;
                    }
                }
                while (velocity.y < 0) {
                    velocity.y += 20;
                }
            }
            sprite.setTextureRect(IntRect(0, 0, 50, 55));
        }

        // cout << velocity.x << "   " << velocity.y << endl;
         // Apply gravity to the character
        velocity.y += gravity * deltaTime;

        // Apply acceleration to the velocity
        if (velocity.x <= 700)
            velocity.x += acceleration.x * deltaTime;


        // Update the character's position
        sprite.setPosition(sprite.getPosition().x + velocity.x * deltaTime, sprite.getPosition().y + velocity.y * deltaTime);

        // Check for collisions with the blocks
        for (int i = 0; i < 46; i++) {
            FloatRect blockBounds(block[i].getGlobalBounds().left, block[i].getGlobalBounds().top, block[i].getGlobalBounds().width, block[i].getGlobalBounds().height);
            FloatRect characterBounds(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top, 60, 103);

            if (characterBounds.intersects(blockBounds)) {
                // Collision detected
                Vector2f characterPosition = sprite.getPosition();
                Vector2f blockPosition = block[i].getPosition();

                if (characterPosition.y + characterBounds.height <= blockPosition.y + 27.0f) {
                    // Character is on top of the block
                    sprite.setPosition(characterPosition.x, blockPosition.y - characterBounds.height);
                    velocity.y = 0.0f;
                    onground = true;
                }
                else if (characterPosition.x + characterBounds.width <= blockPosition.x + 20.0f ) {
                    // Character is colliding from the left
                    sprite.setPosition(blockPosition.x - characterBounds.width, characterPosition.y);
                    velocity.x = 0.0f;
                }
                else if (characterPosition.x >= blockPosition.x + blockBounds.width - 10.0f) {
                    // Character is colliding from the right
                    sprite.setPosition(blockPosition.x + blockBounds.width, characterPosition.y);
                    velocity.x = 0.0f;
                }
                else if (characterPosition.y <= blockPosition.y + blockBounds.height) {
                    // Character is colliding from the bottom
                    sprite.setPosition(characterPosition.x, blockPosition.y + blockBounds.height);
                    velocity.y = 0.0f;
                }
            }
        }

        // Check for collisions with the ground
        if (sprite.getPosition().y + sprite.getGlobalBounds().height >= groundHeight) {
            sprite.setPosition(sprite.getPosition().x, groundHeight - sprite.getGlobalBounds().height);
            velocity.y = 0.0f;
            onground = true;
        }
        currentframe += 0.015 * time;
        if (currentframe > 12)
            currentframe -= 12;
        if (acceleration.x != 0)
            sprite.setTextureRect(IntRect(int(currentframe) * 50, 0, 50, 55));
    }
};
struct Enemy {
    Sprite sprite;
    float speed;
    float x;
    float animation = 0;
    int health = 3 * 19;
};
struct Help {
    Sprite dropShape; //drop
    Sprite targetShape; // block
    int type; // 0, 1, 2, 3

};
vector<Help>dropBag;
Sprite Drops[4]; // 0 pistol, 1 rifle, 2 health, 3 speed
Texture DropsTex[6];
void Setdrops() {
    DropsTex[0].loadFromFile("Textures/pistol3.png");
    DropsTex[1].loadFromFile("Textures/Rifle2.png");
    DropsTex[2].loadFromFile("Textures/heart (3).png");
    DropsTex[3].loadFromFile("Textures/thunder.png");
    DropsTex[4].loadFromFile("Textures/bullet lazer.png");
    DropsTex[5].loadFromFile("Textures/longshot laser.png");
    for (int i = 0; i < 4; i++)
    {
        Drops[i].setTexture(DropsTex[i]);
    }
    Drops[0].setScale(0.15, 0.15);
    Drops[1].setScale(0.15, 0.15);
    Drops[2].setScale(2.6, 2.6);
    Drops[3].setScale(2.5, 2.5);
}
void dropADrop() {
    for (int i = 0; i < dropBag.size(); i++) {
        dropBag[i].dropShape.move(0, 5);
        if (dropBag[i].dropShape.getGlobalBounds().intersects(dropBag[i].targetShape.getGlobalBounds()))
            dropBag[i].dropShape.move(0, -5);
    }
}
void chooseDrop(Sprite ground1[], Clock& timerAdd, Clock& timerDelete) {
    if (timerAdd.getElapsedTime().asSeconds() >= 2) {
        int indexDrop = rand() % 4;
        int indexBlock = rand() % 44;
        Help help;
        help.dropShape = Drops[indexDrop];
        help.targetShape = ground1[indexBlock];
        help.dropShape.setPosition(help.targetShape.getPosition().x + help.targetShape.getScale().x * 125 / 2 - 20, -100);
        help.type = indexDrop;
        dropBag.push_back(help);
        timerAdd.restart();

    }
    if (timerDelete.getElapsedTime().asSeconds() >= 6) {
        if (!dropBag.empty()) {
            dropBag.erase(dropBag.begin());
            timerDelete.restart();
        }
    }
}
void dropCollision(player& player) {
    for (int i = 0; i < dropBag.size(); i++)
    {
        if (player.sprite.getGlobalBounds().intersects(dropBag[i].dropShape.getGlobalBounds())) {
            player.droptype = dropBag[i].type;
            dropBag.erase(dropBag.begin() + i);

        }
    }

}
void checkDrop(player& player) {
    if (player.droptype == -1) {
        return;
    }
    else {
        if (player.droptype == 0) { //pistol
            Bullet bullet;
            bullet.bulletSprite.setTexture(DropsTex[4]);
            bullet.bulletSprite.setScale(0.6, 0.6);
            bullet.speed = 8;
            bullet.cooldownUse = 20;
            bullet.magazine = 5;
            player.canShoot = 1;
            for (int i = 0; i < bullet.magazine; i++)
            {
                player.bullet.push_back(bullet);
                player.index++;
            }
        }
        else if (player.droptype == 1) {// rifle
            Bullet bullet;
            bullet.bulletSprite.setTexture(DropsTex[5]);
            bullet.bulletSprite.setScale(0.35, 0.35);
            bullet.speed = 12;
            bullet.cooldownUse = 10;
            bullet.magazine = 10;
            player.canShoot = 1;
            for (int i = 0; i < bullet.magazine; i++)
            {
                player.bullet.push_back(bullet);
                player.index++;
            }
        }
        else if (player.droptype == 2) {
            if (player.damage > 0)
                player.damage--;
        }
        else if (player.droptype == 3) {
            player.addSpeed = 10;
            player.speed = 1.4;
        }
        player.droptype = -1;
    }
}
void resetSpeed(player& player) {
    if (player.addSpeed <= 0) {
        player.addSpeed = 0;
        player.speed = 1;
        return;

    }
    else {
        player.addSpeed -= 0.01;
    }
}
void bulletCooldown(player& player) {
    if (player.shootCooldown > 0) {
        player.shootCooldown -= 0.00001;
    }
    if (player.shootCooldown <= 0 && player.canShoot == 0) {
        player.canShoot = 1;
    }
}
void moveBullets(vector<Bullet>& bullet) {
    for (int i = 0; i < bullet.size(); i++) {
        if (bullet[i].moveTo == 2) {
            bullet[i].bulletSprite.move(-1 * bullet[i].speed, 0);
        }
        if (bullet[i].moveTo == 1) {
            bullet[i].bulletSprite.move(1 * bullet[i].speed, 0);

        }
        if (bullet[i].bulletSprite.getPosition().x >= 10500 ||
            bullet[i].bulletSprite.getPosition().x <= -100) {
            bullet.erase(bullet.begin() + i);
        }

    }
}
void coin(Sprite coins[]) {
    coins[0].setPosition(430, 570);
    coins[0].setScale(3.4f, 3.4f);
    coins[1].setPosition(630, 570);
    coins[1].setScale(3.4f, 3.4f);
    coins[2].setPosition(830, 570);
    coins[2].setScale(3.4f, 3.4f);
    coins[3].setPosition(1030, 570);
    coins[3].setScale(3.4f, 3.4f);
    coins[4].setPosition(1230, 570);
    coins[4].setScale(3.4f, 3.4f);
    coins[5].setPosition(1430, 570);
    coins[5].setScale(0.4f, 0.4f);
    coins[6].setPosition(1630, 570);
    coins[6].setScale(0.4f, 0.4f);
    coins[7].setPosition(1830, 570);
    coins[7].setScale(0.4f, 0.4f);
    coins[8].setPosition(2030, 570);
    coins[8].setScale(0.4f, 0.4f);
    coins[9].setPosition(2230, 570);
    coins[9].setScale(0.4f, 0.4f);
    coins[10].setPosition(2430, 570);
    coins[10].setScale(0.4f, 0.4f);
    coins[11].setPosition(2630, 570);
    coins[11].setScale(0.4f, 0.4f);
    coins[12].setPosition(2830, 570);
    coins[12].setScale(0.4f, 0.4f);
    coins[13].setPosition(3030, 570);
    coins[13].setScale(0.4f, 0.4f);
    coins[14].setPosition(3230, 570);
    coins[14].setScale(0.4f, 0.4f);
    coins[15].setPosition(3430, 570);
    coins[15].setScale(0.4f, 0.4f);
    coins[16].setPosition(3630, 570);
    coins[16].setScale(0.4f, 0.4f);
    coins[17].setPosition(3830, 570);
    coins[17].setScale(0.4f, 0.4f);
    coins[18].setPosition(4030, 570);
    coins[18].setScale(0.4f, 0.4f);
    coins[19].setPosition(4230, 570);
    coins[19].setScale(0.4f, 0.4f);
    coins[20].setPosition(4430, 570);
    coins[20].setScale(0.4f, 0.4f);
    coins[21].setPosition(4630, 570);
    coins[21].setScale(0.4f, 0.4f);
    coins[22].setPosition(4830, 570);
    coins[22].setScale(0.4f, 0.4f);
    coins[23].setPosition(5030, 570);
    coins[23].setScale(0.4f, 0.4f);
    coins[24].setPosition(5230, 570);
    coins[24].setScale(0.4f, 0.4f);
}
void block(Sprite ground1[])
{
    ground1[0].setPosition(560, 480);
    ground1[0].setScale(1.2f, 1.5f);    //small
    ground1[1].setPosition(1100, 350);
    ground1[1].setScale(1.2f, 1.5f);    //small
    ground1[2].setPosition(1700, 200);
    ground1[2].setScale(1.2f, 1.5f);    //small
    ground1[3].setPosition(2300, 300);
    ground1[3].setScale(1.5f, 1.5f);
    ground1[4].setPosition(2900, 400);
    ground1[4].setScale(1.5f, 1.5f);
    ground1[5].setPosition(3500, 350);
    ground1[5].setScale(1.2f, 1.5f);    //small
    ground1[6].setPosition(4100, 250);
    ground1[6].setScale(1.5f, 1.5f);
    ground1[7].setPosition(4700, 290);
    ground1[7].setScale(1.5f, 1.5f);
    ground1[8].setPosition(5300, 380);
    ground1[8].setScale(1.5f, 1.5f);
    ground1[9].setPosition(5900, 380);
    ground1[9].setScale(1.5f, 1.5f);
    ground1[10].setPosition(6500, 200);
    ground1[10].setScale(1.5f, 1.5f);
    ground1[11].setPosition(7100, 300);
    ground1[11].setScale(1.5f, 1.5f);
    ground1[12].setPosition(7700, 400);
    ground1[12].setScale(1.2f, 1.5f);     //small
    ground1[13].setPosition(8300, 300);
    ground1[13].setScale(1.5f, 1.5f);
    ground1[14].setPosition(8900, 400);
    ground1[14].setScale(1.5f, 1.5f);
    ground1[15].setPosition(9500, 290);
    ground1[15].setScale(1.5f, 1.5f);
    ground1[16].setPosition(10100, 400);
    ground1[16].setScale(1.2f, 1.5f);     //small
    ground1[17].setPosition(10700, 300);
    ground1[17].setScale(1.5f, 1.5f);
    ground1[18].setPosition(11300, 400);
    ground1[18].setScale(1.5f, 1.5f);
    ground1[19].setPosition(11900, 300);
    ground1[19].setScale(1.5f, 1.5f);
    ground1[20].setPosition(12500, 200);
    ground1[20].setScale(1.2f, 1.5f);         //small
    ground1[21].setPosition(13100, 300);
    ground1[21].setScale(1.5f, 1.5f);
    ground1[22].setPosition(13700, 400);
    ground1[22].setScale(1.2f, 1.5f);  //small
    ground1[23].setPosition(560 + 15000, 450);
    ground1[23].setScale(1.2f, 1.5f);    //small
    ground1[24].setPosition(1100 + 15000, 350);
    ground1[24].setScale(1.2f, 1.5f);    //small
    ground1[25].setPosition(1700 + 15000, 200);
    ground1[25].setScale(1.2f, 1.5f);    //small
    ground1[26].setPosition(2300 + 15000, 300);
    ground1[26].setScale(1.5f, 1.5f);
    ground1[27].setPosition(2900 + 15000, 400);
    ground1[27].setScale(1.5f, 1.5f);
    ground1[28].setPosition(3500 + 15000, 350);
    ground1[28].setScale(1.2f, 1.5f);    //small
    ground1[29].setPosition(4100 + 15000, 250);
    ground1[29].setScale(1.5f, 1.5f);
    ground1[30].setPosition(4700 + 15000, 290);
    ground1[30].setScale(1.5f, 1.5f);
    ground1[31].setPosition(5300 + 15000, 380);
    ground1[31].setScale(1.5f, 1.5f);
    ground1[32].setPosition(5900 + 15000, 380);
    ground1[32].setScale(1.5f, 1.5f);
    ground1[33].setPosition(6500 + 15000, 200);
    ground1[33].setScale(1.5f, 1.5f);
    ground1[34].setPosition(7100 + 15000, 300);
    ground1[34].setScale(1.5f, 1.5f);
    ground1[35].setPosition(7700 + 15000, 400);
    ground1[35].setScale(1.2f, 1.5f);     //small
    ground1[36].setPosition(8300 + 15000, 300);
    ground1[36].setScale(1.5f, 1.5f);
    ground1[37].setPosition(8900 + 15000, 400);
    ground1[37].setScale(1.5f, 1.5f);
    ground1[38].setPosition(9500 + 15000, 290);
    ground1[38].setScale(1.5f, 1.5f);
    ground1[39].setPosition(10100 + 15000, 400);
    ground1[39].setScale(1.2f, 1.5f);     //small
    ground1[40].setPosition(10700 + 15000, 300);
    ground1[40].setScale(1.5f, 1.5f);
    ground1[41].setPosition(11300 + 15000, 400);
    ground1[41].setScale(1.5f, 1.5f);
    ground1[42].setPosition(11900 + 15000, 300);
    ground1[42].setScale(1.5f, 1.5f);
    ground1[43].setPosition(12500 + 15000, 200);
    ground1[43].setScale(1.2f, 1.5f);         //small
    ground1[44].setPosition(13100 + 15000, 300);
    ground1[44].setScale(1.5f, 1.5f);
    ground1[45].setPosition(13700 + 15000, 400);
    ground1[45].setScale(1.2f, 1.5f);  //small
}
void playername(RenderWindow& window, RenderWindow& gameplay, string& name)
{
    if (!name.empty())
    {
        name.clear();
    }
    Texture bacgroundmenu;
    bacgroundmenu.loadFromFile("lapper-bggreenhill1.jpg");  // load soora lel window
    Sprite bg;
    bg.setTexture(bacgroundmenu);
    Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF"); // load font lel window 
    Text t1;
    Text t2;    // name 
    t1.setFont(font);
    t2.setFont(font);
    t1.setString("Enter your name: ");
    t1.setCharacterSize(70);
    t2.setCharacterSize(70);
    t1.setPosition(10, 10);
    t2.setPosition(10, 100);
    t1.setFillColor(Color::White);
    t2.setFillColor(Color::White);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::TextEntered)

            {
                name += static_cast<char>(event.text.unicode);  //hatzawd 3ala el stirng el letter eli ana bada5alo
            }
            if (Keyboard::isKeyPressed(Keyboard::BackSpace) && name.size() > 0)
            {
                name.resize(name.size() - 1);

            }
            if (Keyboard::isKeyPressed(Keyboard::Return) && name.size() > 1)
            {
                window.close();
                return;
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
                gameplay.close();
                return;
            }

        }
        t2.setString(name);
        window.clear();
        window.draw(bg);
        window.draw(t1);
        window.draw(t2);
        window.display();

    }


}
void main()
{
    // make a Main window
    RenderWindow MainMenu(VideoMode(1920, 1080), "game");
    Menu mainmenu(MainMenu.getSize().x, MainMenu.getSize().y);

    RectangleShape bg;
    bg.setSize(Vector2f(1920, 1080));
    bg.setScale(1, 1);
    bg.setPosition(0, 0);
    Texture mainbg;
    mainbg.loadFromFile("Textures/lapper-bggreenhill1.jpg");
    bg.setTexture(&mainbg);

    //enter name
    string name;
    Leaderboard user;
    user.initialize();


    RectangleShape sonicabg;
    sonicabg.setSize(Vector2f(795, 475));
    sonicabg.setScale(1, 1);
    sonicabg.setPosition(485, 190);
    Texture Smainbg;
    Smainbg.loadFromFile("Textures/bgsonic.png");
    sonicabg.setTexture(&Smainbg);

    while (MainMenu.isOpen())
    {
        Event event;
        while (MainMenu.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                MainMenu.close();
            }
            if (event.type == Event::KeyReleased)
            {
                if (event.key.code == Keyboard::Up)
                {
                    mainmenu.moveup();
                    break;
                }

                if (event.key.code == Keyboard::Down)
                {
                    mainmenu.movedown();
                    break;

                }
                if (event.key.code == Keyboard::Return)
                {
                    RenderWindow window(sf::VideoMode(1920, 1080), "Sonic Game");
                    window.setFramerateLimit(60);
                    RenderWindow entername(VideoMode(1920, 1080), "Enter Name");
                    RenderWindow Options(VideoMode(1920, 1080), "Controls");
                    RenderWindow About(VideoMode(1920, 1080), "Leaderboard");

                    int x = mainmenu.pressed();
                    if (x == 0)
                    {

                        srand(static_cast<unsigned>(time(NULL)));
                        vector <Sprite> coinslist(50);
                        Clock timerAdd, timerDelete, gametime;

                        //adding score,time,rings
                        Texture scoreimagetexture;
                        scoreimagetexture.loadFromFile("Textures/scoreimage.png");
                        Sprite scoreimage[5];
                        for (int i = 0; i < 3; i++) {
                            scoreimage[i].setTexture(scoreimagetexture);
                            scoreimage[i].setScale(3.5f, 3.5f);
                        }
                        scoreimage[0].setTextureRect(IntRect(0, 0, 50, 55));
                        scoreimage[1].setTextureRect(IntRect(0, 56, 40, 20));
                        scoreimage[2].setTextureRect(IntRect(88, 0, 10, 25));

                        //declaring sonic
                        Texture sonictexture;
                        sonictexture.loadFromFile("Textures/approvedsonic.png");
                        player sonic;
                        sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
                        sonic.sprite.setScale(2.3, 2.3);
                        sonic.sp(sonictexture);


                        //declaring enemy
                        Texture enemytexture;
                        enemytexture.loadFromFile("Textures/crabenemy.png");
                        Enemy enemy;
                        enemy.sprite.setTexture(enemytexture);
                        enemy.speed = 1.8;
                        enemy.sprite.setPosition(1700, 580);
                        enemy.sprite.setTextureRect(IntRect(0, 0, 51, 40));
                        enemy.sprite.setScale(-3.3, 3.3);


                        //setting ground plants
                        Texture groundtextureP;
                        groundtextureP.loadFromFile("Textures/maplevel1.png");
                        Sprite ground2P[50];
                        for (int i = 0; i < 50; i++)
                        {
                            ground2P[i].setTexture(groundtextureP);
                            ground2P[i].setPosition(Vector2f((i * 1800), 385));
                            ground2P[i].setScale(1.8, 1.5);
                            ground2P[i].setOrigin(200, 0);
                        }


                        //setting spikes
                        Texture spikes;
                        spikes.loadFromFile("Textures/spikes.png");
                        Sprite spike[50];
                        for (int i = 0; i < 50; i++)
                        {
                            spike[i].setTexture(spikes);
                            spike[i].setPosition(Vector2f(((i * 1500)), 414));
                            spike[i].setScale(2, 2);
                            spike[i].setOrigin(-200, 0);
                        }


                        //setting ground
                        Texture groundtexture;
                        groundtexture.loadFromFile("Textures/map.png");
                        Sprite ground[5];
                        for (int i = 0; i < 2; i++)
                        {
                            ground[i].setTexture(groundtexture);
                            ground[i].setPosition(Vector2f(i * 5408, 700));
                            ground[i].setScale(2, 2);
                            ground[i].setOrigin(200, 0);
                        }


                        //setting ground2
                        Texture groundtexture2;
                        groundtexture2.loadFromFile("Textures/map2.png");
                        Sprite ground2[20];
                        for (int i = 0; i < 20; i++)
                        {
                            ground2[i].setTexture(groundtexture2);
                            ground2[i].setPosition(Vector2f((i * 1088) + 15000, 695));
                            ground2[i].setScale(2, 3.1);
                            ground2[i].setOrigin(200, 0);
                        }


                        //setting blocks
                        Texture ground1texture;
                        ground1texture.loadFromFile("Textures/block89.png");
                        Texture ground1texture2;
                        ground1texture2.loadFromFile("Textures/oakground.png");
                        Sprite ground1[46];
                        block(ground1);
                        for (int i = 0; i < 23; ++i)
                        {
                            ground1[i].setTexture(ground1texture);
                        }
                        for (int i = 23; i < 46; ++i)
                        {
                            ground1[i].setTexture(ground1texture2);
                        }
                       

                        //Variables
                        int coinCount = 0;
                        int coinAnimationIndicator = 0;
                        bool isCoinVisible = true;
                        bool landed = false;
                        double velocityY = 0;
                        int score = 0;
                        int rings = 0;

                        //creating coins 
                        Texture coinsTextures;
                        coinsTextures.loadFromFile("Textures/coinsprite.png");
                        Sprite coins[25];
                        coin(coins);
                        for (int i = 0; i < 25; i++) {
                            coins[i].setTexture(coinsTextures);
                            coins[i].setTextureRect(IntRect(0, 0, 64, 64));
                            coins[i].setScale(0.4f, 0.4f);
                        }

                        //background 1
                        Texture backgroundtexture;
                        backgroundtexture.loadFromFile("Textures/lapper-bggreenhill1.jpg");
                        Sprite background[50];

                        for (int i = 0; i < 50; ++i)
                        {
                            background[i].setTexture(backgroundtexture);
                            background[i].setPosition(Vector2f((i * 1920), -50));
                            background[i].setOrigin(400, 0);
                            background[i].setScale(1, 1.17);
                        }

                        //Score
                        Font font;
                        font.loadFromFile("Fonts/NiseSegaSonic.TTF");
                        Text text;
                        Text text2;
                        text.setFont(font);
                        text2.setFont(font);
                        text.setString(to_string(score));
                        text2.setString(to_string(rings));
                        text.setFillColor(Color(50, 205, 50, 255));
                        text2.setFillColor(Color(50, 205, 50, 255));
                        text.setPosition(70, 65);
                        text.setCharacterSize(32);
                        text2.setCharacterSize(32);
                        text.setScale(1.45f, 1.45f);
                        text2.setScale(1.45f, 1.45f);

                        // Declare a Text object for the timer
                        Text timerText("", font, 50);
                        timerText.setFillColor(Color(50, 205, 50, 255));
                        timerText.setPosition(350, 250);

                        //2D camera
                        View view(Vector2f(0, 0), Vector2f(1920, 1080));
                        view.setCenter(sonic.sprite.getPosition() - Vector2f(200.0f, 0)); //update
                        window.setView(view);

                        //powerups
                        Setdrops();

                        // load the sound 
                        SoundBuffer coinBuffer;
                        coinBuffer.loadFromFile("Sounds/coinsound.wav");
                        Sound coinSound(coinBuffer);

                        //background 2 
                        Texture backgroundtexture2;
                        backgroundtexture2.loadFromFile("Textures/lapper-bggreenhill2.jpg");
                        Sprite background2[50];
                        for (int i = 0; i < 50; ++i)
                        {
                            background2[i].setTexture(backgroundtexture2);
                            background2[i].setPosition(Vector2f((i * 1920), -50));
                            background2[i].setOrigin(200, 0);
                            background2[i].setScale(1, 1.17);
                        }

                        while (window.isOpen())

                        {

                            //setting time 
                            Clock clock;
                            window.setFramerateLimit(60);
                            float time = clock.getElapsedTime().asMicroseconds();
                            clock.restart();
                            time *= 27.5;

                          
                            //powerups 
                            chooseDrop(ground1, timerAdd, timerDelete);
                            dropADrop();
                            dropCollision(sonic);
                            checkDrop(sonic);
                            resetSpeed(sonic);

                            //bullet 
                            bulletCooldown(sonic);
                            moveBullets(sonic.bullet);

                            
                            Event aevent;
                            while (window.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                {
                                    window.close();
                                }
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
                                    {
                                        window.close();
                                    }
                                }
                            }

                            // Move the player using A,D and space keys
                            if (sonic.last_key_pressed == 1) {
                                sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
                            }
                            if (sonic.last_key_pressed == 2) {
                                sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
                                sonic.sprite.setScale(-2.3, 2.3);
                            }
                            if (Keyboard::isKeyPressed(Keyboard::A)) {
                                sonic.sprite.setScale(-2.3, 2.3);
                            }
                            if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
                                sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
                                sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
                                sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
                                sonic.index--;
                                sonic.canShoot = 0;
                            }
                            //setting position for score
                            text.setPosition(sonic.sprite.getPosition().x - 100, 48);
                            text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
                            timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
                            scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
                            scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
                            scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);


                            //collision between sonic and enemy
                            if (sonic.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds()) || enemy.sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
                            {
                                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580); // Respawn the enemy on the right side of the window 
                                sonic.damage++;
                                if (sonic.damage <= 3)
                                {
                                    scoreimage[2].setTextureRect(IntRect(88 - (10 * sonic.damage), 0, 10, 25));
                                }
                                else
                                {
                                    //sonic dies
                                }
                            }

                            //collision between bullets and enemy
                            for (int j = 0; j < sonic.bullet.size(); j++)
                            {
                                if (sonic.bullet[j].bulletSprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                                    enemy.health -= 19;
                                    sonic.bullet[j].bulletSprite.setScale(0, 0);
                                }
                                if (enemy.health == 0) {
                                    enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580);
                                    score += 5;
                                    text.setString(to_string(score));
                                    enemy.health = 3 * 19;
                                }
                            }

                            //sonic limits
                            if (sonic.sprite.getPosition().x < 0) {
                                sonic.sprite.setPosition(0, sonic.sprite.getPosition().y);
                            }
                            if (sonic.sprite.getPosition().y < 0) {
                                sonic.sprite.setPosition(sonic.sprite.getPosition().x, 0);
                            }
                            if (sonic.sprite.getPosition().x > 16000) {
                                if (sonic.sprite.getPosition().x < 16000) {
                                    sonic.sprite.setPosition(16000, sonic.sprite.getPosition().y);
                                }
                            }

                            //animation of coins
                            for (int i = 0; i < 25; i++) {
                                coinAnimationIndicator += 0.08 * time;
                                if (coinAnimationIndicator > 9)
                                    coinAnimationIndicator -= 9;
                                coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
                            }

                            //incrementing scoring
                            for (int i = 0; i < 25; i++) {
                                if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                                    coinSound.play(); // Play the sound effect
                                    coins[i].setScale(0, 0);
                                    rings++;
                                    text2.setString(to_string(rings));
                                }
                            }

                            //Updating sonic
                            view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
                            sonic.update(time, 1.0f / 40.f, ground1);

                            //enemy animation
                            enemy.sprite.move(-enemy.speed, 0);
                            enemy.sprite.setTextureRect(IntRect(int(enemy.animation) * 52, 0, 52, 40));
                            enemy.animation += 0.1;
                            if (enemy.animation > 4)
                                enemy.animation = 0;

                            Options.close();
                            About.close();
                          
                            playername(entername, window, name);
                            user.update(name, score);

                            if (sonic.sprite.getPosition().x <= 15000) {
                                window.clear();
                                
                                Time elapsedTime = gametime.getElapsedTime();
                                int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
                                int minutes = (totalSeconds % 3600) / 60;
                                int seconds = totalSeconds % 60;

                                // Format the time as a string
                                string timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
                                timerText.setString(timeString); // Set the text string

                                window.setView(view);
                                for (int i = 0; i < 50; ++i)
                                {
                                    window.draw(background[i]);
                                }
                                for (int i = 0; i < 20; i++)
                                {
                                    window.draw(ground2P[i]);
                                }

                                for (int i = 0; i < 25; i++) {
                                    window.draw(coins[i]);
                                }

                                for (int i = 0; i < 20; i++)
                                {
                                    window.draw(spike[i]);
                                }
                                window.draw(enemy.sprite);

                                for (int i = 0; i < 5; i++)
                                {
                                    window.draw(ground[i]);
                                }
                                for (int i = 0; i < sonic.bullet.size(); i++)
                                {
                                    window.draw(sonic.bullet[i].bulletSprite);
                                }

                                for (int i = 0; i < dropBag.size(); i++) {
                                    window.draw(dropBag[i].dropShape);
                                }

                                window.draw(sonic.sprite);
                                for (int i = 0; i < 23; i++) {
                                    window.draw(ground1[i]);
                                }
                                window.draw(text);
                                window.draw(text2);
                                window.draw(timerText);
                                for (int i = 0; i < 3; i++) {
                                    window.draw(scoreimage[i]);
                                }
                                
                                window.display();
                            }
                            //
                            ///
                            ////
                            /////
                            //////
                            /////level 2
                            //////
                            /////
                            ////
                            ///
                            //
                            else if (sonic.sprite.getPosition().x > 15000) {
                                enemy.sprite.setPosition(1700 + 16000, 580);
                                sonic.groundHeight = 744;
                                sonic.update(time, 1.0f / 40.f, ground1);
                                window.clear();

                               
                                Time elapsedTime = gametime.getElapsedTime();
                                int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
                                int minutes = (totalSeconds % 3600) / 60;
                                int seconds = totalSeconds % 60;

                                // Format the time as a string
                                string timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
                                timerText.setString(timeString); // Set the text string

                                window.setView(view);
                                window.draw(enemy.sprite);
                                for (int i = 0; i < 50; i++)
                                {
                                    window.draw(background2[i]);
                                }
                                for (int i = 0; i < 20; i++)
                                {
                                    window.draw(ground2[i]);
                                }
                                for (int i = 0; i < sonic.bullet.size(); i++)
                                {
                                    window.draw(sonic.bullet[i].bulletSprite);
                                }

                                for (int i = 0; i < dropBag.size(); i++) {
                                    window.draw(dropBag[i].dropShape);
                                }
                                window.draw(sonic.sprite);
                                for (int i = 23; i < 46; i++) {
                                    window.draw(ground1[i]);
                                }
                                for (int i = 0; i < 25; i++) {
                                    window.draw(coins[i]);
                                }
                                window.draw(text);
                                window.draw(text2);
                                window.draw(timerText);
                                for (int i = 0; i < 3; i++) {
                                    window.draw(scoreimage[i]);
                                }


                                window.display();
                            }
                        }

                    }
                    if (x == 1)
                    {
                        int j = 0;
                        int i = 0;

                        Texture keysD;
                        keysD.loadFromFile("Textures/D.png");
                        Sprite keys2(keysD);
                        keys2.setPosition(200, 300);
                        keys2.setScale(3, 3);
                        keys2.setTextureRect(IntRect(0, 0, 19, 21));

                        Texture keysA;
                        keysA.loadFromFile("Textures/A.png");
                        Sprite keys4(keysA);
                        keys4.setPosition(200, 500);
                        keys4.setScale(3, 3);
                        keys4.setTextureRect(IntRect(0, 0, 19, 21));

                        Texture keysSpace;
                        keysSpace.loadFromFile("Textures/SPACE.png");
                        Sprite keys5(keysSpace);
                        keys5.setPosition(130, 790);
                        keys5.setScale(3, 3);
                        keys5.setTextureRect(IntRect(98, 0, 98, 21));

                        Texture keysDmove;
                        keysDmove.loadFromFile("Textures/sonicsprite.png");
                        Sprite keysR(keysDmove);
                        keysR.setPosition(550, 240);
                        keysR.setScale(0.8, 0.8);
                        keysR.setTextureRect(IntRect(0, 0, 102, 105));

                        Texture keysblock;
                        keysblock.loadFromFile("Textures/block.png");
                        Sprite keysblock1(keysblock);
                        keysblock1.setPosition(575, 320);
                        keysblock1.setScale(0.5, 0.3);

                        Texture keysAmove;
                        keysAmove.loadFromFile("Textures/sonicsprite.png");
                        Sprite keysL(keysAmove);
                        keysL.setPosition(890, 440);
                        keysL.setScale(-0.8, 0.8);
                        keysL.setTextureRect(IntRect(0, 0, 102, 105));


                        Sprite keysblock2(keysblock);
                        keysblock2.setPosition(575, 520);
                        keysblock2.setScale(0.5, 0.3);



                        while (Options.isOpen())
                        {
                            Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                {
                                    Options.close();

                                }
                                if (aevent.key.code == Keyboard::Escape)
                                {
                                    Options.close();

                                }


                            }
                            Clock clock1;
                            float time1 = clock1.getElapsedTime().asMicroseconds();
                            clock1.restart();
                            time1 *= 27.5;

                            RectangleShape controlbg;
                            controlbg.setSize(Vector2f(1920, 1080));
                            controlbg.setScale(1, 1);
                            controlbg.setPosition(0, 0);
                            Texture ctrlbg;
                            ctrlbg.loadFromFile("Textures/ylow.jpg");
                            controlbg.setTexture(&ctrlbg);


                            RectangleShape bluebg;
                            bluebg.setSize(Vector2f(1300, 500));
                            bluebg.setScale(1, 1.48);
                            bluebg.setPosition(0, 150);
                            Texture bmainbg;
                            bmainbg.loadFromFile("Textures/bluebg.png");
                            bluebg.setTexture(&bmainbg);


                            Font font1;
                            font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
                            Text text1;
                            text1.setFont(font1);
                            text1.setString("CONTROLS");
                            text1.setPosition(450, 100);
                            text1.setCharacterSize(70);
                            text1.setFillColor(Color::White);
                            text1.setOutlineColor(Color::Black);
                            text1.setOutlineThickness(3);


                            RectangleShape rectangle1(Vector2f(460.f, 60.f));
                            rectangle1.setPosition(450, 150);
                            rectangle1.setFillColor(Color{ 255, 71, 76 });

                            RectangleShape rectangle(Vector2f(460.f, 60.f));
                            rectangle.setPosition(450, 90);
                            rectangle.setFillColor(Color{ 135, 206, 235 });



                            keys2.setTextureRect(IntRect(i * 19, 0, 19, 21));
                            keys4.setTextureRect(IntRect(i * 19, 0, 19, 21));
                            keys5.setTextureRect(IntRect(i * 98, 0, 98, 21));
                            keysR.setTextureRect(IntRect(int(j) * 102, 0, 102, 105));
                            keysL.setTextureRect(IntRect(int(j) * 102, 0, 102, 105));
                            i += 1;
                            if (i > 2)
                            {
                                i = 0;

                            }
                            j += 1;
                            if (j > 9)
                            {
                                j -= 9;
                            }
                            keysR.move(20, 0);
                            if (keysR.getPosition().x > 815)
                            {
                                keysR.setPosition(550, 240);
                            }

                            keysL.move(-20, 0);

                            if (keysL.getPosition().x < 630)
                            {
                                keysL.setPosition(890, 440);
                            }


                            window.close();
                            Options.clear();
                            About.close();
                            Options.draw(controlbg);
                            Options.draw(bluebg);
                            Options.draw(rectangle1);
                            Options.draw(rectangle);
                            Options.draw(text1);
                            Options.draw(keys2);
                            Options.draw(keys4);
                            Options.draw(keys5);
                            Options.draw(keysR);
                            Options.draw(keysblock1);
                            Options.draw(keysblock2);
                            Options.draw(keysL);
                            Options.display();

                        }


                    }
                    if (x == 2)
                    {
                        while (About.isOpen())
                        {
                            Event aevent;
                            while (About.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                {
                                    About.close();
                                }
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
                                    {
                                        About.close();
                                    }
                                }
                            }
                            window.close();
                            Options.close();
                            About.clear();
                            About.display();


                        }

                    }
                    if (x == 3) {
                        MainMenu.close();
                        break;
                    }

                }
            }

        }
        MainMenu.clear();
        MainMenu.draw(bg);
        MainMenu.draw(sonicabg);
        mainmenu.draw(MainMenu);
        MainMenu.display();
    }
}
