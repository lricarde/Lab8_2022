// Lab8_2022.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

//Error message if images cannot be found
void LoadTex(Texture& tex, string filename) {
	if (!tex.loadFromFile(filename)) {
		cout << "Could not load" << filename << endl;
	}
}

//Movements of the crossbow and arrow 
void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main() {

    //creates window to display game
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    //Loads crossbow image
    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "Lab8Images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    //Loads arrow image
    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "Lab8Images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    //Creates a "ceiling" for boundary physics
    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    //Creates a wall 
    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    //Creates a wall
    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    //Detects collision of arrow
    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
    };

    //Error message if font file cannot be found
    Font fnt;
    if (!fnt.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }

    //add ducks
    Texture blueTex;
    LoadTex(blueTex, "Lab8Images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;
    for (int i(0); i < 6; i++) {
        PhysicsSprite& duck = ducks.Create();
        duck.setTexture(blueTex);
        int x = 50 + ((700 / 5) * i);
        Vector2f sz = ducks.getSize();
        duck.setCenter(Vector2f(x, 20 + (sz.y / 2)));
        duck.setVelocity(Vector2f(0.25, 0));
        world.AddPhysicsBody(duck);
        duck.onCollision=
            [&drawingArrow, &world, &arrow, &duck, &ducks, &score]
        (PhysicsBodyCollisionResult result) {
            if (result.object2 == arrow) {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
                score += 10;
            }
        };
    }

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            ducks.DoRemovals();
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }

            //Counts how many arrows are shot
            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            window.draw(scoreText);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            arrowCountText.setPosition(Vector2f(790 - GetTextSize(arrowCountText).x, 0));
            window.draw(arrowCountText);

            window.display();
        }
        }

    //displays game over text after five arrows are shot
    window.display(); 
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (true);
}