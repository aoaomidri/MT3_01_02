#pragma once
#include"Matrix.h"



struct Sphere{
	Vector3 center;
	float radius;
	uint32_t color;
};

struct Plane {
	Vector3 normal;//法線
	float distance;//距離
};
//直線
struct Line {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};
//半直線
struct Ray {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};
//線分
struct Segment {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分ベクトル
};

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
//球と球があたったら
bool IsCollision(const Sphere& s1, const Sphere& s2);
//弾と平面が当たったら
bool IsCollision(const Sphere& sphere, const Plane& plane);
//線分と平面の当たり判定
bool IsCollision(const Segment& segment, const Plane& plane);

Vector3 TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

