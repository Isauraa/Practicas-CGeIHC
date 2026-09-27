/*
Práctica 5: Optimización y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;
// Partes del rover exportadas por separado.
Model RoverCuerpo;
Model Brazo01Inferior;
Model Brazo02Central;
Model Brazo03Pinza;
Model RuedaDelanteraIzquierda;
Model RuedaDelanteraDerecha;
Model RuedaCentralIzquierda;
Model RuedaCentralDerecha;
Model RuedaTraseraIzquierda;
Model RuedaTraseraDerecha;

// Partes del holocrón exportadas por separado desde Blender.
Model HolocronCuerpo;
Model HolocronEsquina1;
Model HolocronEsquina2;
Model HolocronEsquina3;
Model HolocronEsquina4;
Model HolocronEsquina5;
Model HolocronEsquina6;
Model HolocronEsquina7;
Model HolocronEsquina8;

// Partes jerárquicas del satélite Landsat.
Model LandsatCuerpo;
Model LandsatPanelPosZ;
Model LandsatPanelNegZ;
Model LandsatAntena;

//Lista de Skybox a crear
Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);


}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	// Inicializar explícitamente cada modelo del rover antes de cargarlo.
	RoverCuerpo = Model();
	Brazo01Inferior = Model();
	Brazo02Central = Model();
	Brazo03Pinza = Model();
	RuedaDelanteraIzquierda = Model();
	RuedaDelanteraDerecha = Model();
	RuedaCentralIzquierda = Model();
	RuedaCentralDerecha = Model();
	RuedaTraseraIzquierda = Model();
	RuedaTraseraDerecha = Model();

	// Cargar el cuerpo, las tres partes del brazo y las seis ruedas con base.
	RoverCuerpo.LoadModel("Models/Cuerpo_Rover.obj");
	Brazo01Inferior.LoadModel("Models/Brazo_01_Inferior.obj");
	Brazo02Central.LoadModel("Models/Brazo_02_Central.obj");
	Brazo03Pinza.LoadModel("Models/Brazo_03_Pinza.obj");
	RuedaDelanteraIzquierda.LoadModel("Models/Rueda_Delantera_Izquierda.obj");
	RuedaDelanteraDerecha.LoadModel("Models/Rueda_Delantera_Derecha.obj");
	RuedaCentralIzquierda.LoadModel("Models/Rueda_Central_Izquierda.obj");
	RuedaCentralDerecha.LoadModel("Models/Rueda_Central_Derecha.obj");
	RuedaTraseraIzquierda.LoadModel("Models/Rueda_Trasera_Izquierda.obj");
	RuedaTraseraDerecha.LoadModel("Models/Rueda_Trasera_Derecha.obj");

	// Inicializar explícitamente cada modelo del holocrón antes de cargarlo.
	HolocronCuerpo = Model();
	HolocronEsquina1 = Model();
	HolocronEsquina2 = Model();
	HolocronEsquina3 = Model();
	HolocronEsquina4 = Model();
	HolocronEsquina5 = Model();
	HolocronEsquina6 = Model();
	HolocronEsquina7 = Model();
	HolocronEsquina8 = Model();

	// Cargar el cuerpo y las ocho esquinas del holocrón.
	HolocronCuerpo.LoadModel("Models/cuerpoH.obj");
	HolocronEsquina1.LoadModel("Models/Esquina1.obj");
	HolocronEsquina2.LoadModel("Models/Esquina2.obj");
	HolocronEsquina3.LoadModel("Models/Esquina3.obj");
	HolocronEsquina4.LoadModel("Models/Esquina4.obj");
	HolocronEsquina5.LoadModel("Models/Esquina5.obj");
	HolocronEsquina6.LoadModel("Models/Esquina6.obj");
	HolocronEsquina7.LoadModel("Models/Esquina7.obj");
	HolocronEsquina8.LoadModel("Models/Esquina8.obj");

	// Inicializar explícitamente cada modelo del Landsat antes de cargarlo.
	LandsatCuerpo = Model();
	LandsatPanelPosZ = Model();
	LandsatPanelNegZ = Model();
	LandsatAntena = Model();

	// Cargar las cuatro piezas separadas del satélite.
	LandsatCuerpo.LoadModel("Models/Landsat/Cuerpo_Landsat.obj");
	LandsatPanelPosZ.LoadModel("Models/Landsat/Panel_Solar_PosZ.obj");
	LandsatPanelNegZ.LoadModel("Models/Landsat/Panel_Solar_NegZ.obj");
	LandsatAntena.LoadModel("Models/Landsat/Antena.obj");

	//Crear Skybox con sus 6 texturas
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelHolocron(1.0);
	glm::mat4 modelSatelite(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Ángulo independiente de cada esquina del holocrón.
	GLfloat anguloEsquina1 = 0.0f;
	GLfloat anguloEsquina2 = 0.0f;
	GLfloat anguloEsquina3 = 0.0f;
	GLfloat anguloEsquina4 = 0.0f;
	GLfloat anguloEsquina5 = 0.0f;
	GLfloat anguloEsquina6 = 0.0f;
	GLfloat anguloEsquina7 = 0.0f;
	GLfloat anguloEsquina8 = 0.0f;
	const GLfloat velocidadEsquinas = 45.0f;

	// Rotación independiente de las seis ruedas con su base.
	GLfloat anguloRuedaDelanteraIzquierda = 0.0f;
	GLfloat anguloRuedaDelanteraDerecha = 0.0f;
	GLfloat anguloRuedaCentralIzquierda = 0.0f;
	GLfloat anguloRuedaCentralDerecha = 0.0f;
	GLfloat anguloRuedaTraseraIzquierda = 0.0f;
	GLfloat anguloRuedaTraseraDerecha = 0.0f;
	const GLfloat velocidadRuedas = 45.0f;

	// Movimiento del satélite completo y rotaciones de sus tres piezas móviles.
	glm::vec3 posicionSatelite(-8.0f, -1.0f, -2.0f);
	GLfloat anguloPanelPosZ = 0.0f;
	GLfloat anguloPanelNegZ = 0.0f;
	GLfloat anguloAntena = 0.0f;
	const GLfloat velocidadSatelite = 3.0f;
	const GLfloat velocidadPartesSatelite = 50.0f;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		GLfloat deltaRotacion = now - lastTime;
		deltaTime = deltaRotacion;
		deltaTime += deltaRotacion / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Controles del holocrón:
		// 1 a 8 giran cada esquina; Shift + número la devuelve a su posición.
		bool* teclas = mainWindow.getsKeys();
		GLfloat sentidoEsquinas =
			(teclas[GLFW_KEY_LEFT_SHIFT] || teclas[GLFW_KEY_RIGHT_SHIFT]) ? -1.0f : 1.0f;

		if (teclas[GLFW_KEY_1]) anguloEsquina1 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_2]) anguloEsquina2 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_3]) anguloEsquina3 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_4]) anguloEsquina4 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_5]) anguloEsquina5 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_6]) anguloEsquina6 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_7]) anguloEsquina7 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;
		if (teclas[GLFW_KEY_8]) anguloEsquina8 += velocidadEsquinas * deltaRotacion * sentidoEsquinas;

		// El intervalo limita el giro entre la posición inicial y 65 grados.
		anguloEsquina1 = glm::clamp(anguloEsquina1, 0.0f, 65.0f);
		anguloEsquina2 = glm::clamp(anguloEsquina2, 0.0f, 65.0f);
		anguloEsquina3 = glm::clamp(anguloEsquina3, 0.0f, 65.0f);
		anguloEsquina4 = glm::clamp(anguloEsquina4, 0.0f, 65.0f);
		anguloEsquina5 = glm::clamp(anguloEsquina5, 0.0f, 65.0f);
		anguloEsquina6 = glm::clamp(anguloEsquina6, 0.0f, 65.0f);
		anguloEsquina7 = glm::clamp(anguloEsquina7, 0.0f, 65.0f);
		anguloEsquina8 = glm::clamp(anguloEsquina8, 0.0f, 65.0f);

		// Controles de las seis ruedas con base:
		// Z/X: delanteras, C/V: centrales, B/N: traseras.
		// Shift invierte el sentido y cada giro se limita a 45 grados.
		GLfloat sentidoRuedas =
			(teclas[GLFW_KEY_LEFT_SHIFT] || teclas[GLFW_KEY_RIGHT_SHIFT]) ? -1.0f : 1.0f;

		if (teclas[GLFW_KEY_Z])
			anguloRuedaDelanteraIzquierda += velocidadRuedas * deltaRotacion * sentidoRuedas;
		if (teclas[GLFW_KEY_X])
			anguloRuedaDelanteraDerecha += velocidadRuedas * deltaRotacion * sentidoRuedas;
		if (teclas[GLFW_KEY_C])
			anguloRuedaCentralIzquierda += velocidadRuedas * deltaRotacion * sentidoRuedas;
		if (teclas[GLFW_KEY_V])
			anguloRuedaCentralDerecha += velocidadRuedas * deltaRotacion * sentidoRuedas;
		if (teclas[GLFW_KEY_B])
			anguloRuedaTraseraIzquierda += velocidadRuedas * deltaRotacion * sentidoRuedas;
		if (teclas[GLFW_KEY_N])
			anguloRuedaTraseraDerecha += velocidadRuedas * deltaRotacion * sentidoRuedas;

		anguloRuedaDelanteraIzquierda = glm::clamp(anguloRuedaDelanteraIzquierda, -45.0f, 45.0f);
		anguloRuedaDelanteraDerecha = glm::clamp(anguloRuedaDelanteraDerecha, -45.0f, 45.0f);
		anguloRuedaCentralIzquierda = glm::clamp(anguloRuedaCentralIzquierda, -45.0f, 45.0f);
		anguloRuedaCentralDerecha = glm::clamp(anguloRuedaCentralDerecha, -45.0f, 45.0f);
		anguloRuedaTraseraIzquierda = glm::clamp(anguloRuedaTraseraIzquierda, -45.0f, 45.0f);
		anguloRuedaTraseraDerecha = glm::clamp(anguloRuedaTraseraDerecha, -45.0f, 45.0f);

		// Movimiento del Satelite_Landsat.
		// J/L: eje X, I/K: eje Y, U/O: eje Z.
		if (teclas[GLFW_KEY_J]) posicionSatelite.x -= velocidadSatelite * deltaRotacion;
		if (teclas[GLFW_KEY_L]) posicionSatelite.x += velocidadSatelite * deltaRotacion;
		if (teclas[GLFW_KEY_I]) posicionSatelite.y += velocidadSatelite * deltaRotacion;
		if (teclas[GLFW_KEY_K]) posicionSatelite.y -= velocidadSatelite * deltaRotacion;
		if (teclas[GLFW_KEY_U]) posicionSatelite.z += velocidadSatelite * deltaRotacion;
		if (teclas[GLFW_KEY_O]) posicionSatelite.z -= velocidadSatelite * deltaRotacion;

		// F, G y H rotan los dos paneles y la antena. 
		GLfloat sentidoSatelite =
			(teclas[GLFW_KEY_LEFT_SHIFT] || teclas[GLFW_KEY_RIGHT_SHIFT]) ? -1.0f : 1.0f;
		if (teclas[GLFW_KEY_F])
			anguloPanelPosZ += velocidadPartesSatelite * deltaRotacion * sentidoSatelite;
		if (teclas[GLFW_KEY_G])
			anguloPanelNegZ += velocidadPartesSatelite * deltaRotacion * sentidoSatelite;
		if (teclas[GLFW_KEY_H])
			anguloAntena += velocidadPartesSatelite * deltaRotacion * sentidoSatelite;

		anguloPanelPosZ = glm::clamp(anguloPanelPosZ, -90.0f, 90.0f);
		anguloPanelNegZ = glm::clamp(anguloPanelNegZ, -90.0f, 90.0f);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[2]->RenderMeshModel();

		// DIBUJO DEL ROVER
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -1.5f));

		// Cuerpo: nodo principal.
		color = glm::vec3(0.75f, 0.75f, 0.78f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RoverCuerpo.RenderModel();

		// Brazo jerárquico de tres partes.
		const glm::vec3 pivoteBrazo01(1.701f, 5.982f, -1.100f);
		const glm::vec3 desplazamientoBrazo02(2.535f, 2.556f, 0.000f);
		const glm::vec3 desplazamientoBrazo03(-2.648f, 2.952f, 0.186f);

		glm::mat4 matrizBrazo01 = model;
		matrizBrazo01 = glm::translate(matrizBrazo01, pivoteBrazo01);
		color = glm::vec3(0.55f, 0.15f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizBrazo01));
		Brazo01Inferior.RenderModel();

		glm::mat4 matrizBrazo02 = matrizBrazo01;
		matrizBrazo02 = glm::translate(matrizBrazo02, desplazamientoBrazo02);
		color = glm::vec3(0.78f, 0.18f, 0.82f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizBrazo02));
		Brazo02Central.RenderModel();

		glm::mat4 matrizBrazo03 = matrizBrazo02;
		matrizBrazo03 = glm::translate(matrizBrazo03, desplazamientoBrazo03);
		color = glm::vec3(0.95f, 0.20f, 0.65f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizBrazo03));
		Brazo03Pinza.RenderModel();

		// Rueda delantera izquierda: tecla Z.
		color = glm::vec3(1.0f, 0.35f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.100f, 4.208f, 3.054f));
		modelaux = glm::rotate(modelaux, anguloRuedaDelanteraIzquierda * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		RuedaDelanteraIzquierda.RenderModel();

		// Rueda delantera derecha: tecla X.
		color = glm::vec3(0.0f, 0.75f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(2.100f, 4.208f, -3.066f));
		modelaux = glm::rotate(modelaux, anguloRuedaDelanteraDerecha * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		RuedaDelanteraDerecha.RenderModel();

		// Rueda central izquierda: tecla C.
		color = glm::vec3(0.15f, 0.80f, 0.25f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-3.035f, 3.284f, 3.009f));
		modelaux = glm::rotate(modelaux, anguloRuedaCentralIzquierda * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		RuedaCentralIzquierda.RenderModel();

		// Rueda central derecha: tecla V.
		color = glm::vec3(1.0f, 0.80f, 0.10f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-3.037f, 3.284f, -3.009f));
		modelaux = glm::rotate(modelaux, anguloRuedaCentralDerecha * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		RuedaCentralDerecha.RenderModel();

		// Rueda trasera izquierda: tecla B.
		color = glm::vec3(0.15f, 0.35f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-3.035f, 3.284f, 3.009f));
		modelaux = glm::rotate(modelaux, anguloRuedaTraseraIzquierda * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		RuedaTraseraIzquierda.RenderModel();

		// Rueda trasera derecha: tecla N.
		color = glm::vec3(0.90f, 0.15f, 0.15f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		modelaux = glm::translate(modelaux, glm::vec3(-3.037f, 3.284f, -3.009f));
		modelaux = glm::rotate(modelaux, anguloRuedaTraseraDerecha * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		RuedaTraseraDerecha.RenderModel();

		// ---------------------------------------------------------------
		// HOLOCRÓN
		// Se traslada el centro original al origen, se reduce su tamaño
		// y después se coloca a la derecha del rover.
		const glm::vec3 centroHolocron(-47.987f, 5.376f, 0.651f);
		const glm::vec3 centroEsquina1(-45.409f, 7.956f, 3.332f);
		const glm::vec3 centroEsquina2(-45.372f, 8.085f, -1.873f);
		const glm::vec3 centroEsquina3(-45.375f, 2.818f, -2.018f);
		const glm::vec3 centroEsquina4(-45.389f, 2.681f, 3.174f);
		const glm::vec3 centroEsquina5(-50.611f, 2.684f, 3.207f);
		const glm::vec3 centroEsquina6(-50.596f, 2.794f, -2.024f);
		const glm::vec3 centroEsquina7(-50.598f, 7.938f, 3.338f);
		const glm::vec3 centroEsquina8(-50.601f, 8.075f, -1.892f);
		const glm::vec3 direccionEsquina1 = glm::normalize(centroEsquina1 - centroHolocron);
		const glm::vec3 direccionEsquina2 = glm::normalize(centroEsquina2 - centroHolocron);
		const glm::vec3 direccionEsquina3 = glm::normalize(centroEsquina3 - centroHolocron);
		const glm::vec3 direccionEsquina4 = glm::normalize(centroEsquina4 - centroHolocron);
		const glm::vec3 direccionEsquina5 = glm::normalize(centroEsquina5 - centroHolocron);
		const glm::vec3 direccionEsquina6 = glm::normalize(centroEsquina6 - centroHolocron);
		const glm::vec3 direccionEsquina7 = glm::normalize(centroEsquina7 - centroHolocron);
		const glm::vec3 direccionEsquina8 = glm::normalize(centroEsquina8 - centroHolocron);
		modelHolocron = glm::mat4(1.0f);
		modelHolocron = glm::translate(modelHolocron, glm::vec3(10.0f, 1.0f, -2.0f));
		modelHolocron = glm::scale(modelHolocron, glm::vec3(0.45f, 0.45f, 0.45f));
		modelHolocron = glm::translate(modelHolocron, -centroHolocron);

		// El cuerpo azul permanece fijo.
		color = glm::vec3(0.05f, 0.35f, 0.90f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHolocron));
		HolocronCuerpo.RenderModel();

		// cada pieza permanece en su lugar y gira sobre su propio centro y eje radial.
		color = glm::vec3(0.85f, 0.58f, 0.12f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Esquina 1: tecla 1.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina1);
		modelaux = glm::rotate(modelaux, anguloEsquina1 * toRadians,
			direccionEsquina1);
		modelaux = glm::translate(modelaux, -centroEsquina1);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina1.RenderModel();

		// Esquina 2: tecla 2.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina2);
		modelaux = glm::rotate(modelaux, anguloEsquina2 * toRadians,
			direccionEsquina2);
		modelaux = glm::translate(modelaux, -centroEsquina2);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina2.RenderModel();

		// Esquina 3: tecla 3.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina3);
		modelaux = glm::rotate(modelaux, anguloEsquina3 * toRadians,
			direccionEsquina3);
		modelaux = glm::translate(modelaux, -centroEsquina3);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina3.RenderModel();

		// Esquina 4: tecla 4.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina4);
		modelaux = glm::rotate(modelaux, anguloEsquina4 * toRadians,
			direccionEsquina4);
		modelaux = glm::translate(modelaux, -centroEsquina4);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina4.RenderModel();

		// Esquina 5: tecla 5.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina5);
		modelaux = glm::rotate(modelaux, anguloEsquina5 * toRadians,
			direccionEsquina5);
		modelaux = glm::translate(modelaux, -centroEsquina5);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina5.RenderModel();

		// Esquina 6: tecla 6.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina6);
		modelaux = glm::rotate(modelaux, anguloEsquina6 * toRadians,
			direccionEsquina6);
		modelaux = glm::translate(modelaux, -centroEsquina6);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina6.RenderModel();

		// Esquina 7: tecla 7.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina7);
		modelaux = glm::rotate(modelaux, anguloEsquina7 * toRadians,
			direccionEsquina7);
		modelaux = glm::translate(modelaux, -centroEsquina7);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina7.RenderModel();

		// Esquina 8: tecla 8.
		modelaux = modelHolocron;
		modelaux = glm::translate(modelaux, centroEsquina8);
		modelaux = glm::rotate(modelaux, anguloEsquina8 * toRadians,
			direccionEsquina8);
		modelaux = glm::translate(modelaux, -centroEsquina8);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		HolocronEsquina8.RenderModel();

		// ---------------------------------------------------------------
		// SATÉLITE LANDSAT
		// El cuerpo es el nodo padre. Los paneles y la antena heredan esta
		// matriz, por lo que todo el satélite se traslada como una unidad.
		const glm::vec3 pivotePanelPosZ(0.0f, 2.25946f, 0.45842f);
		const glm::vec3 pivotePanelNegZ(0.0f, 2.25946f, -0.45842f);
		const glm::vec3 pivoteAntena(0.0f, 2.90124f, 0.0f);

		modelSatelite = glm::mat4(1.0f);
		modelSatelite = glm::translate(modelSatelite, posicionSatelite);
		modelSatelite = glm::scale(modelSatelite, glm::vec3(1.25f, 1.25f, 1.25f));

		// Cuerpo principal: gris metálico.
		color = glm::vec3(0.72f, 0.74f, 0.80f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelSatelite));
		LandsatCuerpo.RenderModel();

		// Panel del lado +Z: tecla F
		color = glm::vec3(0.05f, 0.25f, 0.70f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = modelSatelite;
		modelaux = glm::translate(modelaux, pivotePanelPosZ);
		modelaux = glm::rotate(modelaux, anguloPanelPosZ * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -pivotePanelPosZ);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LandsatPanelPosZ.RenderModel();

		// Panel del lado -Z: tecla G
		color = glm::vec3(0.05f, 0.55f, 0.90f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = modelSatelite;
		modelaux = glm::translate(modelaux, pivotePanelNegZ);
		modelaux = glm::rotate(modelaux, anguloPanelNegZ * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = glm::translate(modelaux, -pivotePanelNegZ);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LandsatPanelNegZ.RenderModel();

		// Antena frontal: tecla H; gira sobre su base y alrededor de su eje Y.
		color = glm::vec3(0.95f, 0.65f, 0.10f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = modelSatelite;
		modelaux = glm::translate(modelaux, pivoteAntena);
		modelaux = glm::rotate(modelaux, anguloAntena * toRadians,
			glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = glm::translate(modelaux, -pivoteAntena);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		LandsatAntena.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
