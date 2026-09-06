//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
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
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
//shaders nuevos se crearían acá
//Shaders para dar un color fijo a los cubos y piramides.
static const char* vShaderRojo = "shaders/shaderrojo.vert";
static const char* vShaderVerde = "shaders/shaderverde.vert";
static const char* vShaderAzul = "shaders/shaderazul.vert";
static const char* vShaderCafe = "shaders/shadercafe.vert";
static const char* vShaderMagenta = "shaders/shadermagenta.vert";
static const char* vShaderAmarillo = "shaders/shaderamarillo.vert";
static const char* vShaderNegro = "shaders/shadernegro.vert";
float angulo = 0.0f;

//color café en RGB : 0.478, 0.255, 0.067
//Piramide que se utilizara en lugar de cada triangulo.

void CreaPiramide()
{
	unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2,
		3, 0, 2,
		1, 0, 3
	};

	GLfloat vertices[] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.0f,  0.5f, -0.25f,
		 0.0f, -0.5f, -0.5f
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(piramide); //indice 0
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}


void CrearCubo()
{
	unsigned int indices[] = {
		0,1,2, 2,3,0,
		1,5,6, 6,2,1,
		7,6,5, 5,4,7,
		4,0,3, 3,7,4,
		4,5,1, 1,0,4,
		3,2,6, 6,7,3
	};

	GLfloat vertices[] = {
		-0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,
		 0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(vertices, indices, 24, 36);
	meshList.push_back(cubo); //indice 1
}



void CrearLetrasyFiguras()
{
	
	//  PIRAMIDE BASE
	// Esta unica malla se reutiliza para todas las piezas triangulares.
	
	unsigned int indices_piramide[] = {
			0, 1, 2,
			1, 3, 2,
			3, 0, 2,
			1, 0, 3
	};

	GLfloat vertices_piramide[] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.0f,  0.5f, -0.25f,
		 0.0f, -0.5f, -0.5f
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMesh(vertices_piramide, indices_piramide, 12, 12);
	meshList.push_back(piramide); //indice 0

	
	//  CUBO BASE
	// Esta unica malla se reutiliza para columnas, cuadrados y rombos.
	
	unsigned int indices_cubo[] = {
			0,1,2, 2,3,0,
			1,5,6, 6,2,1,
			7,6,5, 5,4,7,
			4,0,3, 3,7,4,
			4,5,1, 1,0,4,
			3,2,6, 6,7,3
	};

	GLfloat vertices_cubo[] = {
		-0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f, 0.5f,
		 0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,
		 0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(vertices_cubo, indices_cubo, 24, 36);
	meshList.push_back(cubo); //indice 1


	// LETRAS IBA
	//Cada renglon representa un vertice con el orden: X, Y, Z, R, G, B.
		//Cada barra se construye con dos triangulos y cada triangulo usa tres vertices.
	GLfloat vertices_letras[] = {
		
		// LETRA I - COLOR AZUL (R=0, G=0, B=1)
		// Barra superior: triangulo 1
		-0.95f, 0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.47f, 0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.47f, 0.58f, 0.0f,   0.0f, 0.0f, 1.0f,

		// Barra superior: triangulo 2
		-0.95f, 0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.47f, 0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.95f, 0.58f, 0.0f,   0.0f, 0.0f, 1.0f,

		// Barra vertical: triangulo 1
		-0.76f, 0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.66f, 0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.66f, -0.58f, 0.0f,   0.0f, 0.0f, 1.0f,

		// Barra vertical: triangulo 2
		-0.76f, 0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.66f, -0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.76f, -0.58f, 0.0f,   0.0f, 0.0f, 1.0f,

		// Barra inferior: triangulo 1
		-0.95f, -0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.47f, -0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.47f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,

		// Barra inferior: triangulo 2
		-0.95f, -0.58f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.47f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.95f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,

		
		// LETRA B - COLOR ROJO (R=1, G=0, B=0)
	
		// Barra vertical izquierda: triangulo 1
		-0.32f, 0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.22f, 0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.22f, -0.75f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra vertical izquierda: triangulo 2
		-0.32f, 0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.22f, -0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.32f, -0.75f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra superior: triangulo 1
		-0.22f, 0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, 0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, 0.58f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra superior: triangulo 2
		-0.22f, 0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, 0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.22f, 0.58f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra central: triangulo 1
		-0.22f, 0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, 0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, -0.08f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra central: triangulo 2
		-0.22f, 0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, -0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.22f, -0.08f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra inferior: triangulo 1
		-0.22f, -0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, -0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, -0.75f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra inferior: triangulo 2
		-0.22f, -0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, -0.75f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.22f, -0.75f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra derecha superior: triangulo 1
		0.10f, 0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.20f, 0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.20f, 0.08f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra derecha superior: triangulo 2
		0.10f, 0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.20f, 0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, 0.08f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra derecha inferior: triangulo 1
		0.10f, -0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.20f, -0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.20f, -0.58f, 0.0f,   1.0f, 0.0f, 0.0f,

		// Barra derecha inferior: triangulo 2
		0.10f, -0.08f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.20f, -0.58f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.10f, -0.58f, 0.0f,   1.0f, 0.0f, 0.0f,

		// LETRA A - COLOR VERDE (R=0, G=1, B=0)
		
		// Diagonal izquierda: triangulo 1
		0.34f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.47f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.69f, 0.75f, 0.0f,   0.0f, 1.0f, 0.0f,

		// Diagonal izquierda: triangulo 2
		0.34f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.69f, 0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.60f, 0.75f, 0.0f,   0.0f, 1.0f, 0.0f,

		// Diagonal derecha: triangulo 1
		0.82f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.95f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.69f, 0.75f, 0.0f,   0.0f, 1.0f, 0.0f,

		// Diagonal derecha: triangulo 2
		0.82f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.69f, 0.75f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.60f, 0.75f, 0.0f,   0.0f, 1.0f, 0.0f,

		// Barra central: triangulo 1
		0.46f, 0.06f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.84f, 0.06f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.84f, -0.10f, 0.0f,   0.0f, 1.0f, 0.0f,

		// Barra central: triangulo 2
		0.46f, 0.06f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.84f, -0.10f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.46f, -0.10f, 0.0f,   0.0f, 1.0f, 0.0f
	};

	MeshColor* letras = new MeshColor();
	//Total: 72 vertices x 6 datos = 432 valores dentro del arreglo.
	letras->CreateMeshColor(vertices_letras, 432);
	meshColorList.push_back(letras); //Las letras IBA quedan en meshColorList[0].
}



void CreateShaders()
{
	//Indice 0: shader original para objetos con indices.
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	//Indice 1: shader que lee el color RGB guardado en cada vertice.
	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);

	//Indices 2 a 7: shaders de color fijo para cubos y piramides.
	Shader* rojo = new Shader();
	rojo->CreateFromFiles(vShaderRojo, fShaderColor);
	shaderList.push_back(*rojo); //indice 2

	Shader* verde = new Shader();
	verde->CreateFromFiles(vShaderVerde, fShaderColor);
	shaderList.push_back(*verde); //indice 3

	Shader* azul = new Shader();
	azul->CreateFromFiles(vShaderAzul, fShaderColor);
	shaderList.push_back(*azul); //indice 4

	Shader* cafe = new Shader();
	cafe->CreateFromFiles(vShaderCafe, fShaderColor);
	shaderList.push_back(*cafe); //indice 5

	Shader* magenta = new Shader();
	magenta->CreateFromFiles(vShaderMagenta, fShaderColor);
	shaderList.push_back(*magenta); //indice 6

	Shader* amarillo = new Shader();
	amarillo->CreateFromFiles(vShaderAmarillo, fShaderColor);
	shaderList.push_back(*amarillo); //indice 7

	Shader* negro = new Shader();
	negro->CreateFromFiles(vShaderNegro, fShaderColor);
	shaderList.push_back(*negro); //indice 8
}

void DibujarObjeto(int indiceShader, int indiceMesh,
	float x, float y, float z,
	float escalaX, float escalaY, float escalaZ,
	float giroZ, const glm::mat4& projection)
{
	shaderList[indiceShader].useShader();

	GLuint uniformModel = shaderList[indiceShader].getModelLocation();
	GLuint uniformProjection = shaderList[indiceShader].getProjectLocation();

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(x, y, z));

	//Rotacion ligera sobre Y para observar profundidad.
	//No se rota sobre X para mantener horizontal la barra negra.
	model = glm::rotate(model, -5.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, giroZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(escalaX, escalaY, escalaZ));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	meshList[indiceMesh]->RenderMesh();
}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	CreaPiramide(); //meshList[0]
	CrearCubo();     //meshList[1]
	CrearPiramideCuadrangular(); //meshList[2]

	//Las letras se conservan, pero no se generan en esta ejecucion.
	//CrearLetrasyFiguras();
	CreateShaders();
	glDisable(GL_DEPTH_TEST);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	//Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D( orthogonal) o en 3D) perspectiva
	//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); //fuera del while se usa para inicializar la matriz con una identidad

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//Recibir eventos del usuario
		glfwPollEvents();
		//Limpiar la ventana
		glClearColor(0.72f, 0.72f, 0.72f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		//Para las letras hay que usar el segundo set de shaders con índice 1 en ShaderList 
		//CODIGO LETRAS COMENTADO
		/*
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();

		glUniformMatrix4fv(
			uniformProjection,
			1,
			GL_FALSE,
			glm::value_ptr(projection)
		);
		//DIBUJAR UNICAMENTE LAS LETRAS IBA
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();
		*/
		
		
		// BARRA NEGRA INFERIOR
		
		DibujarObjeto(8, 1, 0.0f, -2.20f, -9.0f,
			9.0f, 0.30f, 0.15f, 0.0f, projection);

		// FIGURA IZQUIERDA
		
		
		DibujarObjeto(3, 0, -3.4f, -1.65f, -8.5f,
			1.15f, 0.75f, 0.45f, 180.0f, projection); //verde

		DibujarObjeto(2, 0, -3.4f,-0.90f,-8.5,
			1.15f, 0.75f, 0.45f, 180.0f, projection); //roja

		DibujarObjeto(7, 0, -3.4f, -0.15f, -8.5f,
			1.15f, 0.75f, 0.45f, 180.0f, projection); //amarilla

		//Las columnas terminan justo sobre la barra negra.
		DibujarObjeto(5, 1, -4.05f, -0.55f, -8.7f,
			0.16f, 3.00f, 0.18f, 0.0f, projection); //cafe

		DibujarObjeto(5, 1, -2.78f, -0.55f, -8.7f,
			0.16f, 3.00f, 0.18f, 0.0f, projection); //cafe

	
		// FIGURA CENTRAL
		
		
		DibujarObjeto(7, 2, -0.64f, -0.51f, -8.7f,
			1.35f, 0.68f, 0.40f, 45.0f, projection); //amarilla

		DibujarObjeto(2, 2, 0.64f, -0.51f, -8.7f,
			1.35f, 0.68f, 0.40f, -45.0f, projection); //roja

		DibujarObjeto(6, 2, -0.64f, -1.79f, -8.7f,
			1.35f, 0.68f, 0.40f, 135.0f, projection); //magenta

		DibujarObjeto(3, 2, 0.64f, -1.79f, -8.7f,
			1.35f, 0.68f, 0.40f, -135.0f, projection); //verde

		DibujarObjeto(4, 1, 0.0f, -1.15f, -8.4f,
			1.25f, 1.25f, 0.22f, 45.0f, projection); //azul

		DibujarObjeto(5, 1, 0.0f, -1.15f, -8.1f,
			0.62f, 0.62f, 0.24f, 45.0f, projection); //cafe

	
		// FIGURA DERECHA
		
		DibujarObjeto(3, 2, 2.75f, -1.55f, -8.5f,
			1.0f, 1.0f, 0.45f, 0.0f, projection); //verde

		DibujarObjeto(2, 2, 3.75f, -1.55f, -8.5f,
			1.0f, 1.0f, 0.45f, 0.0f, projection); //roja

		DibujarObjeto(7, 2, 3.25f, -1.55f, -8.2f,
			1.0f, 1.0f, 0.45f, 180.0f, projection); //amarilla

		DibujarObjeto(6, 2, 3.25f, -0.55f, -8.5f,
			1.0f, 1.0f, 0.45f, 0.0f, projection); //magenta

         

		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}
// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/