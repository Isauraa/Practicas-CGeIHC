// Practica 10: lanzamiento y separacion por keyframes.
#define STB_IMAGE_IMPLEMENTATION
#include <cstdio>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "CoheteKeyframes.h"
#include "Skybox.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Texture pisoTexture;
CoheteKeyframes cohete;
Skybox skybox;
Material Material_brillante, Material_opaco;
DirectionalLight mainLight;
GLfloat deltaTime = 0, lastTime = 0;
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";
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
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



//////////////////// KEYFRAMES DEL COHETE ////////////////////
int main()
{
    mainWindow = Window(1366, 768);
    if (mainWindow.Initialise() != 0) return 1;
    CreateObjects(); CreateShaders();
    if (!shaderList[0].EstaListo()) { std::puts("Error de shader. Revisa la carpeta shaders."); return 1; }
    camera = Camera(glm::vec3(2, 3.2f, 9), glm::vec3(0,1,0), -102.53f, -8.63f, 8, 0.15f);
    pisoTexture = Texture("Textures/piso.tga");
    if (!pisoTexture.LoadTextureA()) return 1;
    if (!cohete.Cargar()) { std::puts("No se pudo cargar el cohete o su TXT."); return 1; }
    std::vector<std::string> caras = {
        "Textures/Skybox/cupertin-lake_rt.tga", "Textures/Skybox/cupertin-lake_lf.tga",
        "Textures/Skybox/cupertin-lake_dn.tga", "Textures/Skybox/cupertin-lake_up.tga",
        "Textures/Skybox/cupertin-lake_bk.tga", "Textures/Skybox/cupertin-lake_ft.tga"};
    skybox = Skybox(caras);
    Material_brillante = Material(0.4f,32);
    Material_opaco = Material(0.1f,4);
    mainLight = DirectionalLight(1,1,1,0.5f,0.7f,-0.3f,-1,-0.4f);
    GLuint uniformProjection, uniformModel, uniformView, uniformEyePosition;
    GLuint uniformColor, uniformSpecularIntensity, uniformShininess, uniformTextureOffset;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        mainWindow.getBufferWidth()/mainWindow.getBufferHeight(),0.1f,1000.0f);
    glm::mat4 model(1), vista(1);
    glm::vec2 offset(0);
    glm::vec3 ojo(0);
    float now = 0;
    int accion = 0;
    glm::vec3 objetivo(0);
    const glm::vec3 ojoGeneral(12, 19, 62);
    const glm::vec3 objetivoGeneral(0, 20, 0);
    const glm::vec3 desplazamientoCerca(2, 1.4f, 9);
    const glm::vec3 arriba(0, 1, 0);
    std::puts("Practica 10 | Cohete por etapas");
    std::puts("F6: reproducir/pausar | F7: reiniciar | F8: siguiente cuadro | F5: guardar | F9: leer");
    std::puts("1: camara libre (WASD y mouse) | 2: seguimiento | 6: vista general fija");
    lastTime = static_cast<float>(glfwGetTime());
    while (!mainWindow.getShouldClose())
    {
        now = static_cast<float>(glfwGetTime());
        deltaTime = now-lastTime; lastTime = now;
        glfwPollEvents();
        while ((accion = mainWindow.ConsumirAccionCohete()) != 0) {
            switch (accion) {
                case 1: cohete.Alternar(); break;
                case 2: cohete.Reiniciar(); cohete.Imprimir(0); break;
                case 3: cohete.Escribir(); break;
                case 4: cohete.Leer(); break;
                case 5: cohete.Paso(); break;
            }
        }
        cohete.Actualizar(deltaTime);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const GLfloat mouseX = mainWindow.getXChange();
        const GLfloat mouseY = mainWindow.getYChange();
        if (mainWindow.getCamaraActiva() == 6) {
            ojo = ojoGeneral;
            objetivo = objetivoGeneral;
        } else if (mainWindow.getCamaraActiva() == 2) {
            objetivo = cohete.pose[3].posicion * cohete.escalaEscena;
            // Al inicio encuadra el cuerpo completo; despues sigue la capsula.
            objetivo.y = std::max(1.8f, objetivo.y - 1.5f);
            ojo = objetivo + desplazamientoCerca;
        } else {
            camera.mouseControl(mouseX, mouseY);
            camera.keyControl(mainWindow.getsKeys(), deltaTime);
            ojo = camera.getCameraPosition();
        }
        if (mainWindow.getCamaraActiva() == 2 || mainWindow.getCamaraActiva() == 6)
            vista = glm::lookAt(ojo, objetivo, arriba);
        else
            vista = camera.calculateViewMatrix();
        skybox.DrawSkybox(vista,projection);
        shaderList[0].UseShader();
        uniformProjection=shaderList[0].GetProjectionLocation();
        uniformModel=shaderList[0].GetModelLocation();
        uniformView=shaderList[0].GetViewLocation();
        uniformEyePosition=shaderList[0].GetEyePositionLocation();
        uniformColor=shaderList[0].getColorLocation();
        uniformSpecularIntensity=shaderList[0].GetSpecularIntensityLocation();
        uniformShininess=shaderList[0].GetShininessLocation();
        uniformTextureOffset=shaderList[0].getOffsetLocation();
        glUniformMatrix4fv(uniformProjection,1,GL_FALSE,glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView,1,GL_FALSE,glm::value_ptr(vista));
        glUniform3fv(uniformEyePosition,1,glm::value_ptr(ojo));
        glUniform3f(uniformColor,1,1,1);
        glUniform2fv(uniformTextureOffset,1,glm::value_ptr(offset));
        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(nullptr,0);
        shaderList[0].SetSpotLights(nullptr,0);
        model=glm::scale(glm::mat4(1),glm::vec3(4,1,4));
        glUniformMatrix4fv(uniformModel,1,GL_FALSE,glm::value_ptr(model));
        Material_opaco.UseMaterial(uniformSpecularIntensity,uniformShininess);
        pisoTexture.UseTexture(); meshList[2]->RenderMeshModel();
        Material_brillante.UseMaterial(uniformSpecularIntensity,uniformShininess);
        cohete.Dibujar(uniformModel);
        glUseProgram(0); mainWindow.swapBuffers();
    }
    return 0;
}
