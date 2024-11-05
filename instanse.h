//
// Created by Aidar Kudabaev on 23.10.2024.
//

#ifndef RASTERIZER_INSTANSE_H
#define RASTERIZER_INSTANSE_H

#include "base.h"
#include "draw.h"

#include <algorithm>
#include <iostream>
#include <math.h>
#include <vector>

class Camera {
public:
  Vector3f position;
  Matrix4x4 rotation;

  double near = 1;
  double viewport_height = 2;
  double viewport_width = 2;

  Camera() {
    this->position = Vector3f();
    this->rotation = Matrix4x4();
  }

  Camera(Vector3f position, Matrix4x4 rotation) {
    this->position = position;
    this->rotation = rotation;
  }
};

class Triangle {
public:
  int v0, v1, v2;
  Vector3f color;
};

class model {
protected:
  std::vector<Vector3f> points;
  std::vector<Triangle> triangles;

  void RenderTriangle(Canvas &canvas, Camera &camera, Triangle triangle);

public:
  model(std::vector<Vector3f> points, std::vector<Triangle> triangles)
      : points(points), triangles(triangles){};

  void Optimize(Vector3f plane_normal, double D);
  void Render(Canvas &canvas, Camera &camera);

  friend class instanse;
};

class instanse {
protected:
  model *obj;
  Vector3f T, S, R;

public:
  instanse(model *obj) : obj(obj), T(0, 0, 0), S(1, 1, 1), R(0, 0, 0){};

  void Render(Canvas &canvas, Camera &camera);

  model *ClipModelAgainstPlanes(Canvas &canvas, Camera &camera, model &model,
                                Vector3f mass_center, double render_sphere);

  void Transform(Vector3f T);

  void Rotate(Vector3f R);

  void Scale(Vector3f S);
};

Matrix4x4 scale(double x, double y, double z);

Matrix4x4 roll(double angle);

Matrix4x4 pitch(double angle);

Matrix4x4 yaw(double angle);

Matrix4x4 translate(double x, double y, double z);

#endif // RASTERIZER_INSTANSE_H
