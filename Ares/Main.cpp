#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <glh/glh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <sstream>

using namespace glh;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderScene();
//void renderSphere();
void renderQuad();
void renderBentQuad();
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Graphics::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;



// timing
float deltaTime = 0.0f;

int main(int argc, char *argv[])
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ares", NULL, NULL);
	if (window == NULL)
	{
		Util::Log::WriteError("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// center window on screen
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (mode->width - SCR_WIDTH) / 2, (mode->height - SCR_HEIGHT) / 2);


	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Util::Log::WriteError("Failed to initialize GLAD");
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// build and compile shaders
	// -------------------------
	Graphics::Shader pbrShader("Data/Shaders/pbr.vs", "Data/Shaders/pbr.fs");
	Graphics::Shader simpleDepthShader("Data/Shaders/simpleDepth.vs", "Data/Shaders/simpleDepth.fs");
	Graphics::Shader instanceShader("Data/Shaders/Instanced.vs", "Data/Shaders/Instanced.fs");


	pbrShader.use();
	pbrShader.setInt("albedoMap", 0);
	pbrShader.setInt("normalMap", 1);
	pbrShader.setInt("metallicMap", 2);
	pbrShader.setInt("roughnessMap", 3);
	pbrShader.setInt("aoMap", 4);
	pbrShader.setInt("depthMap", 5);

	instanceShader.use();
	instanceShader.setInt("albedoMap", 0);
	instanceShader.setInt("normalMap", 1);
	instanceShader.setInt("metallicMap", 2);
	instanceShader.setInt("roughnessMap", 3);
	instanceShader.setInt("aoMap", 4);
	instanceShader.setInt("depthMap", 5);

	// load PBR material textures
	// --------------------------
	unsigned int albedo = loadTexture("Data/Textures/PBR/rocky_dirt/albedo.png");
	unsigned int normal = loadTexture("Data/Textures/PBR/rocky_dirt/normal.png");
	unsigned int metallic = loadTexture("Data/Textures/PBR/rocky_dirt/metallic.png");
	unsigned int roughness = loadTexture("Data/Textures/PBR/rocky_dirt/roughness.png");
	unsigned int ao = loadTexture("Data/Textures/PBR/rocky_dirt/ao.png");
	unsigned int depth = loadTexture("Data/Textures/PBR/rocky_dirt/depth.png");

	// pbr setup
	pbrShader.use();
	pbrShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
	pbrShader.setFloat("ao", 1.0f);
	pbrShader.setFloat("heightScale", 0.1f);

	instanceShader.use();
	instanceShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
	instanceShader.setFloat("ao", 1.0f);
	instanceShader.setFloat("heightScale", 0.0f);

	// lights
	// ------
	glm::vec3 lightPositions[] = {
		glm::vec3(0.0f, 50.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(400.0f, 400.0f, 400.0f),
	};
	int nrRows = 0;
	int nrColumns = 0;
	float spacing = 2.5;
	float rotation[49];
	for (int i = 0; i < 49; i++) {
		rotation[i] = glm::linearRand(0.0f, 360.0f);
	}

	// load models
	// -----------
	Graphics::Skybox skyboxObject = Graphics::Skybox("OceanIslands");
	
	// framebuffer
	Graphics::Framebuffer frameBuffer(SCR_WIDTH, SCR_HEIGHT);
	frameBuffer.AddColourBuffer();
	frameBuffer.AddDepthStencBuffer();
	frameBuffer.CheckStatus();




	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	Graphics::Model pineTree = Graphics::Model("Data/Models/tree/pineTree2.obj", "png");
	//Model pineTree = Model("Data/Models/rock/rock.obj", "jpg");
	//pineTree.LoadTextures(Model::ALBEDO | Model::METALLIC | Model::NORMAL | Model::ROUGHNESS);
	pineTree.LoadTextures(Graphics::Model::ALBEDO);
	
	const unsigned int amount = 10;
	glm::vec3 positions[amount];
	glm::vec3 rotations[amount];
	for (int i = 0; i < amount; i++) {
		positions[i] = glm::vec3(glm::linearRand(-10.0f, 10.0f), 2, glm::linearRand(-10.0f, 10.0f));
		//rotations[i] = glm::vec3(0, glm::linearRand(-3.1415f, 3.1415f), 0);
		rotations[i] = glm::vec3(-1.57f, 0, 0);
	}
	// generate a large list of semi-random model transformation matrices
	// ------------------------------------------------------------------

	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand((int) glfwGetTime()); // initialize random seed	
	float radius = 150.0;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model;

		glm::vec3 pos = glm::vec3(glm::linearRand(-10.0f, 10.0f), 2, glm::linearRand(-10.0f, 10.0f));
		
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, -1.57f, glm::vec3(1, 0, 0));

		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	unsigned int instancedBuffer;
	glGenBuffers(1, &instancedBuffer);
	

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	unsigned int VAO = pineTree.getVAO();
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, instancedBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);





	camera.SetMovementSpeed(1.0f);
	camera.SetPosition(0.0f, 1.8f, 4.0f);

	Graphics::LightBuffer lightMap = Graphics::LightBuffer();






	float lastTime = 0.0f;
	unsigned int frames = 0;
	float lastLog = (float)glfwGetTime();
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		if (currentTime - lastLog >= 1.0f) {
			lastLog += 1.0f;
			Util::Log::Write(Util::Log::LOG_DEBUG, ("FPS: " + std::to_string(frames) + " (" + std::to_string(1000.0/frames) + " ms)"));
			frames = 0;
		}
		frames++;

		// input
		// -----
		processInput(window);

		// rendering passes
		// ------
		
		/*
		/////////////////////////////////////////////////////////////
		// 1. First, render the scene to the depth map for shadows
		simpleDepthShader.use();
		
		glViewport(0, 0, lightMap.GetShadowBufferWidth(), lightMap.GetShadowBufferHeight());
		lightMap.Bind();

		// configure shader and matrices
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(	glm::vec3(-2.0f, 4.0f, -1.0f),
											glm::vec3(0.0f, 0.0f, 0.0f),
											glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		//RenderScene();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/





		/////////////////////////////////////////////////////////////
		// 2. Second, render the scene as normal
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		frameBuffer.Bind();
		frameBuffer.Clear();
		
		glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		view = camera.GetViewMatrix();
		pbrShader.use();
		pbrShader.setMat4("projection", projection);
		pbrShader.setMat4("view", view);
		pbrShader.setVec3("camPos", camera.Position);
		pbrShader.setVec3("viewPos", camera.Position);
		pbrShader.setVec3("lightPos", lightPositions[0]);

		glm::mat4 model;

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
			pbrShader.setVec3("lightPos", newPos);

			model = glm::mat4();
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			pbrShader.setMat4("model", model);
			
			// the quad is now a massive plane for the ground remember!
			//renderQuad();
		}


		// start rendering the groubnd
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, depth);

		model = glm::mat4();
		pbrShader.setMat4("model", model);
		renderQuad(); // finished rendering the ground

		if (true) {
			pbrShader.setFloat("heightScale", 0.0f);
			for (int i = 0; i < amount; i++) {
				pineTree.SetPosition(positions[i].x, positions[i].y, positions[i].z);
				pineTree.SetRotation(rotations[i].x, rotations[i].y, rotations[i].z);
				pineTree.SetScale(0.1f, 0.1f, 0.1f);
				pineTree.Draw(&pbrShader);
			}
			pbrShader.setFloat("heightScale", 0.1f);
		}
		else {

			/* 
			In general, instancing is a win if you're rendering lots of instances (1000
			is quite a bit, but not enough. Think 10,000) which contain a modest number 
			of vertices (20,000 is probably too many. Look more into 100-3000 or so).
			*/

			instanceShader.use();
			instanceShader.setMat4("projection", projection);
			instanceShader.setMat4("view", view);
			instanceShader.setVec3("camPos", camera.Position);
			instanceShader.setVec3("viewPos", camera.Position);
			instanceShader.setVec3("lightPos", lightPositions[0]);

			pineTree.BindTextures();

			glBindVertexArray(pineTree.getVAO());
			glDrawElementsInstanced(GL_TRIANGLES, pineTree.indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}


		/////////////////////////////////////////////////////////////
		// 3. render the skybox
		skyboxObject.Draw(camera.GetViewMatrix(), projection);

		/////////////////////////////////////////////////////////////
		// 4. render the frame buffer to the screen
		frameBuffer.Unbind();
		frameBuffer.DrawToScreen();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void renderScene() {

}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		/*glm::vec3 pos1(-1.0f, 0.0f, -1.0f);
		glm::vec3 pos2(-1.0f, 0.0f, 1.0f);
		glm::vec3 pos3(1.0f, 0.0f, 1.0f);
		glm::vec3 pos4(1.0f, 0.0f, -1.0f);*/

		glm::vec3 pos1(-50.0f, 0.0f, -50.0f);
		glm::vec3 pos2(-50.0f, 0.0f, 50.0f);
		glm::vec3 pos3(50.0f, 0.0f, 50.0f);
		glm::vec3 pos4(50.0f, 0.0f, -50.0f);

		// texture coordinates
		glm::vec2 uv1(0.0f, 50.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(50.0f, 0.0f);
		glm::vec2 uv4(50.0f, 50.0f);
		// normal vector
		glm::vec3 nm(0.0f, 1.0f, 0.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1); // normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2); // texcooords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3); // tangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4); // bitangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int bentQuadVAO = 0;
unsigned int bentQuadIndexCount;
void renderBentQuad()
{
	if (bentQuadVAO == 0)
	{
		glGenVertexArrays(1, &bentQuadVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<unsigned int> indices;

		positions.push_back(glm::vec3(0, 3, -0.3));
		positions.push_back(glm::vec3(1, 3, -0.3));
		positions.push_back(glm::vec3(2, 3, -0.3));
		positions.push_back(glm::vec3(3, 3, -0.3));
		positions.push_back(glm::vec3(0, 2, -0.3));
		positions.push_back(glm::vec3(1, 2, 0));
		positions.push_back(glm::vec3(2, 2, 0));
		positions.push_back(glm::vec3(3, 2, -0.3));
		positions.push_back(glm::vec3(0, 1, -0.3));
		positions.push_back(glm::vec3(1, 1, 0));
		positions.push_back(glm::vec3(2, 1, 0));
		positions.push_back(glm::vec3(3, 1, -0.3));
		positions.push_back(glm::vec3(0, 0, -0.3));
		positions.push_back(glm::vec3(1, 0, -0.3));
		positions.push_back(glm::vec3(2, 0, -0.3));
		positions.push_back(glm::vec3(3, 0, -0.3));


		normals.push_back(glm::normalize(glm::vec3(-0.2, 0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(0, 0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(0, 0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(0.2, 0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(-0.2, 0, 1)));
		normals.push_back(glm::normalize(glm::vec3(-0.05, 0.05, 1)));
		normals.push_back(glm::normalize(glm::vec3(0.05, 0.05, 1)));
		normals.push_back(glm::normalize(glm::vec3(0.2, 0, 1)));
		normals.push_back(glm::normalize(glm::vec3(-0.2, 0, 1)));
		normals.push_back(glm::normalize(glm::vec3(-0.05, -0.05, 1)));
		normals.push_back(glm::normalize(glm::vec3(0.05, -0.05, 1)));
		normals.push_back(glm::normalize(glm::vec3(0.2, 0, 1)));
		normals.push_back(glm::normalize(glm::vec3(-0.2, -0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(0, -0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(0, -0.2, 1)));
		normals.push_back(glm::normalize(glm::vec3(0.2, -0.2, 1)));


		uv.push_back(glm::vec2((float)0 / 3, (float)3 / 3));
		uv.push_back(glm::vec2((float)1 / 3, (float)3 / 3));
		uv.push_back(glm::vec2((float)2 / 3, (float)3 / 3));
		uv.push_back(glm::vec2((float)3 / 3, (float)3 / 3));
		uv.push_back(glm::vec2((float)0 / 3, (float)2 / 3));
		uv.push_back(glm::vec2((float)1 / 3, (float)2 / 3));
		uv.push_back(glm::vec2((float)2 / 3, (float)2 / 3));
		uv.push_back(glm::vec2((float)3 / 3, (float)2 / 3));
		uv.push_back(glm::vec2((float)0 / 3, (float)1 / 3));
		uv.push_back(glm::vec2((float)1 / 3, (float)1 / 3));
		uv.push_back(glm::vec2((float)2 / 3, (float)1 / 3));
		uv.push_back(glm::vec2((float)3 / 3, (float)1 / 3));
		uv.push_back(glm::vec2((float)0 / 3, (float)0 / 3));
		uv.push_back(glm::vec2((float)1 / 3, (float)0 / 3));
		uv.push_back(glm::vec2((float)2 / 3, (float)0 / 3));
		uv.push_back(glm::vec2((float)3 / 3, (float)0 / 3));

		indices.push_back(1 - 1);
		indices.push_back(5 - 1);
		indices.push_back(6 - 1);
		indices.push_back(1 - 1);
		indices.push_back(6 - 1);
		indices.push_back(2 - 1);

		indices.push_back(2 - 1);
		indices.push_back(6 - 1);
		indices.push_back(7 - 1);
		indices.push_back(2 - 1);
		indices.push_back(7 - 1);
		indices.push_back(3 - 1);

		indices.push_back(3 - 1);
		indices.push_back(7 - 1);
		indices.push_back(4 - 1);
		indices.push_back(7 - 1);
		indices.push_back(8 - 1);
		indices.push_back(4 - 1);

		indices.push_back(5 - 1);
		indices.push_back(9 - 1);
		indices.push_back(10 - 1);
		indices.push_back(5 - 1);
		indices.push_back(10 - 1);
		indices.push_back(6 - 1);

		indices.push_back(6 - 1);
		indices.push_back(10 - 1);
		indices.push_back(11 - 1);
		indices.push_back(6 - 1);
		indices.push_back(11 - 1);
		indices.push_back(7 - 1);

		indices.push_back(7 - 1);
		indices.push_back(11 - 1);
		indices.push_back(12 - 1);
		indices.push_back(7 - 1);
		indices.push_back(12 - 1);
		indices.push_back(8 - 1);

		indices.push_back(13 - 1);
		indices.push_back(10 - 1);
		indices.push_back(9 - 1);
		indices.push_back(10 - 1);
		indices.push_back(13 - 1);
		indices.push_back(14 - 1);
			
		indices.push_back(10 - 1);
		indices.push_back(14 - 1);
		indices.push_back(15 - 1);
		indices.push_back(10 - 1);
		indices.push_back(15 - 1);
		indices.push_back(11 - 1);

		indices.push_back(11 - 1);
		indices.push_back(15 - 1);
		indices.push_back(16 - 1);
		indices.push_back(11 - 1);
		indices.push_back(16 - 1);
		indices.push_back(12 - 1);

		bentQuadIndexCount = indices.size();

		// tangent space calculation
		for (unsigned int i = 0; i < bentQuadIndexCount; i += 3) {
			glm::vec3 tangent;
			glm::vec3 bitangent;

			int pos1index = indices.at(i + 0);
			int pos2index = indices.at(i + 1);
			int pos3index = indices.at(i + 2);

			glm::vec3 edge1 = positions.at(pos2index) - positions.at(pos1index);
			glm::vec3 edge2 = positions.at(pos3index) - positions.at(pos1index);
			glm::vec2 deltaUV1 = uv.at(pos2index) - uv.at(pos1index);
			glm::vec2 deltaUV2 = uv.at(pos3index) - uv.at(pos1index);

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent = glm::normalize(tangent);

			bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent = glm::normalize(bitangent);

			tangents.push_back(tangent);
			bitangents.push_back(bitangent);
		}

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}

			if (tangents.size() > 0)
			{
				data.push_back(tangents[i].x);
				data.push_back(tangents[i].y);
				data.push_back(tangents[i].z);
			}
			if (bitangents.size() > 0)
			{
				data.push_back(bitangents[i].x);
				data.push_back(bitangents[i].y);
				data.push_back(bitangents[i].z);
			}
		}
		glBindVertexArray(bentQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bentQuadIndexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		unsigned int stride = (3 + 3 + 2 + 3 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);// positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);// uv
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);// normlas
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);// tangents
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);// bitangents
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
	}

	glBindVertexArray(bentQuadVAO);
	glDrawElements(GL_TRIANGLES, bentQuadIndexCount, GL_UNSIGNED_INT, 0);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.setShift(true);
	else
		camera.setShift(false);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}