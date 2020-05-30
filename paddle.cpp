// Copyright [2015] <Chafic Najjar>

#include "paddle.h"
#include "ball.h"
#include "pong.h"

const int Paddle::HEIGHT = 60;
const int Paddle::WIDTH = 10;

Paddle::Paddle(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

int Paddle::get_x() const {
    return x;
}

int Paddle::get_y() const {
    return y;
}

void Paddle::set_y(int new_y) {
    y = new_y;

    // Paddle shouldn't be allowed to go above or below the screen.
    if (y < 0)
        y = 0;
    else if (y + HEIGHT > SCREEN_HEIGHT)
        y = SCREEN_HEIGHT - HEIGHT;
}

void Paddle::add_to_y(int new_y) {
    y += new_y;

    // Paddle shouldn't be allowed to go above or below the screen.
    if (y < 0)
        y = 0;
    else if (y + HEIGHT > SCREEN_HEIGHT)
        y = SCREEN_HEIGHT - HEIGHT;
}

// Imprecise prediction of ball position on the y-axis.
int Paddle::predict(Ball *ball) {
    // Find slope.
    float slope = static_cast<float>(ball->y - ball->y+ball->dy) /
                  (ball->x - ball->x+ball->dx);

    // Distance between ball and paddle.
    int paddle_distance = ball->x - x;

    // Prediction without taking into consideration upper and
    // bottom wall collisions.
    int predicted_y = abs(slope * -(paddle_distance) + ball->y);

    // Calculate number of reflexions.
    int number_of_reflexions = predicted_y / SCREEN_HEIGHT;

    // Predictions taking into consideration upper and bottom
    // wall collisions.

    // Even number of reflexions.
    if (number_of_reflexions % 2 == 0)
        predicted_y = predicted_y % SCREEN_HEIGHT;

    // Odd number of reflexions.
    else
        predicted_y = SCREEN_HEIGHT - (predicted_y % SCREEN_HEIGHT);

    return predicted_y;
}

// Basic AI movement.
void Paddle::AI(Ball *ball) {
    // Ball on the left 3/5th side of the screen and going left.
    if (ball->x < SCREEN_WIDTH*3/5 && ball->dx < 0) {
        // Follow the ball.
        if (y + (HEIGHT - ball->LENGTH)/2 < ball->predicted_y-2)
            add_to_y(ball->speed/8 * 5);
        else if (y + (HEIGHT - ball->LENGTH)/2 > ball->predicted_y+2)
            add_to_y(-(ball->speed/8 * 5));

    // Ball is anywhere on the screen but going right.
    } else if (ball->dx >= 0) {
        // Left paddle slowly moves to the center.
        if (y + HEIGHT / 2 < SCREEN_HEIGHT/2)
            add_to_y(2);
        else if (y + HEIGHT / 2 > SCREEN_HEIGHT/2)
            add_to_y(-2);
    }
}
