#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
GLuint yujiTexture;

const int TILE_SIZE = 30;

// Example player position (tile-based)
int playerRow = 10;
int playerCol = 10;

// Window size
// const int WINDOW_WIDTH = 600;
// const int WINDOW_HEIGHT = 600;

std::vector<std::string> maze = {
    "#####################", "#.........#.........#", "#.###.###.#.###.###.#",
    "#o###.###.#.###.###o#", "#...................#", "#.###.#.#####.#.###.#",
    "#.....#...#...#.....#", "#####.###   ###.#####", "#####.#  GGG  #.#####",
    "#####.### # ###.#####", "#.........P.........#", "#.###.#.#####.#.###.#",
    "#o..#.#...#...#.#..o#", "###.#.###.#.###.#.###", "#.....#.......#.....#",
    "#.########.########.#", "#...................#", "#####################"};

const int ROWS = maze.size();
const int COLS = maze[0].size();
const int WINDOW_WIDTH = COLS * TILE_SIZE;
const int WINDOW_HEIGHT = ROWS * TILE_SIZE;

void loadTexture(const char *filename) {
  int width, height, channels;

  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);

  if (!data) {
    std::cout << "Failed to load image\n";
    exit(1);
  }

  glGenTextures(1, &yujiTexture);
  glBindTexture(GL_TEXTURE_2D, yujiTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_image_free(data);
}
// ----------------------------
// Draw Yuji sprite
// ----------------------------
void drawPlayer() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, yujiTexture);

  glColor3f(1.0f, 1.0f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float tileX = playerCol * TILE_SIZE;
  float tileY = (WINDOW_HEIGHT - (playerRow + 1) * TILE_SIZE);

  float spriteSize = 34.0f;
  float offset = (TILE_SIZE - spriteSize) / 2.0f;

  float x = tileX + offset;
  float y = tileY + offset;

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(x, y);
  glTexCoord2f(1, 0);
  glVertex2f(x + spriteSize, y);
  glTexCoord2f(1, 1);
  glVertex2f(x + spriteSize, y + spriteSize);
  glTexCoord2f(0, 1);
  glVertex2f(x, y + spriteSize);
  glEnd();

  glDisable(GL_TEXTURE_2D);
}
void drawFilledRect(float x, float y, float w, float h) {
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y + h);
  glVertex2f(x, y + h);
  glEnd();
}

void drawFilledCircle(float cx, float cy, float r, int segments = 30) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);
  for (int i = 0; i <= segments; i++) {
    float angle = 2.0f * 3.1415926f * i / segments;
    float x = cx + r * cos(angle);
    float y = cy + r * sin(angle);
    glVertex2f(x, y);
  }
  glEnd();
}

void drawMaze() {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      char tile = maze[row][col];

      float x = col * TILE_SIZE;
      float y = (ROWS - 1 - row) * TILE_SIZE;

      if (tile == '#') {
        glColor3f(0.286f, 0.725f, 1.0f);
        drawFilledRect(x, y, TILE_SIZE, TILE_SIZE);
      } else if (tile == '.') {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawFilledCircle(x + TILE_SIZE / 2.0f, y + TILE_SIZE / 2.0f, 3.0f);
      } else if (tile == 'o') {
        glColor3f(1.0f, 0.7f, 0.0f); // orange power pellet
        drawFilledCircle(x + TILE_SIZE / 2.0f, y + TILE_SIZE / 2.0f, 7.0f);
      } //  else if (tile == 'P') {
      //   glColor3f(1.0f, 1.0f, 0.0f); // Pac-Man start
      // drawFilledCircle(x + TILE_SIZE / 2.0f, y + TILE_SIZE / 2.0f,
      // TILE_SIZE / 2.5f);
      // }
      else if (tile == 'G') {
        glColor3f(1.0f, 0.0f, 0.0f); // ghost start
        drawFilledCircle(x + TILE_SIZE / 2.0f, y + TILE_SIZE / 2.0f,
                         TILE_SIZE / 2.8f);
      }
    }
  }
}

// checking if walkable
bool isWalkable(int row, int col) {
  if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    return false;

  return maze[row][col] != '#';
}

// for inputs
void handleSpecialKeys(int key, int x, int y) {
  int newRow = playerRow;
  int newCol = playerCol;

  if (key == GLUT_KEY_UP) {
    newRow--;
  } else if (key == GLUT_KEY_DOWN) {
    newRow++;
  } else if (key == GLUT_KEY_LEFT) {
    newCol--;
  } else if (key == GLUT_KEY_RIGHT) {
    newCol++;
  }

  if (isWalkable(newRow, newCol)) {
    playerRow = newRow;
    playerCol = newCol;
  }

  glutPostRedisplay();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0f, 0.0f, 0.0f);
  drawFilledRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  drawMaze();
  drawPlayer();

  glutSwapBuffers();
}
void init() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  loadTexture("yuji.png");
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Pac-Man Maze");

  init();

  glutDisplayFunc(display);
  glutSpecialFunc(handleSpecialKeys);
  glutMainLoop();
  return 0;
}
