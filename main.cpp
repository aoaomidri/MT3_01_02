#include"main.h"
#include "AxisIndicator.h"
#include<imgui.h>
#include<algorithm>

const char kWindowTitle[] = "LE2A_20_ムラカミ_アオイ";

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();
	return matrix_->Add(m1, m2);
}

Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();
	return matrix_->Subtract(m1, m2);
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();
	return matrix_->Multiply(m1, m2);
}

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
	const uint32_t kSubdivision = 16;
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
	
	Matrix4x4 worldMatrix = {
		obb.orientations[0].x,obb.orientations[0].y, obb.orientations[0].z, 0,
		obb.orientations[1].x,obb.orientations[1].y, obb.orientations[1].z, 0,
		obb.orientations[2].x,obb.orientations[2].y, obb.orientations[2].z, 0,
		obb.center.x,obb.center.y,obb.center.z,1
	};	

	Vector3 newPoints[8]{};

	newPoints[0] = obb.size * -1;
	newPoints[1] = { obb.size.x * -1,obb.size.y * -1,obb.size.z };
	newPoints[2] = { obb.size.x,obb.size.y * -1,obb.size.z * -1 };
	newPoints[3] = { obb.size.x,obb.size.y * -1,obb.size.z };
	newPoints[4] = { obb.size.x * -1,obb.size.y,obb.size.z * -1 };
	newPoints[5] = { obb.size.x * -1,obb.size.y,obb.size.z };
	newPoints[6] = { obb.size.x,obb.size.y,obb.size.z * -1 };
	newPoints[7] = obb.size;

	for (int i = 0; i < 8; i++) {
		newPoints[i] = matrix_->TransformNormal(newPoints[i], worldMatrix);
	}

	for (int i = 0; i < 8; i++) {
		newPoints[i] = obb.center + newPoints[i];
		newPoints[i] = TransScreen(newPoints[i], viewProjectionMatrix, viewportMatrix);
	}

	//底面
	Novice::DrawLine(static_cast<int>(newPoints[0].x), static_cast<int>(newPoints[0].y), static_cast<int>(newPoints[1].x), static_cast<int>(newPoints[1].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[1].x), static_cast<int>(newPoints[1].y), static_cast<int>(newPoints[3].x), static_cast<int>(newPoints[3].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[3].x), static_cast<int>(newPoints[3].y), static_cast<int>(newPoints[2].x), static_cast<int>(newPoints[2].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[2].x), static_cast<int>(newPoints[2].y), static_cast<int>(newPoints[0].x), static_cast<int>(newPoints[0].y), color);
	//上面
	Novice::DrawLine(static_cast<int>(newPoints[4].x), static_cast<int>(newPoints[4].y), static_cast<int>(newPoints[5].x), static_cast<int>(newPoints[5].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[5].x), static_cast<int>(newPoints[5].y), static_cast<int>(newPoints[7].x), static_cast<int>(newPoints[7].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[7].x), static_cast<int>(newPoints[7].y), static_cast<int>(newPoints[6].x), static_cast<int>(newPoints[6].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[6].x), static_cast<int>(newPoints[6].y), static_cast<int>(newPoints[4].x), static_cast<int>(newPoints[4].y), color);
	//側面
	Novice::DrawLine(static_cast<int>(newPoints[4].x), static_cast<int>(newPoints[4].y), static_cast<int>(newPoints[0].x), static_cast<int>(newPoints[0].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[5].x), static_cast<int>(newPoints[5].y), static_cast<int>(newPoints[1].x), static_cast<int>(newPoints[1].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[7].x), static_cast<int>(newPoints[7].y), static_cast<int>(newPoints[3].x), static_cast<int>(newPoints[3].y), color);
	Novice::DrawLine(static_cast<int>(newPoints[6].x), static_cast<int>(newPoints[6].y), static_cast<int>(newPoints[2].x), static_cast<int>(newPoints[2].y), color);

}

void DrawViewingFrustum(const viewingFrustum& viewingFrustum_, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	std::unique_ptr<Vector> vec = std::make_unique<Vector>();
	std::unique_ptr<Matrix> mat = std::make_unique<Matrix>();
	//それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};

	Vector3 nearPlanePoints_[4] = { 0 };
	Vector3 farPlanePoints_[4] = { 0 };
	//向きベクトル
	Matrix4x4 CameraMatWorld = mat->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { viewingFrustum_.rotate_ }, { viewingFrustum_.translation_ });

	Vector3 directionNear = vec->Normalize(viewingFrustum_.direction);
	directionNear.z = directionNear.z * viewingFrustum_.nearZ;
	Vector3 directionFar = vec->Normalize(viewingFrustum_.direction);
	directionFar.z = directionFar.z * viewingFrustum_.farZ;


	//近平面の縦横
	nearPlane.y = vec->Length(directionNear) * std::tan(viewingFrustum_.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum_.aspectRatio;
	//遠平面の縦横
	farPlane.y = vec->Length(directionFar) * std::tan(viewingFrustum_.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum_.aspectRatio;

	nearPlanePoints_[0] = { directionNear.x + -nearPlane.x ,directionNear.y + nearPlane.y ,directionNear.z };//左上
	nearPlanePoints_[1] = { directionNear.x + nearPlane.x ,directionNear.y + nearPlane.y ,directionNear.z };//右上
	nearPlanePoints_[2] = { directionNear.x + -nearPlane.x ,directionNear.y + -nearPlane.y ,directionNear.z };//左下
	nearPlanePoints_[3] = { directionNear.x + nearPlane.x ,directionNear.y + -nearPlane.y ,directionNear.z };//右下

	farPlanePoints_[0] = { directionFar.x + -farPlane.x ,directionFar.y + farPlane.y ,directionFar.z };//左上
	farPlanePoints_[1] = { directionFar.x + farPlane.x ,directionFar.y + farPlane.y ,directionFar.z };//右上
	farPlanePoints_[2] = { directionFar.x + -farPlane.x ,directionFar.y + -farPlane.y ,directionFar.z };//左下
	farPlanePoints_[3] = { directionFar.x + farPlane.x ,directionFar.y + -farPlane.y ,directionFar.z };//右下

	for (int i = 0; i < 4; i++) {

		nearPlanePoints_[i] = mat->TransformNormal(nearPlanePoints_[i], CameraMatWorld);
		farPlanePoints_[i] = mat->TransformNormal(farPlanePoints_[i], CameraMatWorld);
	}


	//頂点
	Vector3 points[9]{ 0 };

	ImGui::Begin("nearPlane");
	ImGui::DragFloat2("Size", &nearPlane.x, 0.01f);
	ImGui::End();

	points[0] = viewingFrustum_.translation_;

	points[1] = viewingFrustum_.translation_ + nearPlanePoints_[0];
	points[2] = viewingFrustum_.translation_ + nearPlanePoints_[1];
	points[3] = viewingFrustum_.translation_ + nearPlanePoints_[2];
	points[4] = viewingFrustum_.translation_ + nearPlanePoints_[3];

	points[5] = viewingFrustum_.translation_ + farPlanePoints_[0];
	points[6] = viewingFrustum_.translation_ + farPlanePoints_[1];
	points[7] = viewingFrustum_.translation_ + farPlanePoints_[2];
	points[8] = viewingFrustum_.translation_ + farPlanePoints_[3];
	for (int i = 0; i < 9; i++) {

		points[i] = TransScreen(points[i], viewProjectionMatrix, viewportMatrix);
	}
	//近平面
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[4].x), static_cast<int>(points[4].y), color);
	Novice::DrawLine(static_cast<int>(points[4].x), static_cast<int>(points[4].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	//遠平面
	Novice::DrawLine(static_cast<int>(points[5].x), static_cast<int>(points[5].y), static_cast<int>(points[6].x), static_cast<int>(points[6].y), color);
	Novice::DrawLine(static_cast<int>(points[6].x), static_cast<int>(points[6].y), static_cast<int>(points[8].x), static_cast<int>(points[8].y), color);
	Novice::DrawLine(static_cast<int>(points[8].x), static_cast<int>(points[8].y), static_cast<int>(points[7].x), static_cast<int>(points[7].y), color);
	Novice::DrawLine(static_cast<int>(points[7].x), static_cast<int>(points[7].y), static_cast<int>(points[5].x), static_cast<int>(points[5].y), color);
	//つなぎの線
	Novice::DrawLine(static_cast<int>(points[5].x), static_cast<int>(points[5].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[6].x), static_cast<int>(points[6].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), color);
	Novice::DrawLine(static_cast<int>(points[8].x), static_cast<int>(points[8].y), static_cast<int>(points[4].x), static_cast<int>(points[4].y), color);
	Novice::DrawLine(static_cast<int>(points[7].x), static_cast<int>(points[7].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);

	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), WHITE);
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[4].x), static_cast<int>(points[4].y), WHITE);
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), WHITE);
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), WHITE);
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

bool IsCollision(const OBB& obb, const viewingFrustum& viewingFrustum) {
	std::unique_ptr<Vector> vec = std::make_unique<Vector>();
	std::unique_ptr<Matrix> mat = std::make_unique<Matrix>();
	/*ステップ1視錐台の生成*/
	//頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	//法線の個数
	const int normalLine = 6;

	//それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	//面の頂点
	Vector3 nearPlanePoints_[4] = { 0 };
	Vector3 farPlanePoints_[4] = { 0 };
	//視錐台の行列
	Matrix4x4 FrustumMatWorld = mat->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { viewingFrustum.rotate_ }, { viewingFrustum.translation_ });
	
	//向きベクトルnear
	Vector3 directionNear = vec->Normalize(viewingFrustum.direction);
	directionNear.z = directionNear.z * viewingFrustum.nearZ;
	//向きベクトルfar
	Vector3 directionFar = vec->Normalize(viewingFrustum.direction);
	directionFar.z = directionFar.z * viewingFrustum.farZ;


	//近平面の縦横
	nearPlane.y = vec->Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	//遠平面の縦横
	farPlane.y = vec->Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = { directionNear.x + -nearPlane.x ,directionNear.y + nearPlane.y ,directionNear.z };//左上
	nearPlanePoints_[1] = { directionNear.x + nearPlane.x ,directionNear.y + nearPlane.y ,directionNear.z };//右上
	nearPlanePoints_[2] = { directionNear.x + -nearPlane.x ,directionNear.y + -nearPlane.y ,directionNear.z };//左下
	nearPlanePoints_[3] = { directionNear.x + nearPlane.x ,directionNear.y + -nearPlane.y ,directionNear.z };//右下

	farPlanePoints_[0] = { directionFar.x + -farPlane.x ,directionFar.y + farPlane.y ,directionFar.z };//左上
	farPlanePoints_[1] = { directionFar.x + farPlane.x ,directionFar.y + farPlane.y ,directionFar.z };//右上
	farPlanePoints_[2] = { directionFar.x + -farPlane.x ,directionFar.y + -farPlane.y ,directionFar.z };//左下
	farPlanePoints_[3] = { directionFar.x + farPlane.x ,directionFar.y + -farPlane.y ,directionFar.z };//右下

	//頂点
	Vector3 FrustumPoints[FrustumVertex]{ 0 };
	//near
	FrustumPoints[0] = nearPlanePoints_[0];
	FrustumPoints[1] = nearPlanePoints_[1];
	FrustumPoints[2] = nearPlanePoints_[2];
	FrustumPoints[3] = nearPlanePoints_[3];
	//far
	FrustumPoints[4] = farPlanePoints_[0];
	FrustumPoints[5] = farPlanePoints_[1];
	FrustumPoints[6] = farPlanePoints_[2];
	FrustumPoints[7] = farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	//obbの行列
	Matrix4x4 worldMatrix = {
		obb.orientations[0].x,obb.orientations[0].y, obb.orientations[0].z, 0,
		obb.orientations[1].x,obb.orientations[1].y, obb.orientations[1].z, 0,
		obb.orientations[2].x,obb.orientations[2].y, obb.orientations[2].z, 0,
		obb.center.x,obb.center.y,obb.center.z,1
	};

	obbPoints[0] = obb.size * -1;
	obbPoints[1] = { obb.size.x * -1,obb.size.y * -1,obb.size.z };
	obbPoints[2] = { obb.size.x,obb.size.y * -1,obb.size.z * -1 };
	obbPoints[3] = { obb.size.x,obb.size.y * -1,obb.size.z };
	obbPoints[4] = { obb.size.x * -1,obb.size.y,obb.size.z * -1 };
	obbPoints[5] = { obb.size.x * -1,obb.size.y,obb.size.z };
	obbPoints[6] = { obb.size.x,obb.size.y,obb.size.z * -1 };
	obbPoints[7] = obb.size;

	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = mat->TransformNormal(obbPoints[i], worldMatrix);
		obbPoints[i] = obb.center + obbPoints[i];
	}

	/*ステップ3 OBBを視錐台のローカル座標に変換*/
	//視錐台の逆行列
	Matrix4x4 FrustumInverceMat = mat->Inverce(FrustumMatWorld);
	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = mat->Transform(obbPoints[i], FrustumInverceMat);
	}
	
	/*ステップ4 当たり判定*/
	//near面から
	Vector3 v01 = FrustumPoints[1] - FrustumPoints[0];
	Vector3 v12 = FrustumPoints[2] - FrustumPoints[1];

	//far
	Vector3 v65 = FrustumPoints[5] - FrustumPoints[6];
	Vector3 v54 = FrustumPoints[4] - FrustumPoints[5];

	//left
	Vector3 v02 = FrustumPoints[2] - FrustumPoints[0];
	Vector3 v26 = FrustumPoints[6] - FrustumPoints[2];

	//right
	Vector3 v53 = FrustumPoints[3] - FrustumPoints[5];
	Vector3 v31 = FrustumPoints[1] - FrustumPoints[3];

	//up
	Vector3 v41 = FrustumPoints[1] - FrustumPoints[4];
	Vector3 v10 = FrustumPoints[0] - FrustumPoints[1];

	//down
	Vector3 v23 = FrustumPoints[3] - FrustumPoints[2];
	Vector3 v36 = FrustumPoints[6] - FrustumPoints[3];


	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	//near
	normal[0] = vec->Normalize(vec->Cross(v01, v12));
	//far
	normal[1] = vec->Normalize(vec->Cross(v65, v54));
	//left
	normal[2] = vec->Normalize(vec->Cross(v02, v26));
	//right
	normal[3] = vec->Normalize(vec->Cross(v53, v31));
	//up
	normal[4] = vec->Normalize(vec->Cross(v41, v10));
	//down
	normal[5] = vec->Normalize(vec->Cross(v23, v36));
	for (int i = 0; i < 8; i++){
		distance[0 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[0], normal[0]);
		distance[1 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[4], normal[1]);
		distance[2 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[0], normal[2]);
		distance[3 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[1], normal[3]);
		distance[4 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[0], normal[4]);
		distance[5 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[2], normal[5]);
	}

		for (int i = 0; i < 8; i++){
		if (distance[0 + i * normalLine] <= 0.0f &&
			distance[1 + i * normalLine] <= 0.0f &&
			distance[2 + i * normalLine] <= 0.0f &&
			distance[3 + i * normalLine] <= 0.0f &&
			distance[4 + i * normalLine] <= 0.0f &&
			distance[5 + i * normalLine] <= 0.0f) {
				return true;
			}
		}
	
	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();	

	std::unique_ptr<Vector> vec_ = std::make_unique<Vector>();


	Vector3 cameraTransform = { 0.0f,0.0f,0.0f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	/*Triangle triangle = {};
	triangle.vertices[0] = { -1.0f,0.0f,0.0f };
	triangle.vertices[1] = { 0.0f,1.0f,0.0f };
	triangle.vertices[2] = { 1.0f,0.0f,0.0f };*/
		
	/*Vector3 controlPoints[4] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
		{-0.53f,-0.26f,-0.15f}
	};*/

	uint32_t Linecolor = WHITE;

	//uint32_t triangleColor = WHITE;
		
	Plane plane{
		{0.0f,1.0f,0.0f},
		1.0f,
		WHITE
	};

	Vector3 translates[3] = {
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
	};


	Sphere sphere{
		.center = translates[0],
		.radius = 0.5f,
		.color = WHITE
	};

	AABB aabb1{
		.min{-0.5f,-0.5f,-0.5f},
		.max{0.5f,0.5f,0.5f},
		.color = WHITE
	};

	Segment segment_{ 
		{-0.7f,0.3f,0.0f},
		{1.0f,-0.3f,0.0f} };

	Vector3 rotate{ 0.0f,0.0f,0.0f };

	OBB obb{
		.center{-1.0f,0.0f,0.0f},
		.orientations = {{1.0f,0.0f,0.0f},
						 {0.0f,1.0f,0.0f},
						 {0.0f,0.0f,1.0f}},
		.size{0.2f,0.2f,0.2f},
		.color=WHITE
	};

	Vector2 localCameraPos{ 0.0f,0.0f };

	float cameraLength = -8.0f;

	Spring spring{
		.anchor = {0.0f,1.0f,0.0f},
		.naturalLength = 0.7f,
		.stiffness = 100.0f,
		.dampingCoefficient=2.0f
	};

	Ball ball = {
		.position = {1.0f,0.2f,0.6f},
		.mass = 2.0f,
		.radius = 0.05f,
		.color = BLUE
	};

	Pendulum pendulum = {
		.anchor = {0.0f,1.0f,0.0f},
		.length = 0.8f,
		.angle = 0.7f,
		.angularVelocity = 0.0f,
		.angularAcceleration = 0.0f
	};

	ConicalPendulum conicalPendulum = {
		.anchor = {0.0f,1.0f,0.0f},
		.length = 0.8f,
		.harfApexAngle = 0.7f,
		.angle = 0.0f,
		.angularVelocity = 0.0f
	};

	viewingFrustum viewingFrustum_ = {
		.translation_ = {0.0f,0.0f,0.0f},
		.rotate_ = {0.0f,0.0f,0.0f},
		.direction = {0.0f,0.0f,5.0f},
		.verticalFOV = static_cast<float>(45.0f * M_PI / 180.0f),
		.aspectRatio = (16.0f / 9.0f),
		.nearZ = 0.5f,
		.farZ = 1.5f,
	};


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
		Matrix4x4 rotateMatrix = matrix_->MakeRotateMatrix(rotate);
		for (int i = 0; i < 3; i++){
			obb.orientations[i].x = rotateMatrix.m[i][0];
			obb.orientations[i].y = rotateMatrix.m[i][1];
			obb.orientations[i].z = rotateMatrix.m[i][2];
		}


		
		ImGui::Begin("viewingFrustum");
		
		ImGui::DragFloat3("rotate", &viewingFrustum_.rotate_.x, 0.01f);
		ImGui::DragFloat3("translation", &viewingFrustum_.translation_.x, 0.01f);
		ImGui::SliderFloat("nearZ", &viewingFrustum_.nearZ, 0.1f, viewingFrustum_.farZ - 0.1f, "%.1f");
		ImGui::SliderFloat("farZ", &viewingFrustum_.farZ, viewingFrustum_.nearZ + 0.1f, 3.0f, "%.1f");
		ImGui::DragFloat3("direction", &viewingFrustum_.direction.x, 0.01f);
		//ImGui::DragFloat3("sphere_translation", &sphere.center.x, 0.01f);
		
		ImGui::End();

		ImGui::Begin("OBB");
		ImGui::DragFloat3("OBBrotate", &rotate.x, 0.01f);
		ImGui::DragFloat3("OBBCenter", &obb.center.x, 0.01f);
		ImGui::End();

		if (IsCollision(obb,viewingFrustum_)) {
			obb.color = RED;
		}
		else {
			obb.color = WHITE;
		}


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		//DrawAABB(aabb1,viewProjectionMatrix, viewportMatrix, aabb1.color);

		DrawOBB(obb, viewProjectionMatrix, viewportMatrix, obb.color);

		//DrawSphere(sphere, viewProjectionMatrix, viewportMatrix);

		//DrawLine(pendulum.anchor, sphere.center, viewProjectionMatrix, viewportMatrix, WHITE);
		
		//DrawLine(segment_.origin, segment_.diff, viewProjectionMatrix, viewportMatrix, WHITE);

		//DrawPlane(plane, viewProjectionMatrix, viewportMatrix, plane.color);
		/*DrawBezier(controlPoints[0], controlPoints[1], controlPoints[2],
			viewProjectionMatrix, viewportMatrix, lineColor);*/
		/*DrawCatmullRom(controlPoints[0], controlPoints[0], controlPoints[1], controlPoints[2],
			viewProjectionMatrix, viewportMatrix, lineColor);
		DrawCatmullRom(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3],
			viewProjectionMatrix, viewportMatrix, lineColor);
		DrawCatmullRom(controlPoints[1], controlPoints[2], controlPoints[3], controlPoints[3],
			viewProjectionMatrix, viewportMatrix, lineColor);*/


		/*DrawSphere(sphere0, viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere1, viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere2, viewProjectionMatrix, viewportMatrix);

		DrawLine(sphere0.center, sphere1.center, viewProjectionMatrix, viewportMatrix, Linecolor);
		DrawLine(sphere1.center, sphere2.center, viewProjectionMatrix, viewportMatrix, Linecolor);*/
		//DrawSphere(sphere3, viewProjectionMatrix, viewportMatrix);

		//DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, triangle.color);

		DrawViewingFrustum(viewingFrustum_, viewProjectionMatrix, viewportMatrix, GREEN);
		
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
