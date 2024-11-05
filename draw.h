//
// Created by Aidar Kudabaev on 23.10.2024.
//

#ifndef RASTERIZER_DRAW_H
#define RASTERIZER_DRAW_H

#include "base.h"
#include <algorithm>
#include <utility>
#include <vector>

std::vector<std::tuple<int, double>> Interpolate(int i0, int d0, int i1, int d1,
                                                 double z0, double z1);
void DrawLine(Canvas canvas, Point2i p0, Point2i p1, Vector3f color, double z0,
              double z1);
void DrawTriangle(Canvas canvas, Point2i p0, Point2i p1, Point2i p2,
                  Vector3f color, double z0, double z1, double z2);
void DrawFilledTriangle(Canvas canvas, Point2i p0, Point2i p1, Point2i p2,
                        Vector3f color, double z0, double z1, double z2);
void DrawCube(Canvas canvas, double viewportD, double Vh, double Vw,
              std::vector<Vector3f> points);

Vector3f SpaceToViewPort(Vector3f point, double d);
Point2i ViewportToScreen(Canvas canvas, Vector3f point, double Vh, double Vw);

#endif // RASTERIZER_DRAW_H
