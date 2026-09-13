
//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases 
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;

// Lista para las figuras que tienen color por vértice
vector<MeshColor*> meshColorList;

vector<Shader> shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks


void CrearPiramideCuadrangularColor()
{
	/*
	Cada vértice contiene:
	X, Y, Z, R, G, B
	*/

	GLfloat verticesPiramideColor[] = {

		// Cara frontal roja
		 0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
		 0.0f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f,

		 // Cara izquierda verde
		 -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
		  0.0f,  0.5f,  0.0f,    0.0f, 1.0f, 0.0f,

		  // Cara trasera amarilla
		  -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
		   0.0f,  0.5f,  0.0f,    1.0f, 1.0f, 0.0f,

		   // Cara derecha magenta
			0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
			0.0f,  0.5f,  0.0f,    1.0f, 0.0f, 1.0f,

			// Base azul: primer triángulo
			-0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,

			 // Base azul: segundo triángulo
			 -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
			  0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,
			 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f
	};

	MeshColor* piramideColor = new MeshColor();

	piramideColor->CreateMeshColor(
		verticesPiramideColor,
		108
	);

	meshColorList.push_back(piramideColor);
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* piramidet = new Mesh();
	piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(piramidet);

}


//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,//frontal
		3,2,4,//izquierda
		2,1,4,//trasera
		1,0,4,//derecha
		0,1,2,//abajo1
		0,2,3//abajo2

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramidec = new Mesh();
	piramidec->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramidec);
}



/*
Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	// Shader para figuras con color por vértice
	Shader* shader2 = new Shader();

	shader2->CreateFromFiles(
		vShaderColor,
		fShaderColor
	);

	shaderList.push_back(*shader2);
}


int main()
{
	// Crear e inicializar la ventana
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	// Crear las figuras geométricas y guardarlas en meshList
	CrearCubo();                   // índice 0
	CrearPiramideTriangular();     // índice 1
	CrearCilindro(32, 1.0f);       // índice 2
	CrearCono(32, 1.0f);           // índice 3
	CrearPiramideCuadrangular();   // índice 4
	CrearPiramideCuadrangularColor(); //indice 5

	// Crear los shaders
	CreateShaders();

	/*
	La cámara inicia en el origen y observa hacia el eje Z negativo.
	Se utiliza -90 grados en yaw para mirar directamente hacia el cohete.
	*/
	camera = Camera(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f,
		0.0f,
		0.3f,
		0.3f
	);

	// Variables para obtener las ubicaciones de las uniform
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	// Matriz de proyección en perspectiva
	glm::mat4 projection = glm::perspective(
		glm::radians(60.0f),
		(GLfloat)mainWindow.getBufferWidth() /
		(GLfloat)mainWindow.getBufferHeight(),
		0.1f,
		100.0f
	);

	// Inicializar y cargar la esfera
	sp.init();
	sp.load();

	glm::mat4 model(1.0f);
	glm::vec3 color(0.0f, 0.0f, 0.0f);

	// Ciclo principal del programa
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = (GLfloat)glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;

		// Recibir eventos del teclado y del mouse
		glfwPollEvents();

		// Controlar la cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(
			mainWindow.getXChange(),
			mainWindow.getYChange()
		);

		// Limpiar la ventana y el búfer de profundidad
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/*SHADER PARA EL COHETE 
		// Activar el shader principal 
		shaderList[0].useShader();

		// Obtener la ubicación de las variables uniform
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		// Enviar la matriz de proyección al shader
		glUniformMatrix4fv(
			uniformProjection,
			1,
			GL_FALSE,
			glm::value_ptr(projection)
		);

		// Enviar la matriz de vista de la cámara
		glUniformMatrix4fv(
			uniformView,
			1,
			GL_FALSE,
			glm::value_ptr(camera.calculateViewMatrix())
		);
		*/

		
		// Activar el shader que acepta color por vértice (SHADER DE LAS PIRAMIDES)
		shaderList[1].useShader();

		uniformModel =
			shaderList[1].getModelLocation();

		uniformProjection =
			shaderList[1].getProjectLocation();

		uniformView =
			shaderList[1].getViewLocation();


		// Enviar la matriz de proyección
		glUniformMatrix4fv(
			uniformProjection,
			1,
			GL_FALSE,
			glm::value_ptr(projection)
		);


		// Enviar la matriz de vista
		glUniformMatrix4fv(
			uniformView,
			1,
			GL_FALSE,
			glm::value_ptr(camera.calculateViewMatrix())
		);
		
		
		//Matriz general de la figura.
		
		glm::mat4 unionPiramides(1.0f);

		unionPiramides = glm::translate(
			unionPiramides,
			glm::vec3(0.0f, 0.0f, -6.0f)
		);

		unionPiramides = glm::scale(
			unionPiramides,
			glm::vec3(2.0f, 2.0f, 2.0f)
		);

		unionPiramides = glm::rotate(
			unionPiramides,
			glm::radians(mainWindow.getrotax()),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		unionPiramides = glm::rotate(
			unionPiramides,
			glm::radians(mainWindow.getrotay()),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		unionPiramides = glm::rotate(
			unionPiramides,
			glm::radians(mainWindow.getrotaz()),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);


		
		// PAREJA SUPERIOR

		// Pirámide superior exterior
		model = glm::translate(
			unionPiramides,
			glm::vec3(0.0f, 0.75f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-45.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		// Pirámide superior interior
		model = glm::translate(
			unionPiramides,
			glm::vec3(0.0f, 0.25f, 0.0f)
		);

		// Punta hacia el centro
		model = glm::rotate(
			model,
			glm::radians(180.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(135.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		
		// PAREJA INFERIOR

		// Pirámide inferior exterior
		model = glm::translate(
			unionPiramides,
			glm::vec3(0.0f, -0.75f, 0.0f)
		);

		// Punta hacia abajo
		model = glm::rotate(
			model,
			glm::radians(180.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(135.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		// Pirámide inferior interior
		model = glm::translate(
			unionPiramides,
			glm::vec3(0.0f, -0.25f, 0.0f)
		);

		// Punta hacia el centro
		model = glm::rotate(
			model,
			glm::radians(-45.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		
		// PAREJA IZQUIERDA

		// Pirámide izquierda exterior
		model = glm::translate(
			unionPiramides,
			glm::vec3(-0.75f, 0.0f, 0.0f)
		);

		// Punta hacia la izquierda
		model = glm::rotate(
			model,
			glm::radians(90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-45.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		// Pirámide izquierda interior
		model = glm::translate(
			unionPiramides,
			glm::vec3(-0.25f, 0.0f, 0.0f)
		);

		// Punta hacia el centro
		model = glm::rotate(
			model,
			glm::radians(-90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(135.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		// PAREJA DERECHA

		// Pirámide derecha exterior
		model = glm::translate(
			unionPiramides,
			glm::vec3(0.75f, 0.0f, 0.0f)
		);

		// Punta hacia la derecha
		model = glm::rotate(
			model,
			glm::radians(-90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(135.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();


		// Pirámide derecha interior
		model = glm::translate(
			unionPiramides,
			glm::vec3(0.25f, 0.0f, 0.0f)
		);

		// Punta hacia el centro
		model = glm::rotate(
			model,
			glm::radians(90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-45.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.7071f, 0.5f, 0.7071f)
		);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		meshColorList[0]->RenderMeshColor();
		
				
		
		
		
		/* (COHETE) Todas las piezas parten de esta matriz para que el cohete
		completo pueda trasladarse y rotarse como un solo objeto.
		
		glm::mat4 cohete(1.0f);

		cohete = glm::translate(
			cohete,
			glm::vec3(0.0f, 0.0f, -7.0f)
		);

		cohete = glm::rotate(
			cohete,
			glm::radians(mainWindow.getrotax()),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		cohete = glm::rotate(
			cohete,
			glm::radians(mainWindow.getrotay()),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		cohete = glm::rotate(
			cohete,
			glm::radians(mainWindow.getrotaz()),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);


		
		// CUERPO PRINCIPAL: CILINDRO
	

		model = glm::translate(
			cohete,
			glm::vec3(0.0f, 0.0f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.65f, 2.50f, 0.65f)
		);

		color = glm::vec3(0.75f, 0.75f, 0.80f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[2]->RenderMeshGeometry();


		
		// PUNTA DEL COHETE: CONO
		

		model = glm::translate(
			cohete,
			glm::vec3(0.0f, 1.90f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.65f, 1.30f, 0.65f)
		);

		color = glm::vec3(1.0f, 0.15f, 0.10f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[3]->RenderMeshGeometry();


		
		// CUERPO DEL COHETE: CUBO

		model = glm::translate(
			cohete,
			glm::vec3(0.0f, -1.45f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(1.05f, 0.40f, 1.05f)
		);

		color = glm::vec3(0.15f, 0.25f, 0.80f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[0]->RenderMesh();


		
		//  VENTANA DEL COHETE: ESFERA APLANADA
		

		model = glm::translate(
			cohete,
			glm::vec3(0.0f, 0.40f, 0.58f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.30f, 0.30f, 0.12f)
		);

		color = glm::vec3(0.10f, 0.75f, 1.0f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		sp.render();


		
		// ALETA IZQUIERDA: PIRÁMIDE
		
		model = glm::translate(
			cohete,
			glm::vec3(-0.80f, -1.15f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.65f, 0.90f, 0.25f)
		);

		color = glm::vec3(1.0f, 0.20f, 0.10f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[4]->RenderMesh();


		
		//  ALETA DERECHA: OTRA INSTANCIA DE PIRÁMIDE
		
		model = glm::translate(
			cohete,
			glm::vec3(0.80f, -1.15f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.65f, 0.90f, 0.25f)
		);

		color = glm::vec3(1.0f, 0.20f, 0.10f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[4]->RenderMesh();


		
		// PROPULSOR IZQUIERDO: CILINDRO
		
		model = glm::translate(
			cohete,
			glm::vec3(-0.48f, -1.75f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.22f, 0.75f, 0.22f)
		);

		color = glm::vec3(0.30f, 0.30f, 0.35f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[2]->RenderMeshGeometry();


		// PROPULSOR DERECHO: CILINDRO
		
		model = glm::translate(
			cohete,
			glm::vec3(0.48f, -1.75f, 0.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.22f, 0.75f, 0.22f)
		);

		color = glm::vec3(0.30f, 0.30f, 0.35f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[2]->RenderMeshGeometry();

		//  FUEGO IZQUIERDO: CONO INVERTIDO

		model = glm::translate(
			cohete,
			glm::vec3(-0.48f, -2.35f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(180.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.20f, 0.80f, 0.20f)
		);

		color = glm::vec3(1.0f, 0.55f, 0.0f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[3]->RenderMeshGeometry();


		
		// FUEGO DERECHO: CONO INVERTIDO
		
		model = glm::translate(
			cohete,
			glm::vec3(0.48f, -2.35f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(180.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.20f, 0.80f, 0.20f)
		);

		color = glm::vec3(1.0f, 0.55f, 0.0f);

		glUniformMatrix4fv(
			uniformModel,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniform3fv(
			uniformColor,
			1,
			glm::value_ptr(color)
		);

		meshList[3]->RenderMeshGeometry();
		*/

	
		// Desactivar el shader
		glUseProgram(0);

		// Mostrar en pantalla lo dibujado
		mainWindow.swapBuffers();
	}

	return 0;
}


