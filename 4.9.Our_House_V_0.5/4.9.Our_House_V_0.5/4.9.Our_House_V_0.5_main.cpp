#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

#define CAM_TRANSLATION_SPEED 0.25f
#define CAM_ROTATION_SPEED 0.1f

#define NUMBER_OF_CAMERAS 4

typedef struct _CAMERA {
	glm::vec3 pos, center;
	glm::vec3 uaxis, vaxis, naxis;
	float fov_y, aspect_ratio, near_clip, far_clip;
	int move_status;
	int rotateDirection;

	void move(glm::vec3 dir) {
		pos += dir;
	}
} CAMERA;
CAMERA camera[NUMBER_OF_CAMERAS];
int currentCamera = 0;

glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ModelViewMatrix, ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];

#include "Object_Definitions.h"

typedef struct _VIEWPORT {
	int x, y, w, h;
} VIEWPORT;
VIEWPORT viewport[NUMBER_OF_CAMERAS];



typedef struct _CALLBACK_CONTEXT {
	int left_button_status, rotation_mode_cow, timestamp_cow;
	int prevx, prevy;
	float rotation_angle_cow;
} CALLBACK_CONTEXT;
CALLBACK_CONTEXT cc;


bool keyState[108] = { 0 };
void initialize_camera(void);
void motion(int x, int y);
void mousepress(int button, int state, int x, int y);
void mousewheel(int button, int dir, int x, int y);
void keySpecialOperation();

void set_ViewMatrix_from_camera_frame(glm::mat4 &_ViewMatrix, CAMERA &_camera) {
	_ViewMatrix = glm::mat4(1.0f);
	_ViewMatrix[0].x = _camera.uaxis.x;
	_ViewMatrix[0].y = _camera.vaxis.x;
	_ViewMatrix[0].z = _camera.naxis.x;
	
	_ViewMatrix[1].x = _camera.uaxis.y;
	_ViewMatrix[1].y = _camera.vaxis.y;
	_ViewMatrix[1].z = _camera.naxis.y;
	
	_ViewMatrix[2].x = _camera.uaxis.z;
	_ViewMatrix[2].y = _camera.vaxis.z;
	_ViewMatrix[2].z = _camera.naxis.z;
	
	_ViewMatrix = glm::translate(_ViewMatrix, -_camera.pos);
}

void renew_cam_position_along_axis(CAMERA &_camera, float del, glm::vec3 trans_axis) {
	_camera.pos += CAM_TRANSLATION_SPEED * del*trans_axis;
}

void renew_cam_orientation_rotation_around_axis (CAMERA &_camera, float angle, glm::vec3 rot_axis) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED*TO_RADIAN*angle, rot_axis));

	_camera.uaxis = RotationMatrix * _camera.uaxis;
	_camera.vaxis = RotationMatrix * _camera.vaxis;
	_camera.naxis = RotationMatrix * _camera.naxis;
}

void display(void) {
	keySpecialOperation();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
		glViewport(viewport[i].x, viewport[i].y, 
			viewport[i].w, viewport[i].h);

		glLineWidth(2.0f);
		draw_axes(ViewMatrix[i], i);
		//for(int j=0; j<NUMBER_OF_CAMERAS; j++)
		
		draw_camera_axex(i);
		//printf("%f %f %f\n", camera[i].pos.x, camera[i].pos.y, camera[i].pos.z);
		glLineWidth(1.0f);

		draw_static_object(&(static_objects[OBJ_BUILDING]), 0, i);

		draw_static_object(&(static_objects[OBJ_TABLE]), 0, i);
		draw_static_object(&(static_objects[OBJ_TABLE]), 1, i);

		draw_static_object(&(static_objects[OBJ_LIGHT]), 0, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 1, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 2, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 3, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 4, i);

		draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, i);
		draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0, i);
		draw_static_object(&(static_objects[OBJ_FRAME]), 0, i);
		draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0, i);
		draw_static_object(&(static_objects[OBJ_COW]), 0, i);

		draw_animated_tiger(i);
	}

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;


	case 'q':
		camera[currentCamera].rotateDirection = (camera[currentCamera].rotateDirection + 1) % 3;
		
		break;
	}
}

void keySpecial(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		keyState[GLUT_KEY_LEFT] = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		keyState[GLUT_KEY_RIGHT] = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		keyState[GLUT_KEY_DOWN] = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		keyState[GLUT_KEY_UP] = true;
		glutPostRedisplay();
		break;
	}
}

void keySpecialUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		keyState[GLUT_KEY_LEFT] = false;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		keyState[GLUT_KEY_RIGHT] = false;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		keyState[GLUT_KEY_DOWN] = false;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		keyState[GLUT_KEY_UP] = false;
		glutPostRedisplay();
		break;
	}
}

void keySpecialOperation() {
	Object& obj = static_objects[OBJ_COW];
	CAMERA& cam = camera[3];
	
	if (keyState[GLUT_KEY_LEFT] == true) {
		obj.move(0, glm::vec3(1.0f, 0.0f, 0.0f));
		cam.move(glm::vec3(0.0f, -1.0f, 0.0f));
		//obj->setPosition(obj->getPosition() - glm::vec3(3.0f, 0.0f, 0.0f));
	}
	if (keyState[GLUT_KEY_RIGHT] == true) {
		//obj->setPosition(obj->getPosition() + glm::vec3(3.0f, 0.0f, 0.0f));
		obj.move(0, glm::vec3(-1.0f, 0.0f, 0.0f));
		cam.move(glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (keyState[GLUT_KEY_UP] == true) {
		//obj->setPosition(obj->getPosition() + glm::vec3(0.0f, 3.0f, 0.0f));
		obj.move(0, glm::vec3(0.0f, 1.0f, 0.0f));
		cam.move(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (keyState[GLUT_KEY_DOWN] == true) {
		//obj->setPosition(obj->getPosition() - glm::vec3(0.0f, 3.0f, 0.0f));
		obj.move(0, glm::vec3(0.0f, -1.0f, 0.0f));
		cam.move(glm::vec3(0.0f, 0.0f, -1.0f));
	}
	printf("%f %f %f\n", camera[3].pos.x, camera[3].pos.y, camera[3].pos.z);
	set_ViewMatrix_from_camera_frame(ViewMatrix[3], camera[3]);
}

void reshape(int width, int height) {
	float aspect_ratio;

	//glViewport(0, 0, width, height);

	viewport[0].x = viewport[0].y = 0;
	viewport[0].w = (int)(0.60f*width); viewport[0].h = (int)(height);

	viewport[1].x = (int)(0.60f*width); viewport[1].y = 0;
	viewport[1].w = (int)(0.40f*width); viewport[1].h = (int)(0.40f*height);

	viewport[2].x = (int)(0.60f*width); viewport[2].y = (int)(0.40f*height);
	viewport[2].w = (int)(0.40f*width); viewport[2].h = (int)(0.30f*height);

	viewport[3].x = (int)(0.60f*width); viewport[3].y = (int)(0.70f*height);
	viewport[3].w = (int)(0.40f*width); viewport[3].h = (int)(0.30f*height);
	
	//camera[0].aspect_ratio = (float)width / height;
	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
		camera[i].aspect_ratio = (float)viewport[i].w / viewport[i].h;
		ProjectionMatrix[i] = glm::perspective(camera[i].fov_y*TO_RADIAN, camera[i].aspect_ratio, camera[i].near_clip, camera[i].far_clip);
	}
	/*camera[0].aspect_ratio = (float)viewport[0].w / viewport[0].h;
	ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);

	camera[1].aspect_ratio = (float)width / height;
	ProjectionMatrix[1] = glm::perspective(camera[1].fov_y*TO_RADIAN, camera[1].aspect_ratio, camera[1].near_clip, camera[1].far_clip);

	camera[2].aspect_ratio = (float)width / height;
	ProjectionMatrix[2] = glm::perspective(camera[2].fov_y*TO_RADIAN, camera[2].aspect_ratio, camera[2].near_clip, camera[2].far_clip);

	camera[3].aspect_ratio = (float)width / height;
	ProjectionMatrix[3] = glm::perspective(camera[3].fov_y*TO_RADIAN, camera[3].aspect_ratio, camera[3].near_clip, camera[3].far_clip);*/

	glutPostRedisplay();
}

void timer_scene(int timestamp_scene) {
	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keySpecial);
	glutSpecialUpFunc(keySpecialUp);
	glutMouseFunc(mousepress);
	glutMouseWheelFunc(mousewheel);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			cc.left_button_status = GLUT_DOWN;
			camera[currentCamera].move_status = 1;
			cc.prevx = x; cc.prevy = y;
		}
		else if (state == GLUT_UP) {
			cc.left_button_status = GLUT_UP;
			camera[currentCamera].move_status = 0;
		}
	}
}

void mousewheel(int button, int dir, int x, int y) {
	if (dir > 0) {
		printf("wheel up\n");
		camera[currentCamera].fov_y -= 1.0f;
		if (camera[currentCamera].fov_y < 0.0f)
			camera[currentCamera].fov_y = 0.0f;
	}
	else {
		printf("wheel down\n");
		camera[currentCamera].fov_y += 1.0f;
	}

	ProjectionMatrix[currentCamera] = glm::perspective(camera[currentCamera].fov_y*TO_RADIAN, camera[currentCamera].aspect_ratio, camera[currentCamera].near_clip, camera[currentCamera].far_clip);
}

void motion(int x, int y) {
	float delx, dely;

	if (!camera[currentCamera].move_status) return;

	delx = (float)(x - cc.prevx); dely = (float)(cc.prevy - y);
	cc.prevx = x; cc.prevy = y;

	int modifer_status = glutGetModifiers();

	switch (modifer_status) { // you may define the key combinations yourself.
	case GLUT_ACTIVE_SHIFT:
		renew_cam_position_along_axis(camera[currentCamera], dely, camera[currentCamera].vaxis);
		renew_cam_position_along_axis(camera[currentCamera], delx, camera[currentCamera].uaxis);
		//printf("motion1\n");
		break;
	/*case GLUT_ACTIVE_SHIFT | GLUT_ACTIVE_CTRL:
		renew_cam_orientation_rotation_around_axis(camera[currentCamera], dely, -camera[currentCamera].uaxis);
		printf("motion2\n");
		break;*/
	default:
		renew_cam_position_along_axis(camera[currentCamera], dely, -camera[currentCamera].naxis);
		if(camera[currentCamera].rotateDirection == 0)
			renew_cam_orientation_rotation_around_axis(camera[currentCamera], delx, -camera[currentCamera].vaxis);
		if (camera[currentCamera].rotateDirection == 1)
			renew_cam_orientation_rotation_around_axis(camera[currentCamera], delx, camera[currentCamera].naxis);
		if (camera[currentCamera].rotateDirection == 2)
			renew_cam_orientation_rotation_around_axis(camera[currentCamera], delx, -camera[currentCamera].uaxis);
		//printf("motion3\n");
		break;
	}
	set_ViewMatrix_from_camera_frame(ViewMatrix[currentCamera], camera[currentCamera]);
	
	//ViewMatrix = glm::lookAt(camera.pos, camera.center, camera.vaxis);
	//ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


	glutPostRedisplay();
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	initialize_camera();

	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);

	
	ViewMatrix[0] = glm::lookAt(camera[0].pos, camera[0].center, camera[0].vaxis);
	ViewMatrix[1] = glm::lookAt(camera[1].pos, camera[1].center, camera[1].vaxis);
	ViewMatrix[2] = glm::lookAt(camera[2].pos, camera[2].center, camera[2].vaxis);
	if (1) {
		//ViewMatrix[2] = glm::lookAt(glm::vec3(120.0f, 90.0f, 600.0f), glm::vec3(120.0f, 90.0f, 0.0f),
		//	glm::vec3(-10.0f, 0.0f, 0.0f));
		
	}
	if (1) {
		/*ViewMatrix[1] = glm::lookAt(glm::vec3(500.0f, 90.0f, 25.0f), glm::vec3(0.0f, 90.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));*/
		
	}

	if (1) {
		//ViewMatrix = glm::lookAt(glm::vec3(600.0f, 600.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f),
		//	glm::vec3(0.0f, 0.0f, 1.0f));
		
	}
}

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
	camera[1].far_clip = 10000.0f;
	set_ViewMatrix_from_camera_frame(ViewMatrix[1], camera[1]);


	camera[2].pos = glm::vec3(120.0f, 90.0f, 200.0f);
	camera[2].center = glm::vec3(120.0f, 90.0f, 0.0f);

	_vup = glm::vec3(-1.0f, 0.0f, 0.0f);
	_vpn = camera[2].pos - camera[2].center;
	camera[2].uaxis = glm::normalize(glm::cross(_vup, _vpn));
	camera[2].vaxis = glm::normalize(glm::cross(_vpn, camera[2].uaxis));
	camera[2].naxis = glm::normalize(_vpn);

	camera[2].move_status = 0;
	camera[2].fov_y = 90.0f;
	camera[2].aspect_ratio = 1.0f; // will be set when the viewing window pops up.
	camera[2].near_clip = 1.0f;
	camera[2].far_clip = 10000.0f;
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
	set_ViewMatrix_from_camera_frame(ViewMatrix[3], camera[3]);

	//

	//camera_selected = 0;
}


void prepare_scene(void) {
	define_axes();
	define_static_objects();
	define_animated_tiger();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170 Our_House_GLSL_V_0.5";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'c', 'f', 'd', 'ESC'" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
