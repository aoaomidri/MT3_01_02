#include"main.h"
#include "AxisIndicator.h"
#include<imgui.h>
#include<algorithm>
#include<fstream>
#include<iostream>


const char kWindowTitle[] = "LE2A_20_ムラカミ_アオイ";


Vector3 TransScreen(const Vector3& transform,const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
	Vector3 kLocalVertices = { 0.0f,0.0f,0.0f };
	Matrix4x4 worldMatrix = { 0.0f };
	Matrix4x4 worldViewProjectionMatrix = { 0.0f };
	Vector3 ndcVertex = { 0.0f };
	Vector3 screenVertices = { 0.0f };

	worldMatrix = matrix->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, transform);
	worldViewProjectionMatrix = matrix->Multiply(worldMatrix, viewProjectionMatrix);
	ndcVertex = matrix->Transform(kLocalVertices, worldViewProjectionMatrix);
	screenVertices = matrix->Transform(ndcVertex, viewportMatrix);
	return screenVertices;

}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvely = (kGridHalfWidth * 2.0f) / float(kSubdivision);
	//始点終点群
	Vector3 backPosirion = { 0.0f,0.0f,0.0f };
	Vector3 frontPosition = { 0.0f,0.0f,0.0f };
	Vector3 leftPosirion = { 0.0f,0.0f,0.0f };
	Vector3 rightPosition = { 0.0f,0.0f,0.0f };

	uint32_t color = 0xAAAAAAFF;

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		backPosirion = { -kGridHalfWidth + (kGridEvely * xIndex),0.0f,kGridHalfWidth };
		frontPosition = { -kGridHalfWidth + (kGridEvely * xIndex),0.0f,-kGridHalfWidth };
		backPosirion = TransScreen(backPosirion,viewProjectionMatrix, viewportMatrix);
		frontPosition = TransScreen(frontPosition, viewProjectionMatrix, viewportMatrix);
		if (xIndex == 5) {
			color = 0x000000FF;
		}
		else {
			color = 0xAAAAAAFF;
		}
		Novice::DrawLine(static_cast<int>(backPosirion.x), static_cast<int>(backPosirion.y), static_cast<int>(frontPosition.x), static_cast<int>(frontPosition.y), color);
	}
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		leftPosirion = { -kGridHalfWidth,0.0f, -kGridHalfWidth + (kGridEvely * zIndex) };
		rightPosition = { kGridHalfWidth,0.0f, -kGridHalfWidth + (kGridEvely * zIndex) };
		if (zIndex == 5) {
			color = 0x000000FF;
		}
		else {
			color = 0xAAAAAAFF;
		}
		leftPosirion = TransScreen(leftPosirion, viewProjectionMatrix, viewportMatrix);
		rightPosition = TransScreen(rightPosition, viewProjectionMatrix, viewportMatrix);
		Novice::DrawLine(static_cast<int>(leftPosirion.x), static_cast<int>(leftPosirion.y), static_cast<int>(rightPosition.x), static_cast<int>(rightPosition.y), color);
	}
}

void DrawLine(const Vector3& start, const Vector3& end, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,uint32_t color) {
	Vector3 startPos = start;
	Vector3 endPos = end;

	startPos = TransScreen(startPos, viewProjectionMatrix, viewportMatrix);
	endPos = TransScreen(endPos, viewProjectionMatrix, viewportMatrix);

	Novice::DrawLine(static_cast<int>(startPos.x), static_cast<int>(startPos.y), static_cast<int>(endPos.x), static_cast<int>(endPos.y), color);
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 8;
	const float kLatEvery = float(M_PI / kSubdivision);//緯度一つ分の角度
	const float kLonEvery = float((M_PI * 2.0f) / kSubdivision);//経度一つ分の角度
	//緯度の方向に分割-π/2～π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = float(-M_PI / 2.0f + kLatEvery * latIndex);//現在の緯度(θ)
		//経度の方向に分割 0～2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			float lon = lonIndex * kLonEvery;//現在の経度(φ)
			//world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = { std::cosf(lat) * std::cosf(lon) * sphere.radius + sphere.center.x,std::sinf(lat) * sphere.radius + sphere.center.y,std::cosf(lat) * std::sinf(lon) * sphere.radius + sphere.center.z };

			b = { std::cosf(lat + kLatEvery) * std::cosf(lon) * sphere.radius + sphere.center.x,std::sinf(lat + kLatEvery) * sphere.radius + sphere.center.y,std::cosf(lat + kLatEvery) * std::sinf(lon) * sphere.radius + sphere.center.z };

			c = { std::cosf(lat) * std::cosf(lon + kLonEvery) * sphere.radius + sphere.center.x,std::sinf(lat) * sphere.radius + sphere.center.y,std::cosf(lat) * std::sinf(lon + kLonEvery) * sphere.radius + sphere.center.z };
			//a,b,cをScreen座標系まで変換
			a = TransScreen(a, viewProjectionMatrix, viewportMatrix);
			b = TransScreen(b, viewProjectionMatrix, viewportMatrix);
			c = TransScreen(c, viewProjectionMatrix, viewportMatrix);
			//ab,acで線を引く
			Novice::DrawLine(static_cast<int>(a.x), static_cast<int>(a.y), static_cast<int>(b.x), static_cast<int>(b.y), sphere.color);
			Novice::DrawLine(static_cast<int>(a.x), static_cast<int>(a.y), static_cast<int>(c.x), static_cast<int>(c.y), sphere.color);
		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	std::unique_ptr<Vector> vec = std::make_unique<Vector>();
	Vector3 center = vec->Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4]{};
	perpendiculars[0] = vec->Normalize(vec->Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = vec->Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	Vector3 points[4]{};
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = vec->Multiply(2.0f, perpendiculars[index]);
		Vector3 point = center + extend;
		points[index] = TransScreen(point, viewProjectionMatrix, viewportMatrix);
	}
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);

}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 points[3]{};
	for (int i = 0; i < 3; i++){
		points[i] = triangle.vertices[i];
	}
	for (int i = 0; i < 3; i++){
		points[i] = TransScreen(points[i], viewProjectionMatrix, viewportMatrix);
	}
	Novice::DrawTriangle(
		static_cast<int>(points[0].x), static_cast<int>(points[0].y),
		static_cast<int>(points[1].x), static_cast<int>(points[1].y),
		static_cast<int>(points[2].x), static_cast<int>(points[2].y),
		color, kFillModeWireFrame);
}

void DrawAABB(const AABB& aabb,const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	
	Vector3 points[8]{};

	points[0] = aabb.min;
	points[1] = { aabb.min.x,aabb.min.y,aabb.max.z };
	points[2] = { aabb.max.x,aabb.min.y,aabb.min.z };
	points[3] = { aabb.max.x,aabb.min.y,aabb.max.z };
	points[4] = { aabb.min.x,aabb.max.y,aabb.min.z };
	points[5] = { aabb.min.x,aabb.max.y,aabb.max.z };
	points[6] = { aabb.max.x,aabb.max.y,aabb.min.z };
	points[7] = aabb.max;

	for (int i = 0; i < 8; i++) {
		points[i] = TransScreen(points[i], viewProjectionMatrix, viewportMatrix);
	}
	//底面
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
	//上面
	Novice::DrawLine(static_cast<int>(points[4].x), static_cast<int>(points[4].y), static_cast<int>(points[5].x), static_cast<int>(points[5].y), color);
	Novice::DrawLine(static_cast<int>(points[5].x), static_cast<int>(points[5].y), static_cast<int>(points[7].x), static_cast<int>(points[7].y), color);
	Novice::DrawLine(static_cast<int>(points[7].x), static_cast<int>(points[7].y), static_cast<int>(points[6].x), static_cast<int>(points[6].y), color);
	Novice::DrawLine(static_cast<int>(points[6].x), static_cast<int>(points[6].y), static_cast<int>(points[4].x), static_cast<int>(points[4].y), color);
	//側面
	Novice::DrawLine(static_cast<int>(points[4].x), static_cast<int>(points[4].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
	Novice::DrawLine(static_cast<int>(points[5].x), static_cast<int>(points[5].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[7].x), static_cast<int>(points[7].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[6].x), static_cast<int>(points[6].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);

}

void DrawOBB(const OBB& obb,const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();
	std::unique_ptr<Vector> vector_ = std::make_unique<Vector>();	
	
	Vector3 orientNum = (obb.orientations[0] + obb.orientations[1] + obb.orientations[2]) * obb.size;

	Vector3 pointMax = orientNum;

	Vector3 pointMin = { -orientNum.x,-orientNum.y,-orientNum.z };

	Vector3 points[8]{};

	points[0] = pointMin;
	points[1] = { pointMin.x,pointMin.y,pointMax.z };
	points[2] = { pointMax.x,pointMin.y,pointMin.z };
	points[3] = { pointMax.x,pointMin.y,pointMax.z };
	points[4] = { pointMin.x,pointMax.y,pointMin.z };
	points[5] = { pointMin.x,pointMax.y,pointMax.z };
	points[6] = { pointMax.x,pointMax.y,pointMin.z };
	points[7] = pointMax;

	Vector3 kLocalVertices = { 0.0f,0.0f,0.0f };
	Matrix4x4 worldMatrix = {
		obb.orientations[0].x,obb.orientations[0].y, obb.orientations[0].z, 0,
		obb.orientations[1].x,obb.orientations[1].y, obb.orientations[1].z, 0,
		obb.orientations[2].x,obb.orientations[2].y, obb.orientations[2].z, 0,
		obb.center.x,obb.center.y,obb.center.z,1
	};
	Matrix4x4 worldViewProjectionMatrix = { 0.0f };
	Vector3 ndcVertex = { 0.0f };
	Vector3 screenVertices = { 0.0f };
	
	

	for (int i = 0; i < 8; i++) {
		worldMatrix.m[3][0] = points[i].x;
		worldMatrix.m[3][1] = points[i].y;
		worldMatrix.m[3][2] = points[i].z;
		worldViewProjectionMatrix = matrix_->Multiply(worldMatrix, viewProjectionMatrix);
		ndcVertex = matrix_->Transform(kLocalVertices, worldViewProjectionMatrix);
		screenVertices = matrix_->Transform(ndcVertex, viewportMatrix);
		points[i] = screenVertices;
	}

	ImGui::Begin("WorldMatrix");
	ImGui::DragFloat4("WorldMatrix[0]", &worldMatrix.m[0][0], 0.01f);
	ImGui::DragFloat4("WorldMatrix[1]", &worldMatrix.m[1][0], 0.01f);
	ImGui::DragFloat4("WorldMatrix[2]", &worldMatrix.m[2][0], 0.01f);
	ImGui::DragFloat3("WorldMatrix[3]", &points[0].x, 0.01f);
	ImGui::End();
	//底面
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
	//上面
	Novice::DrawLine(static_cast<int>(points[4].x), static_cast<int>(points[4].y), static_cast<int>(points[5].x), static_cast<int>(points[5].y), color);
	Novice::DrawLine(static_cast<int>(points[5].x), static_cast<int>(points[5].y), static_cast<int>(points[7].x), static_cast<int>(points[7].y), color);
	Novice::DrawLine(static_cast<int>(points[7].x), static_cast<int>(points[7].y), static_cast<int>(points[6].x), static_cast<int>(points[6].y), color);
	Novice::DrawLine(static_cast<int>(points[6].x), static_cast<int>(points[6].y), static_cast<int>(points[4].x), static_cast<int>(points[4].y), color);
	//側面
	Novice::DrawLine(static_cast<int>(points[4].x), static_cast<int>(points[4].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
	Novice::DrawLine(static_cast<int>(points[5].x), static_cast<int>(points[5].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[7].x), static_cast<int>(points[7].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[6].x), static_cast<int>(points[6].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);

}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	std::unique_ptr<Vector> vector_ = std::make_unique<Vector>();
	//分割数
	const uint32_t divisionNumber = 16;
	//曲線の変数
	Vector3 bezier0[divisionNumber + 1] = {};
	float t = 0.0f;

	

	for (uint32_t i = 0; i < divisionNumber+1; i++){
		t = i / static_cast<float>(divisionNumber);

		Vector3 p0p1 = vector_->Lerp(controlPoint0, controlPoint1, t);

		Vector3 p1p2 = vector_->Lerp(controlPoint1, controlPoint2, t);

		Vector3 p = vector_->Lerp(p0p1, p1p2, t);

		bezier0[i] = p;

		bezier0[i] = TransScreen(bezier0[i], viewProjectionMatrix, viewportMatrix);

	}

	for (uint32_t i = 0; i < divisionNumber; i++) {
		Novice::DrawLine(static_cast<int>(bezier0[i].x), static_cast<int>(bezier0[i].y),
			static_cast<int>(bezier0[i + 1].x), static_cast<int>(bezier0[i + 1].y), color);
	}
}

void DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	std::unique_ptr<Vector> vector_ = std::make_unique<Vector>();
	//分割数
	const uint32_t divisionNumber = 8;
	//曲線の変数
	Vector3 CatmullRom[divisionNumber + 1] = {};
	float t = 0.0f;



	for (uint32_t i = 0; i < divisionNumber + 1; i++) {
		t = i / static_cast<float>(divisionNumber);

		Vector3 p = vector_->makeCatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t);

		CatmullRom[i] = p;

		CatmullRom[i] = TransScreen(CatmullRom[i], viewProjectionMatrix, viewportMatrix);

	}

	for (uint32_t i = 0; i < divisionNumber; i++) {
		Novice::DrawLine(static_cast<int>(CatmullRom[i].x), static_cast<int>(CatmullRom[i].y),
			static_cast<int>(CatmullRom[i + 1].x), static_cast<int>(CatmullRom[i + 1].y), color);
	}
}


Vector3 Add(const Vector3& Vec1, const Vector3& Vec2) {
	Vector3 result;
	result.x = Vec1.x + Vec2.x;
	result.y = Vec1.y + Vec2.y;
	result.z = Vec1.z + Vec2.z;
	return result;
}

Vector3 Subtract(const Vector3& Vec1, const Vector3& Vec2) {
	Vector3 result;
	result.x = Vec1.x - Vec2.x;
	result.y = Vec1.y - Vec2.y;
	result.z = Vec1.z - Vec2.z;
	return result;
}

Vector3 Mult(const Vector3& Vec1, const Vector3& Vec2) {
	Vector3 result;
	result.x = Vec1.x * Vec2.x;
	result.y = Vec1.y * Vec2.y;
	result.z = Vec1.z * Vec2.z;
	return result;
}


//内積
float Dot(const Vector3& Vec1, const Vector3& Vec2) {
	float result;
	result = Vec1.x * Vec2.x + Vec1.y * Vec2.y + Vec1.z * Vec2.z;
	return result;
}

Vector3 Project(const Vector3& v1,const Vector3& v2) {
	Vector3 result{ 0 };
	Vector* vec = new Vector;
	result = vec->Multiply(Dot(v1, vec->Normalize(v2)), vec->Normalize(v2));

	return result;
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result{ 0.0f };
	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	result.x = segment.origin.x + project.x;
	result.y = segment.origin.y + project.y;
	result.z = segment.origin.z + project.z;
	return result;
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	Vector* vec = new Vector;
	float distance = vec->Length(s1.center - s2.center);

	if (distance <= s1.radius + s2.radius) {
		return true;
	}
	else {
		return false;
	}


}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	Vector* vec = new Vector;
	//平面と点の距離
	float k = sqrtf(powf(vec->Dot(plane.normal, sphere.center) - plane.distance, 2.0f));

	Novice::ScreenPrintf(0, 0, "distance = %.1f", k);
	if (k <= sphere.radius) {
		return true;
	}
	return false;
}

bool IsCollision(const Segment& segment, const Plane& plane) {
	float dot = Dot(plane.normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}else {
		return false;
	}
}

bool IsCollision(const Triangle& triangle, const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	std::unique_ptr<Vector> vec = std::make_unique<Vector>();

	Sphere sphere = {
		{0.0f,0.0f,0.0f},
		0.02f,
		BLACK
	};


	Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
	Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
	Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];
	
	Vector3 normal = vec->Normalize(vec->Cross(v01, v12));

	float distance = vec->Dot(triangle.vertices[1], normal);

	float dot = Dot(normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (distance - Dot(segment.origin, normal)) / dot;

	Vector3 point = segment.origin + vec->Multiply(t, segment.diff);

	sphere.center = point;

	DrawSphere(sphere, viewProjectionMatrix, viewportMatrix);

	Vector3 v0p = point - triangle.vertices[0];
	Vector3 v1p = point - triangle.vertices[1];
	Vector3 v2p = point - triangle.vertices[2];

	Vector3 cross01 = vec->Cross(v01, v1p);
	Vector3 cross12 = vec->Cross(v12, v2p);
	Vector3 cross20 = vec->Cross(v20, v0p);

	//Novice::ScreenPrintf(10, 10, "t = %.1f", t);
	if (vec->Dot(cross01, normal) >= 0.0f &&
		vec->Dot(cross12, normal) >= 0.0f &&
		vec->Dot(cross20, normal) >= 0.0f &&
		t >= 0.0f && t <= 1.0f) {
		
		return true;
	}
	else {
		return false;
	}
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x)&&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y)&&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z))
	{
		return true;
	}
	else {
		return false;
	}
}

bool IsCollision(const Sphere& sphere, const AABB& aabb) {
	std::unique_ptr<Vector> vec = nullptr;
	vec = std::make_unique<Vector>();
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
	};

	float distance = vec->Length(closestPoint - sphere.center);

	if (distance <= sphere.radius) {
		return true;
	}
	else {
		return false;
	}

}

bool IsCollision(const Segment& segment, const AABB& aabb) {
	Vector3 tMin = {
		(aabb.min.x - segment.origin.x) / segment.diff.x,
		(aabb.min.y - segment.origin.y) / segment.diff.y,
		(aabb.min.z - segment.origin.z) / segment.diff.z
	};
	Vector3 tMax = {
		(aabb.max.x - segment.origin.x) / segment.diff.x,
		(aabb.max.y - segment.origin.y) / segment.diff.y,
		(aabb.max.z - segment.origin.z) / segment.diff.z
	};

	Vector3 tNear = {
		min(tMin.x,tMax.x),
		min(tMin.y,tMax.y),
		min(tMin.z,tMax.z)
	};

	Vector3 tFar = {
		max(tMin.x,tMax.x),
		max(tMin.y,tMax.y),
		max(tMin.z,tMax.z)
	};

	float tmin = max(max(tNear.x, tNear.y), tNear.z);

	float tmax = min(min(tFar.x, tFar.y), tFar.z);

	Novice::ScreenPrintf(0, 0, "tmin/tmax = %.2f", tmin / tmax);
	Novice::ScreenPrintf(0, 20, "tmin = %.2f", tmin );
	Novice::ScreenPrintf(0, 40, "tmax = %.2f", tmax);

	if (tmax == 0.0f) {
		return false;
	}
	

	if ((tmin <= tmax) && (tmax > 0.0f) && (tmin < 1.0f)) {
		return true;
	}
	else {
		return false;
	}

}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();	

	std::unique_ptr<Vector> vec_ = std::make_unique<Vector>();

	uint32_t lineColor = BLUE;

	Vector3 cameraTransform = { 0.0f,0.0f,0.0f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	/*Triangle triangle = {};
	triangle.vertices[0] = { -1.0f,0.0f,0.0f };
	triangle.vertices[1] = { 0.0f,1.0f,0.0f };
	triangle.vertices[2] = { 1.0f,0.0f,0.0f };*/
		
	Vector3 controlPoints[4] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
		{-0.53f,-0.26f,-0.15f}
	};

	std::vector<Vector3> ControlPoints;

	for (int i = 0; i < 4; i++)
	{
		ControlPoints.push_back(controlPoints[i]);
	}
	

	//uint32_t lineColor = BLUE;

	uint32_t Linecolor = WHITE;

	//uint32_t triangleColor = WHITE;
		
	/*Plane plane{
		{0.0f,1.0f,0.0f},
		1.0f,
		WHITE
	};*/

	/*Vector3 translates[3] = {
		{0.2f,1.0f,0.0f},
		{0.4f,0.0f,0.0f},
		{0.3f,0.0f,0.0f}
	};

	Vector3 rotates[3] = {
		{0.0f,0.0f,-6.8f},
		{0.0f,0.0f,-1.4f},
		{0.0f,0.0f,0.0f}
	};

	Vector3 scales[3] = {
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f}
	};*/

	Sphere PLsphere{
		.center = controlPoints[0],
		.radius = 0.05f,
		.color = RED
	};

	std::vector<Sphere> spheres;//〆切α

	Sphere sphere[4]{
		{{0.0f},0.02f,BLACK},
		{{0.0f},0.02f,BLACK},
		{{0.0f},0.02f,BLACK},
		{{0.0f},0.02f,BLACK}
	};

	for (int i = 0; i < 4; i++)
	{
		spheres.push_back(sphere[i]);
	}
	

	//AABB aabb1{
	//	.min{-0.5f,-0.5f,-0.5f},
	//	.max{0.5f,0.5f,0.5f},
	//	.color = WHITE
	//};
	
	/*AABB aabb2{
		.min{0.2f,0.2f,0.2f},
		.max{1.0f,1.0f,1.0f},
		.color = WHITE
	};*/

	/*Segment segment_{ 
		{-0.7f,0.3f,0.0f},
		{2.0f,-0.5f,0.0f} };*/

	Vector3 rotate{ 0.0f,0.0f,0.0f };

	/*OBB obb{
		.center{-1.0f,0.0f,0.0f},
		.orientations = {{1.0f,0.0f,0.0f},
						 {0.0f,1.0f,0.0f},
						 {0.0f,0.0f,1.0f}},
		.size{0.5f,0.5f,0.5f},
		.color=WHITE
	};*/

	Vector2 localCameraPos{ 0.0f,0.0f };

	float cameraLength = -8.0f;

	//キャトムル-ロム移動用
	//分割数
	const uint32_t divisionNumber = 8;
	float point = 0.0f;
	float t = 0.0f;


	int linePass = 0;

	const int firstLinePass = 0;
	int LastLinePass = 2;

	bool isMove = false;

	using json = nlohmann::json;

	const std::string kDirectoryPath = "Elements/";
	
	const std::string kDirectoryName = "Elements";

	const std::string kItemName = "Points";
	
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		//////カメラぐるぐる
		//左右
		if (keys[DIK_A]) {
			cameraRotate.y += 0.03f;
		}
		else if (keys[DIK_D]) {
			cameraRotate.y -= 0.03f;
		}
		//上下
		if (keys[DIK_W]) {
			cameraRotate.x -= 0.03f;
		}
		else if (keys[DIK_S]) {
			cameraRotate.x += 0.03f;
		}
		//ズームインorアウト
		if (keys[DIK_E]) {
			cameraLength += 0.1f;
		}
		else if (keys[DIK_Q]) {
			cameraLength -= 0.1f;
		}
		//カメラの左右上下移動
		if (keys[DIK_UP]) {
			localCameraPos.y += 0.03f;
		}
		else if (keys[DIK_DOWN]) {
			localCameraPos.y -= 0.03f;
		}
		//上下
		if (keys[DIK_LEFT]) {
			localCameraPos.x -= 0.03f;
		}
		else if (keys[DIK_RIGHT]) {
			localCameraPos.x += 0.03f;
		}
		if (keys[DIK_R]&&!preKeys[DIK_R]){
			cameraLength = -8.0f;
			localCameraPos = { 0.0f,0.0f };
			cameraRotate = { 0.26f,0.0f,0.0f };
		}

		
		//中央からのカメラの距離
		Vector3 offset = { localCameraPos.x,localCameraPos.y,cameraLength };
		Matrix4x4 newRotateMatrix = matrix_->MakeRotateMatrix(cameraRotate);
		offset = matrix_->TransformNormal(offset, newRotateMatrix);
		cameraTransform =/*中心とするオブジェクトのTransformを足す*/ offset;

		//カメラの座標変換
		Matrix4x4 cameraMatrix = matrix_->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTransform);
		Matrix4x4 viewMatrix = matrix_->Inverce(cameraMatrix);
		Matrix4x4 projectionMatrix = matrix_->MakePerspectiveFovMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = matrix_->Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = matrix_->MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		//メインの処理を書きこむ
		for (size_t i = 0; i < ControlPoints.size(); ++i){
			spheres[i].center = ControlPoints[i];
		}

		if (isMove){
			point += 0.1f;
		}
		if (point > static_cast<float>(divisionNumber) - 0.1f) {
			point = 0.0f;
			linePass += 1;
			if (linePass > LastLinePass) {
				linePass = firstLinePass;
			}
		}

		t = point / static_cast<float>(divisionNumber);



		if (linePass == 0) {
			Vector3 p = vec_->makeCatmullRom(ControlPoints[0], ControlPoints[0], ControlPoints[1], ControlPoints[2], t);
			PLsphere.center = p;
		}
		if (linePass >= 1&& linePass != LastLinePass) {
			Vector3 p = vec_->makeCatmullRom(ControlPoints[linePass - 1], ControlPoints[linePass], ControlPoints[linePass + 1], ControlPoints[linePass + 2], t);
			PLsphere.center = p;
		}
		if (linePass == LastLinePass) {
			Vector3 p = vec_->makeCatmullRom(ControlPoints[LastLinePass - 1], ControlPoints[LastLinePass], ControlPoints[LastLinePass + 1], ControlPoints[LastLinePass+1], t);
			PLsphere.center = p;
		}


		ImGui::Begin("Catmull-Rom");
		for (size_t i = 0; i < ControlPoints.size(); ++i){
			ImGui::DragFloat3(("Points" + std::to_string(i)).c_str(), &ControlPoints[i].x, 0.01f);
		}
		ImGui::End();

		ImGui::Begin("MakeCatmull-Rom");
		if (ImGui::Button("Add Element")) {
			Vector3 newPoint = { 0.0f };
			Sphere newSphere = {
				.center = newPoint,
				.radius = 0.01f,
				.color = BLACK
			};

			LastLinePass++;

			ControlPoints.push_back(newPoint);
			spheres.push_back(newSphere);
		}
		if (ImGui::Button("Delete Element")) {
			if (!ControlPoints.empty() && !spheres.empty()) {
				LastLinePass--;

				ControlPoints.erase(ControlPoints.begin());
				spheres.erase(spheres.begin());
			}
		}
		if (ImGui::Button("Save Element")){
			//曲線保存用
			json root;

			root = json::object();

			for (size_t i = 0; i < ControlPoints.size(); ++i) {
				root[kItemName][i] = json::array({ControlPoints[i].x,ControlPoints[i].y, ControlPoints[i].z});


				std::filesystem::path dir(kDirectoryPath);
				if (!std::filesystem::exists(kDirectoryName)) {
					std::filesystem::create_directory(kDirectoryName);
				}
				// 書き込むjsonファイルのフルパスを合成する
				std::string filePath = kDirectoryPath + kItemName + ".json";
				// 書き込み用ファイルストリーム
				std::ofstream ofs;
				// ファイルを書き込みように開く
				ofs.open(filePath);
				//ファイルオープン失敗
				if (ofs.fail()) {
					std::string message = "Failed open data file for write.";
					MessageBoxA(nullptr, message.c_str(), "Element", 0);
					assert(0);
					break;
				}
				//ファイルにjson文字列を書き込む(インデント幅4)
				ofs << std::setw(4) << root << std::endl;
				//ファイルを閉じる
				ofs.close();
			}
			std::string message = std::format("{}.json saved.", "Points");
			MessageBoxA(nullptr, message.c_str(), "Element", 0);

		}
		/*if (ImGui::Button("Load Element")){
			LoadFiles();
		}*/

		ImGui::End();


		ImGui::Begin("PL");
		ImGui::DragFloat("point", &point, 0.01f);
		ImGui::Checkbox("StartMove", &isMove);
		ImGui::End();
		
		ImGui::Begin("FPS");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		//DrawAABB(aabb1,viewProjectionMatrix, viewportMatrix, aabb1.color);

		//DrawSphere(sphere, viewProjectionMatrix, viewportMatrix);

		//Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);
		
		/*DrawBezier(controlPoints[0], controlPoints[1], controlPoints[2],
			viewProjectionMatrix, viewportMatrix, lineColor);*/
		DrawCatmullRom(ControlPoints[0], ControlPoints[0], ControlPoints[1], ControlPoints[2],
			viewProjectionMatrix, viewportMatrix, lineColor);
		for (int i = 1; i < LastLinePass; i++){
			DrawCatmullRom(ControlPoints[i - 1], ControlPoints[i], ControlPoints[i + 1], ControlPoints[i + 2],
				viewProjectionMatrix, viewportMatrix, lineColor);
		}

		DrawCatmullRom(ControlPoints[LastLinePass - 1], ControlPoints[LastLinePass], ControlPoints[LastLinePass + 1], ControlPoints[LastLinePass + 1],
			viewProjectionMatrix, viewportMatrix, lineColor);

		for (size_t i = 0; i < spheres.size(); ++i) {
			DrawSphere(spheres[i], viewProjectionMatrix, viewportMatrix);
		}

		DrawSphere(PLsphere, viewProjectionMatrix, viewportMatrix);

		//DrawLine(sphere0.center, sphere1.center, viewProjectionMatrix, viewportMatrix, Linecolor);
		//DrawLine(sphere1.center, sphere2.center, viewProjectionMatrix, viewportMatrix, Linecolor);
		//DrawSphere(sphere3, viewProjectionMatrix, viewportMatrix);
		//DrawPlane(plane, viewProjectionMatrix, viewportMatrix, WHITE);

		//DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, triangle.color);
		
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void LoadFiles() {
	const std::string kDirectoryPath_ = "Elements/";

	if (!std::filesystem::exists(kDirectoryPath_)) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Element", 0);
		assert(0);
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}

}

void LoadFile(const std::string& groupName) {
	const std::string kDirectoryPath = "Elements/";

	const std::string kItemName = "Points";

	std::vector<Vector3> myVector;

	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		assert(0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kItemName);
	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (const auto& root_ : root[kItemName]) {
		Vector3 v{};

		from_json(root_, v);

		myVector.push_back(v);

	}

}

void from_json(const nlohmann::json& j, Vector3& v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}