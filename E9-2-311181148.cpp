// Practica 09-2: holocron y cohete 
 
#define STB_IMAGE_IMPLEMENTATION
#include <cstdio>
#include <fstream>
#include <vector>
#include <cmath>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Camera.h"
#include "Shader_light.h"
#include "Mesh_tn.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include "Material.h"
#include "DirectionalLight.h"

const float toRadians = 3.14159265f / 180.0f;
const float pasoFijo = 1.0f / 120.0f;
const float alturaPiso = -1.0f;
const float separacionEtapa1 = 5.0f, separacionEtapa2 = 8.0f;
Window mainWindow(1366, 768);
Camera camera;
Shader shader;
Skybox skybox;
MeshModel plano;
Texture pisoTexture;
Material metal, opaco;
DirectionalLight luzPrincipal;
Model HolocronCuerpo, Esquina1, Esquina2, Esquina3, Esquina4;
Model Esquina5, Esquina6, Esquina7, Esquina8;
Model CoheteEtapa1, CoheteEtapa2, CoheteEtapa3;
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
GLuint uniformEye = 0, uniformColor = 0, uniformOffset = 0;
GLuint uniformSpecular = 0, uniformShininess = 0;
float tiempoPractica = 0, tiempoVueloCohete = 0;
float aperturaHolocron = 0, separacionHolocron = 0, giroHolocron = 0;
GLfloat now = 0, lastTime = 0, deltaTime = 0, acumulador = 0;
glm::mat4 projection(1), view(1), matrizPlano(1), matrizHolocron(1);
glm::mat4 matrizEsquina(1), matrizEtapa1(1), matrizEtapa2(1), matrizEtapa3(1);
glm::vec3 ojo(0);

const glm::vec3 centroHolocron(-47.987f, 5.376f, 0.651f);
const glm::vec3 centroEsquina1(-45.409f, 7.956f, 3.332f);
const glm::vec3 centroEsquina2(-45.372f, 8.085f, -1.873f);
const glm::vec3 centroEsquina3(-45.375f, 2.818f, -2.018f);
const glm::vec3 centroEsquina4(-45.389f, 2.681f, 3.174f);
const glm::vec3 centroEsquina5(-50.611f, 2.684f, 3.207f);
const glm::vec3 centroEsquina6(-50.596f, 2.794f, -2.024f);
const glm::vec3 centroEsquina7(-50.598f, 7.938f, 3.338f);
const glm::vec3 centroEsquina8(-50.601f, 8.075f, -1.892f);

// Cada OBJ de etapa conserva sus coordenadas dentro del cohete completo.
const glm::vec3 pivoteEtapa1(0, 3.385f, 0);
const glm::vec3 mitadEtapa1(1.489f, 3.385f, 1.489f);
const glm::vec3 pivoteEtapa2(0, 8.113f, 0);
const glm::vec3 mitadEtapa2(0.815f, 1.344f, 0.815f);

bool ComprobarRecursos()
{
    const char* archivos[] = {
        "Models/cuerpoH.obj", "Models/cuerpoH.mtl",
        "Models/Esquina1.obj", "Models/Esquina1.mtl",
        "Models/Esquina2.obj", "Models/Esquina2.mtl",
        "Models/Esquina3.obj", "Models/Esquina3.mtl",
        "Models/Esquina4.obj", "Models/Esquina4.mtl",
        "Models/Esquina5.obj", "Models/Esquina5.mtl",
        "Models/Esquina6.obj", "Models/Esquina6.mtl",
        "Models/Esquina7.obj", "Models/Esquina7.mtl",
        "Models/Esquina8.obj", "Models/Esquina8.mtl",
        "Models/Cohete_Etapa1.obj", "Models/Cohete_Etapa2.obj",
        "Models/Cohete_Etapa3.obj", "Models/Cohete_Saturno.mtl",
        "Textures/piso.tga", "Textures/plain.png",
        "Textures/holocron_azul.png", "Textures/holocron_oro.png",
        "Textures/holocron_cristal.png", "Textures/saturno_1_etage.png",
        "Textures/saturno_2_etage.png", "Textures/saturno_3_etage.png",
        "Textures/saturno_3_etage1.png", "Textures/saturno_blanco.png",
        "Textures/saturno_motor.png", "shaders/shader_light.vert",
        "shaders/shader_light.frag", "shaders/skybox.vert", "shaders/skybox.frag"
    };
    bool completo = true;
    for (const char* archivo : archivos)
    {
        std::ifstream prueba(archivo, std::ios::binary);
        if (!prueba) { std::printf("No se encontro: %s\n", archivo); completo = false; }
    }
    return completo;
}

void CrearPlano()
{
    // Mismos vertices, coordenadas UV, normales e indices del plano de E07.
    GLfloat vertices[] = {
        -10, 0, -10,  0, 0,   0, -1, 0,
         10, 0, -10, 10, 0,   0, -1, 0,
        -10, 0,  10,  0, 10,  0, -1, 0,
         10, 0,  10, 10, 10,  0, -1, 0
    };
    unsigned int indices[] = { 0, 2, 1, 1, 2, 3 };
    plano.CreateMeshModel(vertices, indices, 32, 6);
    matrizPlano = glm::translate(glm::mat4(1), glm::vec3(0, alturaPiso, 0));
    matrizPlano = glm::scale(matrizPlano, glm::vec3(30, 1, 30));
}

void PrepararHolocron()
{
    aperturaHolocron = 0.5f * (1.0f - std::cos(2 * 3.14159265f * tiempoPractica / 8));
    separacionHolocron = 4 * aperturaHolocron;
    giroHolocron = 65 * aperturaHolocron;
    matrizHolocron = glm::translate(glm::mat4(1), glm::vec3(-13, 10, 0));
    matrizHolocron = glm::scale(matrizHolocron, glm::vec3(0.8f));
    matrizHolocron = glm::translate(matrizHolocron, -centroHolocron);
}

glm::vec3 PosicionCohete(float t)
{
    float curva = glm::max(t - 4, 0.0f);
    return glm::vec3(12 + 0.12f * curva * curva,
        alturaPiso + 0.45f * t * t, -4);
}

glm::vec3 VelocidadCohete(float t)
{
    float curva = glm::max(t - 4, 0.0f);
    return glm::vec3(0.24f * curva, 0.9f * t, 0);
}

float InclinacionCohete(float t)
{
    glm::vec3 velocidad = VelocidadCohete(t);
    return -std::atan2(velocidad.x, glm::max(velocidad.y, 0.01f));
}

glm::mat4 MatrizVuelo(float t)
{
    glm::mat4 matriz = glm::translate(glm::mat4(1), PosicionCohete(t));
    return glm::rotate(matriz, InclinacionCohete(t), glm::vec3(0, 0, 1));
}

// Al separarse, la etapa conserva su posicion de salida en la escena.
// Despues se aparta y cae. Ya no recibe las transformaciones del cohete.
glm::mat4 MatrizEtapaSeparada(float instante, const glm::vec3& pivote,
    const glm::vec3& mitad, float sentido)
{
    float dt = tiempoVueloCohete - instante;
    if (dt < 0) return MatrizVuelo(tiempoVueloCohete);
    glm::mat4 salida = MatrizVuelo(instante);
    glm::vec3 centro = glm::vec3(salida * glm::vec4(pivote, 1));
    glm::vec3 velocidad = VelocidadCohete(instante);
    float deslizamiento = glm::min(dt, 6.0f);
    centro.x += (velocidad.x + sentido * 2) * deslizamiento;
    centro.z += sentido * 0.4f * deslizamiento;
    centro.y += velocidad.y * dt - 3 * dt * dt;
    float avanceGiro = glm::smoothstep(0.0f, 2.0f, dt);
    float giro = glm::mix(InclinacionCohete(instante), sentido * 90 * toRadians, avanceGiro);
    // Extremo inferior de la caja girada: evita atravesar el plano.
    float apoyo = std::abs(std::sin(giro)) * mitad.x + std::abs(std::cos(giro)) * mitad.y;
    centro.y = glm::max(centro.y, alturaPiso + apoyo + 0.01f);
    glm::mat4 matriz = glm::translate(glm::mat4(1), centro);
    matriz = glm::rotate(matriz, giro, glm::vec3(0, 0, 1));
    return glm::translate(matriz, -pivote);
}

void PrepararCohete()
{
    // Dos segundos de espera y vuelo continuo. Cada etapa cae con su propio tiempo.
    tiempoVueloCohete = glm::max(tiempoPractica - 2, 0.0f);
    matrizEtapa1 = MatrizEtapaSeparada(separacionEtapa1, pivoteEtapa1, mitadEtapa1, -1);
    matrizEtapa2 = MatrizEtapaSeparada(separacionEtapa2, pivoteEtapa2, mitadEtapa2, 1);
    matrizEtapa3 = MatrizVuelo(tiempoVueloCohete);
}

void ReiniciarPractica()
{
    tiempoPractica = 0;
    PrepararHolocron();
    PrepararCohete();
}

void ActualizarPractica(float paso)
{
    tiempoPractica += paso;
    PrepararHolocron();
    PrepararCohete();
}

bool CargarEscena()
{
    shader.CreateFromFiles("shaders/shader_light.vert", "shaders/shader_light.frag");
    if (!shader.EstaListo()) return false;
    pisoTexture = Texture("Textures/piso.tga");
    if (!pisoTexture.LoadTextureA()) return false;
    CrearPlano();
    HolocronCuerpo.LoadModel("Models/cuerpoH.obj");
    Esquina1.LoadModel("Models/Esquina1.obj");
    Esquina2.LoadModel("Models/Esquina2.obj");
    Esquina3.LoadModel("Models/Esquina3.obj");
    Esquina4.LoadModel("Models/Esquina4.obj");
    Esquina5.LoadModel("Models/Esquina5.obj");
    Esquina6.LoadModel("Models/Esquina6.obj");
    Esquina7.LoadModel("Models/Esquina7.obj");
    Esquina8.LoadModel("Models/Esquina8.obj");
    CoheteEtapa1.LoadModel("Models/Cohete_Etapa1.obj");
    CoheteEtapa2.LoadModel("Models/Cohete_Etapa2.obj");
    CoheteEtapa3.LoadModel("Models/Cohete_Etapa3.obj");
    if (!HolocronCuerpo.EstaCargado() || !Esquina1.EstaCargado() ||
        !Esquina2.EstaCargado() || !Esquina3.EstaCargado() ||
        !Esquina4.EstaCargado() || !Esquina5.EstaCargado() ||
        !Esquina6.EstaCargado() || !Esquina7.EstaCargado() ||
        !Esquina8.EstaCargado() || !CoheteEtapa1.EstaCargado() ||
        !CoheteEtapa2.EstaCargado() || !CoheteEtapa3.EstaCargado()) return false;
    opaco = Material(0.15f, 12);
    metal = Material(0.5f, 64);
    luzPrincipal = DirectionalLight(1, 1, 1, 0.35f, 0.6f, -0.4f, -1, -0.3f);
    std::vector<std::string> caras;
    caras.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_lf.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_up.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_dn.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_ft.tga");
    skybox = Skybox(caras);
    return true;
}

void DibujarEsquina(Model& pieza, const glm::vec3& centro)
{
    glm::vec3 radial = glm::normalize(centro - centroHolocron);
    matrizEsquina = glm::translate(matrizHolocron, radial * separacionHolocron);
    matrizEsquina = glm::translate(matrizEsquina, centro);
    matrizEsquina = glm::rotate(matrizEsquina, giroHolocron * toRadians, radial);
    matrizEsquina = glm::translate(matrizEsquina, -centro);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizEsquina));
    pieza.RenderModel();
}

void DibujarEscena()
{
    glUniform3f(uniformColor, 1, 1, 1);
    opaco.UseMaterial(uniformSpecular, uniformShininess);
    pisoTexture.UseTexture();
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizPlano));
    plano.RenderMeshModel();
    metal.UseMaterial(uniformSpecular, uniformShininess);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizHolocron));
    HolocronCuerpo.RenderModel();
    DibujarEsquina(Esquina1, centroEsquina1);
    DibujarEsquina(Esquina2, centroEsquina2);
    DibujarEsquina(Esquina3, centroEsquina3);
    DibujarEsquina(Esquina4, centroEsquina4);
    DibujarEsquina(Esquina5, centroEsquina5);
    DibujarEsquina(Esquina6, centroEsquina6);
    DibujarEsquina(Esquina7, centroEsquina7);
    DibujarEsquina(Esquina8, centroEsquina8);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizEtapa1));
    CoheteEtapa1.RenderModel();
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizEtapa2));
    CoheteEtapa2.RenderModel();
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizEtapa3));
    CoheteEtapa3.RenderModel();
}

int main()
{
    if (!ComprobarRecursos()) { std::getchar(); return 1; }
    if (mainWindow.Initialise() != 0) return 1;
    if (!CargarEscena())
    {
        std::puts("No se pudo cargar la escena. Revisa los mensajes anteriores.");
        std::getchar(); return 1;
    }
    ReiniciarPractica();
    camera = Camera(glm::vec3(65, 65, 95), glm::vec3(0, 1, 0), -125, -12, 20, 0.2f);
    projection = glm::perspective(45 * toRadians,
        mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
    uniformModel = shader.GetModelLocation();
    uniformProjection = shader.GetProjectionLocation();
    uniformView = shader.GetViewLocation();
    uniformEye = shader.GetEyePositionLocation();
    uniformColor = shader.getColorLocation();
    uniformOffset = shader.getOffsetLocation();
    uniformSpecular = shader.GetSpecularIntensityLocation();
    uniformShininess = shader.GetShininessLocation();
    lastTime = static_cast<GLfloat>(glfwGetTime());
    std::puts("WASD y mouse: camara | P: pausa | R: reinicia | Escape: salir");
    while (!mainWindow.getShouldClose())
    {
        now = static_cast<GLfloat>(glfwGetTime());
        deltaTime = glm::min(now - lastTime, 0.1f);
        lastTime = now;
        glfwPollEvents();
        mainWindow.ActualizarControles(deltaTime, deltaTime, camera);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        if (mainWindow.ConsumirReinicio())
        {
            ReiniciarPractica();
            acumulador = 0;
        }
        if (!mainWindow.getPausa()) acumulador += deltaTime;
        while (acumulador >= pasoFijo)
        {
            ActualizarPractica(pasoFijo);
            acumulador -= pasoFijo;
        }
        ojo = camera.getCameraPosition();
        view = camera.calculateViewMatrix();
        glClearColor(0.04f, 0.06f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.DrawSkybox(view, projection);
        shader.UseShader();
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(uniformEye, 1, glm::value_ptr(ojo));
        glUniform2f(uniformOffset, 0, 0);
        shader.SetDirectionalLight(&luzPrincipal);
        shader.SetPointLights(nullptr, 0);
        shader.SetSpotLights(nullptr, 0);
        DibujarEscena();
        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}
