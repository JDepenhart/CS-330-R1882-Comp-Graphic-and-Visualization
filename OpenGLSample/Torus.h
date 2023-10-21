#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Torus
{
	struct GLMesh
	{
		GLuint vao; // Vertex array object
		GLuint vbos[2]; // Vertex buffer object
		GLuint numVert; // number of verticies in an object
		GLuint numIndicies;// number if indicies in an object
	};

public:
	GLMesh torusMesh;

private:
	void UCreateTorus(GLMesh& mesh);
	void UDestroyTorus(GLMesh& mesh);

public:
	void CreateMesh();
	void DestroyMesh();
};

