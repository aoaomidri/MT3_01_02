﻿#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include <Novice.h>
#include<stdint.h>
#include<iostream>

class Vector
{
private:
	float x, y, z;
	Vector3 vec;

public:
	//足し算
	Vector3 Add(const Vector3& v1, const Vector3 v2);
	//引き算
	Vector3 Subtract(const Vector3& v1, const Vector3 v2);

	Vector3 Cross(const Vector3& vA, const Vector3& vB);

	Vector3 Perpendicular(const Vector3& vector);
	//ラープ
	Vector3 Lerp(const Vector3& v1, const Vector3 v2, float t);

	Vector3 makeCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, float t);
	
	// スカラー倍
	Vector3 Multiply(float scalar, const Vector3& Vec);

	//正規化
	const Vector3 Normalize(const Vector3& v);
	//長さだけ
	float Length(const Vector3& v);
	//平面方程式
	float planeEquation(const Vector3& normal, const Vector3& vector);

	float VectorAngle(const Vector3& v1, const Vector3& v2);

	float Dot(const Vector3& v1, const Vector3& v2);
};

