#pragma warning (disable : 4996)
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

//#define NUMBER_OF_CAMERAS 4
enum _CAMERA_INFO {
	CAMERA_MAIN,
	CAMERA_ORTHO1,
	CAMERA_ORTHO2,
	CAMERA_ORTHO3,
	CAMERA_PERSPECTIVE1,
	CAMERA_PERSPECTIVE2,
	CAMERA_PERSPECTIVE3,
	NUMBER_OF_CAMERAS
};

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define WC_AXIS_LENGTH		60.0f




glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ModelViewMatrix, ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER; // computed only once in initialize_camera()


#include "Geometry.h"
#include "Object_Definitions.h"
#include "Camera.h"
#include "Car.h"

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

bool orthoOrPerspective = false;
bool showViewingVolume = true;


bool keyState[108] = { 0 };
void initialize_camera(void);
void motion(int x, int y);
void mousepress(int button, int state, int x, int y);
void mousewheel(int button, int dir, int x, int y);
void keySpecialOperation();
//void prepare_geom_obj(int geom_obj_ID, char *filename, GEOM_OBJ_TYPE geom_obj_type);

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
void camera_rotation_around_axis(CAMERA &_camera, float angle, glm::vec3 rot_axis) {
	glm::mat3 RotationMatrix;

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), TO_RADIAN*angle, rot_axis));

	_camera.uaxis = RotationMatrix * _camera.uaxis;
	_camera.vaxis = RotationMatrix * _camera.vaxis;
	_camera.naxis = RotationMatrix * _camera.naxis;
}

void display(void) {
	keySpecialOperation();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
		if (orthoOrPerspective == false) {
			if (i == 4 || i == 5 || i == 6)
				continue;
		}
		else {
			if (i == 1 || i == 2 || i == 3)
				continue;
		}
		glViewport(viewport[i].x, viewport[i].y, 
			viewport[i].w, viewport[i].h);

		

		glLineWidth(2.0f);
		draw_axes(ViewMatrix[i], i);
		draw_camera_axex(i);
		glLineWidth(1.0f);

		draw_static_object(&(static_objects[OBJ_BUILDING]), 0, i);

		draw_static_object(&(static_objects[OBJ_TABLE]), 0, i);
		draw_static_object(&(static_objects[OBJ_TABLE]), 1, i);

		draw_static_object(&(static_objects[OBJ_LIGHT]), 0, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 1, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 2, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 3, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 4, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 5, i);
		draw_static_object(&(static_objects[OBJ_LIGHT]), 6, i);

		draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, i);
		//draw_static_object(&(static_objects[OBJ_TEAPOT]), 1, i);
		draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0, i);
		draw_static_object(&(static_objects[OBJ_FRAME]), 0, i);
		draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0, i);
		draw_static_object(&(static_objects[OBJ_COW]), 1, i);
		//draw_static_object(&(static_objects[OBJ_COW1]), 0, i);
		//draw_static_object(&(static_objects[OBJ_COW2]), 0, i);

		draw_car_dummy(i);

		
		draw_spider(i);
		draw_ben(i);

		draw_two_hier_obj(&(static_objects[OBJ_TABLE]), &(static_objects[OBJ_TEAPOT]), 2, 1, i);
		draw_animated_tiger(i);
	}

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;
	CAMERA& cam = camera[CAMERA_PERSPECTIVE1];
	Object& obj = static_objects[OBJ_LIGHT];
	int idx = 6;
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
	/*case 'd':
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
		break;*/

	case 'w':
		obj.move(idx, glm::vec3(0.0f, -1.0f, 0.0f));
		//ben_pos += glm::vec3(0.0f, -1.0f, 0.0f);
		//cam.move(glm::vec3(0.0f, -1.0f, 0.0f));
		break;
	case 'a':
		obj.move(idx, glm::vec3(0.0f, 1.0f, 0.0f));
		//ben_pos += glm::vec3(0.0f, 1.0f, 0.0f);
		//cam.move(glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 's':
		obj.move(idx, glm::vec3(-1.0f, 0.0f, 0.0f));
		//ben_pos += glm::vec3(-1.0f, 0.0f, 0.0f);
		//cam.move(glm::vec3(-1.0f, 0.0f, 0.0f));
		break;
	case 'd':
		obj.move(idx, glm::vec3(1.0f, 0.0f, 0.0f));
		//ben_pos += glm::vec3(1.0f, 0.0f, 0.0f);
		//cam.move(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 'r':
		//obj.move(1, glm::vec3(0.0f, 0.0f, -1.0f));
		//ben_pos += glm::vec3(0.0f, 0.0f, -1.0f);
		//cam.move(glm::vec3(0.0f, 0.0f, -1.0f));
		break;
	case 'e':
		//obj.move(1, glm::vec3(0.0f, 0.0f, 1.0f));
		//ben_pos += glm::vec3(0.0f, 0.0f, 1.0f);
		//cam.move(glm::vec3(0.0f, 0.0f, 1.0f));
		break;

	case 'o':
		showViewingVolume = !showViewingVolume;
		for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
			camera[i].isViewingVolumeVisible = false;
		}
		camera[CAMERA_MAIN].isViewingVolumeVisible = showViewingVolume;
		if (orthoOrPerspective == false) {
			camera[CAMERA_ORTHO1].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_ORTHO2].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_ORTHO3].isViewingVolumeVisible = showViewingVolume;
		}
		else {
			camera[CAMERA_PERSPECTIVE1].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_PERSPECTIVE2].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_PERSPECTIVE3].isViewingVolumeVisible = showViewingVolume;
		}
		break;

	case 'p':
		orthoOrPerspective = !orthoOrPerspective;
		dotColor[0][0] = 255.0f; dotColor[0][1] = 255.0f; dotColor[0][2] = 0.0f;
		dotColor[1][0] = 0.0f; dotColor[1][1] = 255.0f; dotColor[1][2] = 255.0f;
		for (int i = 1; i < NUMBER_OF_CAMERAS; i++) {
			camera[i].isViewingVolumeVisible = false;
		}
		if (orthoOrPerspective == false) {
			camera[CAMERA_ORTHO1].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_ORTHO2].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_ORTHO3].isViewingVolumeVisible = showViewingVolume;
		}
		else {
			camera[CAMERA_PERSPECTIVE1].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_PERSPECTIVE2].isViewingVolumeVisible = showViewingVolume;
			camera[CAMERA_PERSPECTIVE3].isViewingVolumeVisible = showViewingVolume;
		}
		break;

	case 'q':
		camera[currentCamera].rotateDirection = (camera[currentCamera].rotateDirection + 1) % 3;
		
		break;
	}
	printf("%f %f %f\n", obj.pos[idx].x, obj.pos[idx].y, obj.pos[idx].z);
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
	Object& obj = static_objects[OBJ_COW1];
	CAMERA& cam = camera[3];
	int idx = 6;
	
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
	//printf("%f %f %f\n", obj.pos.x, obj.pos.y, obj.pos.z);
	set_ViewMatrix_from_camera_frame(ViewMatrix[3], camera[3]);
}

void reshape(int width, int height) {

	//glViewport(0, 0, width, height);

	viewport[0].x = viewport[0].y = 0;
	viewport[0].w = (int)(0.60f*width); viewport[0].h = (int)(height);

	viewport[1].x = (int)(0.60f*width); viewport[1].y = 0;
	viewport[1].w = (int)(0.40f*width); viewport[1].h = (int)(0.40f*height);

	viewport[2].x = (int)(0.60f*width); viewport[2].y = (int)(0.40f*height);
	viewport[2].w = (int)(0.40f*width); viewport[2].h = (int)(0.30f*height);

	viewport[3].x = (int)(0.60f*width); viewport[3].y = (int)(0.70f*height);
	viewport[3].w = (int)(0.40f*width); viewport[3].h = (int)(0.30f*height);

	viewport[4].x = (int)(0.60f*width); viewport[4].y = 0;
	viewport[4].w = (int)(0.40f*width); viewport[4].h = (int)(0.40f*height);

	viewport[5].x = (int)(0.60f*width); viewport[5].y = (int)(0.40f*height);
	viewport[5].w = (int)(0.40f*width); viewport[5].h = (int)(0.30f*height);

	viewport[6].x = (int)(0.60f*width); viewport[6].y = (int)(0.70f*height);
	viewport[6].w = (int)(0.40f*width); viewport[6].h = (int)(0.30f*height);
	
	//camera[0].aspect_ratio = (float)width / height;
	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) {
		camera[i].aspect_ratio = (float)viewport[i].w / viewport[i].h;
		/*ProjectionMatrix[i] = glm::perspective(camera[i].fov_y*TO_RADIAN, camera[i].aspect_ratio, camera[i].near_clip, camera[i].far_clip);
		if (i == 1)
			ProjectionMatrix[i] = glm::ortho(-(float)viewport[i].w*0.15f, (float)viewport[i].w*0.15f, -(float)viewport[i].h*0.1f, (float)viewport[i].h*0.1f,
				camera[i].near_clip, camera[i].far_clip);
		if (i == 2)
			ProjectionMatrix[i] = glm::ortho(-(float)viewport[i].w*0.2f, (float)viewport[i].w*0.2f, -(float)viewport[i].h*0.3f, (float)viewport[i].h*0.3f, 
				camera[i].near_clip, camera[i].far_clip);
		if (i == 3)
			ProjectionMatrix[i] = glm::ortho(-(float)viewport[i].w*0.2f, (float)viewport[i].w*0.2f, -(float)viewport[i].h*0.1f, (float)viewport[i].h*0.1f,
				camera[i].near_clip, camera[i].far_clip);*/
		
		//camera[i].viewingVolume.update_line(camera[i].fov_y, camera[i].near_clip, camera[i].far_clip, camera[i].aspect_ratio);
		//printf("camerai : %d %d %f\n", viewport[i].w, viewport[i].h, camera[i].aspect_ratio);
	}
	ProjectionMatrix[0] = glm::perspective(camera[CAMERA_MAIN].fov_y*TO_RADIAN, camera[CAMERA_MAIN].aspect_ratio, camera[CAMERA_MAIN].near_clip, camera[CAMERA_MAIN].far_clip);
	
	ProjectionMatrix[1] = glm::ortho(-(float)viewport[CAMERA_ORTHO1].w*0.15f, (float)viewport[CAMERA_ORTHO1].w*0.15f, -(float)viewport[CAMERA_ORTHO1].h*0.1f, (float)viewport[CAMERA_ORTHO1].h*0.1f,
		camera[CAMERA_ORTHO1].near_clip, camera[CAMERA_ORTHO1].far_clip);
	camera[CAMERA_ORTHO1].viewingVolume.viewingVolumeWidth = viewport[CAMERA_ORTHO1].w*0.15f;
	camera[CAMERA_ORTHO1].viewingVolume.viewingVolumeHeight = viewport[CAMERA_ORTHO1].h*0.1f;
	
	ProjectionMatrix[2] = glm::ortho(-(float)viewport[CAMERA_ORTHO2].w*0.2f, (float)viewport[CAMERA_ORTHO2].w*0.2f, -(float)viewport[CAMERA_ORTHO2].h*0.3f, (float)viewport[CAMERA_ORTHO2].h*0.3f,
		camera[CAMERA_ORTHO2].near_clip, camera[CAMERA_ORTHO2].far_clip);
	camera[CAMERA_ORTHO2].viewingVolume.viewingVolumeWidth = viewport[CAMERA_ORTHO1].w*0.2f;
	camera[CAMERA_ORTHO2].viewingVolume.viewingVolumeHeight = viewport[CAMERA_ORTHO1].h*0.3f;
	
	ProjectionMatrix[3] = glm::ortho(-(float)viewport[CAMERA_ORTHO3].w*0.2f, (float)viewport[CAMERA_ORTHO3].w*0.2f, -(float)viewport[CAMERA_ORTHO3].h*0.1f, (float)viewport[CAMERA_ORTHO3].h*0.1f,
		camera[CAMERA_ORTHO3].near_clip, camera[CAMERA_ORTHO3].far_clip);
	camera[CAMERA_ORTHO3].viewingVolume.viewingVolumeWidth = viewport[CAMERA_ORTHO1].w*0.2f;
	camera[CAMERA_ORTHO3].viewingVolume.viewingVolumeHeight = viewport[CAMERA_ORTHO1].h*0.1f;

	ProjectionMatrix[4] = glm::perspective(camera[CAMERA_PERSPECTIVE1].fov_y*TO_RADIAN, camera[CAMERA_PERSPECTIVE1].aspect_ratio, camera[CAMERA_PERSPECTIVE1].near_clip, camera[CAMERA_PERSPECTIVE1].far_clip);

	ProjectionMatrix[5] = glm::perspective(camera[CAMERA_PERSPECTIVE2].fov_y*TO_RADIAN, camera[CAMERA_PERSPECTIVE2].aspect_ratio, camera[CAMERA_PERSPECTIVE2].near_clip, camera[CAMERA_PERSPECTIVE2].far_clip);

	ProjectionMatrix[6] = glm::perspective(camera[CAMERA_PERSPECTIVE3].fov_y*TO_RADIAN, camera[CAMERA_PERSPECTIVE3].aspect_ratio, camera[CAMERA_PERSPECTIVE3].near_clip, camera[CAMERA_PERSPECTIVE3].far_clip);

	for (int i = 0; i < NUMBER_OF_CAMERAS; i++) 
		camera[i].viewingVolume.update_line(camera[i].fov_y, camera[i].near_clip, camera[i].far_clip, camera[i].aspect_ratio);
	
	glutPostRedisplay();
}

void timer_scene(int timestamp_scene) {

	//float addColor1 = ((rand() % 255) - (rand() % 255)) / 255.0f;
	float addColor1 = sin(((timestamp_scene*10)%180)*TO_RADIAN);
	float addColor2 = sin(((timestamp_scene*14+45)%180)*TO_RADIAN);
	float addColor3 = sin(((timestamp_scene*17+90)%180)*TO_RADIAN);
	static int spider_move_count = 0;
	//printf("%f\n", addColor1);

	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	if (cur_dir_spider) {
		cur_frame_spider--;
		if (cur_frame_spider < 0) {
			cur_frame_spider = N_SPIDER_FRAMES - 1;
			spider_move_count++;
			if (spider_move_count > 2) {
				cur_frame_spider = 0;
				spider_move_count = 0;
				cur_dir_spider = false;
			}
		}
		spider_pos += glm::vec3(0.0, 0.0, 0.1f);
	}
	else {
		cur_frame_spider++;
		if (cur_frame_spider > N_SPIDER_FRAMES - 1) {
			cur_frame_spider = 0;
			spider_move_count++;
			if (spider_move_count > 2) {
				cur_frame_spider = N_SPIDER_FRAMES - 1;
				spider_move_count = 0;
				cur_dir_spider = true;
			}
			//cur_frame_spider = 0;
		}
		spider_pos += glm::vec3(0.0, 0.0, -0.1f);
	}
	//cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	rotation_angle_car = ((timestamp_scene) % 360)*TO_RADIAN;
	static_objects[OBJ_TEAPOT].rotationAngle[1] = ((timestamp_scene*10) % 360)*TO_RADIAN;
	/*static_objects[OBJ_LIGHT].material[5].diffuse.r = 0.75164f * addColor1;
	static_objects[OBJ_LIGHT].material[5].diffuse.g= 0.60648f * addColor2;
	static_objects[OBJ_LIGHT].material[5].diffuse.b = 0.22648f * addColor3;

	static_objects[OBJ_LIGHT].material[6].diffuse.r = 0.15664f * addColor1;
	static_objects[OBJ_LIGHT].material[6].diffuse.g = 0.84248f * addColor2;
	static_objects[OBJ_LIGHT].material[6].diffuse.b = 0.02540f * addColor3;*/
	//printf("%f\n", static_objects[OBJ_TEAPOT].rotationAngle[1]);

	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void timer_scene2(int timestamp_scene) {
	static bool cameraBlink = false;
	
	if (orthoOrPerspective == false) {
		if (cameraBlink == false) {
			cameraBlink = true;
			dotColor[orthoOrPerspective][0] = 0.0f;
			dotColor[orthoOrPerspective][1] = 0.0f;
			dotColor[orthoOrPerspective][2] = 0.0f;
		}
		else {
			cameraBlink = false;
			dotColor[orthoOrPerspective][0] = 255.0f;
			dotColor[orthoOrPerspective][1] = 255.0f;
			dotColor[orthoOrPerspective][2] = 0.0f;
		}
	}
	else {
		if (cameraBlink == false) {
			cameraBlink = true;
			dotColor[orthoOrPerspective][0] = 255.0f;
			dotColor[orthoOrPerspective][1] = 255.0f;
			dotColor[orthoOrPerspective][2] = 255.0f;
		}
		else {
			cameraBlink = false;
			dotColor[orthoOrPerspective][0] = 0.0f;
			dotColor[orthoOrPerspective][1] = 255.0f;
			dotColor[orthoOrPerspective][2] = 255.0f;
		}
	}
	
	

	glutPostRedisplay();
	glutTimerFunc(100, timer_scene2, 0);
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

	
	//ViewMatrix[0] = glm::lookAt(camera[0].pos, camera[0].center, camera[0].vaxis);
	//ViewMatrix[1] = glm::lookAt(camera[1].pos, camera[1].center, camera[1].vaxis);
	//ViewMatrix[2] = glm::lookAt(camera[2].pos, camera[2].center, camera[2].vaxis);
}

void prepare_scene(void) {
	define_axes();
	define_static_objects();
	define_animated_tiger();

	ModelMatrix_CAR_BODY = ModelMatrix_CAR_WHEEL = ModelMatrix_CAR_NUT = glm::mat4(1.0f);

	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, "Data/car_body_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, "Data/car_wheel_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, "Data/car_nut_triangles_v.txt", GEOM_OBJ_TYPE_V);

	prepare_spider();
	prepare_ben();
}

void cleanup(void) {
	cleanup_OpenGL_stuffs();

	free_geom_obj(GEOM_OBJ_ID_CAR_BODY);
	free_geom_obj(GEOM_OBJ_ID_CAR_WHEEL);
	free_geom_obj(GEOM_OBJ_ID_CAR_NUT);
	free_geom_obj(GEOM_OBJ_ID_CAR_BODY);
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
	glutTimerFunc(100, timer_scene2, 0);
	glutCloseFunc(cleanup);
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
int main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170 Our_House_GLSL_V_0.5";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'c', 'f', 'd', 'ESC'" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1680, 1050);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();

	return 0;
}
