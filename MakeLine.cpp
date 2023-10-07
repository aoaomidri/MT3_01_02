#include "MakeLine.h"
bool MakeLine::LoadChackItem(const std::string& directoryPath, const std::string& itemName) {
	// 書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + kItemName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Adjustment_Item", 0);
		ifs.close();
		return false;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(kItemName);
	//未登録チェック
	if (itGroup != root.end()){
		return true;
	}
	else{
		return false;
	}
}

void MakeLine::Initialize() {		

	if (LoadChackItem(kDirectoryPath,kItemName)){
		chackOnlyNumber = 1;

		LoadFiles();
	}
	else {
		chackOnlyNumber = 0;

		for (int i = 0; i < 4; i++)
		{
			ControlPoints.push_back(controlPoint[i]);
		}

		for (int i = 0; i < 4; i++)
		{
			spheres.push_back(sphere[i]);
		}

	}

	LastLinePass = static_cast<int>(ControlPoints.size()) - 2;
	
}

void MakeLine::Update() {
	for (size_t i = 0; i < ControlPoints.size(); ++i) {
		spheres[i].center = ControlPoints[i];
	}

	if (isMove) {
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
	if (linePass >= 1 && linePass != LastLinePass) {
		Vector3 p = vec_->makeCatmullRom(ControlPoints[linePass - 1], ControlPoints[linePass], ControlPoints[linePass + 1], ControlPoints[linePass + 2], t);
		PLsphere.center = p;
	}
	if (linePass == LastLinePass) {
		Vector3 p = vec_->makeCatmullRom(ControlPoints[LastLinePass - 1], ControlPoints[LastLinePass], ControlPoints[LastLinePass + 1], ControlPoints[LastLinePass + 1], t);
		PLsphere.center = p;
	}


}

void MakeLine::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	DrawImgui();

	DrawCatmullRom(ControlPoints[0], ControlPoints[0], ControlPoints[1], ControlPoints[2],
		viewProjectionMatrix, viewportMatrix, Linecolor);
	for (int i = 1; i < LastLinePass; i++) {
		DrawCatmullRom(ControlPoints[i - 1], ControlPoints[i], ControlPoints[i + 1], ControlPoints[i + 2],
			viewProjectionMatrix, viewportMatrix, Linecolor);
	}

	DrawCatmullRom(ControlPoints[LastLinePass - 1], ControlPoints[LastLinePass], ControlPoints[LastLinePass + 1], ControlPoints[LastLinePass + 1],
		viewProjectionMatrix, viewportMatrix, Linecolor);

	for (size_t i = 0; i < spheres.size(); ++i) {
		DrawSphere(spheres[i], viewProjectionMatrix, viewportMatrix);
	}

	DrawSphere(PLsphere, viewProjectionMatrix, viewportMatrix);

}

void MakeLine::DrawImgui() {
	ImGui::Begin("PL");
	ImGui::DragFloat("point", &point, 0.01f);
	ImGui::Checkbox("StartMove", &isMove);
	ImGui::Text("Size = %d", ControlPoints.size());
	ImGui::Text("Number = %d", chackOnlyNumber);
	ImGui::End();

	ImGui::Begin("Catmull-Rom");
	for (size_t i = 0; i < ControlPoints.size(); ++i) {
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
	if (ImGui::Button("Save Element")) {
		//曲線保存用
		json root;

		root = json::object();

		for (size_t i = 0; i < ControlPoints.size(); ++i) {
			root[kItemName][i] = json::array({ ControlPoints[i].x,ControlPoints[i].y, ControlPoints[i].z });


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
}

void MakeLine::LoadFiles() {
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

void MakeLine::LoadFile(const std::string& groupName) {

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
		Sphere sphere_{
			.center = {0.0f},
			.radius = 0.01f,
			.color = BLACK
		};

		from_json(root_, v);

		sphere_.center = v;

		ControlPoints.push_back(v);

		spheres.push_back(sphere_);
	}

}

void MakeLine::from_json(const nlohmann::json& j, Vector3& v) {
	v.x = j.at(0).get<float>();
	v.y = j.at(1).get<float>();
	v.z = j.at(2).get<float>();
}

void MakeLine::DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 8;
	const float kLatEvery = float(M_PI / kSubdivision);//緯度一つ分の角度
	const float kLonEvery = float((M_PI * 2.0f) / kSubdivision);//経度一つ分の角度
	//緯度の方向に分割-π/2～π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = float(-M_PI / 2.0f + kLatEvery * latIndex);//現在の緯度(θ)
		//経度の方向に分割 0～2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
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

void MakeLine::DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3,
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

Vector3 MakeLine::TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
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
