#pragma once
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

	Vector3 Cross(const Vector3& vA, const Vector3& vB);

	//正規化
	const Vector3 Normalize(const Vector3& v);
	//長さだけ
	float Length(const Vector3& v);


};

