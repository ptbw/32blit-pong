// Copyright [2015] <Chafic Najjar>

#include "ball.h"

#include <cmath>
#include <random>

#include "pong.h"
#include "paddle.h"

// This will prevent linker errors in case the same
// names are used in other files.
namespace {
    std::random_device rd;
    std::mt19937 gen(rd());
}

// Ball dimensions.
const int Ball::LENGTH = 10;

Ball::Ball(int x, int y) {
    // Ball status.
    status = READY;

    // Ball position.
    this->x = x;
    this->y = y;

    // Ball movement.
    dx = 0;
    dy = 0;

    bounce = false;
    speed = 8;
    angle = 0.0f;
    hits = 0;
    predicted_y = 0;
}

void Ball::launch_ball(Paddle *ai_paddle) {
    std::uniform_int_distribution<int> dir(0, 1);
    int direction = 1+(-2)*(dir(gen)%2);  // Either 1 or -1.

    std::uniform_int_distribution<int> ang(-60, 60);
    angle = ang(gen);  // Between -60 and 60.

    dx = direction*speed*std::cos(angle*3.14159/180.0f);  // Speed on the x-axis.
    dy = speed*std::sin(angle*3.14159/180.0f);  // Speed on the y-axis.

    status = LAUNCHED;
}

void Ball::bounces_off(Paddle *paddle) {
    if (paddle == nullptr) return;

    hits++;

    int sign = (paddle->get_x() < SCREEN_WIDTH/2) ? 1 : -1;

    int relative_y = (y - paddle->get_y() + LENGTH);

    angle = (2.14f * relative_y - 75.0f);

    // Convert angle to radian, find its cos() and multiply by the speed.
    dx = sign*speed*std::cos(angle*3.14159/180.0f);

    // Convert angle to radina, find its sin() and multiply by the speed.
    dy = speed*std::sin(angle*3.14159/180.0f);
}

void Ball::update_speed() {
    // Increment ball speed for every 6 hits.
    if (hits == 5) {
        speed++;
        hits = 0;
    }
}

bool Ball::wall_collision() {
    return (y + dy < 0) || (y + LENGTH + dy >= SCREEN_HEIGHT);
}

bool Ball::collides_with(Paddle *paddle) {
    if (paddle->get_x() < SCREEN_WIDTH/2) {
        // Check if collision with left paddle occurs in next frame
        if (x > paddle->get_x() + Paddle::WIDTH ||
            x < paddle->get_x() ||
            !(y + LENGTH > paddle->get_y() &&
              y <= paddle->get_y() + Paddle::HEIGHT))
            return false;
        else
            return true;
    } else {
        // Check if collision with right paddle occurs in next frame.
        if (x + LENGTH < paddle->get_x() ||
            x > paddle->get_x() + Paddle::WIDTH ||
            !(y + LENGTH > paddle->get_y() &&
              y <= paddle->get_y() + Paddle::HEIGHT))
            return false;
        else
            return true;
    }
}

// Reset ball to initial state.
void Ball::reset() {
    x = SCREEN_WIDTH/2 - LENGTH/2;
    y = SCREEN_HEIGHT/2;

    // Ball is fixed.
    dx = 0;
    dy = 0;
    status = READY;

    // Speed and hit counter are reset to their initial positions.
    speed = 8;
    hits = 0;
}
