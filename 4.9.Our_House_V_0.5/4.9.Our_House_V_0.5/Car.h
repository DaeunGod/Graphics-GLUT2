#pragma once

#define rad 1.7f
#define ww 1.0f

float rotation_angle_car = 0.0f;

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

	ModelMatrix_CAR_BODY = glm::rotate(glm::mat4(1.0f), -rotation_angle_car, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(300.0f, 75.0f, 20.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, .0f));
	ModelMatrix_CAR_BODY = glm::scale(ModelMatrix_CAR_BODY, glm::vec3(3.0f, 3.0f, 3.0f));
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
	//ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 0

						   //ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_BODY, -10.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 1


	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
	//ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 2

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut( cameraIndex );  // draw wheel 3
}