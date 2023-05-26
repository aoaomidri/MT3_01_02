#pragma once
#include"Matrix.h"


struct Sphere{
	Vector3 center;
	float radius;
};

struct Line {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

struct Ray {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

struct Segment {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

