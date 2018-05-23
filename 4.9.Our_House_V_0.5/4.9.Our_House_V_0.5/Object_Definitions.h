
// The object modelling tasks performed by this file are usually done 
// by reading a scene configuration file or through a help of graphics user interface!!!

//extern CAMERA camera[NUMBER_OF_CAMERAS];

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

typedef struct _material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
} Material;

#define N_MAX_GEOM_COPIES 10
typedef struct _Object {
	char filename[512];

	GLenum front_face_mode; // clockwise or counter-clockwise
	int n_triangles;

	int n_fields; // 3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	GLfloat *vertices; // pointer to vertex array data
	GLfloat xmin, xmax, ymin, ymax, zmin, zmax; // bounding box <- compute this yourself

	GLuint VBO, VAO; // Handles to vertex buffer object and vertex array object

	int n_geom_instances;
	glm::mat4 ModelMatrix[N_MAX_GEOM_COPIES];
	Material material[N_MAX_GEOM_COPIES];

	glm::vec3 pos[N_MAX_GEOM_COPIES];
	float rotationAngle[N_MAX_GEOM_COPIES];

	void move(int index, glm::vec3 dir) {
		//ModelMatrix[index] = glm::translate(ModelMatrix[index], dir);
		pos[index] += dir;
		//static_objects[OBJ_COW1].pos = glm::vec3(215.0f, 100.0f, 9.5f);
		ModelMatrix[index] = glm::translate(glm::mat4(1.0f), pos[index]);
		ModelMatrix[index] = glm::scale(ModelMatrix[index],
			glm::vec3(1.0f, -1.0f, 1.0f));
		ModelMatrix[index] = glm::rotate(ModelMatrix[index],
			180.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	}
} Object;

//#define N_MAX_STATIC_OBJECTS		10
//Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
//int n_static_objects = 0;

//#define OBJ_BUILDING		0
//#define OBJ_TABLE			1
//#define OBJ_LIGHT			2
//#define OBJ_TEAPOT			3
//#define OBJ_NEW_CHAIR		4
//#define OBJ_FRAME			5
//#define OBJ_NEW_PICTURE		6
//#define OBJ_COW				7
//#define OBJ_COW1			8
//#define OBJ_COW2			9
enum STATIC_OBJS {
	OBJ_BUILDING,	
	OBJ_TABLE,		
	OBJ_LIGHT,		
	OBJ_TEAPOT,		
	OBJ_NEW_CHAIR,	
	OBJ_FRAME,		
	OBJ_NEW_PICTURE,	
	OBJ_COW,			
	OBJ_COW1,		
	OBJ_COW2,		
	N_MAX_STATIC_OBJECTS
};

Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
int n_static_objects = 0;

typedef struct _Material_Parameters {
	float ambient_color[4], diffuse_color[4], specular_color[4], emissive_color[4];
	float specular_exponent;
} Material_Parameters;

// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat *spider_vertices[N_SPIDER_FRAMES];
int cur_frame_spider = 0;
bool cur_dir_spider = false; //false: down side, true: up side

glm::vec3 spider_pos = glm::vec3(106.0f, 79.0f, 22.0f);

Material_Parameters material_spider;

// ben object
#define N_BEN_FRAMES 1
GLuint ben_VBO, ben_VAO;
int ben_n_triangles[N_BEN_FRAMES];
int ben_vertex_offset[N_BEN_FRAMES];
GLfloat *ben_vertices[N_BEN_FRAMES];
int cur_frame_ben = 0;

glm::vec3 ben_pos = glm::vec3(83.0f, 151.0f, 10.0f);
float ben_angle = 0.0f;

Material_Parameters material_ben;

int read_geometry(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(*object, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void compute_AABB(Object *obj_ptr) {
	// Do it yourself.
}
	 
void prepare_geom_of_static_object(Object *obj_ptr) {
	int n_bytes_per_vertex, n_bytes_per_triangle;
	//char filename[512];

	n_bytes_per_vertex = obj_ptr->n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	obj_ptr->n_triangles = read_geometry(&(obj_ptr->vertices), n_bytes_per_triangle, obj_ptr->filename);

	// Initialize vertex buffer object.
	glGenBuffers(1, &(obj_ptr->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glBufferData(GL_ARRAY_BUFFER, obj_ptr->n_triangles*n_bytes_per_triangle, obj_ptr->vertices, GL_STATIC_DRAW);

	compute_AABB(obj_ptr);
	free(obj_ptr->vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &(obj_ptr->VAO));
	glBindVertexArray(obj_ptr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void define_static_objects(void) {
	// building
	strcpy(static_objects[OBJ_BUILDING].filename, "Data/Building1_vnt.geom");
	static_objects[OBJ_BUILDING].n_fields = 8;

	static_objects[OBJ_BUILDING].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUILDING]));

	static_objects[OBJ_BUILDING].n_geom_instances = 1;

    static_objects[OBJ_BUILDING].ModelMatrix[0] = glm::mat4(1.0f);
	
	static_objects[OBJ_BUILDING].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].exponent = 128.0f*0.1f;

	// table
	strcpy(static_objects[OBJ_TABLE].filename, "Data/Table_vn.geom");
	static_objects[OBJ_TABLE].n_fields = 6;

	static_objects[OBJ_TABLE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TABLE]));

	static_objects[OBJ_TABLE].n_geom_instances = 4;

	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[0], 
		glm::vec3(0.5f, 0.5f, 0.5f));

	static_objects[OBJ_TABLE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[0].ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[0].diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	static_objects[OBJ_TABLE].material[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[0].exponent = 15.0f;

	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[1],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[1].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[1].diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	static_objects[OBJ_TABLE].material[1].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[1].exponent = 128.0f*0.078125f;

	static_objects[OBJ_TABLE].pos[2] = glm::vec3(48.0f, 45.0f, 0.0f);
	static_objects[OBJ_TABLE].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_TABLE].pos[2]);
	static_objects[OBJ_TABLE].ModelMatrix[2] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[2],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[2].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[2].diffuse = glm::vec4(134.0f / 255.0f, 191.0f / 255.0f, 233.0f / 255.0f, 1.0f);
	static_objects[OBJ_TABLE].material[2].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[2].exponent = 128.0f*0.078125f;

	static_objects[OBJ_TABLE].pos[3] = glm::vec3(0.0f, 0.0f, 0.0f);
	static_objects[OBJ_TABLE].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_TABLE].pos[3]);
	static_objects[OBJ_TABLE].ModelMatrix[3] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[3],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[3].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[3].diffuse = glm::vec4(186.0f / 255.0f, 68.0f / 255.0f, 255.0f / 255.0f, 1.0f);
	static_objects[OBJ_TABLE].material[3].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[3].exponent = 128.0f*0.078125f;

	// Light
	strcpy(static_objects[OBJ_LIGHT].filename, "Data/Light_vn.geom");
	static_objects[OBJ_LIGHT].n_fields = 6;

	static_objects[OBJ_LIGHT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(static_objects + OBJ_LIGHT);

	static_objects[OBJ_LIGHT].n_geom_instances = 8;

	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[3],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[4],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[4].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].pos[5] = glm::vec3(166.0f, 153.0f, 49.0f);
	static_objects[OBJ_LIGHT].ModelMatrix[5] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_LIGHT].pos[5]);
	static_objects[OBJ_LIGHT].ModelMatrix[5] = glm::scale(static_objects[OBJ_LIGHT].ModelMatrix[5], glm::vec3(1.0f, -1.0f, 1.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[5] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[5],
		180.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	

	static_objects[OBJ_LIGHT].material[5].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[5].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].pos[6] = glm::vec3(38.0f, 154.0f, 49.0f);
	static_objects[OBJ_LIGHT].ModelMatrix[6] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_LIGHT].pos[6]);
	static_objects[OBJ_LIGHT].ModelMatrix[6] = glm::scale(static_objects[OBJ_LIGHT].ModelMatrix[6], glm::vec3(1.0f, -1.0f, 1.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[6] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[6],
		180.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));


	static_objects[OBJ_LIGHT].material[6].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[6].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[6].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[6].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[6].exponent = 128.0f*0.4f;



	// teapot
	strcpy(static_objects[OBJ_TEAPOT].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT].n_fields = 6;

	static_objects[OBJ_TEAPOT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT]));

	static_objects[OBJ_TEAPOT].n_geom_instances = 2;

	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[0],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].exponent = 128.0f*0.6f;


	static_objects[OBJ_TEAPOT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 50.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[1] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[1],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].rotationAngle[1] = 0.0f;

	static_objects[OBJ_TEAPOT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].ambient = glm::vec4(15.0f/255.0f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].diffuse = glm::vec4(115.0f / 255.0f, 204.0f / 255.0f, 63.0f / 255.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[1].exponent = 128.0f*0.6f;

	/*static_objects[OBJ_TEAPOT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 50.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[2] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[1],
		glm::vec3(2.0f, 2.0f, 2.0f));


	static_objects[OBJ_TEAPOT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[2].ambient = glm::vec4(15.0f / 255.0f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[2].diffuse = glm::vec4(115.0f / 255.0f, 204.0f / 255.0f, 63.0f / 255.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[2].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[2].exponent = 128.0f*0.6f;*/


	// new_chair
	strcpy(static_objects[OBJ_NEW_CHAIR].filename, "Data/new_chair_vnt.geom");
	static_objects[OBJ_NEW_CHAIR].n_fields = 8;

	static_objects[OBJ_NEW_CHAIR].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_CHAIR]));

	static_objects[OBJ_NEW_CHAIR].n_geom_instances = 2;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].exponent = 128.0f*0.078125f;

	static_objects[OBJ_NEW_CHAIR].pos[1] = glm::vec3(250.0f, 104.0f, 0.0f);
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_NEW_CHAIR].pos[1]);
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[1],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[1],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].diffuse = glm::vec4(69.0f/255.0f, 72.0f/255.0f, 1.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[1].exponent = 128.0f*0.078125f;

	// frame
	strcpy(static_objects[OBJ_FRAME].filename, "Data/Frame_vn.geom");
	static_objects[OBJ_FRAME].n_fields = 6;

	static_objects[OBJ_FRAME].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_FRAME]));

	static_objects[OBJ_FRAME].n_geom_instances = 1;

	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[0],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[0].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[0].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[0].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[0].exponent = 128.0f*0.1f;


	// new_picture
	strcpy(static_objects[OBJ_NEW_PICTURE].filename, "Data/new_picture_vnt.geom");
	static_objects[OBJ_NEW_PICTURE].n_fields = 8;

	static_objects[OBJ_NEW_PICTURE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_PICTURE]));

	static_objects[OBJ_NEW_PICTURE].n_geom_instances = 1;

	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].exponent = 128.0f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_COW].filename, "Data/cow_vn.geom");
	static_objects[OBJ_COW].n_fields = 6;

	static_objects[OBJ_COW].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_COW]));

	static_objects[OBJ_COW].n_geom_instances = 2;

	static_objects[OBJ_COW].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::scale(static_objects[OBJ_COW].ModelMatrix[0],
		glm::vec3(30.0f, 30.0f, 30.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

 
	static_objects[OBJ_COW].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_COW].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_COW].material[0].exponent = 0.21794872f*0.6f;

	static_objects[OBJ_COW].pos[1] = glm::vec3(189.0f, 116.0f, 29.5f);
	static_objects[OBJ_COW].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_COW].pos[1]);
	static_objects[OBJ_COW].ModelMatrix[1] = glm::scale(static_objects[OBJ_COW].ModelMatrix[1],
		glm::vec3(1.0f, 25.0f, 25.0f));
	//static_objects[OBJ_COW].ModelMatrix[1] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[1],
		//90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[1] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));


	static_objects[OBJ_COW].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_COW].material[1].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[1].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[1].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_COW].material[1].exponent = 0.21794872f*0.6f;

	n_static_objects = 8;
}

void draw_static_object(Object *obj_ptr, int instance_ID, int cameraIndex) {
	glFrontFace(obj_ptr->front_face_mode);

	ModelViewMatrix = ViewMatrix[cameraIndex] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);
}

GLuint VBO_axes, VAO_axes;
GLfloat vertices_axes[6][3] = {
	{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

void define_axes(void) {  
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//#define WC_AXIS_LENGTH		60.0f
void draw_axes(glm::mat4 obj, int cameraIndex) {
	//ModelViewMatrix = glm::scale(ViewMatrix[cameraIndex], glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	ModelViewMatrix = glm::scale(obj, glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

void draw_axes() {
	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

#define N_TIGER_FRAMES 12
Object tiger[N_TIGER_FRAMES];
struct {
	int cur_frame = 0;
	float rotation_angle_z = 0.0f;
	float rotation_angle_x = 0.0f;
	glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 pos = glm::vec3(111.0f, 139.0f, 0.0f);
	float size = 0.1f;

	glm::vec3 uaxis;
	glm::vec3 vaxis;
	glm::vec3 naxis;

	void move(float to) {
		pos += -naxis * to;
	}

	void rotate(float _angle, glm::vec3 axis = glm::vec3(0.0f, 0.0f, 1.0f)) {
		glm::mat3 rot = glm::mat3(1.0f);
		rotation_angle_z -= _angle*TO_RADIAN;


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
		pos = glm::vec3(111.0f, 139.0f, 0.0f);
		glm::vec3 center = pos + glm::vec3( 0.0f, -1.0f, 0.0f);

		_vpn = pos - center;
		uaxis = glm::normalize(glm::cross(_vup, _vpn));
		vaxis = glm::normalize(glm::cross(_vpn, uaxis));
		naxis = glm::normalize(_vpn);

		rotation_angle_z = 0.0f;
		rotation_angle_x = 0.0f;

		tiger_data.rotate(90, tiger_data.vaxis);
	}
} tiger_data;

void define_animated_tiger(void) {
	tiger_data.init();
	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(tiger[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);

		tiger[i].n_fields = 8;
		tiger[i].front_face_mode = GL_CW;
		prepare_geom_of_static_object(&(tiger[i]));

		tiger[i].n_geom_instances = 1;

		tiger[i].ModelMatrix[0] = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

		tiger[i].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		tiger[i].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		tiger[i].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		tiger[i].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		tiger[i].material[0].exponent = 128.0f*0.21794872f;
	}
	
	//tiger_data.calcModelMatrix();
	
	//tiger_data.move(50.0f);
}

void draw_animated_tiger(int cameraIndex) {
	glm::mat4 _model = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 obj = glm::mat4(1.0f);

	tiger[tiger_data.cur_frame].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), tiger_data.pos);
	tiger[tiger_data.cur_frame].ModelMatrix[0] = glm::rotate(tiger[tiger_data.cur_frame].ModelMatrix[0], -tiger_data.rotation_angle_z, glm::vec3(0.0f, 0.0f, 1.0f));
	tiger[tiger_data.cur_frame].ModelMatrix[0] = glm::rotate(tiger[tiger_data.cur_frame].ModelMatrix[0], tiger_data.rotation_angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
	tiger[tiger_data.cur_frame].ModelMatrix[0] = glm::scale(tiger[tiger_data.cur_frame].ModelMatrix[0], glm::vec3(tiger_data.size));

	ModelViewMatrix = ViewMatrix[cameraIndex] * tiger[tiger_data.cur_frame].ModelMatrix[0];
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);


	rot[0][0] = tiger_data.uaxis.x; rot[1][0] = tiger_data.vaxis.x; rot[2][0] = tiger_data.naxis.x;
	rot[0][1] = tiger_data.uaxis.y; rot[1][1] = tiger_data.vaxis.y; rot[2][1] = tiger_data.naxis.y;
	rot[0][2] = tiger_data.uaxis.z; rot[1][2] = tiger_data.vaxis.z; rot[2][2] = tiger_data.naxis.z;

	obj = glm::translate(glm::mat4(1.0f), tiger_data.pos);
	//obj = glm::rotate(obj, -tiger_data.rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	obj = obj * rot;
	obj = glm::scale(obj, glm::vec3(WC_AXIS_LENGTH*0.5f, WC_AXIS_LENGTH*0.5f, WC_AXIS_LENGTH*0.5f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * obj;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);

	//draw_axes(ViewMatrix[cameraIndex], cameraIndex);
}

void prepare_spider(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles*n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_spider.diffuse_color[0] = 153.0f / 255.0f;
	material_spider.diffuse_color[1] = 130.0f / 255.0f;
	material_spider.diffuse_color[2] = 255.0f / 255.0f;
	material_spider.diffuse_color[3] = 1.0f;

}

void draw_spider(int cameraIndex) {
	
	glFrontFace(GL_CW);

	ModelViewMatrix = glm::translate(ViewMatrix[cameraIndex], spider_pos);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(5.0f, -5.0f, -5.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, 180*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, material_spider.diffuse_color[0], material_spider.diffuse_color[1], material_spider.diffuse_color[2]);
	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);
}

void prepare_ben(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, ben_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_BEN_FRAMES; i++) {
		sprintf(filename, "Data/ben/ben_vnt_%d%d.geom", i / 10, i % 10);
		ben_n_triangles[i] = read_geometry(&ben_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		ben_n_total_triangles += ben_n_triangles[i];

		if (i == 0)
			ben_vertex_offset[i] = 0;
		else
			ben_vertex_offset[i] = ben_vertex_offset[i - 1] + 3 * ben_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &ben_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ben_VBO);
	glBufferData(GL_ARRAY_BUFFER, ben_n_total_triangles*n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_BEN_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, ben_vertex_offset[i] * n_bytes_per_vertex,
			ben_n_triangles[i] * n_bytes_per_triangle, ben_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_BEN_FRAMES; i++)
		free(ben_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &ben_VAO);
	glBindVertexArray(ben_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ben_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_ben.diffuse_color[0] = 0.951323f;
	material_ben.diffuse_color[1] = 0.931232f;
	material_ben.diffuse_color[2] = 0.642333f;
	material_ben.diffuse_color[3] = 1.0f;
}

void draw_ben(int cameraIndex) {
	glm::mat4 _model = glm::mat4(1.0f);


	_model = glm::translate(glm::mat4(1.0f), ben_pos);
	_model = glm::scale(_model, glm::vec3(30.0f, 5.0f, 30.0f));
	
	_model = glm::rotate(_model, -ben_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	_model = glm::rotate(_model, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));


	ModelViewMatrix = ViewMatrix[cameraIndex] * _model;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glFrontFace(GL_CW);

	glUniform3f(loc_primitive_color, material_ben.diffuse_color[0], material_ben.diffuse_color[1], material_ben.diffuse_color[2]);
	glBindVertexArray(ben_VAO);
	glDrawArrays(GL_TRIANGLES, ben_vertex_offset[cur_frame_ben], 3 * ben_n_triangles[cur_frame_ben]);
	glBindVertexArray(0);

	_model = glm::translate(glm::mat4(1.0f), ben_pos + glm::vec3(0.0f, -20.0f, 0.0f));
	_model = glm::scale(_model, glm::vec3(30.0f, 5.0f, 30.0f));

	_model = glm::rotate(_model, ben_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	_model = glm::rotate(_model, -90.0f * TO_RADIAN, glm::vec3(0.0f, -1.0f, 0.0f));


	ModelViewMatrix = ViewMatrix[cameraIndex] * _model;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glFrontFace(GL_CW);

	glUniform3f(loc_primitive_color, material_ben.diffuse_color[0], material_ben.diffuse_color[1], material_ben.diffuse_color[2]);
	glBindVertexArray(ben_VAO);
	glDrawArrays(GL_TRIANGLES, ben_vertex_offset[cur_frame_ben], 3 * ben_n_triangles[cur_frame_ben]);
	glBindVertexArray(0);
}

void update_ben_motion(int timestamp_scene) {
	ben_angle = (timestamp_scene % 360) * TO_RADIAN;
}

GLuint path_VBO, path_VAO;
GLfloat *path_vertices;
int path_n_vertices;

glm::vec3 path_pos = glm::vec3(196.0f, 51.0f, 0.0f);

void prepare_path(void) { // Draw path.
						  //	return;
	path_n_vertices = read_path_file(&path_vertices, "Data/path.txt");
	//printf("%d %f\n", path_n_vertices, path_vertices[(path_n_vertices - 1)]);
	// Initialize vertex array object.
	glGenVertexArrays(1, &path_VAO);
	glBindVertexArray(path_VAO);
	glGenBuffers(1, &path_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, path_VBO);
	glBufferData(GL_ARRAY_BUFFER, path_n_vertices * 3 * sizeof(float), path_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_path(int cameraIndex) {
	glm::mat4 _pos = glm::mat4(1.0f);

	_pos = glm::translate(_pos, path_pos);
	_pos = glm::scale(_pos, glm::vec3(0.5f, 1.7f, 0.5f));
	//_pos = glm::rotate(_pos, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));


	ModelViewMatrix = ViewMatrix[cameraIndex] * _pos;
	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(path_VAO);
	glUniform3f(loc_primitive_color, 1.000f, 1.000f, 0.000f); // color name: Yellow
	glDrawArrays(GL_LINE_STRIP, 0, path_n_vertices);
}

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < N_MAX_STATIC_OBJECTS; i++) {
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &spider_VAO);
	glDeleteBuffers(1, &spider_VBO);

	glDeleteVertexArrays(1, &ben_VAO);
	glDeleteBuffers(1, &ben_VBO);

	glDeleteVertexArrays(1, &path_VAO);
	glDeleteBuffers(1, &path_VBO);
	
}

void draw_two_hier_obj(Object *obj_ptr1, Object *obj_ptr2, int obj1_instance_ID, int obj2_instance_ID, int cameraIndex) {
	glm::mat4 _modelMatrix = glm::mat4(1.0f);
	//obj_ptr1->ModelMatrix[obj1_instance_ID] = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	obj_ptr1->ModelMatrix[obj1_instance_ID] = glm::translate(glm::mat4(1.0f), obj_ptr1->pos[obj1_instance_ID]);
	//obj_ptr1->ModelMatrix[obj1_instance_ID] = glm::rotate(obj_ptr1->ModelMatrix[obj1_instance_ID], self_rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	_modelMatrix = obj_ptr1->ModelMatrix[obj1_instance_ID];
	obj_ptr1->ModelMatrix[obj1_instance_ID] = glm::scale(obj_ptr1->ModelMatrix[obj1_instance_ID], glm::vec3(0.5f, 1.0f, 0.5f));

	ModelViewMatrix = ViewMatrix[cameraIndex] * obj_ptr1->ModelMatrix[obj1_instance_ID];

	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	draw_static_object(obj_ptr1, obj1_instance_ID, cameraIndex);

	obj_ptr2->ModelMatrix[obj2_instance_ID] = glm::translate(_modelMatrix, glm::vec3(0.0f, 0.0f, 8.0f));
	obj_ptr2->ModelMatrix[obj2_instance_ID] = glm::scale(obj_ptr2->ModelMatrix[obj2_instance_ID], glm::vec3(2.0f, 2.0f, 2.0f));
	obj_ptr2->ModelMatrix[obj2_instance_ID] = glm::rotate(obj_ptr2->ModelMatrix[obj2_instance_ID], -obj_ptr2->rotationAngle[obj2_instance_ID], glm::vec3(0.0f, 0.0f, 1.0f));
	
	ModelViewMatrix = ViewMatrix[cameraIndex] * obj_ptr2->ModelMatrix[obj2_instance_ID];

	ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_static_object(obj_ptr2, obj2_instance_ID, cameraIndex);

	//ModelViewMatrix = ViewMatrix[cameraIndex] * obj_ptr->ModelMatrix[instance_ID];
	//ModelViewProjectionMatrix = ProjectionMatrix[cameraIndex] * ModelViewMatrix;
}

void update_chair_motion(int timestamp_scene ) {
	float t = (timestamp_scene%360) * TO_RADIAN;
	glm::vec3 _pos = glm::vec3(16.0f * sinf(t) * sinf(t) * sinf(t), 0.0f, 13 * cosf(t) - 5 * cosf(2 * t) - 2 * cosf(3 * t) - cosf(4 * t)) * 0.5f;

	_pos.x += 200.0f;
	_pos.y += 20.0f;
	_pos.z += 10.0f;
	static_objects[OBJ_NEW_CHAIR].pos[1] = _pos;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_NEW_CHAIR].pos[1]);
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[1],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[1] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[1], 
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	//pos.y += 200.0f;
}

void update_table_motion(int timestamp_scene) {
	float t = (timestamp_scene % 360) * TO_RADIAN;
	glm::vec3 _pos = glm::vec3(0.0f, 0.0f, sinf(t)) * 3.0f;

	//_pos.x += 200.0f;
	_pos.z += 10.0f;
	//_pos.z += 10.0f;
	static_objects[OBJ_TABLE].pos[3] = _pos;

	static_objects[OBJ_TABLE].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), static_objects[OBJ_TABLE].pos[3]+glm::vec3(50.0f, 130.0f, 5.0f));
	static_objects[OBJ_TABLE].ModelMatrix[3] = glm::rotate(static_objects[OBJ_TABLE].ModelMatrix[3], t,
		glm::vec3(0.0f, 1.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[3] = glm::translate(static_objects[OBJ_TABLE].ModelMatrix[3], -static_objects[OBJ_TABLE].pos[3]);
	static_objects[OBJ_TABLE].ModelMatrix[3] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[3],
		glm::vec3(0.4f, 0.3f, 0.3f));
	
}

enum TIGER_MOVE {
	TIGER_MOVE1,
	TIGER_MOVE2,
	TIGER_MOVE3,
	TIGER_MOVE4,
	TIGER_MOVE5,
	TIGER_MOVE6,
	TIGER_MOVE7,
	TIGER_MOVE8,
	TIGER_MOVE9,
	TIGER_MOVE10,
	TIGER_MOVE11,
	TIGER_MOVE12,
	TIGER_MOVE13,
	TIGER_MOVE14,
	TIGER_MOVE15,
	TIGER_MOVE16,
	NUMBER_OF_TIGER_MOVE
};
TIGER_MOVE tiger_move_status = TIGER_MOVE1;
int tiger_move_count = 0;

void update_tiger_motion(int timestamp_scene) {
	
	if (tiger_move_status == TIGER_MOVE1) {
		if (tiger_move_count > 28) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE2;
		}
		tiger_move_count++;
		tiger_data.move(1.0f);
		if (tiger_move_count % 2 == 0)
			tiger_data.rotate(2, tiger_data.vaxis);
		
	}
	else if (tiger_move_status == TIGER_MOVE2) {
		if (tiger_move_count > 60) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE3;
		}
		
		tiger_move_count++;
		
		tiger_data.rotate(-3, tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE3) {
		if (tiger_move_count > 62) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE4;
		}

		tiger_move_count++;
		if(tiger_move_count < 10)
			tiger_data.rotate(-2, tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE4) {
		if (tiger_move_count > 69) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE5;
		}

		tiger_move_count++;
		if (tiger_move_count < 41)
			tiger_data.rotate(2, tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE5) {
		if (tiger_move_count > 65) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE6;
		}

		tiger_move_count++;
		tiger_data.rotate(3, tiger_data.vaxis);
		tiger_data.move(0.6f);
	}
	else if (tiger_move_status == TIGER_MOVE6) {
		if (tiger_move_count > 75) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE7;
		}

		tiger_move_count++;
		if (tiger_move_count < 15 )
			tiger_data.rotate(3, tiger_data.vaxis);
		if(tiger_move_count > 43)
			tiger_data.rotate(1, tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE7) {
		if (tiger_move_count > 73) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE8;
		}

		tiger_move_count++;
		if (tiger_move_count <= 30)
		tiger_data.rotate_x(3, tiger_data.uaxis);
		tiger_data.move(0.6f);
	}
	else if (tiger_move_status == TIGER_MOVE8) {
		if (tiger_move_count > 70) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE9;
		}

		tiger_move_count++;
		if (tiger_move_count <= 30)
			tiger_data.rotate_x(3, tiger_data.uaxis);
		tiger_data.move(0.6f);
	}
	else if (tiger_move_status == TIGER_MOVE9) {
		if (tiger_move_count > 15) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE10;
		}

		tiger_move_count++;
		if (tiger_move_count <= 10)
			tiger_data.rotate(-2, -tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE10) {
		if (tiger_move_count > 40) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE11;
		}

		tiger_move_count++;
		if (tiger_move_count <= 20)
			tiger_data.rotate(3, -tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE11) {
		if (tiger_move_count > 45) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE12;
		}

		tiger_move_count++;
		if (tiger_move_count <= 20)
			tiger_data.rotate(-2, -tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE12) {
		if (tiger_move_count > 35) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE13;
		}

		tiger_move_count++;
		if (tiger_move_count <= 30)
			tiger_data.rotate(3, -tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE13) {
		if (tiger_move_count > 68) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE14;
		}

		tiger_move_count++;
		if (tiger_move_count <= 30)
			tiger_data.rotate(3, -tiger_data.vaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE14) {
		if (tiger_move_count > 66) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE15;
		}

		tiger_move_count++;
		if (tiger_move_count <= 30)
			tiger_data.rotate(3, -tiger_data.vaxis);
		if(tiger_move_count>= 20 && tiger_move_count < 50)
			tiger_data.rotate_x(3, tiger_data.uaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE15) {
		if (tiger_move_count > 19) {
			tiger_move_count = 0;
			tiger_move_status = TIGER_MOVE16;
		}

		tiger_move_count++;
		if (tiger_move_count < 18) {
			tiger_data.rotate_x(5, tiger_data.uaxis);
			//tiger_data.rotate(-3, tiger_data.vaxis);

		}//if (tiger_move_count >= 20 && tiger_move_count <= 50)
			//tiger_data.rotate_x(3, tiger_data.uaxis);
		tiger_data.move(1.0f);
	}
	else if (tiger_move_status == TIGER_MOVE16) {
		if (tiger_move_count > 19) {
			tiger_move_count = 0;
			tiger_move_status = NUMBER_OF_TIGER_MOVE;
		}

		tiger_move_count++;
		if (tiger_move_count <= 18) {
			//tiger_data.rotate_x(5, tiger_data.uaxis);
			tiger_data.rotate(-5, tiger_data.vaxis);

		}//if (tiger_move_count >= 20 && tiger_move_count <= 50)
		 //tiger_data.rotate_x(3, tiger_data.uaxis);
		tiger_data.move(0.6f);
	}
	else if (tiger_move_status == NUMBER_OF_TIGER_MOVE) {
		tiger_data.init();
		tiger_move_status = TIGER_MOVE1;
	}
}


