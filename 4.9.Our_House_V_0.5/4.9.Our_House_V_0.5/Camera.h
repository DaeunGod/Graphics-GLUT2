#pragma once


GLfloat line_color[3] = { 255.0f, 255.0f, 130.0f };

GLfloat vertices_3Ddot[24][3] = {
	{ -1.0f, -1.0f, -1.0f },{ -1.0f, -1.0f, 1.0f },
{ -1.0f, -1.0f, 1.0f },{ 1.0f, -1.0f, 1.0f },
{ 1.0f, -1.0f, 1.0f },{ 1.0f, -1.0f, -1.0f },
{ 1.0f, -1.0f, -1.0f },{ -1.0f, -1.0f, -1.0f },

{ -1.0f, -1.0f, -1.0f },{ -1.0f, 1.0f, -1.0f },
{ -1.0f, -1.0f, 1.0f },{ -1.0f, 1.0f, 1.0f },
{ 1.0f, -1.0f, 1.0f },{ 1.0f, 1.0f, 1.0f },
{ 1.0f, -1.0f, -1.0f },{ 1.0f, 1.0f, -1.0f },

{ -1.0f, 1.0f, 1.0f },{ -1.0f, 1.0f, -1.0f },
{ 1.0f, 1.0f, 1.0f },{ -1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f },
{ -1.0f, 1.0f, -1.0f },{ 1.0f, 1.0f, -1.0f },
};
GLuint VBO_3Ddot, VAO_3Ddot;
GLfloat dotColor[2][3] = { { 255.0f, 255.0f, 0.0f },{ 0.0f, 255.0f, 255.0f } };

void define_3Ddot(void) {
	glGenBuffers(1, &VBO_3Ddot);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_3Ddot);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_3Ddot), &vertices_3Ddot[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_3Ddot);
	glBindVertexArray(VAO_3Ddot);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_3Ddot);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

typedef struct _CAMERA {
	glm::vec3 prepos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pos, center;
	glm::vec3 uaxis, vaxis, naxis;
	float fov_y, aspect_ratio, near_clip, far_clip;
	int move_status;
	int rotateDirection;

	bool isViewingVolumeVisible = true;
	bool cameraOrthoOrPerspective = false; /// Ortho: false, Perspective: true

	

	typedef struct _VIEWINGVOLUME {
		GLfloat line[24][3];
		GLuint VBO_line, VAO_line;
		bool cameraOrthoOrPerspective;
		float viewingVolumeWidth;
		float viewingVolumeHeight;

		void prepare_line(bool _cameraOrthoOrPerspective) { 	// y = x - win_height/4
			cameraOrthoOrPerspective = _cameraOrthoOrPerspective;
			for (int i = 0; i < 24; i++) {
				for (int j = 0; j < 3; j++) {
					line[i][j] = 0;
				}
			}

			// Initialize vertex buffer object.
			glGenBuffers(1, &VBO_line);

			glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

			// Initialize vertex array object.
			glGenVertexArrays(1, &VAO_line);
			glBindVertexArray(VAO_line);

			glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
			glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void update_line(float fov_y, float near_clip, float far_clip, float aspectRatio) { 	// y = x - win_height/4
			if (cameraOrthoOrPerspective) {
				calc_perspective_viewingvolume(fov_y, near_clip, far_clip, aspectRatio);
			}
			else {
				calc_orthogonal_viewingvolume(near_clip, far_clip);
			}
			
			//hFov = 2.0f*atan(aspectRatio*tan(vFov*0.5f));

			//printf("%f %f\n", fov_y*TO_RADIAN, distance);
			/// line
			

			glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void draw_line(void) { // Draw line in its MC.
							   // y = x - win_height/4
							   //glLineWidth(0.5f);
			GLfloat color[2][3] = { { 255.0f, 255.0f, 0.0f }, { 0.0f, 255.0f, 255.0f } };
			glUniform3fv(loc_primitive_color, 1, color[cameraOrthoOrPerspective]);
			glBindVertexArray(VAO_line);
			glDrawArrays(GL_LINES, 0, 24);
			glBindVertexArray(0);
			//glLineWidth(1.0f);
		}

		void calc_perspective_viewingvolume(float fov_y, float near_clip, float far_clip, float aspectRatio) {
			float y1 = far_clip * tan(fov_y*0.5f * TO_RADIAN);
			float x1 = y1 * aspectRatio;
			float y2 = near_clip * tan(fov_y*0.5f * TO_RADIAN);
			float x2 = y2 * aspectRatio;

			line[0][0] = x2;
			line[0][1] = y2;
			line[0][2] = -near_clip;
			line[1][0] = x1;
			line[1][1] = y1;
			line[1][2] = -far_clip;

			line[2][0] = x2;
			line[2][1] = -y2;
			line[2][2] = -near_clip;
			line[3][0] = x1;
			line[3][1] = -y1;
			line[3][2] = -far_clip;

			line[4][0] = -x2;
			line[4][1] = y2;
			line[4][2] = -near_clip;
			line[5][0] = -x1;
			line[5][1] = y1;
			line[5][2] = -far_clip;

			line[6][0] = -x2;
			line[6][1] = -y2;
			line[6][2] = -near_clip;
			line[7][0] = -x1;
			line[7][1] = -y1;
			line[7][2] = -far_clip;

			/// far plane
			line[8][0] = x1;
			line[8][1] = y1;
			line[8][2] = -far_clip;
			line[9][0] = x1;
			line[9][1] = -y1;
			line[9][2] = -far_clip;

			line[10][0] = x1;
			line[10][1] = -y1;
			line[10][2] = -far_clip;
			line[11][0] = -x1;
			line[11][1] = -y1;
			line[11][2] = -far_clip;

			line[12][0] = -x1;
			line[12][1] = -y1;
			line[12][2] = -far_clip;
			line[13][0] = -x1;
			line[13][1] = y1;
			line[13][2] = -far_clip;

			line[14][0] = -x1;
			line[14][1] = y1;
			line[14][2] = -far_clip;
			line[15][0] = x1;
			line[15][1] = y1;
			line[15][2] = -far_clip;

			/// near plane
			line[16][0] = x2;
			line[16][1] = y2;
			line[16][2] = -near_clip;
			line[17][0] = x2;
			line[17][1] = -y2;
			line[17][2] = -near_clip;

			line[18][0] = x2;
			line[18][1] = -y2;
			line[18][2] = -near_clip;
			line[19][0] = -x2;
			line[19][1] = -y2;
			line[19][2] = -near_clip;

			line[20][0] = -x2;
			line[20][1] = -y2;
			line[20][2] = -near_clip;
			line[21][0] = -x2;
			line[21][1] = y2;
			line[21][2] = -near_clip;

			line[22][0] = -x2;
			line[22][1] = y2;
			line[22][2] = -near_clip;
			line[23][0] = x2;
			line[23][1] = y2;
			line[23][2] = -near_clip;
		}

		void calc_orthogonal_viewingvolume(float near_clip, float far_clip) {
			line[0][0] = -viewingVolumeWidth;
			line[0][1] = -viewingVolumeHeight;
			line[0][2] = -near_clip;
			line[1][0] = -viewingVolumeWidth;
			line[1][1] = -viewingVolumeHeight;
			line[1][2] = -far_clip;

			line[2][0] = viewingVolumeWidth;
			line[2][1] = -viewingVolumeHeight;
			line[2][2] = -near_clip;
			line[3][0] = viewingVolumeWidth;
			line[3][1] = -viewingVolumeHeight;
			line[3][2] = -far_clip;

			line[4][0] = -viewingVolumeWidth;
			line[4][1] = viewingVolumeHeight;
			line[4][2] = -near_clip;
			line[5][0] = -viewingVolumeWidth;
			line[5][1] = viewingVolumeHeight;
			line[5][2] = -far_clip;

			line[6][0] = viewingVolumeWidth;
			line[6][1] = viewingVolumeHeight;
			line[6][2] = -near_clip;
			line[7][0] = viewingVolumeWidth;
			line[7][1] = viewingVolumeHeight;
			line[7][2] = -far_clip;

			/// far plane
			line[8][0] = viewingVolumeWidth;
			line[8][1] = viewingVolumeHeight;
			line[8][2] = -far_clip;
			line[9][0] = -viewingVolumeWidth;
			line[9][1] = viewingVolumeHeight;
			line[9][2] = -far_clip;

			line[10][0] = -viewingVolumeWidth;
			line[10][1] = viewingVolumeHeight;
			line[10][2] = -far_clip;
			line[11][0] = -viewingVolumeWidth;
			line[11][1] = -viewingVolumeHeight;
			line[11][2] = -far_clip;

			line[12][0] = -viewingVolumeWidth;
			line[12][1] = -viewingVolumeHeight;
			line[12][2] = -far_clip;
			line[13][0] = viewingVolumeWidth;
			line[13][1] = -viewingVolumeHeight;
			line[13][2] = -far_clip;

			line[14][0] = viewingVolumeWidth;
			line[14][1] = -viewingVolumeHeight;
			line[14][2] = -far_clip;
			line[15][0] = viewingVolumeWidth;
			line[15][1] = viewingVolumeHeight;
			line[15][2] = -far_clip;

			/// near plane
			line[16][0] = viewingVolumeWidth;
			line[16][1] = viewingVolumeHeight;
			line[16][2] = -near_clip;
			line[17][0] = viewingVolumeWidth;
			line[17][1] = -viewingVolumeHeight;
			line[17][2] = -near_clip;

			line[18][0] = viewingVolumeWidth;
			line[18][1] = -viewingVolumeHeight;
			line[18][2] = -near_clip;
			line[19][0] = -viewingVolumeWidth;
			line[19][1] = -viewingVolumeHeight;
			line[19][2] = -near_clip;

			line[20][0] = -viewingVolumeWidth;
			line[20][1] = -viewingVolumeHeight;
			line[20][2] = -near_clip;
			line[21][0] = -viewingVolumeWidth;
			line[21][1] = viewingVolumeHeight;
			line[21][2] = -near_clip;

			line[22][0] = -viewingVolumeWidth;
			line[22][1] = viewingVolumeHeight;
			line[22][2] = -near_clip;
			line[23][0] = viewingVolumeWidth;
			line[23][1] = viewingVolumeHeight;
			line[23][2] = -near_clip;
		}

	}VIEWINGVOLUME;
	VIEWINGVOLUME viewingVolume;

	void move(glm::vec3 dir) {
		pos += dir;
		prepos = pos;
	}

	void draw_3Ddot() {

		//GLfloat color[2][3] = { { 255.0f, 255.0f, 0.0f },{ 0.0f, 255.0f, 255.0f } };

		glBindVertexArray(VAO_3Ddot);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 0, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 2, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 4, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 6, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 8, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 10, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 12, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 14, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 16, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 18, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 20, 2);
		glUniform3fv(loc_primitive_color, 1, dotColor[cameraOrthoOrPerspective]);
		glDrawArrays(GL_LINES, 22, 2);
		glBindVertexArray(0);
	}

	void shake(glm::vec3 dir) {
		pos += dir;
	}

} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int currentCamera = 0;




//GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };


//GLfloat color[3] = { 255.0f, 255.0f, 0.0f };


void set_ViewMatrix_from_camera_frame(glm::mat4 &_ViewMatrix, CAMERA &_camera);
void renew_cam_orientation_rotation_around_axis(CAMERA &_camera, float angle, glm::vec3 rot_axis);
void camera_rotation_around_axis(CAMERA &_camera, float angle, glm::vec3 rot_axis);


void initialize_camera(void) {
	// called only once when the OpenGL system is initialized!!!
	// only ViewMatrix[*] are set up in this function.
	// ProjectionMatrix will be initialized in the reshape callback function when the window pops up.

	// Camera 0
	glm::vec3 _vup = glm::vec3(0.0f, 0.0f, 1.0f), _vpn;

	camera[CAMERA_MAIN].pos = glm::vec3(600.0f, 600.0f, 200.0f);
	camera[CAMERA_MAIN].center = glm::vec3(125.0f, 80.0f, 25.0f);
	//camera.uaxis = glm::vec3(1.0f, 0.0f, 0.0f);

	_vpn = camera[CAMERA_MAIN].pos - camera[CAMERA_MAIN].center;
	camera[CAMERA_MAIN].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_MAIN].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_MAIN].uaxis));
	camera[CAMERA_MAIN].naxis = glm::normalize(_vpn);

	camera[CAMERA_MAIN].rotateDirection = 0;
	//camera.naxis = glm::vec3(0.0f, -1.0f, 0.0f);

	camera[CAMERA_MAIN].move_status = 0;
	camera[CAMERA_MAIN].fov_y = 30.0f;
	camera[CAMERA_MAIN].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_MAIN].near_clip = 1.0f;
	camera[CAMERA_MAIN].far_clip = 10000.0f;
	camera[CAMERA_MAIN].cameraOrthoOrPerspective = true;

	camera[CAMERA_MAIN].viewingVolume.prepare_line(true);

	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_MAIN], camera[CAMERA_MAIN]);

	////Camera 1
	camera[CAMERA_ORTHO1].pos = glm::vec3(300.0f, 80.0f, 25.0f);
	camera[CAMERA_ORTHO1].center = glm::vec3(0.0f, 80.0f, 25.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[CAMERA_ORTHO1].pos - camera[CAMERA_ORTHO1].center;
	camera[CAMERA_ORTHO1].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_ORTHO1].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_ORTHO1].uaxis));
	camera[CAMERA_ORTHO1].naxis = glm::normalize(_vpn);

	camera[CAMERA_ORTHO1].move_status = 0;
	camera[CAMERA_ORTHO1].fov_y = 30.0f;
	camera[CAMERA_ORTHO1].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_ORTHO1].near_clip = 1.0f;
	camera[CAMERA_ORTHO1].far_clip = 300.0f;

	camera[CAMERA_ORTHO1].viewingVolume.prepare_line(false);


	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_ORTHO1], camera[CAMERA_ORTHO1]);

	////Camera 2
	camera[CAMERA_ORTHO2].pos = glm::vec3(120.0f, 90.0f, 100.0f);
	camera[CAMERA_ORTHO2].center = glm::vec3(120.0f, 90.0f, 0.0f);

	_vup = glm::vec3(-1.0f, 0.0f, 0.0f);
	_vpn = camera[CAMERA_ORTHO2].pos - camera[CAMERA_ORTHO2].center;
	camera[CAMERA_ORTHO2].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_ORTHO2].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_ORTHO2].uaxis));
	camera[CAMERA_ORTHO2].naxis = glm::normalize(_vpn);

	camera_rotation_around_axis(camera[CAMERA_ORTHO2], 90, camera[CAMERA_ORTHO2].naxis);

	camera[CAMERA_ORTHO2].move_status = 0;
	camera[CAMERA_ORTHO2].fov_y = 60.0f;
	camera[CAMERA_ORTHO2].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_ORTHO2].near_clip = 1.0f;
	camera[CAMERA_ORTHO2].far_clip = 100.0f;

	camera[CAMERA_ORTHO2].viewingVolume.prepare_line(false);

	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_ORTHO2], camera[CAMERA_ORTHO2]);

	////Camera 3
	camera[CAMERA_ORTHO3].pos = glm::vec3(120.0f, 200.0f, 25.0f);
	camera[CAMERA_ORTHO3].center = glm::vec3(120.0f, 0.0f, 25.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[CAMERA_ORTHO3].pos - camera[CAMERA_ORTHO3].center;
	camera[CAMERA_ORTHO3].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_ORTHO3].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_ORTHO3].uaxis));
	camera[CAMERA_ORTHO3].naxis = glm::normalize(_vpn);

	camera[CAMERA_ORTHO3].move_status = 0;
	camera[CAMERA_ORTHO3].fov_y = 90.0f;
	camera[CAMERA_ORTHO3].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_ORTHO3].near_clip = 1.0f;
	camera[CAMERA_ORTHO3].far_clip = 300.0f;

	camera[CAMERA_ORTHO3].viewingVolume.prepare_line(false);
	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_ORTHO3], camera[CAMERA_ORTHO3]);

	////Camera 4
	camera[CAMERA_PERSPECTIVE1].pos = glm::vec3(223.0f, 153.0f, 42.0f);
	camera[CAMERA_PERSPECTIVE1].center = glm::vec3(183.0f, 120.0f, 11.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[CAMERA_PERSPECTIVE1].pos - camera[CAMERA_PERSPECTIVE1].center;
	camera[CAMERA_PERSPECTIVE1].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_PERSPECTIVE1].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_PERSPECTIVE1].uaxis));
	camera[CAMERA_PERSPECTIVE1].naxis = glm::normalize(_vpn);

	camera[CAMERA_PERSPECTIVE1].move_status = 0;
	camera[CAMERA_PERSPECTIVE1].fov_y = 90.0f;
	camera[CAMERA_PERSPECTIVE1].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_PERSPECTIVE1].near_clip = 1.0f;
	camera[CAMERA_PERSPECTIVE1].far_clip = 100.0f;
	camera[CAMERA_PERSPECTIVE1].cameraOrthoOrPerspective = true;

	camera[CAMERA_PERSPECTIVE1].viewingVolume.prepare_line(true);
	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_PERSPECTIVE1], camera[CAMERA_PERSPECTIVE1]);

	////Camera 5
	camera[CAMERA_PERSPECTIVE2].pos = glm::vec3(74.0f, 115.0f, 42.0f);
	camera[CAMERA_PERSPECTIVE2].center = glm::vec3(74.0f, 66.0f, 2.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[CAMERA_PERSPECTIVE2].pos - camera[CAMERA_PERSPECTIVE2].center;
	camera[CAMERA_PERSPECTIVE2].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_PERSPECTIVE2].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_PERSPECTIVE2].uaxis));
	camera[CAMERA_PERSPECTIVE2].naxis = glm::normalize(_vpn);

	camera[CAMERA_PERSPECTIVE2].move_status = 0;
	camera[CAMERA_PERSPECTIVE2].fov_y = 90.0f;
	camera[CAMERA_PERSPECTIVE2].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_PERSPECTIVE2].near_clip = 1.0f;
	camera[CAMERA_PERSPECTIVE2].far_clip = 200.0f;
	camera[CAMERA_PERSPECTIVE2].cameraOrthoOrPerspective = true;

	camera[CAMERA_PERSPECTIVE2].viewingVolume.prepare_line(true);
	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_PERSPECTIVE2], camera[CAMERA_PERSPECTIVE2]);

	////Camera 6
	camera[CAMERA_PERSPECTIVE3].pos = glm::vec3(122.0f, 55.0f, 42.0f);
	camera[CAMERA_PERSPECTIVE3].center = glm::vec3(135.0f, 105.0f, 11.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[CAMERA_PERSPECTIVE3].pos - camera[CAMERA_PERSPECTIVE3].center;
	camera[CAMERA_PERSPECTIVE3].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_PERSPECTIVE3].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_PERSPECTIVE3].uaxis));
	camera[CAMERA_PERSPECTIVE3].naxis = glm::normalize(_vpn);

	camera[CAMERA_PERSPECTIVE3].move_status = 0;
	camera[CAMERA_PERSPECTIVE3].fov_y = 80.0f;
	camera[CAMERA_PERSPECTIVE3].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_PERSPECTIVE3].near_clip = 1.0f;
	camera[CAMERA_PERSPECTIVE3].far_clip = 80.0f;
	camera[CAMERA_PERSPECTIVE3].cameraOrthoOrPerspective = true;

	//printf("%d \n", camera[CAMERA_PERSPECTIVE3].cameraOrthoOrPerspective);

	camera[CAMERA_PERSPECTIVE3].viewingVolume.prepare_line(true);
	set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_PERSPECTIVE3], camera[CAMERA_PERSPECTIVE3]);

	////Camera 7
	camera[CAMERA_PERSPECTIVE4].pos = glm::vec3(120.0f, 120.0f, 100.0f);
	camera[CAMERA_PERSPECTIVE4].center = glm::vec3(130.0f, 55.0f, 33.5f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[CAMERA_PERSPECTIVE3].pos - camera[CAMERA_PERSPECTIVE4].center;
	camera[CAMERA_PERSPECTIVE4].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[CAMERA_PERSPECTIVE4].vaxis = glm::normalize(glm::cross(_vpn, camera[CAMERA_PERSPECTIVE4].uaxis));
	camera[CAMERA_PERSPECTIVE4].naxis = glm::normalize(_vpn);

	camera[CAMERA_PERSPECTIVE4].move_status = 0;
	camera[CAMERA_PERSPECTIVE4].fov_y = 30.0f;
	camera[CAMERA_PERSPECTIVE4].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[CAMERA_PERSPECTIVE4].near_clip = 1.0f;
	camera[CAMERA_PERSPECTIVE4].far_clip = 100.0f;
	camera[CAMERA_PERSPECTIVE4].cameraOrthoOrPerspective = true;

	//printf("%d \n", camera[CAMERA_PERSPECTIVE3].cameraOrthoOrPerspective);

	camera[CAMERA_PERSPECTIVE4].viewingVolume.prepare_line(true);
	//set_ViewMatrix_from_camera_frame(ViewMatrix[CAMERA_PERSPECTIVE4], camera[CAMERA_PERSPECTIVE4]);


	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
		camera[i].isViewingVolumeVisible = false;
	}
	define_3Ddot();
	camera[CAMERA_MAIN].isViewingVolumeVisible = true;
	camera[CAMERA_ORTHO1].isViewingVolumeVisible = true;
	camera[CAMERA_ORTHO2].isViewingVolumeVisible = true;
	camera[CAMERA_ORTHO3].isViewingVolumeVisible = true;
	//camera[CAMERA_PERSPECTIVE4].isViewingVolumeVisible = true;
	

	//camera_selected = 0;
}

void draw_camera_axex(int index) {
	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
		if (i == index)
			continue;

		glm::mat4 obj = glm::translate(glm::mat4(1), camera[i].pos);
		glm::mat4 rot = glm::mat4(1);

		//if (camera[i].isViewingVolumeVisible) {
		//	/*rot[0][0] = camera[i].uaxis.x; rot[1][0] = camera[i].uaxis.y; rot[2][0] = camera[i].uaxis.z;
		//	rot[0][1] = camera[i].vaxis.x; rot[1][1] = camera[i].vaxis.y; rot[2][1] = camera[i].vaxis.z;
		//	rot[0][2] = camera[i].naxis.x; rot[1][2] = camera[i].naxis.y; rot[2][2] = camera[i].naxis.z;*/
		//	rot[0][0] = camera[i].uaxis.x; rot[1][0] = camera[i].vaxis.x; rot[2][0] = camera[i].naxis.x;
		//	rot[0][1] = camera[i].uaxis.y; rot[1][1] = camera[i].vaxis.y; rot[2][1] = camera[i].naxis.y;
		//	rot[0][2] = camera[i].uaxis.z; rot[1][2] = camera[i].vaxis.z; rot[2][2] = camera[i].naxis.z;

		//	obj = obj * rot;
		//	ModelViewMatrix = ViewMatrix[index] * obj;
		//	ModelViewProjectionMatrix = ProjectionMatrix[index] * ModelViewMatrix;
		//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		//	camera[i].viewingVolume.draw_line();
		//}
		glLineWidth(1.0f);
		rot[0][0] = camera[i].uaxis.x; rot[1][0] = camera[i].vaxis.x; rot[2][0] = camera[i].naxis.x;
		rot[0][1] = camera[i].uaxis.y; rot[1][1] = camera[i].vaxis.y; rot[2][1] = camera[i].naxis.y;
		rot[0][2] = camera[i].uaxis.z; rot[1][2] = camera[i].vaxis.z; rot[2][2] = camera[i].naxis.z;

		obj = obj * rot;
		ModelViewMatrix = ViewMatrix[index] * obj;
		ModelViewProjectionMatrix = ProjectionMatrix[index] * ModelViewMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (camera[i].isViewingVolumeVisible) {
			camera[i].viewingVolume.draw_line();
		}
		
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(WC_AXIS_LENGTH*0.5f, WC_AXIS_LENGTH *0.5f, WC_AXIS_LENGTH * 0.5f));
		ModelViewProjectionMatrix = ProjectionMatrix[index] * ModelViewMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		glBindVertexArray(VAO_axes);
		glUniform3fv(loc_primitive_color, 1, axes_color[0]);
		glDrawArrays(GL_LINES, 0, 2);
		glUniform3fv(loc_primitive_color, 1, axes_color[1]);
		glDrawArrays(GL_LINES, 2, 2);
		glUniform3fv(loc_primitive_color, 1, axes_color[2]);
		glDrawArrays(GL_LINES, 4, 2);
		glBindVertexArray(0);

		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1/WC_AXIS_LENGTH*0.5f, 1/WC_AXIS_LENGTH *0.5f, 1/WC_AXIS_LENGTH * 0.5f));
		ModelViewProjectionMatrix = ProjectionMatrix[index] * ModelViewMatrix;
		glLineWidth(10.0f);
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		camera[i].draw_3Ddot();
		glLineWidth(1.0f);
		
	}
}