#pragma once

#define rad 1.7f
#define ww 1.0f



//-28.251429 - 20.022858
//glm::vec3(196.0f, 51.0f, 0.0f);

struct {
	int cur_frame = 0;
	float rotation_angle_z = 0.0f;
	float rotation_angle_x = 0.0f;
	glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f);

	float rotation_angle_car = 0.0f;
	glm::vec3 pos = glm::vec3(196.0f - 28.251429f*0.5f, 51.0f - 20.022858f*0.5f, 5.0f);
	float size = 0.1f;

	glm::vec3 uaxis;
	glm::vec3 vaxis;
	glm::vec3 naxis;

	void move(float to) {
		pos += -naxis * to;
	}

	void rotate(float _angle, glm::vec3 axis = glm::vec3(0.0f, 0.0f, 1.0f)) {
		glm::mat3 rot = glm::mat3(1.0f);
		//rotation_angle_z -= _angle * TO_RADIAN;
		rotation_angle_car -= _angle * TO_RADIAN;


		rot = glm::mat3(glm::rotate(glm::mat4(1.0f), _angle*TO_RADIAN, axis));

		uaxis = rot * uaxis;
		vaxis = rot * vaxis;
		naxis = rot * naxis;

		//printf("%f %f %f %f\n", rotation_angle * TO_DEGREE, naxis.x, naxis.y, naxis.z);
	}

	void rotate_x(float _angle, glm::vec3 axis = glm::vec3(0.0f, 0.0f, 1.0f)) {
		glm::mat3 rot = glm::mat3(1.0f);
		rotation_angle_x -= _angle * TO_RADIAN;


		rot = glm::mat3(glm::rotate(glm::mat4(1.0f), _angle*TO_RADIAN, axis));

		uaxis = rot * uaxis;
		vaxis = rot * vaxis;
		naxis = rot * naxis;

		//printf("%f %f %f %f\n", rotation_angle * TO_DEGREE, naxis.x, naxis.y, naxis.z);
	}

	void init() {
		glm::vec3 _vup = glm::vec3(0.0f, 0.0f, 1.0f), _vpn;
		pos = glm::vec3(180.0f, 61.0f, 5.0f);
		glm::vec3 center = pos + glm::vec3(0.0f, 1.0f, 0.0f);

		_vpn = pos - center;
		uaxis = glm::normalize(glm::cross(_vup, _vpn));
		vaxis = glm::normalize(glm::cross(_vpn, uaxis));
		naxis = glm::normalize(_vpn);

		rotation_angle_z = 0.0f;
		rotation_angle_x = 0.0f;
	}
} car_data;

void draw_wheel_and_nut(int cameraIndex) {
	// angle is used in Hierarchical_Car_Correct later
	int i;

	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel

	for (i = 0; i < 5; i++) {
		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));

		ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_NUT;
		ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

		//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_NUT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
	}
}

void draw_car_dummy(int cameraIndex) {

	//ModelViewMatrix = ViewMatrix[cameraIndex] * obj_ptr->ModelMatrix[instance_ID];
	//ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	ModelMatrix_CAR_BODY = glm::translate(glm::mat4(1.0f), car_data.pos);
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, -car_data.rotation_angle_car, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, -90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	
	//ModelMatrix_CAR_BODY = glm::scale(ModelMatrix_CAR_BODY, glm::vec3(3.0f, 3.0f, 3.0f));
	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_BODY;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	//ModelViewProjectionMatrix = ViewProjectionMatrix[cameraIndex] * ModelMatrix_CAR_BODY;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine
	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY); // draw body

	glLineWidth(2.0f);
	draw_axes(); // draw MC axes of body
	glLineWidth(1.0f);

	ModelMatrix_CAR_DRIVER = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_DRIVER = glm::rotate(ModelMatrix_CAR_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_DRIVER;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_DRIVER;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(5.0f);
	draw_axes(); // draw camera frame at driver seat
	glLineWidth(1.0f);


	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, 4.5f));
	
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, -20.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, car_data.rotation_angle_car, glm::vec3(0.0f, 0.0f, 1.0f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 0

						   //ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_BODY, -10.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, car_data.rotation_angle_car, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 1


	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, -20.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, car_data.rotation_angle_car, glm::vec3(0.0f, 0.0f, 1.0f));
	

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 2

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, car_data.rotation_angle_car, glm::vec3(0.0f, 0.0f, 1.0f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 3
}