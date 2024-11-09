#include "Camera.h"
#include "Window.h"
#include "vector"
#include <algorithm>


void Camera::settrack(Vector3 map, Vector3 mapsize)
{
	mapCenter = mapsize * map; // Center of the map
	float radius = mapsize.x * 0.7f; // Radius of the circle
	int numTrackPoints = 10; // Number of points in the circle path

	for (int i = 0; i < numTrackPoints; ++i) {
		float angle = 3.14 + (float)i / numTrackPoints * 2.0f * 3.14; // Divide the circle equally
		float x = mapCenter.x + radius * cos(angle);
		float z = mapCenter.z + radius * sin(angle);
		float y = mapCenter.y + mapsize.y * 0.5f; // Adjust the height as needed
		trackPoints.push_back(Vector3(x, y, z));
	}

}


void Camera::UpdateCamera(float dt) {

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_G)) {  // Toggle key to switch between auto and free mode
		ToggleFreeMode();
	}

	if (freeMode)
	{
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);
		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}
		Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
		Vector3 forward = rotation * Vector3(0, 0, -1);
		Vector3 right = rotation * Vector3(1, 0, 0);
		float speed = 100.0f * dt;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= forward * speed;

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position -= right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position += right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y += speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y -= speed;
		}
	}
	else {
		
		if (trackPoints.size() < 2) return;  // Ensure there are enough points

		// Find the current and next points
		int currentPoint = (int)trackProgress;
		int nextPoint = (currentPoint + 1) % trackPoints.size();

		Vector3 startPos = trackPoints[currentPoint];
		Vector3 endPos = trackPoints[nextPoint];
		float t = trackProgress - currentPoint; // Fraction of movement between points

		// Interpolate between points for smooth movement
		position = startPos * (1.0f - t) + endPos * t;

		// Increment track progress for smooth circular movement
		trackProgress += dt * 0.1f; // Adjust speed factor as needed
		if (trackProgress >= trackPoints.size()) {
			trackProgress = 0.0f; // Loop back to the start
		}

		// Adjust yaw and pitch to always face the map center
		Vector3 directionToCenter = (mapCenter - position).Normalised();
		yaw = atan2(-directionToCenter.x, -directionToCenter.z) * (180.0f / 3.14);
		pitch = asin(-directionToCenter.y) * (180.0f / 3.14);
	}

}
Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) * Matrix4::Translation(-position);
};
