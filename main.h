#pragma once
#include"Matrix.h"


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

struct Sphere{
	Vector3 center;
	float radius;
};

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);