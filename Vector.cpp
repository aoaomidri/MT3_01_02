#include "Vector.h"
#include<assert.h>
Vector3 Vector::Add(const Vector3& v1, const Vector3 v2) {
	Vector3 result{};
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vector3 Vector::Subtract(const Vector3& v1, const Vector3 v2) {
	Vector3 result{};
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

Vector3 Vector::Cross(const Vector3& vA, const Vector3& vB) {
	Vector3 result{};

	result = {
		(vA.y * vB.z) - (vA.z * vB.y),
		(vA.z * vB.x) - (vA.x * vB.z),
		(vA.x * vB.y) - (vA.y * vB.x)
	};

	return result;
}

const Vector3 Vector::Normalize(const Vector3& v) {
	Vector3 result{ 0, 0, 0 };
	float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));

	assert(bulletNorm != 0.0f);

	result = {
		(v.x / bulletNorm),
		(v.y / bulletNorm),
		(v.z / bulletNorm)
	};

	return result;

}

Vector3 Vector::Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return{ 0.0f,-vector.z,vector.y };

}


Vector3 Vector::Multiply(float scalar, const Vector3& Vec) {
	Vector3 result{};
	result.x = Vec.x * scalar;
	result.y = Vec.y * scalar;
	result.z = Vec.z * scalar;
	return result;
}

Vector3 Vector::Lerp(const Vector3& v1, const Vector3 v2, float t) {

	return Multiply(t, v1) + Multiply((1 - t), v2);
}

Vector3 Vector::makeCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, float t) {
	Vector3 result{};
	result= Multiply(0.5f,
		Multiply(powf(t, 3.0f), (Multiply(-1.0f, controlPoint0) + Multiply(3.0f, controlPoint1)
			+ Multiply(-3.0f, controlPoint2) + controlPoint3)) +
		Multiply(powf(t, 2.0f), (Multiply(2.0f, controlPoint0) + Multiply(-5.0f, controlPoint1)
			+ Multiply(4.0f, controlPoint2) - controlPoint3)) +
		Multiply(t, (controlPoint2 - controlPoint0)) +
		Multiply(2.0f, controlPoint1));

	return result;

}


float Vector::Length(const Vector3& v) {
	float result{ 0.0f };
	float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
	result = bulletNorm;
	return result;

}

float Vector::planeEquation(const Vector3& normal, const Vector3& vector) {
	float result{};
	result = (normal.x * vector.x) + (normal.y * vector.y) + (normal.z * vector.z);
	return result;
}

float Vector::VectorAngle(const Vector3& v1, const Vector3& v2) {
	float result = 0;
	float dot = ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
	float denominatorA = sqrtf((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z));
	float denominatorB = sqrtf((v2.x * v2.x) + (v2.y * v2.y) + (v2.z * v2.z));

	result = (dot / (denominatorA * denominatorB));


	return std::acosf(result);
}

float Vector::Dot(const Vector3& v1, const Vector3& v2) {
	float result = 0;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}
