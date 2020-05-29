#include "pong.h"

#include <iostream>

#include "ball.h"
#include "paddle.h"

#include "paddle_hit.h"
#include "wall_hit.h"
#include "score_update.h"

using namespace blit;

// Screen resolution.
const uint16_t screen_width = SCREEN_WIDTH;
const uint16_t screen_height = SCREEN_HEIGHT;

// Game objects.
Ball* ball;
Paddle* left_paddle;
Paddle* right_paddle;

// Scores.
int left_score;
int right_score;

// Indicates when rendering new score is necessary.
bool left_score_changed;

// Indicates when rendering new score is necessary.
bool right_score_changed;

int gamepad_direction;  // gamepad direction.
int mouse_x, mouse_y;   // Mouse coordinates.

void init();
void update(uint32_t time);
void render(uint32_t time);

void play_paddle_hit();
void play_score_update();
void play_wall_hit();
void buffCallBack(void *);    //Declare our callback here instead of putting the whole thing here.

// Static wave config
static uint32_t wavSize = 0;
static uint16_t wavPos = 0;
static uint16_t wavSampleRate = 0;
static const uint16_t *wavSample;

/* setup */
void init() 
{
	set_screen_mode(ScreenMode::hires);
	
	screen.alpha = 255;
	screen.mask = nullptr;
	screen.pen = Pen(0, 0, 0, 0);
	screen.clear();	 

    // Instantiate game objects.
    ball = new Ball(screen_width/2-ball->LENGTH/2,
            screen_height/2-ball->LENGTH/2);
    left_paddle = new Paddle(40, screen_height/2-Paddle::HEIGHT/2);
    right_paddle = new Paddle(screen_width-(40+Paddle::WIDTH),
            screen_height/2-Paddle::HEIGHT/2);
    
    // Setup channel
	channels[0].waveforms                  = Waveform::WAVE; // Set type to WAVE
	channels[0].callback_waveBufferRefresh = &buffCallBack;  // Set callback address
    
    // Scores.
    left_score = 0;
    right_score = 0;

    // Indicates when rendering new score is necessary.
    left_score_changed = true;

    // Indicates when rendering new score is necessary.
    right_score_changed = true;
    
	ball->status = ball->READY;		
}

void play_paddle_hit()
{
	wavSample = paddle_hit_wav;     // Set sample to the array in glass.h
	wavSize = paddle_hit_wav_len;   // Set the array length to the value in glass.h
	channels[0].trigger_attack();   // Start the playback.
}

void play_score_update()
{
	wavSample = score_update_wav;
	wavSize = score_update_wav_len;
	channels[0].trigger_attack();
}

void play_wall_hit()
{
	wavSample = wall_hit_wav;
	wavSize = wall_hit_wav_len;
	channels[0].trigger_attack();
}	
	

// Called everytime audio buffer ends
void buffCallBack(void *) {

  // Copy 64 bytes to the channel audio buffer
  for (int x = 0; x < 64; x++) {
    // If current sample position is greater than the sample length, fill the rest of the buffer with zeros.
    // Note: The sample used here has an offset, so we adjust by 0x7f. 
    channels[0].wave_buffer[x] = (wavPos < wavSize) ? wavSample[wavPos] - 0x7f : 0;

    // As the engine is 22050Hz, we can timestretch to match by incrementing our sample every other step (every even 'x')
    if (wavSampleRate == 11025) {
      if (x % 2) wavPos++;
    } else {
      wavPos++;
    }
  }
  
  // For this example, clear the values
  if (wavPos >= wavSize) {
    channels[0].off();        // Stop playback of this channel.
    //Clear buffer
    wavSample = nullptr;
    wavSize = 0;
    wavPos = 0;
    wavSampleRate = 0;
  }
}


/*
	{SDLK_DOWN,   blit::Button::DPAD_DOWN},
	{SDLK_UP,     blit::Button::DPAD_UP},
	{SDLK_LEFT,   blit::Button::DPAD_LEFT},
	{SDLK_RIGHT,  blit::Button::DPAD_RIGHT},

	// wasd
	{SDLK_w,       blit::Button::DPAD_UP},
	{SDLK_a,       blit::Button::DPAD_LEFT},
	{SDLK_s,       blit::Button::DPAD_DOWN},
	{SDLK_d,       blit::Button::DPAD_RIGHT},

	// action buttons
	{SDLK_z,       blit::Button::A},
	{SDLK_x,       blit::Button::B},
	{SDLK_c,       blit::Button::X},
	{SDLK_y,       blit::Button::Y},

	// system buttons
	{SDLK_1,       blit::Button::HOME},
	{SDLK_2,       blit::Button::MENU},
	{SDLK_3,       blit::Button::JOYSTICK},
*/

// Update game values.
void update(uint32_t time_ms) {
    // Paddle movement.
	static uint16_t last_buttons = 0;
    uint16_t changed = buttons ^ last_buttons;
    uint16_t pressed = changed & buttons;
    uint16_t released = changed & ~buttons;    
    
	if((pressed & Button::A) && (ball->status == ball->READY)) {
		ball->status = ball->LAUNCH;
	}
	
	if(buttons & Button::DPAD_UP) {
		right_paddle->add_to_y(-10);
	}
	else if(buttons & Button::DPAD_DOWN) {
		right_paddle->add_to_y(10);
	}
	
    // AI paddle movement.
    left_paddle->AI(ball);

    // Launch ball.
    if (ball->status == ball->READY) {
        return;
    } else if (ball->status == ball->LAUNCH) {
        ball->launch_ball(left_paddle);
        ball->predicted_y = left_paddle->predict(ball);
    }

    // Update ball speed.
    ball->update_speed();

    // Collision.
    if (ball->collides_with(left_paddle)) {
        ball->bounces_off(left_paddle);
        // Play collision sound.
        play_paddle_hit();        
    } else if (ball->collides_with(right_paddle)) {
        ball->bounces_off(right_paddle);
        // Play collision sound.
        play_paddle_hit();
        // Predict ball position on the y-axis.
        ball->predicted_y = left_paddle->predict(ball);           
    }

    // Upper and bottom walls collision.
    if (ball->wall_collision()) {
        ball->dy *= -1;  // Reverse ball direction on y-axis.
        play_wall_hit();
    }

    // Update ball coordinates.
    ball->x += ball->dx;
    ball->y += ball->dy;

    // Ball goes out.
    if (ball->x > SCREEN_WIDTH || ball->x < 0) {
        // Change score.
        if (ball->x > SCREEN_WIDTH) {
            left_score++;
            left_score_changed = true;
        } else {
            right_score++;
            right_score_changed = true;
        }
        play_score_update();
        ball->reset();
    }
    
    last_buttons = buttons;       
}

// Render objects on screen.
void render(uint32_t time_ms) {
    uint32_t ms_start = now();

	// Clear screen (background color). Dark grey.	
	screen.pen = Pen(0, 0, 0, 255);
	screen.clear();
	
    // Paddle color. White.
	screen.pen = Pen(255,255,255,255);
	
    // Render left paddle.
    screen.rectangle(blit::Rect(left_paddle->get_x(), left_paddle->get_y(), Paddle::WIDTH, Paddle::HEIGHT));
    
    // Render right paddle.
    screen.rectangle(blit::Rect(right_paddle->get_x(), right_paddle->get_y(), Paddle::WIDTH, Paddle::HEIGHT));
    
    // Render ball
    screen.rectangle(blit::Rect(ball->x, ball->y, ball->LENGTH, ball->LENGTH));
    
    // Render left score
	screen.text(std::to_string(left_score), minimal_font, blit::Point(SCREEN_WIDTH * 4 / 10, SCREEN_HEIGHT / 12));
	left_score_changed = false;
	if (left_score == 5) {
		screen.text("Winner Player 1", minimal_font, blit::Point(SCREEN_WIDTH * 4 / 10 + 3, SCREEN_HEIGHT / 4));			
	}
	
	// Render right scores
	screen.text(std::to_string(right_score), minimal_font, blit::Point(SCREEN_WIDTH * 6 / 10, SCREEN_HEIGHT / 12));
	right_score_changed = false;
	if (right_score == 5) {
		screen.text("Winner Player 2", minimal_font, blit::Point(SCREEN_WIDTH * 4 / 10 + 3, SCREEN_HEIGHT / 4));			
	}
	
	if(ball->status != ball->READY) {
		if(left_score == 5 || right_score == 5)
		{	
			left_score = 0;
			right_score = 0;
			left_score_changed = true;
			right_score_changed = true;				
		}
	}
	
	if (ball->status == ball->READY) {
		// Draw "Press A to start".
		screen.text("A to start", minimal_font, blit::Point(SCREEN_WIDTH * 4 / 10 + 3, SCREEN_HEIGHT / 3));				
	}	
}


