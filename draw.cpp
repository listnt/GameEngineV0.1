//
// Created by Aidar Kudabaev on 23.10.2024.
//

#include "draw.h"

std::vector<std::tuple<int, double>> Interpolate(int i0, int d0, int i1, int d1,
                                                 double z0, double z1) {
  std::vector<std::tuple<int, double>> values;
  if (i0 == i1) {
    if (z0 < z1) {
      values.push_back({d0, 1 / z0});
    } else {
      values.push_back({d0, 1 / z1});
    }
    return values;
  }

  double a = double(d1 - d0) / double(i1 - i0);
  double d = d0;

  for (int i = i0; i <= i1; i++) {
    values.push_back(
        {int(d), 1 / z0 + (i - i0) * (1 / z1 - 1 / z0) / (i1 - i0)});
    d = d + a;
  }

  return values;
}

void DrawLine(Canvas canvas, Point2i p0, Point2i p1, Vector3f color, double z0,
              double z1) {
  if (abs(p1.x - p0.x) > abs(p1.y - p0.y)) {
    if (p0.x > p1.x) {
      std::swap(p0, p1);
      std::swap(z0, z1);
    }

    auto ys = Interpolate(p0.x, p0.y, p1.x, p1.y, z0, z1);

    for (int i = p0.x; i <= p1.x; i++) {
      auto index = std::get<0>(ys[i - p0.x]) * canvas.canvas_height + i;
      auto z = 1/std::get<1>(ys[i - p0.x]);
      if (canvas.zbuffer[index].z > z) {
        canvas.zbuffer[index] = ZbufferRecord(z, color);
      }
    }
  } else {
    if (p0.y > p1.y) {
      std::swap(p0, p1);
      std::swap(z0, z1);
    }

    auto xs = Interpolate(p0.y, p0.x, p1.y, p1.x, z0, z1);
    for (int i = p0.y; i <= p1.y; i++) {
      auto index = std::get<0>(xs[i - p0.y]) + i * canvas.canvas_height;
      auto z = 1/std::get<1>(xs[i - p0.y]);

      if (canvas.zbuffer[index].z > z) {
        canvas.zbuffer[index] = ZbufferRecord(z, color);
      }
    }
  }
}

void DrawTriangle(Canvas canvas, Point2i p0, Point2i p1, Point2i p2,
                  Vector3f color, double z0, double z1, double z2) {
  DrawLine(canvas, p0, p1, color, z0, z1);
  DrawLine(canvas, p1, p2, color, z1, z2);
  DrawLine(canvas, p2, p0, color, z2, z0);
}

void DrawFilledTriangle(Canvas canvas, Point2i p0, Point2i p1, Point2i p2,
                        Vector3f color, double z0, double z1, double z2) {
  int tmp;
  if (p1.y < p0.y) {
    std::swap(p1, p0);
    std::swap(z1, z0);
  }
  if (p2.y < p0.y) {
    std::swap(p2, p0);
    std::swap(z2, z0);
  }
  if (p2.y < p1.y) {
    std::swap(p2, p1);
    std::swap(z2, z1);
  }

  auto x01 = Interpolate(p0.y, p0.x, p1.y, p1.x, z0, z1);
  auto x12 = Interpolate(p1.y, p1.x, p2.y, p2.x, z1, z2);
  auto x02 = Interpolate(p0.y, p0.x, p2.y, p2.x, z0, z2);

  x01.pop_back();

  x01.insert(x01.end(), x12.begin(), x12.end());

  std::vector<std::tuple<int, double>> *x_left;
  std::vector<std::tuple<int, double>> *x_right;

  int m = x01.size() / 2;

  if (std::get<0>(x02[m]) < std::get<0>(x01[m])) {
    x_left = &x02;
    x_right = &x01;
  } else {
    x_left = &x01;
    x_right = &x02;
  }

  for (int y = p0.y; y <= p2.y; y++) {
    auto x0 = std::get<0>((*x_left)[y - p0.y]);
    auto x1 = std::get<0>((*x_right)[y - p0.y]);
    auto z0 = 1/std::get<1>((*x_left)[y - p0.y]);
    auto z1 = 1/std::get<1>((*x_right)[y - p0.y]);

    if (x1 == x0) {
      if (canvas.zbuffer[x0 + y * canvas.canvas_height].z > std::min(z0, z1)) {
        canvas.zbuffer[x0 + y * canvas.canvas_height] =
            ZbufferRecord(std::min(z0, z1), color);
      }

      continue;
    }

    for (int x = x0; x <= x1; x++) {

      auto z = z0 + (z1 - z0) * (x - x0) / (x1 - x0);

      auto index = x + y * canvas.canvas_height;
      if (canvas.zbuffer[index].z > z) {
        canvas.zbuffer[x + y * canvas.canvas_height] = ZbufferRecord(z, color);
      }
    }
  }
}

Vector3f SpaceToViewPort(Vector3f point, double d) {
  Vector3f res;

  res.x = (point.x * d) / point.z;

  res.y = (point.y * d) / point.z;

  res.z = d;

  return res;
}

Point2i ViewportToScreen(Canvas canvas, Vector3f point, double Vh, double Vw) {
  return {
      int(((point.x * canvas.canvas_width)) / Vw + canvas.canvas_width / 2),
      int(((point.y * canvas.canvas_height)) / Vh + canvas.canvas_height / 2)};
}

void DrawCube(Canvas canvas, double viewportD, double Vh, double Vw,
              std::vector<Vector3f> points) {
  if (points.size() != 8) {
    return;
  }

  Point2i p0, p1, p2, p3, p4, p5, p6, p7;

  p0 = ViewportToScreen(canvas, SpaceToViewPort(points[0], viewportD), Vh, Vw);
  p1 = ViewportToScreen(canvas, SpaceToViewPort(points[1], viewportD), Vh, Vw);
  p2 = ViewportToScreen(canvas, SpaceToViewPort(points[2], viewportD), Vh, Vw);
  p3 = ViewportToScreen(canvas, SpaceToViewPort(points[3], viewportD), Vh, Vw);
  p4 = ViewportToScreen(canvas, SpaceToViewPort(points[4], viewportD), Vh, Vw);
  p5 = ViewportToScreen(canvas, SpaceToViewPort(points[5], viewportD), Vh, Vw);
  p6 = ViewportToScreen(canvas, SpaceToViewPort(points[6], viewportD), Vh, Vw);
  p7 = ViewportToScreen(canvas, SpaceToViewPort(points[7], viewportD), Vh, Vw);

  DrawLine(canvas, p0, p1, {1, 0, 0}, points[0].z, points[1].z);
  DrawLine(canvas, p1, p2, {1, 0, 0}, points[1].z, points[2].z);
  DrawLine(canvas, p2, p3, {1, 0, 0}, points[2].z, points[3].z);
  DrawLine(canvas, p3, p0, {1, 0, 0}, points[3].z, points[0].z);

  DrawLine(canvas, p4, p5, {0, 1, 0}, points[4].z, points[5].z);
  DrawLine(canvas, p5, p6, {0, 1, 0}, points[5].z, points[6].z);
  DrawLine(canvas, p6, p7, {0, 1, 0}, points[6].z, points[7].z);
  DrawLine(canvas, p7, p4, {0, 1, 0}, points[7].z, points[4].z);

  DrawLine(canvas, p0, p4, {0, 0, 1}, points[0].z, points[4].z);
  DrawLine(canvas, p1, p5, {0, 0, 1}, points[1].z, points[5].z);
  DrawLine(canvas, p2, p6, {0, 0, 1}, points[2].z, points[6].z);
  DrawLine(canvas, p3, p7, {0, 0, 1}, points[3].z, points[7].z);
}