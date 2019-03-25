#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicity's sake...)
*/
void Camera::UpdateCamera(float msec)	{
	//Update the mouse by how much
	roll -= 5.0f * (Window::GetMouse()->GetWheelMovement());
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw	  -= (Window::GetMouse()->GetRelativePosition().x);

	roll = min(roll, 90.0f);
	roll = max(roll, -90.0f);

	/*	if (roll < 0) {
	roll += 360.0f;
	}
	if (roll > 360.0f) {
	roll -= 360.0f;
	}
	*/


	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch,90.0f);
	pitch = max(pitch,-90.0f);

	if(yaw <0) {
		yaw+= 360.0f;
	}
	if(yaw > 360.0f) {
		yaw -= 360.0f;
	}

	msec *= 10.0f;

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
//		position += Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(0,0,-1) * msec;
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0, 0, -1) * msec * speedz;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
//		position -= Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(0,0,-1) * msec;
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0, 0, -1) * msec * speedz;
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		/*position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * speedx;*/
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *  Vector3(-1, 0, 0) * msec * speedx;

	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * speedx;
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += msec * speedy;

	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= msec * speedy;

	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);
		

	//return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
	//Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
	//Matrix4::Translation(-position);

	/*return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
	Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
	Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
	Matrix4::Translation(-position);
*/
	//return	Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) * // Bizarre!!!
	//Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
	//Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
	//Matrix4::Translation(-position);

	//return	Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) * // Bizarre!!!
	//Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
	//Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
	//Matrix4::Translation(-position);

	//return	Matrix4::Rotation(-roll, Vector3(0, 0, 1)) * //A bit strange!
	//Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
	//Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
	//Matrix4::Translation(-position);

	//return	Matrix4::Rotation(-roll, Vector3(0, 0, 1)) * // Bizarre!!!
	//Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
	//Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
	//Matrix4::Translation(-position);
	

};
