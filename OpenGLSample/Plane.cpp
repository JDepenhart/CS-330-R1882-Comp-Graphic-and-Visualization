#include "Plane.h"
#include <vector>

using namespace std;

void Plane::CreateMesh()
{
	UCreatePlane(planeMesh);
}

void Plane::DestroyMesh()
{
	UDestroyPlane(planeMesh);
}

// Creates Sphere mesh
void Plane::UCreatePlane(GLMesh& mesh)
{
	// Vertex data
	GLfloat vertices[] = {
		// Vertex Positions		// Color			// Texture coords	// Index
		-1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,	0.0f, 0.0f,			//0
		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,	1.0f, 0.0f,			//1
		1.0f,  0.0f, -1.0f,		1.0f, 1.0f, 1.0f,	1.0f, 1.0f,			//2
		-1.0f, 0.0f, -1.0f,		1.0f, 1.0f, 1.0f,	0.0f, 1.0f,			//3
	};

	// Index data
	GLuint indices[] = {
		0,1,2,
		0,3,2
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3; // coordinates per vertx (x, z, y)
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.numVert = sizeof(vertices) / (sizeof(vertices[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));  // Vertex data
	mesh.numIndicies = sizeof(indices) / sizeof(indices[0]);  // index data

	// Creates VAO
	glGenVertexArrays(1, &mesh.vao); // generates VAO
	glBindVertexArray(mesh.vao);	// activates VAO

	// Creates VBO
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates VBO 0
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Sends Vertex to the GPU

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); // Activates VBO 1
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// pointer for coordinates
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);

}

// Destroys mesh
void Plane::UDestroyPlane(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao); // destroys VAO
	glDeleteBuffers(2, mesh.vbos); // destroys VBOs
}

