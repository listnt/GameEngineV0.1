//
// Created by Aidar Kudabaev on 23.10.2024.
//

#ifndef RASTERIZER_BASE_H
#define RASTERIZER_BASE_H

#include "math.h"
#include <algorithm>
#include <vector>

class Vector3f {
public:
    double x, y, z;

    Vector3f() : x(0), y(0), z(0) {}

    Vector3f(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3f operator-(Vector3f v) const {
        return {this->x - v.x, this->y - v.y, this->z - v.z};
    }

    Vector3f operator+(Vector3f v) const {
        return {this->x + v.x, this->y + v.y, this->z + v.z};
    }

    Vector3f operator*(double v) const {
        return {this->x * v, this->y * v, this->z * v};
    }

    Vector3f operator/(double v) const {
        return {this->x / v, this->y / v, this->z / v};
    }

    double dot(Vector3f v) const {
        return this->x * v.x + this->y * v.y + this->z * v.z;
    }

    double length() {
        return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    void normalize() {
        double length = this->length();
        this->x /= length;
        this->y /= length;
        this->z /= length;
    }
};

class Point2i {
public:
    int x, y;

    Point2i() : x(0), y(0) {}

    Point2i(int x, int y) : x(x), y(y) {}

    Point2i operator+(Point2i p) const { return {this->x + p.x, this->y + p.y}; };

    Point2i operator-(Point2i p) const { return {this->x - p.x, this->y - p.y}; };
};

class Point3i {
public:
    int x, y, z;

    Point3i() : x(0), y(0), z(0) {}

    Point3i(int x, int y, int z) : x(x), y(y), z(z) {}

    Point3i operator+(Point3i p) const {
        return {this->x + p.x, this->y + p.y, this->z + p.z};
    };

    Point3i operator-(Point3i p) const {
        return {this->x - p.x, this->y - p.y, this->z - p.z};
    };
};

class ZbufferRecord {
public:
    double z;
    Vector3f color;

    ZbufferRecord() {
        this->z = 0;
        this->color = Vector3f();
    }

    ZbufferRecord(double z, Vector3f color) {
        this->z = z;
        this->color = color;
    }
};

class Canvas {
public:
    ZbufferRecord *zbuffer;
    int canvas_width;
    int canvas_height;

    Canvas(int canvas_width, int canvas_height) {
        this->canvas_width = canvas_width;
        this->canvas_height = canvas_height;

        zbuffer = new ZbufferRecord[canvas_width * canvas_height];
        for (int i = 0; i < canvas_height; i++) {
            for (int j = 0; j < canvas_width; j++) {
                zbuffer[i * canvas_width + j] =
                        ZbufferRecord(99999.0, Vector3f(0.0, 0.0, 0.0));
            }
        }
    }
};

class Matrix4x4 {
public:
    double **m;

    Matrix4x4() {
        m = new double *[4];
        for (int i = 0; i < 4; i++) {
            m[i] = new double[4];
            for (int j = 0; j < 4; j++) {
                m[i][j] = 0;
            }
        }

        m[0][0] = 1;
        m[1][1] = 1;
        m[2][2] = 1;
        m[3][3] = 1;
    }

    Vector3f operator*(Vector3f v) const {
        double x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3];
        double y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3];
        double z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3];
        // double w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + m[3][3];
        double w = 1;
        return Vector3f(x / w, y / w, z / w);
    }

    Matrix4x4 operator*(Matrix4x4 m2) const {
        Matrix4x4 res;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                res.m[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    res.m[i][j] += m[i][k] * m2.m[k][j];
                }
            }
        }

        return res;
    }
};

Matrix4x4 inverse(Matrix4x4 m);

std::vector<double> flatten(Matrix4x4 m);

#endif // RASTERIZER_BASE_H
