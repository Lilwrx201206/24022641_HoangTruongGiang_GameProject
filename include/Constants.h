#ifndef CONSTANTS_H
#define CONSTANTS_H

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 256;
const int PLAYER_WIDTH = 24;
const int PLAYER_HEIGHT = 20;
const int OBSTACLE_WIDTH = 24;
const int OBSTACLE_HEIGHT = 20;
const int GRAVITY = 1;
const int JUMP_STRENGTH = -15;

const float SPEED_INCREMENT = 1.05f;
const int OBSTACLES_TO_INCREMENT = 10;

const int PLAYER_FRAME_WIDTH = 24;
const int PLAYER_FRAME_HEIGHT = 20;
const int PLAYER_FRAME_COUNT = 6;
const int PLAYER_ANIMATION_FPS = 20;

const int PLAYER_JUMP_FRAME_WIDTH = 24;
const int PLAYER_JUMP_FRAME_HEIGHT = 24;
const int PLAYER_JUMP_FRAME_COUNT = 5;
const int PLAYER_JUMP_ANIMATION_FPS = 10;

const int OBSTACLE_FRAME_WIDTH = 32;
const int OBSTACLE_FRAME_HEIGHT = 24;
const int OBSTACLE_FRAME_COUNT = 6;
const int OBSTACLE_ANIMATION_FPS = 10;

const int MAX_HIGHSCORES = 5;
const int MAP_WIDTH = 448;
const int MAP_HEIGHT = 256;
#endif