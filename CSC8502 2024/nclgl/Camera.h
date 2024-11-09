#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "vector"

class Camera
{
public:
	Camera(void) : freeMode(false), trackProgress(0.0f) {
		yaw = 0.0f;
		pitch = 0.0f;
	};
	 Camera(float pitch, float yaw, Vector3 position) : freeMode(false), trackProgress(0.0f) {
		this -> pitch = pitch;
		this -> yaw = yaw;
		this -> position = position;
	}
	~Camera(void) {};
	 void UpdateCamera(float dt = 1.0f);
	 void settrack(Vector3 map, Vector3 mapsize);
	 Matrix4 BuildViewMatrix();
	 Vector3 GetPosition() const { return position; }
	 void SetPosition(Vector3 val) { position = val; }
	 float GetYaw() const { return yaw; }
	 void SetYaw(float y) { yaw = y; }
	 float GetPitch() const { return pitch; }
	 void SetPitch(float p) { pitch = p; }

	 void AddTrackPoint(const Vector3& point) { trackPoints.push_back(point); }
	 void ToggleFreeMode() { freeMode = !freeMode; trackProgress = 0.0f; }
	
protected:
	float yaw;
	float pitch;
	Vector3 position;
	Vector3 mapCenter;

	bool freeMode = false;
	std::vector<Vector3> trackPoints;
	float trackProgress;

};

