#include "Torus.h"

#include <vector>

using namespace std;

void Torus::CreateMesh()
{
	UCreateTorus(torusMesh);
}

void Torus::DestroyMesh()
{
	UDestroyTorus(torusMesh);
}

void Torus::UCreateTorus(GLMesh& mesh)
{
	int _mainSegments = 30;
	int _tubeSegments = 30;
	float _mainRadius = 1.0f;
	float _tubeRadius = .1f;

	auto mainSegmentAngleStep = glm::radians(360.0f / float(_mainSegments));
	auto tubeSegmentAngleStep = glm::radians(360.0f / float(_tubeSegments));

	std::vector<glm::vec3> vertex_list;
	std::vector<std::vector<glm::vec3>> segments_list;
	std::vector<glm::vec3> normals_list;
	std::vector<glm::vec2> texture_coords;
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 normal;
	glm::vec3 vertex;
	glm::vec2 text_coord;

	// generate the torus vertices
	auto currentMainSegmentAngle = 0.0f;
	for (auto i = 0; i < _mainSegments; i++)
	{
		// Calculate sine and cosine of main segment angle
		auto sinMainSegment = sin(currentMainSegmentAngle);
		auto cosMainSegment = cos(currentMainSegmentAngle);
		auto currentTubeSegmentAngle = 0.0f;
		std::vector<glm::vec3> segment_points;
		for (auto j = 0; j < _tubeSegments; j++)
		{
			// Calculate sine and cosine of tube segment angle
			auto sinTubeSegment = sin(currentTubeSegmentAngle);
			auto cosTubeSegment = cos(currentTubeSegmentAngle);

			// Calculate vertex position on the surface of torus
			auto surfacePosition = glm::vec3(
				(_mainRadius + _tubeRadius * cosTubeSegment) * cosMainSegment,
				(_mainRadius + _tubeRadius * cosTubeSegment) * sinMainSegment,
				_tubeRadius * sinTubeSegment);

			//vertex_list.push_back(surfacePosition);
			segment_points.push_back(surfacePosition);

			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}
		segments_list.push_back(segment_points);
		segment_points.clear();

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	float horizontalStep = 1.0 / _mainSegments;
	float verticalStep = 1.0 / _tubeSegments;
	float u = 0.0;
	float v = 0.0;

	// connect the various segments together, forming triangles
	for (int i = 0; i < _mainSegments; i++)
	{
		for (int j = 0; j < _tubeSegments; j++)
		{
			if (((i + 1) < _mainSegments) && ((j + 1) < _tubeSegments))
			{
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
				vertex_list.push_back(segments_list[i][j + 1]);
				normal = normalize(segments_list[i][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v + verticalStep));
				vertex_list.push_back(segments_list[i + 1][j + 1]);
				normal = normalize(segments_list[i + 1][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v + verticalStep));
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
				vertex_list.push_back(segments_list[i + 1][j]);
				normal = normalize(segments_list[i + 1][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v));
				vertex_list.push_back(segments_list[i + 1][j + 1]);
				normal = normalize(segments_list[i + 1][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v - verticalStep));
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
			}
			else
			{
				if (((i + 1) == _mainSegments) && ((j + 1) == _tubeSegments))
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][0]);
					normal = normalize(segments_list[i][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, 0));
					vertex_list.push_back(segments_list[0][0]);
					normal = normalize(segments_list[0][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[0][j]);
					normal = normalize(segments_list[0][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v));
					vertex_list.push_back(segments_list[0][0]);
					normal = normalize(segments_list[0][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}
				else if ((i + 1) == _mainSegments)
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][j + 1]);
					normal = normalize(segments_list[i][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v + verticalStep));
					vertex_list.push_back(segments_list[0][j + 1]);
					normal = normalize(segments_list[0][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v + verticalStep));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[0][j]);
					normal = normalize(segments_list[0][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v));
					vertex_list.push_back(segments_list[0][j + 1]);
					normal = normalize(segments_list[0][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v + verticalStep));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}
				else if ((j + 1) == _tubeSegments)
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][0]);
					normal = normalize(segments_list[i][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, 0));
					vertex_list.push_back(segments_list[i + 1][0]);
					normal = normalize(segments_list[i + 1][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i + 1][j]);
					normal = normalize(segments_list[i + 1][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, v));
					vertex_list.push_back(segments_list[i + 1][0]);
					normal = normalize(segments_list[i + 1][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}

			}
			v += verticalStep;
		}
		v = 0.0;
		u += horizontalStep;
	}

	std::vector<GLfloat> combined_values;

	// combine interleaved vertices, normals, and texture coords
	for (int i = 0; i < vertex_list.size(); i++)
	{
		vertex = vertex_list[i];
		normal = normals_list[i];
		text_coord = texture_coords[i];
		combined_values.push_back(vertex.x);
		combined_values.push_back(vertex.y);
		combined_values.push_back(vertex.z);
		combined_values.push_back(normal.x);
		combined_values.push_back(normal.y);
		combined_values.push_back(normal.z);
		combined_values.push_back(text_coord.x);
		combined_values.push_back(text_coord.y);
	}

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.numVert = vertex_list.size();
	mesh.numIndicies = 0;

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create VBOs
	glGenBuffers(1, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * combined_values.size(), combined_values.data(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void Torus::UDestroyTorus(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao); // destroys VAO
	glDeleteBuffers(2, mesh.vbos); // destroys VBOs
}

