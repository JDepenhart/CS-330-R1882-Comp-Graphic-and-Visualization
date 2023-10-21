#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Box
{
	struct GLMesh
	{
		GLuint vao; // Vertex array object
		GLuint vbos[2]; // Vertex buffer object
		GLuint numVert; // number of verticies in an object
		GLuint numIndicies;// number if indicies in an object
	};

public:
	GLMesh boxMesh;

public:
	void CreateMesh();
	void DestroyMesh();

private:
	void UCreateBox(GLMesh& mesh);
	void UDestroyBox(GLMesh& mesh);
};

