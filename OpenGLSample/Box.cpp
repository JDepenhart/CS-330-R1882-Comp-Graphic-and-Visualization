#include "Box.h"

void Box::CreateMesh()
{
	UCreateBox(boxMesh);
}

void Box::DestroyMesh()
{
	UDestroyBox(boxMesh);
}

void Box::UCreateBox(GLMesh& mesh)
{
	// Position and Color data
	GLfloat verts[] = {
		//Positions				Normals				 Texture
		//Back Face				 
		0.5f, 0.5f, -0.5f,		0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  //0
		0.5f, -0.5f, -0.5f,		0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  //1
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  //2
		-0.5f, 0.5f, -0.5f,		0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  //3

		//Bottom Face			
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  //4
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  //5
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  //6
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  //7

		//Left Face				
		-0.5f, 0.5f, -0.5f,		1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  //8
		-0.5f, -0.5f,  -0.5f,	1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  //9
		-0.5f,  -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  //10
		-0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  //11

		//Right Face			
		0.5f,  0.5f,  0.5f,		1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  //12
		0.5f,  -0.5f, 0.5f,		1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  //13
		0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  //14
		0.5f, 0.5f, -0.5f,		1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  //15

		//Top Face				
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  //16
		-0.5f,  0.5f, 0.5f,		0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  //17
		0.5f,  0.5f,  0.5f,		0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  //18
		0.5f,  0.5f,  -0.5f,	0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  //19

		//Front Face			
		-0.5f, 0.5f,  0.5f,	    0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  //20
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  //21
		0.5f,  -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  //22
		0.5f,  0.5f,  0.5f,		0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  //23
	};

	// Index data
	GLuint indices[] = {
		0,1,2,
		0,3,2,
		4,5,6,
		4,7,6,
		8,9,10,
		8,11,10,
		12,13,14,
		12,15,14,
		16,17,18,
		16,19,18,
		20,21,22,
		20,23,22
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.numVert = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.numIndicies = sizeof(indices) / sizeof(indices[0]);

	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); // Activates the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void Box::UDestroyBox(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao); // destroys VAO
	glDeleteBuffers(2, mesh.vbos); // destroys VBOs
}
