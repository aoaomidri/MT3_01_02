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

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector* vec = new Vector;
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
	Vector* vec = new Vector;

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
		t>=0.0f&&t<=1.0f) {
		
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
	Vector3 cameraTransform = { 0.0f,2.85f,-10.0f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	Triangle triangle = {};
	triangle.vertices[0] = { -1.0f,0.0f,0.0f };
	triangle.vertices[1] = { 0.0f,1.0f,0.0f };
	triangle.vertices[2] = { 1.0f,0.0f,0.0f };

	

	uint32_t color = WHITE;

	uint32_t triangleColor = WHITE;

	Plane plane{
		{0.0f,1.0f,0.0f},
		1.0f
	};

	Segment segment_{ {-1.0f,-0.5f,0.0f},{1.5f,1.0f,1.0f} };
	
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

		Vector3 start = TransScreen(segment_.origin, viewProjectionMatrix, viewportMatrix);
		Vector3 end = TransScreen(segment_.origin + segment_.diff, viewProjectionMatrix, viewportMatrix);

		if (IsCollision( triangle,segment_, viewProjectionMatrix, viewportMatrix)) {
			triangleColor = RED;
		}
		else {
			triangleColor = WHITE;
		}
		
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTransform.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Triangle.v0", &plane.normal.x, 0.01f);
		ImGui::DragFloat3("Triangle.v1", &plane.normal.x, 0.01f);
		ImGui::DragFloat3("Triangle.v2", &plane.normal.x, 0.01f);
		ImGui::DragFloat3("Segment.origin", &segment_.origin.x, 0.01f);
		ImGui::DragFloat3("Segment.diff", &segment_.diff.x, 0.01f);
		

		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);

		//DrawPlane(plane, viewProjectionMatrix, viewportMatrix, WHITE);

		DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, triangleColor);
		
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
