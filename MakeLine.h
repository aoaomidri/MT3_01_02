#pragma once

#include<imgui.h>
#include<algorithm>
#include<fstream>
#include<iostream>
#include"Matrix.h"
#include<memory>
#include<nlohmann/json.hpp>

class MakeLine{
public:
	struct Sphere {
		Vector3 center;
		float radius;
		uint32_t color;
	};
	//初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	

private:
	std::unique_ptr<Matrix> matrix_ = std::make_unique<Matrix>();

	std::unique_ptr<Vector> vec_ = std::make_unique<Vector>();

	Vector3 controlPoint[4] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
		{-0.53f,-0.26f,-0.15f}
	};

	std::vector<Vector3> ControlPoints;

	Sphere PLsphere{
		.center = controlPoint[0],
		.radius = 0.05f,
		.color = RED
	};

	Sphere sphere[4]{
		{{0.0f},0.02f,BLACK},
		{{0.0f},0.02f,BLACK},
		{{0.0f},0.02f,BLACK},
		{{0.0f},0.02f,BLACK}
	};

	std::vector<Sphere> spheres;

	//キャトムル-ロム移動用
	//分割数
	uint32_t Linecolor = WHITE;

	const uint32_t divisionNumber = 8;
	float point = 0.0f;
	float t = 0.0f;

	int chackOnlyNumber = 0;

	int linePass = 0;

	const int firstLinePass = 0;
	int LastLinePass = 2;

	bool isMove = false;

	using json = nlohmann::json;

	const std::string kDirectoryPath = "Resources/Elements/";

	const std::string kDirectoryName = "Resources/Elements";

	const std::string kItemName = "Points";

	Vector3 TransScreen(const Vector3& transform, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	void DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3,
		const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

	void LoadFiles();

	void LoadFile(const std::string& groupName);

	void from_json(const json& j, Vector3& v);

	void DrawImgui();

	bool LoadChackItem(const std::string& directoryPath, const std::string& itemName);

};

