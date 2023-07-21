﻿#pragma once
#include"Matrix.h"
#include<memory>
#include<math.h>


struct Sphere{
	Vector3 center;
	float radius;
	uint32_t color;
};

struct Plane {
	Vector3 normal;//法線
	float distance;//距離
	uint32_t color;
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

//三角形
struct Triangle {
	Vector3 vertices[3];
	uint32_t color;
};

//AABB
struct AABB {
	Vector3 min;//最小点
	Vector3 max;//最大点
	uint32_t color;
};

//OBB
struct OBB{
	Vector3 center;//中心点
	Vector3 orientations[3];//座標軸。正規化・直行必須
	Vector3 size;//座標軸の長さの半分。中心から面までの距離
	uint32_t color;
};

struct Spring {
	//アンカー。固定された端の位置
	Vector3 anchor;
	float naturalLength;//自然長
	float stiffness;//剛性。ばね定数x
	float dampingCoefficient;//減衰係数
};

struct Ball {
	Vector3 position;//ボールの位置
	Vector3 velocity;//ボールの速度
	Vector3 acceleration;//ボールの加速度
	float mass;//ボールの質量
	float radius;//ボールの半径
	unsigned int color;//ボールの色
};

struct Pendulum {
	Vector3 anchor;//アンカーポイント。固定された端の位置
	float length;  //紐の長さ
	float angle;   //現在の角度
	float angularVelocity;	  //角速度
	float angularAcceleration;//角加速度
};

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

//球と球の当たり判定
bool IsCollision(const Sphere& s1, const Sphere& s2);
//弾と平面の当たり判定
bool IsCollision(const Sphere& sphere, const Plane& plane);
//線分と平面の当たり判定
bool IsCollision(const Segment& segment, const Plane& plane);
//三角形の平面と線分の当たり判定
bool IsCollision(const Triangle& triangle, const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
//AABB同士の当たり判定
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
//AABBと球の当たり判定
bool IsCollision(const Sphere& sphere, const AABB& aabb);
//AABBと線分の当たり判定
bool IsCollision(const Segment& segment, const AABB& aabb);

Vector3 TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

