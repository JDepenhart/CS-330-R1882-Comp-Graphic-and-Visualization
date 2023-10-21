#include <iostream>
#include <cstdlib>
#include <GL/glew.h>        
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Cylinder.h"
#include "Sphere.h"
#include "Plane.h"
#include "Torus.h"
#include "Box.h"
#include "Texture.h"
#include "camera.h"

using namespace std;

// Shader macro
#ifndef GLSL 
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif 

// namespace for glWindow
namespace
{
	// window title
	const char* const windowTitle = "Jeremy Depenhart";

	// window width and height
	const int windowWidth = 800;
	const int windowHeight = 600;

	// GL mesh data
	struct GLMesh
	{
		GLuint vao; // vertex array object
		GLuint vbos[2]; // vertex buffer objects
		GLuint nIndices; // amount of indecies in a mesh
	};

	// Window
	GLFWwindow* gWindow = nullptr;

	// Meshes 
	Sphere sphere;
	Cylinder cylinder;
	Plane plane;
	Torus torus;
	Box box;

	// Texture
	Texture texture;
	GLuint gTextureGlass;
	GLuint gTextureLogo;
	GLuint gTextureCap;
	GLuint gTextureDarkWood;
	GLuint gTextureMetal;
	GLuint gTextureMatteBlack;
	GLuint gTextureArt;
 	GLuint gTexturePaper;
	GLuint gTextureWallLight;
	GLuint gTextureCanvas;

	glm::vec2 gUVScale(1.0f, 1.0f);

	// Shader
	GLuint gModelProgramId;
	//GLuint gLampProgramId;

	// camera
	Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
	float gLastX = windowWidth / 2.0f;
	float gLastY = windowHeight / 2.0f;
	bool gFirstMouse = true;

	// timing
	float gDeltaTime = 0.0f;
	float gLastFrame = 0.0f;

	//Lighing 
	// position and scale
	glm::vec3 gModelPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 gModelScale(2.0);

	// Cube and light color
	glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f);
	glm::vec3 gLightColor(0.5f, 0.5f, 0.5f);
	glm::vec3 gLightColor2(1.0f, 1.0f, 0.0f);

	// Light position and scale
	glm::vec3 gLightPosition(0.0f, 0.5f, 10.0f);
	glm::vec3 gLightPosition2(0.0f, -1.0f, -10.0f);

	glm::vec3 gLightScale(0.3f);
}

#pragma region Definitions
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
#pragma endregion

#pragma region Shader Source code
const GLchar* cubeVertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
	layout(location = 1) in vec3 normal; // VAP position 1 for normals
	layout(location = 2) in vec2 textureCoordinate;

	out vec3 vertexNormal; // For outgoing normals to fragment shader
	out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
	out vec2 vertexTextureCoordinate;

	//Uniform / Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

		vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

		vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
		vertexTextureCoordinate = textureCoordinate;
	}
);

// Fragment shader source code
const  GLchar* cubeFragmentShaderSource = GLSL(440,
	in vec3 vertexNormal; // For incoming normals
	in vec3 vertexFragmentPos; // For incoming fragment position
	in vec2 vertexTextureCoordinate;
	out vec4 fragmentColor; // used to transfer fragment color data

	uniform vec3 objectColor;

	// Lights
	uniform vec3 lightColor; // light 1
	uniform vec3 lightPos;
	uniform vec3 lightColor2; // light 2
	uniform vec3 lightPos2;

	uniform vec3 viewPosition;
	uniform sampler2D uTexture;
	uniform sampler2D uTextureExtra;
	uniform bool multipleTextures;
	uniform vec2 uvScale;

	void main()
	{
		// Ambient lighting (global)
		float ambientStrength = 0.1f; // Ambient strength
		vec3 ambient = ambientStrength * glm::vec3(1.0f, 1.0f, 1.0f); // Generates color

		// Diffuse lighting
		vec3 norm = normalize(vertexNormal);
		// Light 1
		vec3 lightDirection = normalize(lightPos - vertexFragmentPos);
		float impact = max(dot(norm, lightDirection), 0.0);
		vec3 diffuse = impact * lightColor;
		// Light 2
		vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos);
		float impact2 = max(dot(norm, lightDirection2), 0.0);
		vec3 diffuse2 = impact2 * lightColor2;

		// Specular lighting
		// Light 1
		float specularIntensity = 1.0f; // Set specular light strength
		float highlightSize = 7.0f; // Set specular highlight size
		// Light 2
		float specularIntensity2 = 0.01f; // Set specular light strength
		float highlightSize2 = 3.0f; // Set specular highlight size

		vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction

		// Calculates reflection vector 
		vec3 reflectDir = reflect(-lightDirection, norm); // Light 1
		vec3 reflectDir2 = reflect(-lightDirection2, norm); // Light 2

		//Calculate specular component
		// Light 1
		float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
		vec3 specular = specularIntensity * specularComponent * lightColor;
		// Light 2
		float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
		vec3 specular2 = specularIntensity2 * specularComponent2 * lightColor2;

		// Texture holds the color to be used for all three components
		vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

		// Calculates phong
		vec3 phong = (ambient + diffuse + specular) * textureColor.xyz; // Light 1
		vec3 phong2 = (ambient + diffuse2 + specular2) * textureColor.xyz; // Light 2

		fragmentColor = vec4(phong + phong2, 1.0); // Send lighting results to GPU

		if (multipleTextures)
		{
			vec4 extraTexture = texture(uTextureExtra, vertexTextureCoordinate * uvScale);
			vec3 phong = (ambient + diffuse + specular) * extraTexture.xyz;
			vec3 phong2 = (ambient + diffuse2 + specular2) * extraTexture.xyz; // Light 2

			if (extraTexture.a != 0.0)
				fragmentColor = vec4(phong + phong2, 1.0); // Send lighting results to GPU
		}
	}
);

// possibly add lamp shaders
#pragma endregion

int main(int argc, char* argv[])
{
	// if UInitialize returns false
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE; // Terminates program

	// calls UCreateMesh
	cylinder.CreateMesh();
	sphere.CreateMesh();
	plane.CreateMesh();
	torus.CreateMesh();
	box.CreateMesh();

	// if UCreateShaderProgram returns false
	if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gModelProgramId))
		return EXIT_FAILURE; // Terminates program

	// Loads textures
	const char* texFilename = "../resources/textures/Glass.png";
	if (!texture.UCreateTexture(texFilename, gTextureGlass))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/BottleCap.png";
	if (!texture.UCreateTexture(texFilename, gTextureCap))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/DarkWood.png";
	if (!texture.UCreateTexture(texFilename, gTextureDarkWood))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/Metal.png";
	if (!texture.UCreateTexture(texFilename, gTextureMetal))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/MatteBlack.png";
	if (!texture.UCreateTexture(texFilename, gTextureMatteBlack))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/ShiaLisa.png";
	if (!texture.UCreateTexture(texFilename, gTextureArt))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/Paper.png";
	if (!texture.UCreateTexture(texFilename, gTexturePaper))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/WallLight.png";
	if (!texture.UCreateTexture(texFilename, gTextureWallLight))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/Canvas.png";
	if (!texture.UCreateTexture(texFilename, gTextureCanvas))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "../resources/textures/ShiaLogo.png";	// Needs to be on top
	if (!texture.UCreateTexture(texFilename, gTextureLogo))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	glUseProgram(gModelProgramId);
	glUniform1i(glGetUniformLocation(gModelProgramId, "uTextureBase"), 0);
	glUniform1i(glGetUniformLocation(gModelProgramId, "uTextureExtra"), 9);

	// Sets BG to black (red, green, blue, alpha)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// loops to render window while glfwWindowShouldClose returns false
	while (!glfwWindowShouldClose(gWindow))
	{
		// frame timing
		float currentFrame = glfwGetTime(); // gets current frame
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// calls UProcessInput
		UProcessInput(gWindow);

		// calls URender
		URender();

		// calls glfwPollEvents
		glfwPollEvents();
	}

	// Destroys meshes
	cylinder.DestroyMesh();
	sphere.DestroyMesh();
	plane.DestroyMesh();
	torus.DestroyMesh();
	box.DestroyMesh();

	// Destroys Textures
	texture.UDestroyTexture(gTextureGlass);
	// Add other destroy functions for other textures

	// calls UDestroyShaderProgram
	UDestroyShaderProgram(gModelProgramId);

	// Terminates program
	exit(EXIT_SUCCESS);
}

#pragma region Window creation & Handling
// creates window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// glfw window initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // For Apple Devices

	// GLFW window creation
	* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (*window == NULL) // Window is NULL
	{
		// gives error, closes window and returns false
		cout << "FAILURE: could not create Window!" << endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// mouse capture
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW initialization
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	// GLEW initialization fails
	if (GLEW_OK != GlewInitResult)
	{
		// gives error and returns false
		cerr << glewGetErrorString(GlewInitResult) << endl;
		return false;
	}

	// Displays OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}

// Processes user input
void UProcessInput(GLFWwindow* window)
{
	// "esc" key closes window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// "W" key moves camera forwards
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);

	// "S" key moves camera backwards
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);

	// "A" key moves camera left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);

	// "D" key moves camera right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

	// "Q" key moves camea up
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);

	// "E" key moves camera down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
}

// callback for mouse movement
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos;

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// callback for mouse scroll
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// mouse click event
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
	break;

	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}
}

// Processes window resizing
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Handles frame rendering
void URender()
{
	// Projections
	glm::mat4 perspective = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);
	glm::mat4 orthographic = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	glm::mat4 projection = perspective;
	// Projection indicator
	bool isPerspective = false;

	// "P" key projection changes
	if (glfwGetKey(gWindow, GLFW_KEY_P) == GLFW_PRESS && isPerspective == true)
	{
		isPerspective = false;
		projection = perspective;
	}
	if (glfwGetKey(gWindow, GLFW_KEY_P) == GLFW_PRESS && isPerspective == false)
	{
		isPerspective = true;
		projection = orthographic;
	}

	// z-depth
	glEnable(GL_DEPTH_TEST);

	// sets window color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gModelProgramId);

	// Transforms camera
	glm::mat4 view = gCamera.GetViewMatrix();

	// sends transform data to the shader program
	GLint modelLoc = glGetUniformLocation(gModelProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gModelProgramId, "view");
	GLint projLoc = glGetUniformLocation(gModelProgramId, "projection");
	GLuint texturesLoc = glGetUniformLocation(gModelProgramId, "texture");
	GLuint multipleTexturesLoc = glGetUniformLocation(gModelProgramId, "multipleTextures");
	glUniform1i(multipleTexturesLoc, false);

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(gModelProgramId, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(gModelProgramId, "lightColor"); // Light color 1
	GLint lightPositionLoc = glGetUniformLocation(gModelProgramId, "lightPos"); // Light pos 1
	GLint lightColorLoc2 = glGetUniformLocation(gModelProgramId, "lightColor2"); // Light color 2
	GLint lightPositionLoc2 = glGetUniformLocation(gModelProgramId, "lightPos2"); // Light pos 2
	GLint viewPositionLoc = glGetUniformLocation(gModelProgramId, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);

	// Light 1 color and position
	glUniform3f(lightColorLoc, gLightColor.x, gLightColor.y, gLightColor.z);
	glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
	// Light 2 color and position
	glUniform3f(lightColorLoc2, gLightColor2.x, gLightColor2.y, gLightColor2.z);
	glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

	const glm::vec3 cameraPosition = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	GLint UVScaleLoc = glGetUniformLocation(gModelProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

#pragma region Bottle
	/*BOTTLE*/
	// bottle parent
	glm::mat4 bottleScale = glm::scale(glm::vec3(0.09f, 0.09f, 0.09f)); // scale
	glm::mat4 bottleRotation = glm::rotate(190.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // rotation
	glm::mat4 bottleTranslation = glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)); // translation
	glm::mat4 bottleModel = bottleTranslation * bottleRotation * bottleScale;

	//Bottle Cylinder 1 base
	// Turns on multiple textures
	multipleTexturesLoc = glGetUniformLocation(gModelProgramId, "multipleTextures");
	glUniform1i(multipleTexturesLoc, false);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureGlass);

	glActiveTexture(GL_TEXTURE9);// GL_TEXTURE8 number must be changes to the highest one
	glBindTexture(GL_TEXTURE_2D, gTextureLogo);

	glBindVertexArray(cylinder.cylinderMesh.vao);

	glm::mat4 scale = glm::scale(glm::vec3(3.0f, 10.0f, 3.0f)); // scale
	glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f)); // rotation
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)); // translation
	glm::mat4 model = bottleModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gModelProgramId, texturesLoc, 1.0f, 1.0f, 0.0f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);

	multipleTexturesLoc = glGetUniformLocation(gModelProgramId, "multipleTextures");
	glUniform1i(multipleTexturesLoc, true);

	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);

	// Turns off multiple textures
	multipleTexturesLoc = glGetUniformLocation(gModelProgramId, "multipleTextures");
	glUniform1i(multipleTexturesLoc, false);

	// Deactivates the VAO
	glBindVertexArray(0);

	//Cylinder 2 top
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureGlass);
	glBindVertexArray(cylinder.cylinderMesh.vao);

	scale = glm::scale(glm::vec3(1.6f, 7.0f, 1.6f)); // scale
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f)); // rotation
	translation = glm::translate(glm::vec3(0.0f, 12.0f, 0.0f)); // translation
	model = bottleModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gModelProgramId, texturesLoc, 1.0f, 1.0f, 1.0f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);

	// Deactivates the VAO
	glBindVertexArray(0);

	//Cylinder 3 lid
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCap);
	glBindVertexArray(cylinder.cylinderMesh.vao);

	scale = glm::scale(glm::vec3(1.75f, 0.5f, 1.75f)); // scale
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f)); // rotation
	translation = glm::translate(glm::vec3(0.0f, 19.0f, 0.0f)); // translation
	model = bottleModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gModelProgramId, texturesLoc, 1.0f, 1.0f, 1.0f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);

	// Deactivates the VAO
	glBindVertexArray(0);

	// sphere
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureGlass);
	glBindVertexArray(sphere.sphererMesh.vao);

	scale = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 10.0f, 0.0f));
	model = bottleModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gModelProgramId, texturesLoc, 0.0f, 0.0f, 1.0f, 1.0f);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, sphere.sphererMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(sphere.sphererMesh.vao);
#pragma endregion

#pragma region Binder
	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*BINDER*/
	// binder parent
	glm::mat4 binderScale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); // scale
	glm::mat4 binderRotation = glm::rotate(-0.85f, glm::vec3(0.0f, 1.0f, 0.0f)); // rotation
	glm::mat4 binderTranslation = glm::translate(glm::vec3(2.5f, 0.101f, 0.0f)); // translation
	glm::mat4 binderModel = binderTranslation * binderRotation * binderScale;

	// Ring 1 Middle
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureMetal);
	glBindVertexArray(torus.torusMesh.vao);

	scale = glm::scale(glm::vec3(0.08f, 0.08f, 0.08f));
	rotation = glm::rotate(1.57f, glm::vec3(1.0, 0.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.386f, 0.0f, 0.0f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, torus.torusMesh.numVert);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Ring 2 Top
	glBindVertexArray(torus.torusMesh.vao);

	scale = glm::scale(glm::vec3(0.08f, 0.08f, 0.08f));
	rotation = glm::rotate(1.57f, glm::vec3(1.0, 0.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.386f, 0.38f, 0.0f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, torus.torusMesh.numVert);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Ring 3 Bottom
	glBindVertexArray(torus.torusMesh.vao);

	scale = glm::scale(glm::vec3(0.08f, 0.08f, 0.08f));
	rotation = glm::rotate(1.57f, glm::vec3(1.0, 0.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.386f, -0.4f, 0.0f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, torus.torusMesh.numVert);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Ring Holder
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(0.13f, 1.0f, 0.02f));
	rotation = glm::rotate(0.3f, glm::vec3(0.0, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.41f, 0.0f, -0.08f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Spine
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureMatteBlack);
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(0.2f, 1.1f, 0.01f));
	rotation = glm::rotate(0.3f, glm::vec3(0.0, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.42f, 0.0f, -0.087f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Back
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(0.75f, 1.1f, 0.01f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.045f, 0.0f, -0.115f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Front
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(0.75f, 1.1f, 0.01f));
	rotation = glm::rotate(-2.3f, glm::vec3(0.0, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.76f, 0.0f, 0.22f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Paper
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexturePaper);
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(0.7f, 1.0f, 0.001f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	model = binderModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

#pragma endregion

#pragma region Painting
	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*PAINTING*/
	// painting parent
	glm::mat4 paintingScale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)); // scale
	glm::mat4 paintingRotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // rotation
	glm::mat4 paintingTranslation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)); // translation
	glm::mat4 paintingModel = paintingTranslation * paintingRotation * paintingScale;

	// Base
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCanvas);
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(1.5f, 2.0f, 0.3f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-1.0f, 0.0f, -1.0f));
	model = paintingModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Art
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureArt);
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(1.5f, 2.0f, 0.001f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-1.0f, 0.0f, -0.85f));
	model = paintingModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0); 
#pragma endregion

#pragma region Wall Light
	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*WALL LIGHT*/
	// WALL LIGHT parent
	glm::mat4 lightScale = glm::scale(glm::vec3(1.0f, 1.0f, 0.4f)); // scale
	glm::mat4 lightRotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // rotation
	glm::mat4 lightTranslation = glm::translate(glm::vec3(1.0f, 1.7f, -1.0f)); // translation
	glm::mat4 lightModel = lightTranslation * lightRotation * lightScale;

	// Base
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureMetal);
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(1.8f, 0.3f, 0.3f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-1.0f, 0.0f, -1.0f));
	model = lightModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Light
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureWallLight);
	glBindVertexArray(box.boxMesh.vao);

	scale = glm::scale(glm::vec3(1.4f, 0.25f, 0.001f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-0.85f, 0.0f, -0.85f));
	model = lightModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, box.boxMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Button
	glBindVertexArray(sphere.sphererMesh.vao);

	scale = glm::scale(glm::vec3(0.05f, 0.05f, 0.05f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(-1.7f, 0.0f, -0.85f));
	model = lightModel * translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gModelProgramId, texturesLoc, 0.0f, 0.0f, 1.0f, 1.0f);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, sphere.sphererMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(sphere.sphererMesh.vao);
#pragma endregion

#pragma region Main Plane
	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*Main Plane*/
	// Plane
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureDarkWood);
	glBindVertexArray(plane.planeMesh.vao);

	scale = glm::scale(glm::vec3(3.0f, 1.0f, 1.5f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.7f, -1.0f, 0.0f));
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, plane.planeMesh.numIndicies, GL_UNSIGNED_INT, (void*)0);
#pragma endregion

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glUseProgram(0);

	glfwSwapBuffers(gWindow);
}
#pragma endregion

#pragma region Shader Program Functions
// Creates Shader Program
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Creates Shader Program Object
	programId = glCreateProgram();

	// Create vertex and fragment objects
	GLuint vtxShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrives shader program source
	glShaderSource(vtxShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragShaderId, 1, &fragShaderSource, NULL);

	// vertex shader
	glCompileShader(vtxShaderId); // Compiles the vertex shader 
	glGetShaderiv(vtxShaderId, GL_COMPILE_STATUS, &success); // Checks for errors
	if (!success)// if vertex shader fails to compile
	{
		glGetShaderInfoLog(vtxShaderId, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;

		return false;
	}

	// fragment shader
	glCompileShader(fragShaderId); // Compiles the fragment shader
	glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &success); // Checks for errors
	if (!success)// if fragment shader fails to compile
	{
		glGetShaderInfoLog(fragShaderId, sizeof(infoLog), NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;

		return false;
	}

	// Attaches compiled shaders to the shader program
	glAttachShader(programId, vtxShaderId);
	glAttachShader(programId, fragShaderId);

	// Shader Link
	glLinkProgram(programId);   // links the shader program
	glGetProgramiv(programId, GL_LINK_STATUS, &success); // Checks for errors
	if (!success) // if shader fails to Link
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;

		return false;
	}

	glUseProgram(programId); // Uses the shader program

	return true;
}

// Destroys shader program
void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}
#pragma endregion