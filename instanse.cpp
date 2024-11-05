//
// Created by Aidar Kudabaev on 23.10.2024.
//

#include "instanse.h"

void model::Render(Canvas &canvas, Camera &camera) {
  for (auto &triangle : triangles) {
    RenderTriangle(canvas, camera, triangle);
  }
}

void model::RenderTriangle(Canvas &canvas, Camera &camera, Triangle triangle) {
  auto vp0 = SpaceToViewPort(points[triangle.v0], camera.near);
  auto p0 = ViewportToScreen(canvas, vp0, camera.viewport_height,
                             camera.viewport_width);

  auto vp1 = SpaceToViewPort(points[triangle.v1], camera.near);
  auto p1 = ViewportToScreen(canvas, vp1, camera.viewport_height,
                             camera.viewport_width);

  auto vp2 = SpaceToViewPort(points[triangle.v2], camera.near);
  auto p2 = ViewportToScreen(canvas, vp2, camera.viewport_height,
                             camera.viewport_width);

  DrawFilledTriangle(canvas, p0, p1, p2, triangle.color, points[triangle.v0].z,
                     points[triangle.v1].z, points[triangle.v2].z);
}

double signedDistance(Vector3f point, Vector3f plane_normal, double D) {
  return plane_normal.dot(point) + D;
}

Vector3f Intersection(Vector3f start_point, Vector3f direction,
                      Vector3f plane_normal, double D) {
  double t = (-D - plane_normal.dot(start_point)) / plane_normal.dot(direction);

  return start_point + direction * t;
}

void model::Optimize(Vector3f plane_normal, double D) {
  for (int i = 0; i < triangles.size(); i++) {
    auto d0 = signedDistance(points[triangles[i].v0], plane_normal, D);
    auto d1 = signedDistance(points[triangles[i].v1], plane_normal, D);
    auto d2 = signedDistance(points[triangles[i].v2], plane_normal, D);

    int mask = (d0 > 0) << 2 | (d1 > 0) << 1 | (d2 > 0);

    if (mask == 0b111) {
      continue;
    } else if (mask == 0b000) {
      triangles.erase(triangles.begin() + i);
      i--;
    } else if (mask == 0b001 || mask == 0b010 || mask == 0b100) {
      int A, B, C;
      if (d0 > 0) {
        A = triangles[i].v0;
        B = triangles[i].v1;
        C = triangles[i].v2;
      } else if (d1 > 0) {
        A = triangles[i].v1;
        B = triangles[i].v2;
        C = triangles[i].v0;
      } else if (d2 > 0) {
        A = triangles[i].v2;
        B = triangles[i].v0;
        C = triangles[i].v1;
      }

      auto B1 = Intersection(points[A], points[B] - points[A], plane_normal, D);
      auto C1 = Intersection(points[A], points[C] - points[A], plane_normal, D);

      points.push_back(B1);
      points.push_back(C1);

      triangles[i].v0 = A;
      triangles[i].v1 = points.size() - 2;
      triangles[i].v2 = points.size() - 1;
    } else if (mask == 0b011 || mask == 0b101 || mask == 0b110) {
      int A, B, C;

      if (d0 < 0) {
        C = triangles[i].v0;
        A = triangles[i].v1;
        B = triangles[i].v2;
      } else if (d1 < 0) {
        C = triangles[i].v1;
        A = triangles[i].v2;
        B = triangles[i].v0;
      } else if (d2 < 0) {
        C = triangles[i].v2;
        A = triangles[i].v0;
        B = triangles[i].v1;
      } else {
        continue;
      }

      auto A1 = Intersection(points[A], points[C] - points[A], plane_normal, D);
      auto B1 = Intersection(points[B], points[C] - points[B], plane_normal, D);

      points.push_back(A1);
      points.push_back(B1);

      triangles[i].v0 = A;
      triangles[i].v1 = B;
      triangles[i].v2 = points.size() - 2;

      triangles.push_back(Triangle{B, int(points.size()) - 1,
                                   int(points.size()) - 2, triangles[i].color});
    }
  }
  return;
}

void printDebug(Matrix4x4 instanse_scale, Matrix4x4 isntance_rotate,
                Matrix4x4 instanse_translate) {
  auto flat_scale = flatten(instanse_scale);
  auto flat_rotate = flatten(isntance_rotate);
  auto flat_translate = flatten(instanse_translate);

  std::cout << "Scale: ";
  for (int i = 0; i < 16; i++) {
    std::cout << flat_scale[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Rotate: ";
  for (int i = 0; i < 16; i++) {
    std::cout << flat_rotate[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Translate: ";
  for (int i = 0; i < 16; i++) {
    std::cout << flat_translate[i] << " ";
  }
  std::cout << std::endl;
}

void instanse::Render(Canvas &canvas, Camera &camera) {
  bool debug = true;

  auto local_model = *obj;

  auto instanse_scale = scale(S.x, S.y, S.z);
  auto isntance_rotate = roll(R.z) * pitch(R.y) * yaw(R.x);
  auto instanse_translate = translate(T.x, T.y, T.z);

  if (debug) {
    // printDebug(instanse_scale, isntance_rotate, instanse_translate);
  }

  auto instanse_matrix = instanse_scale * isntance_rotate * instanse_translate;

  Matrix4x4 camera_matrix =
      translate(camera.position.x, camera.position.y, camera.position.z) *
      inverse(camera.rotation);

  Vector3f mass_center;
  double render_sphere = 0.1f;

  for (auto &point : local_model.points) {
    point = camera_matrix * (instanse_matrix * point);

    mass_center = mass_center + point;
  }
  mass_center = mass_center / local_model.points.size();

  for (auto &point : local_model.points) {
    if ((point - mass_center).length() > render_sphere) {
      render_sphere = (point - mass_center).length();
    }
  }

  model *optimized_model = this->ClipModelAgainstPlanes(
      canvas, camera, local_model, mass_center, render_sphere);
  if (optimized_model == nullptr) {
    return;
  }

  optimized_model->Render(canvas, camera);
}

model *instanse::ClipModelAgainstPlanes(Canvas &canvas, Camera &camera,
                                        model &local_model,
                                        Vector3f mass_center,
                                        double render_sphere_radius) {
  std::vector<std::tuple<Vector3f, double>> planes = {
      {Vector3f(1 / sqrt(2), 0, 1 / sqrt(2)), 0},  // Left plane
      {Vector3f(-1 / sqrt(2), 0, 1 / sqrt(2)), 0}, // Right plane
      {Vector3f(0, 1 / sqrt(2), 1 / sqrt(2)), 0},  // Bottom plane
      {Vector3f(0, -1 / sqrt(2), 1 / sqrt(2)), 0}, // Top plane
      {Vector3f(0, 0, 1), -camera.near},            // Near plane
  };

  for (auto &plane : planes) {
    auto d = std::get<0>(plane).dot(mass_center);
    if (d < -render_sphere_radius) {
      return nullptr;
    }

    if (abs(d) < render_sphere_radius) {
      local_model.Optimize(std::get<0>(plane), std::get<1>(plane));
    }
  }

  return &local_model;
}

void instanse::Transform(Vector3f T) { this->T = this->T + T; }

void instanse::Rotate(Vector3f R) { this->R = this->R + R; }

void instanse::Scale(Vector3f S) { this->S = this->S + S; }

Matrix4x4 scale(double x, double y, double z) {
  Matrix4x4 S = Matrix4x4();
  S.m[0][0] = x;
  S.m[1][1] = y;
  S.m[2][2] = z;
  S.m[3][3] = 1;

  return S;
}

Matrix4x4 roll(double angle) {
  Matrix4x4 R = Matrix4x4();
  R.m[0][0] = 1;
  R.m[1][1] = cos(angle);
  R.m[1][2] = -sin(angle);
  R.m[2][1] = sin(angle);
  R.m[2][2] = cos(angle);
  R.m[3][3] = 1;

  return R;
}

Matrix4x4 pitch(double angle) {
  Matrix4x4 R = Matrix4x4();
  R.m[0][0] = cos(angle);
  R.m[0][2] = sin(angle);
  R.m[1][1] = 1;
  R.m[2][0] = -sin(angle);
  R.m[2][2] = cos(angle);
  R.m[3][3] = 1;

  return R;
}

Matrix4x4 yaw(double angle) {
  Matrix4x4 R = Matrix4x4();
  R.m[0][0] = cos(angle);
  R.m[0][1] = -sin(angle);
  R.m[1][0] = sin(angle);
  R.m[1][1] = cos(angle);
  R.m[2][2] = 1;
  R.m[3][3] = 1;

  return R;
}

Matrix4x4 translate(double x, double y, double z) {
  Matrix4x4 T = Matrix4x4();
  T.m[0][3] = x;
  T.m[1][3] = y;
  T.m[2][3] = z;
  T.m[0][0] = 1;
  T.m[1][1] = 1;
  T.m[2][2] = 1;
  T.m[3][3] = 1;

  return T;
}
