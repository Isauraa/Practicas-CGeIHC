/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
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
//clases para dar orden y limpieza al còdigo
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
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




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
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
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

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}



// Version terminada del rover aplicando modelado jerarquico.

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(18, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();


	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.3f);


	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(65.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux2(1.0);
	glm::mat4 raizSonda(1.0f);
	glm::mat4 ejePanelIzquierdo(1.0f);
	glm::mat4 ejePanelDerecho(1.0f);
	glm::mat4 ejeAntena(1.0f);

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();


		//articulacion1 hasta articulación5 sólo son puntos de rotación o articulación, en este caso no dibujaremos esferas que los representen
		// RAIZ DEL ROVER. E, R y T giran toda la figura completa.
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -1.0f, -18.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.50f, 0.50f, 0.50f));
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
		modelaux = model; //guardamos la matriz de modelo para que la base se mueva con el objeto
		// Creando la cabina del rover ¿CUÁNTAS UNIDADES MEDIRÁ EN Z?
		model = glm::translate(model, glm::vec3(1.0f, 2.0f, 0.0f));//PARA LLEGAR AL CENTRO DE LA CABINA a PARTIR DEL ORIGEN
		model = glm::scale(model, glm::vec3(8.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular
		//meshList[3]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro, cono, pirámide base cuadrangular
		//sp.render(); //dibuja esfera


		// BASE
		//model = glm::mat4(1.0); //Si dejamos está linea, la base no estará unida al centro del objeto. se debe comentar para que la base se mueva con el objeto.
		/*En su lugar usamos la matriz auxiliar modelaux para que la base se mueva con el objeto.
		Lo que debemos de saber es: de las transformaciones geométricas que se aplican a la cabina y
		cuales queremos que se apliquen a la base. En este caso, sólo queremos que se aplique la traslación del origen, no la rotación ni el escalado de la cabina.
		Por lo tanto, debemos de guardar en modelaux sólo la traslación del origen y luego aplicarla a la base.
		*/
		model = modelaux;
		//NOS POSICIONAMOS EN EL CENTRO DE LA BASE
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
		modelaux = model; //guardamos la matriz de modelo para conectar los siguientes objetos a la base
		model = glm::scale(model, glm::vec3(10.0f, 1.5f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh();

		model = modelaux;
		modelaux2 = model; //guardamos la matriz de modelo para conectar los siguientes objetos a la base
		/*
		A PARTIR DE ESTE PUNTO HAY QUE ACOMODAR VARIOS ELEMENTOS:
		LA BASE PARA LA LLANTA FRONTAL
		LA BASE PARA EL BRAZO X2
		LA BASE PARA LA LLANTA DE EN MEDIO X2
		LA BASE PARA LA LLANTA DE ATRÁS X2
		.........¿FALTA ALGO MÁS? PROFUNDIAD: 6 LLANTAS EN TOTAL
		CÓMO ESTAREMOS REGRESANDO A ESTE PUNTO EN ESPECÍFICO VARIAS VECES, DEBEMOS DE USAR UNA NUEVA MATRIZ AUXILIAR PARA CADA UNO DE LOS ELEMENTOS
		QUE SE QUIERAN CONECTAR A LA BASE, YA QUE SI USAMOS LA MISMA MATRIZ AUXILIAR, SE SOBRESCRIBIRÁN LAS TRANSFORMACIONES GEOMÉTRICAS DE CADA ELEMENTO
		Y NO SE PODRÁN CONECTAR CORRECTAMENTE.

		*/

		//BASE PARA EL BRAZO
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.75f, 1.25f, 3.0f));
		modelaux = model;
		//model:glm::scale(model, glm::vec3(, , ));
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();
		model = modelaux;

		//Eje de Rotación esfera
		model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();
		model = modelaux;

		//Brazo parte 1
		//ROTAR 135° alrededor de Z
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.25f, 1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMeshGeometry();

		//Eje de Rotación esfera 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();
		model = modelaux;

		//Brazo parte 2. Conserva la matriz de la segunda articulacion.
		glm::mat4 ejeBrazo2 = modelaux;
		ejeBrazo2 = glm::rotate(ejeBrazo2, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(ejeBrazo2, glm::vec3(0.0f, 1.25f, 0.0f));
		model = glm::scale(model, glm::vec3(0.45f, 2.5f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Eje de la pinza. La tecla H mueve la pinza y sus dos dedos.
		model = glm::translate(ejeBrazo2, glm::vec3(0.0f, 2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//Soporte central de la pinza.
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.45f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 0.35f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.55f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//Dedo izquierdo de la pinza.
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.45f, 0.95f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.9f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.65f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//Dedo derecho de la pinza.
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.45f, 0.95f, 0.0f));
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.9f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.65f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		//CONSTRUCCION EXPLICITA DE LAS SEIS LLANTAS

		//ESTRUCTURA DELANTERA IZQUIERDA: dos barras paralelas.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.05f, -0.51f, -4.12f));
		model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.13f, 2.05f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.42f, 0.42f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.05f, -0.29f, -4.12f));
		model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.13f, 2.05f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.42f, 0.42f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que conecta las barras delanteras con la base.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-4.10f, -0.08f, -4.06f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f, 0.22f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Parte horizontal de la barra delantera.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.45f, -0.72f, -4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.24f, 1.10f, 0.24f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Parte vertical de la barra delantera.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-6.0f, -1.535f, -4.12f));
		model = glm::scale(model, glm::vec3(0.24f, 1.63f, 0.24f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la barra delantera con las barras largas.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-6.0f, -0.72f, -4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.30f, 0.14f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//ESTRUCTURA DELANTERA DERECHA: dos barras paralelas.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.05f, -0.51f, 4.12f));
		model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.13f, 2.05f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.42f, 0.42f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.05f, -0.29f, 4.12f));
		model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.13f, 2.05f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.42f, 0.42f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que conecta las barras delanteras con la base.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-4.10f, -0.08f, 4.06f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f, 0.22f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Parte horizontal de la barra delantera.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-5.45f, -0.72f, 4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.24f, 1.10f, 0.24f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Parte vertical de la barra delantera.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-6.0f, -1.535f, 4.12f));
		model = glm::scale(model, glm::vec3(0.24f, 1.63f, 0.24f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.35f, 0.35f, 0.35f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la barra delantera con las barras largas.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-6.0f, -0.72f, 4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.30f, 0.14f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//LLANTA DELANTERA IZQUIERDA.
		glm::mat4 matrizLlantaDelanteraIzquierda = modelaux2;
		matrizLlantaDelanteraIzquierda = glm::translate(matrizLlantaDelanteraIzquierda, glm::vec3(-6.0f, -2.35f, -4.50f));
		matrizLlantaDelanteraIzquierda = glm::rotate(matrizLlantaDelanteraIzquierda, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(matrizLlantaDelanteraIzquierda, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.75f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.32f, 0.36f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Centro gris.
		model = glm::rotate(matrizLlantaDelanteraIzquierda, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.82f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.65f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la estructura con el eje de la llanta.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-6.0f, -2.35f, -4.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.50f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//LLANTA DELANTERA DERECHA.
		glm::mat4 matrizLlantaDelanteraDerecha = modelaux2;
		matrizLlantaDelanteraDerecha = glm::translate(matrizLlantaDelanteraDerecha, glm::vec3(-6.0f, -2.35f, 4.50f));
		matrizLlantaDelanteraDerecha = glm::rotate(matrizLlantaDelanteraDerecha, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(matrizLlantaDelanteraDerecha, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.75f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.32f, 0.36f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Centro gris.
		model = glm::rotate(matrizLlantaDelanteraDerecha, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.82f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.65f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la estructura con el eje de la llanta.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-6.0f, -2.35f, 4.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.50f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//ESTRUCTURA EN L CENTRAL IZQUIERDA.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.60f, -0.95f, -4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.50f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.50f, -1.65f, -4.12f));
		model = glm::rotate(model, glm::radians(-36.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.72f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la parte horizontal de la barra con la barra inclinada.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.00f, -0.95f, -4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.32f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//ESTRUCTURA EN L CENTRAL DERECHA.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.60f, -0.95f, 4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.50f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.50f, -1.65f, 4.12f));
		model = glm::rotate(model, glm::radians(-36.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.72f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la parte horizontal de la barra con la barra inclinada.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.00f, -0.95f, 4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.32f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//LLANTA CENTRAL IZQUIERDA.
		glm::mat4 matrizLlantaCentralIzquierda = modelaux2;
		matrizLlantaCentralIzquierda = glm::translate(matrizLlantaCentralIzquierda, glm::vec3(0.0f, -2.35f, -4.50f));
		matrizLlantaCentralIzquierda = glm::rotate(matrizLlantaCentralIzquierda, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(matrizLlantaCentralIzquierda, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.75f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.32f, 0.36f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Centro gris.
		model = glm::rotate(matrizLlantaCentralIzquierda, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.82f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.65f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la estructura con el eje de la llanta.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -2.35f, -4.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.50f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//LLANTA CENTRAL DERECHA.
		glm::mat4 matrizLlantaCentralDerecha = modelaux2;
		matrizLlantaCentralDerecha = glm::translate(matrizLlantaCentralDerecha, glm::vec3(0.0f, -2.35f, 4.50f));
		matrizLlantaCentralDerecha = glm::rotate(matrizLlantaCentralDerecha, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(matrizLlantaCentralDerecha, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.75f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.32f, 0.36f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Centro gris.
		model = glm::rotate(matrizLlantaCentralDerecha, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.82f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.65f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la estructura con el eje de la llanta.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -2.35f, 4.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.50f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//ESTRUCTURA EN L TRASERA IZQUIERDA.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.40f, -0.95f, -4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.50f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = modelaux2;
		model = glm::translate(model, glm::vec3(3.50f, -1.65f, -4.12f));
		model = glm::rotate(model, glm::radians(36.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.72f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la parte horizontal de la L con la barra inclinada.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(3.00f, -0.95f, -4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.32f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//ESTRUCTURA EN L TRASERA DERECHA.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.40f, -0.95f, 4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.50f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = modelaux2;
		model = glm::translate(model, glm::vec3(3.50f, -1.65f, 4.12f));
		model = glm::rotate(model, glm::radians(36.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.28f, 1.72f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.75f, 0.75f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la parte horizontal de la L con la barra inclinada.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(3.00f, -0.95f, 4.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.32f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//LLANTA TRASERA IZQUIERDA.
		glm::mat4 matrizLlantaTraseraIzquierda = modelaux2;
		matrizLlantaTraseraIzquierda = glm::translate(matrizLlantaTraseraIzquierda, glm::vec3(4.0f, -2.35f, -4.50f));
		matrizLlantaTraseraIzquierda = glm::rotate(matrizLlantaTraseraIzquierda, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(matrizLlantaTraseraIzquierda, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.75f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.32f, 0.36f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Centro gris.
		model = glm::rotate(matrizLlantaTraseraIzquierda, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.82f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.65f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la estructura con el eje de la llanta.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(4.0f, -2.35f, -4.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.50f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//LLANTA TRASERA DERECHA.
		glm::mat4 matrizLlantaTraseraDerecha = modelaux2;
		matrizLlantaTraseraDerecha = glm::translate(matrizLlantaTraseraDerecha, glm::vec3(4.0f, -2.35f, 4.50f));
		matrizLlantaTraseraDerecha = glm::rotate(matrizLlantaTraseraDerecha, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(matrizLlantaTraseraDerecha, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.75f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.32f, 0.36f, 0.42f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Centro gris.
		model = glm::rotate(matrizLlantaTraseraDerecha, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.82f, 0.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.65f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//Perno que une la estructura con el eje de la llanta.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(4.0f, -2.35f, 4.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.50f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.10f, 0.35f, 0.95f);;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// SONDA ROSETTA  
		raizSonda = glm::mat4(1.0f);
		raizSonda = glm::translate(raizSonda, glm::vec3(5.0f, -0.25f, -18.0f));
		raizSonda = glm::rotate(raizSonda, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		raizSonda = glm::rotate(raizSonda, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		raizSonda = glm::rotate(raizSonda, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		raizSonda = glm::scale(raizSonda, glm::vec3(0.55f, 0.55f, 0.55f));

		// CUERPO CENTRAL.
		model = raizSonda;
		model = glm::scale(model, glm::vec3(3.2f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.72f, 0.48f, 0.08f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// PARTE SUPERIOR DE INSTRUMENTOS.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(0.0f, 1.75f, -0.35f));
		model = glm::scale(model, glm::vec3(2.35f, 0.5f, 2.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.82f, 0.82f, 0.78f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// CAJA DEL INSTRUMENTO OSIRIS.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(-0.75f, 1.95f, 0.9f));
		model = glm::scale(model, glm::vec3(0.75f, 0.55f, 0.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.25f, 0.25f, 0.28f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// LENTE DE OSIRIS.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(-0.75f, 1.95f, 1.28f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.23f, 0.22f, 0.23f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.05f, 0.25f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// MODULO PHILAE REPRESENTADO EN UN COSTADO.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(1.05f, -0.15f, 1.65f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.70f, 0.70f, 0.72f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// EJE DEL PANEL SOLAR IZQUIERDO.
		ejePanelIzquierdo = raizSonda;
		ejePanelIzquierdo = glm::translate(ejePanelIzquierdo, glm::vec3(-1.6f, 0.0f, 0.0f));
		ejePanelIzquierdo = glm::rotate(ejePanelIzquierdo,
			glm::radians(0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

		// UNION DEL CUERPO CON EL PANEL IZQUIERDO.
		model = ejePanelIzquierdo;
		model = glm::translate(model, glm::vec3(-0.45f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.9f, 0.14f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.67f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// CINCO SECCIONES DEL PANEL IZQUIERDO.
		model = ejePanelIzquierdo;
		model = glm::translate(model, glm::vec3(-1.55f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.04f, 0.16f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelIzquierdo;
		model = glm::translate(model, glm::vec3(-2.80f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.05f, 0.22f, 0.70f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelIzquierdo;
		model = glm::translate(model, glm::vec3(-4.05f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.04f, 0.16f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelIzquierdo;
		model = glm::translate(model, glm::vec3(-5.30f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.05f, 0.22f, 0.70f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelIzquierdo;
		model = glm::translate(model, glm::vec3(-6.55f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.04f, 0.16f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// EJE DEL PANEL SOLAR DERECHO.
		ejePanelDerecho = raizSonda;
		ejePanelDerecho = glm::translate(ejePanelDerecho, glm::vec3(1.6f, 0.0f, 0.0f));
		ejePanelDerecho = glm::rotate(ejePanelDerecho,
			glm::radians(0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

		// UNION DEL CUERPO CON EL PANEL DERECHO.
		model = ejePanelDerecho;
		model = glm::translate(model, glm::vec3(0.45f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.9f, 0.14f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.65f, 0.65f, 0.67f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// CINCO SECCIONES DEL PANEL DERECHO.
		model = ejePanelDerecho;
		model = glm::translate(model, glm::vec3(1.55f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.04f, 0.16f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelDerecho;
		model = glm::translate(model, glm::vec3(2.80f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.05f, 0.22f, 0.70f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelDerecho;
		model = glm::translate(model, glm::vec3(4.05f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.04f, 0.16f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelDerecho;
		model = glm::translate(model, glm::vec3(5.30f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.05f, 0.22f, 0.70f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = ejePanelDerecho;
		model = glm::translate(model, glm::vec3(6.55f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f, 2.45f, 0.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.04f, 0.16f, 0.55f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// ANTENA PARABOLICA JERARQUICA.
		ejeAntena = raizSonda;
		ejeAntena = glm::translate(ejeAntena, glm::vec3(0.0f, 0.25f, 1.5f));
		ejeAntena = glm::rotate(ejeAntena,
			glm::radians(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		// SOPORTE DE LA ANTENA.
		model = ejeAntena;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.55f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f, 1.10f, 0.18f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.62f, 0.62f, 0.64f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// PLATO PARABOLICO
		model = ejeAntena;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.68f, 0.34f, 0.68f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.88f, 0.88f, 0.84f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		// ALIMENTADOR CENTRAL DE LA ANTENA.
		model = ejeAntena;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.72f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.10f, 0.70f, 0.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.25f, 0.25f, 0.27f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// CABEZA DEL ALIMENTADOR.
		model = ejeAntena;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.10f));
		model = glm::scale(model, glm::vec3(0.17f, 0.17f, 0.17f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.18f, 0.18f, 0.20f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// PROPULSOR INFERIOR IZQUIERDO.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(-1.05f, -1.72f, -0.85f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f, 0.55f, 0.18f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.48f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		// PROPULSOR INFERIOR DERECHO.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(1.05f, -1.72f, -0.85f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f, 0.55f, 0.18f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.48f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry();

		// DOS ANTENAS PEQUENAS.
		model = raizSonda;
		model = glm::translate(model, glm::vec3(-1.05f, 2.35f, -0.30f));
		model = glm::rotate(model, glm::radians(-18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.06f, 1.05f, 0.06f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.80f, 0.80f, 0.82f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		model = raizSonda;
		model = glm::translate(model, glm::vec3(1.05f, 2.35f, -0.30f));
		model = glm::rotate(model, glm::radians(18.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.06f, 1.05f, 0.06f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.80f, 0.80f, 0.82f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();


		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
