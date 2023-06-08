#include "Vector.h"
#include<assert.h>

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

float Vector::Length(const Vector3& v) {
	float result{ 0.0f };
	float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
	result = bulletNorm;
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
