#define GL_SILENCE_DEPRECATION

#include "base.h"
#include "draw.h"
#include "instanse.h"
#include <GLUT/glut.h>
#include <chrono>
#include <iostream>
#include <sstream>

using namespace std;
using namespace std::chrono;

double pixel_size = 0.03;
const double canvasZ = 0.5;
const int canvas_width = 500, canvas_height = 500;
Vector3f camera_position = Vector3f(0, 0, 0);

vector<Vector3f> cubePoints = {{-1, 1, 4}, {1, 1, 4}, {1, -1, 4}, {-1, -1, 4},
                               {-1, 1, 5}, {1, 1, 5}, {1, -1, 5}, {-1, -1, 5}};
vector<Triangle> cubeTriangles = {
    {0, 1, 2, {1, 0, 0}}, {0, 2, 3, {1, 0, 0}}, {4, 0, 3, {0, 1, 0}},
    {4, 3, 7, {0, 1, 0}}, {5, 4, 7, {0, 0, 1}}, {5, 7, 6, {0, 0, 1}},
    {1, 5, 6, {1, 1, 0}}, {1, 6, 2, {1, 1, 0}}, {4, 5, 1, {1, 0, 1}},
    {4, 1, 0, {1, 0, 1}}, {2, 6, 7, {0, 1, 1}}, {2, 7, 3, {0, 1, 1}}};

instanse *cube = new instanse(new model(cubePoints, cubeTriangles));
instanse *cube2 = new instanse(new model(cubePoints, cubeTriangles));

Camera camera = Camera();

void DrawSystem() {
  glBegin(GL_LINES);
  glVertex3f(-100.0, 0.0, 0.0);
  glVertex3f(100.0, 0.0, 0.0);
  glVertex3f(0.0, 100.0, 0.0);
  glVertex3f(0.0, -100.0, 0.0);
  for (int i = -100; i < 101; i++) {
    glVertex3f(-0.1, i, 0);
    glVertex3f(0.1, i, 0);
    glVertex3f(i, -0.1, 0);
    glVertex3f(i, 0.1, 0);
  }
  glEnd();
}

double orX = 10, orY = 10, trX = 0, trY = 0;

void Initialize() {
  glClearColor(1.0, 1.0, 1.0, 1.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-orX, orX, -orY, orY, -10, 10);
}

void processSpecialKeys(int key, int x, int y) {
  if (key == GLUT_KEY_F1)
    exit(1);
  if (key == GLUT_KEY_LEFT) {
    // trX -= 1 * orX / 10;
    // glLoadIdentity();
    // glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);

    camera.position.x -= 1;
  }
  if (key == GLUT_KEY_RIGHT) {
    // trX += 1 * orX / 10;
    // glLoadIdentity();
    // glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);

    camera.position.x += 1;
  }
  if (key == GLUT_KEY_UP) {
    // trY += 1 * orY / 10;
    // glLoadIdentity();
    // glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);

    camera.position.y += 1;
  }
  if (key == GLUT_KEY_DOWN) {
    // trY -= 1 * orY / 10;
    // glLoadIdentity();
    // glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);

    camera.position.y -= 1;
  }
  if (key == GLUT_KEY_F12) {
    // trY = 0;
    // trX = 0;
    // orX = 10;
    // orY = 10;
    // glLoadIdentity();
    // glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);

    camera.rotation = camera.rotation * roll(180 / M_PI * 90);
  }

  if (key == GLUT_KEY_F11) {
    // trY = 0;
    // trX = 0;
    // orX = 10;
    // orY = 10;
    // glLoadIdentity();
    // glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);

    camera.rotation = camera.rotation * roll(180 / M_PI * (-90));
  }
}

void processNormalKeys(unsigned char key, int x, int y) {
  switch (key) {
  case ('+'):
    orX /= 1.5;
    orY /= 1.5;
    cout << orX << orY << endl;
    glLoadIdentity();
    glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);
    break;
  case ('-'):
    orX *= 1.5;
    orY *= 1.5;
    cout << orX << orY << endl;
    glLoadIdentity();
    glOrtho(trX - orX, trX + orX, trY - orY, trY + orY, -10, 10);
    break;
  case ('w'):
    cube->Transform({0, 0, 1});
    break;
  case ('s'):
    cube->Transform({0, 0, -1});
    break;
  case ('a'):
    cube->Transform({-1, 0, 0});
    break;
  case ('d'):
    cube->Transform({1, 0, 0});
    break;
  case ('q'):
    cube->Rotate({0, 0.2, 0});
    break;
  case ('e'):
    cube->Rotate({0, -0.2, 0});
    break;
  }
}

int frame = 0;
int lastFrame = 0;

auto begT = high_resolution_clock::now();
auto endT = high_resolution_clock::now();

Canvas canvas = Canvas(canvas_width, canvas_height);

// Основная фунция отрисовки.Выполняется каждый кадр
void Draw() {
  frame++;
  endT = high_resolution_clock::now();

  if (duration_cast<milliseconds>(endT - begT).count() > 1000) {
    begT = high_resolution_clock::now();
    cout << "fps:" << frame - lastFrame << endl;
    lastFrame = frame;
  }

  // очищаем сцену
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 0.0, 0.0);
  DrawSystem();

  //   cube->Rotate({0, 0.1, 0});
  cube->Render(canvas, camera);
  cube2->Render(canvas, camera);

  glColor3f(0.5, 0.5, 0.5);
  glBegin(GL_QUADS);
  for (int i = 0; i < canvas_height; i++) {
    for (int j = 0; j < canvas_width; j++) {
      glColor3f(canvas.zbuffer[j + i * canvas_height].color.x,
                canvas.zbuffer[j + i * canvas_height].color.y,
                canvas.zbuffer[j + i * canvas_height].color.z);
      glVertex3f(j * pixel_size - canvas_width * pixel_size / 2,
                 i * pixel_size - canvas_height * pixel_size / 2, 5);
      glVertex3f(j * pixel_size - canvas_width * pixel_size / 2,
                 (i - 1) * pixel_size - canvas_height * pixel_size / 2, 5);
      glVertex3f((j + 1) * pixel_size - canvas_width * pixel_size / 2,
                 (i - 1) * pixel_size - canvas_height * pixel_size / 2, 5);
      glVertex3f((j + 1) * pixel_size - canvas_width * pixel_size / 2,
                 i * pixel_size - canvas_height * pixel_size / 2, 5);

      canvas.zbuffer[j + i * canvas_height] =
          ZbufferRecord(999999, Vector3f(1, 1, 1));
    }
  }
  glEnd();

  glutSwapBuffers();
  glFlush();
}

int main(int argc, char **argv) {
  setlocale(LC_ALL, "Russian");
  cout.precision(6);

  cube2->Transform({2, 2, 2});

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(800, 800);     // Указываем размер окна
  glutInitWindowPosition(100, 100); // Позиция окна
  glutCreateWindow("Graphs");       // Имя окна
  Initialize();                     // Вызов функции Initialize

  glutDisplayFunc(Draw); // Вызов функции отрисовки
  glutIdleFunc(Draw);
  // Функция обработки нормальных клавиш, т.е. клавиш у которых есть ASCII кода.
  // Например: "ц" , "а"  и др. Определенны как 'char'
  glutKeyboardFunc(processNormalKeys);
  // Функция обработки специальных клавиш, т.е. клавиш у которых нет ASCII кода.
  // Например:стрелки, Esc, F1,F2 и др. Определенны в Константе GLUT_KEY...
  glutSpecialFunc(processSpecialKeys);

  /*glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  */
  glutMainLoop(); // Основной цикл

  return 0;
}