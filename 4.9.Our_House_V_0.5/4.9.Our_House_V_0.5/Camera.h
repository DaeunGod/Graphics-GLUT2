#pragma once


GLfloat line_color[3] = { 0.4533f, 0.2334f, 0.422f };

typedef struct _CAMERA {
	glm::vec3 pos, center;
	glm::vec3 uaxis, vaxis, naxis;
	float fov_y, aspect_ratio, near_clip, far_clip;
	int move_status;
	int rotateDirection;

	bool isViewingVolumeVisible = true;

	typedef struct _VIEWINGVOLUME {
		GLfloat line[24][3];
		GLuint VBO_line, VAO_line;

		void prepare_line(void) { 	// y = x - win_height/4
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
			float distance = far_clip - near_clip;
			float y1 = distance * tan(fov_y*0.5f * TO_RADIAN);
			//float hFov = 2.0f*atan(aspectRatio*tan(fov_y*0.5f*TO_RADIAN));
			//float hFov = x1 * aspectRatio;
			//float y1 = distance * tan(hFov*0.5*TO_RADIAN);
			float x1 = y1 * aspectRatio;
			float y2 = near_clip * tan(fov_y*0.5f * TO_RADIAN);
			float x2 = y2 * aspectRatio;
			//hFov = 2.0f*atan(aspectRatio*tan(vFov*0.5f));

			printf("%f %f\n", fov_y*TO_RADIAN, distance);
			/// line
			line[0][0] = x2;
			line[0][1] = y2;
			line[0][2] = -near_clip;
			line[1][0] = x1;
			line[1][1] = y1;
			line[1][2] = -distance;

			line[2][0] = x2;
			line[2][1] = -y2;
			line[2][2] = -near_clip;
			line[3][0] = x1;
			line[3][1] = -y1;
			line[3][2] = -distance;

			line[4][0] = -x2;
			line[4][1] = y2;
			line[4][2] = -near_clip;
			line[5][0] = -x1;
			line[5][1] = y1;
			line[5][2] = -distance;

			line[6][0] = -x2;
			line[6][1] = -y2;
			line[6][2] = -near_clip;
			line[7][0] = -x1;
			line[7][1] = -y1;
			line[7][2] = -distance;

			/// far plane
			line[8][0] = x1;
			line[8][1] = y1;
			line[8][2] = -distance;
			line[9][0] = x1;
			line[9][1] = -y1;
			line[9][2] = -distance;

			line[10][0] = x1;
			line[10][1] = -y1;
			line[10][2] = -distance;
			line[11][0] = -x1;
			line[11][1] = -y1;
			line[11][2] = -distance;

			line[12][0] = -x1;
			line[12][1] = -y1;
			line[12][2] = -distance;
			line[13][0] = -x1;
			line[13][1] = y1;
			line[13][2] = -distance;

			line[14][0] = -x1;
			line[14][1] = y1;
			line[14][2] = -distance;
			line[15][0] = x1;
			line[15][1] = y1;
			line[15][2] = -distance;

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

			glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void draw_line(void) { // Draw line in its MC.
							   // y = x - win_height/4
							   //glLineWidth(0.5f);
			glUniform3fv(loc_primitive_color, 1, line_color);
			glBindVertexArray(VAO_line);
			glDrawArrays(GL_LINES, 0, 24);
			glBindVertexArray(0);
			//glLineWidth(1.0f);
		}

	}VIEWINGVOLUME;
	VIEWINGVOLUME viewingVolume;

	void move(glm::vec3 dir) {
		pos += dir;
	}
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int currentCamera = 0;

void set_ViewMatrix_from_camera_frame(glm::mat4 &_ViewMatrix, CAMERA &_camera);


void initialize_camera(void) {
	// called only once when the OpenGL system is initialized!!!
	// only ViewMatrix[*] are set up in this function.
	// ProjectionMatrix will be initialized in the reshape callback function when the window pops up.

	// Camera 0
	glm::vec3 _vup = glm::vec3(0.0f, 0.0f, 1.0f), _vpn;


	camera[0].pos = glm::vec3(600.0f, 600.0f, 200.0f);
	camera[0].center = glm::vec3(125.0f, 80.0f, 25.0f);
	//camera.uaxis = glm::vec3(1.0f, 0.0f, 0.0f);

	_vpn = camera[0].pos - camera[0].center;
	camera[0].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[0].vaxis = glm::normalize(glm::cross(_vpn, camera[0].uaxis));
	camera[0].naxis = glm::normalize(_vpn);

	camera[0].rotateDirection = 0;
	//camera.naxis = glm::vec3(0.0f, -1.0f, 0.0f);

	camera[0].move_status = 0;
	camera[0].fov_y = 30.0f;
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[0].near_clip = 1.0f;
	camera[0].far_clip = 10000.0f;

	camera[0].viewingVolume.prepare_line();

	set_ViewMatrix_from_camera_frame(ViewMatrix[0], camera[0]);

	////Camera 1
	camera[1].pos = glm::vec3(500.0f, 90.0f, 25.0f);
	camera[1].center = glm::vec3(0.0f, 90.0f, 25.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[1].pos - camera[1].center;
	camera[1].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[1].vaxis = glm::normalize(glm::cross(_vpn, camera[1].uaxis));
	camera[1].naxis = glm::normalize(_vpn);

	camera[1].move_status = 0;
	camera[1].fov_y = 30.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[1].near_clip = 1.0f;
	camera[1].far_clip = 300.0f;

	camera[1].viewingVolume.prepare_line();


	set_ViewMatrix_from_camera_frame(ViewMatrix[1], camera[1]);


	camera[2].pos = glm::vec3(120.0f, 90.0f, 200.0f);
	camera[2].center = glm::vec3(120.0f, 90.0f, 0.0f);

	_vup = glm::vec3(-1.0f, 0.0f, 0.0f);
	_vpn = camera[2].pos - camera[2].center;
	camera[2].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[2].vaxis = glm::normalize(glm::cross(_vpn, camera[2].uaxis));
	camera[2].naxis = glm::normalize(_vpn);

	camera[2].move_status = 0;
	camera[2].fov_y = 45.0f;
	camera[2].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[2].near_clip = 100.0f;
	camera[2].far_clip = 300.0f;

	camera[2].viewingVolume.prepare_line();

	set_ViewMatrix_from_camera_frame(ViewMatrix[2], camera[2]);

	camera[3].pos = glm::vec3(223.0f, 153.0f, 42.0f);
	camera[3].center = glm::vec3(183.0f, 120.0f, 11.0f);

	_vup = glm::vec3(0.0f, 0.0f, 1.0f);
	_vpn = camera[3].pos - camera[3].center;
	camera[3].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[3].vaxis = glm::normalize(glm::cross(_vpn, camera[3].uaxis));
	camera[3].naxis = glm::normalize(_vpn);

	camera[3].move_status = 0;
	camera[3].fov_y = 90.0f;
	camera[3].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[3].near_clip = 1.0f;
	camera[3].far_clip = 100.0f;

	camera[3].viewingVolume.prepare_line();
	set_ViewMatrix_from_camera_frame(ViewMatrix[3], camera[3]);

	camera[0].isViewingVolumeVisible = false;
	camera[1].isViewingVolumeVisible = false;
	//camera[2].isViewingVolumeVisible = false;
	camera[3].isViewingVolumeVisible = false;
	//

	//camera_selected = 0;
}