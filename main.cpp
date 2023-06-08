#include"main.h"
#include<imgui.h>
const char kWindowTitle[] = "LE2A_20_ムラカミ_アオイ";


Vector3 TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	Matrix* matrix = new Matrix;
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
		backPosirion = TransScreen(backPosirion, viewProjectionMatrix, viewportMatrix);
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

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 8;
	const float kLatEvery = float(M_PI / kSubdivision);//緯度一つ分の角度
	const float kLonEvery = float((M_PI * 2.0f) / kSubdivision);//経度一つ分の角度
	//緯度の方向に分割-π/2～π/2
	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex){
		float lat = -float( ((M_PI) / 2.0f) + (kLatEvery * latIndex));//現在の緯度(θ)
		//経度の方向に分割 0～2π
		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex){
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

// スカラー倍
Vector3 Multiply(float scalar, const Vector3 & Vec) {
	Vector3 result;
	result.x = Vec.x * scalar;
	result.y = Vec.y * scalar;
	result.z = Vec.z * scalar;
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
	result = Multiply(Dot(v1, vec->Normalize(v2)), vec->Normalize(v2));

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

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	Matrix* matrix = new Matrix;
	Vector* vec = new Vector;
	Vector3 cameraTransform = { 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	Sphere sphere1 = {
			{-1.0f,0.0f,0.0f},
			0.5f,
			BLACK
	};
	Sphere sphere2 = {
			{1.0f,0.0f,0.0f},
			0.5f,
			BLACK
	};

	Vector3 vector1 = { 1,2,1 };
	Vector3 vector2 = { -2,2,4 };
	float result = 0.0f;
	
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
		Matrix4x4 cameraMatrix = matrix->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTransform);
		Matrix4x4 viewMatrix = matrix->Inverce(cameraMatrix);
		Matrix4x4 projectionMatrix = matrix->MakePerspectiveFovMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = matrix->Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = matrix->MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		result = vec->VectorAngle(vector1, vector2) * 180.0f / static_cast<float>(M_PI);

		if (IsCollision(sphere1, sphere2)) {
			sphere1.color = RED;
		}
		else {
			sphere1.color = BLACK;
		}
		
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTransform.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere1Center", &sphere1.center.x, 0.01f);
		ImGui::DragFloat("Sphere1Radius", &sphere1.radius, 0.01f);
		ImGui::DragFloat3("Sphere2Center", &sphere2.center.x, 0.01f);
		ImGui::DragFloat("Sphere2Radius", &sphere2.radius, 0.01f);
		ImGui::End();

		Novice::ScreenPrintf(0, 0, "Result %.5f", result);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere1, viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere2, viewProjectionMatrix, viewportMatrix);
		
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
