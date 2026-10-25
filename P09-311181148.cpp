/* Practica 09: animacion basica por etapas, contadores y banderas.
   Nave sobre el circuito y Landsat con paneles y antena automaticos.
   Los controles concretos del teclado permanecen en Window.cpp. */
#define STB_IMAGE_IMPLEMENTATION
#include <cstdio>
#include <vector>
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
#include "Model.h"
#include "Skybox.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
   // Animacion basica: incrementos constantes y cambio de etapa por contador.
   // No se calculan curvas, senos, cosenos, splines ni interpolaciones de posiciones.
   // Cada fila avanza sobre un tramo ya medido de Models/track.obj (escala 0.003).
   // pasos, incremento X, Y, Z, giro Y e inclinacion X por actualizacion.
   // El reloj llama a estas funciones 120 veces por segundo.
   // Version de prueba: desplazamientos con hasta 4 decimales y giros con 3.
struct TramoNave
{
    int pasos;
    float dx, dy, dz, giroY, giroX;
};

static const TramoNave tramosNave[] = {
    {115, 0.05f, 0.0f, 0.0003f, -0.054f, -0.021f}, // 0 Curva
    {49, 0.0484f, 0.0f, 0.0107f, -0.303f, 0.0f}, // 1 Curva
    {65, 0.0431f, 0.0f, 0.0249f, -0.288f, 0.0f}, // 2 Curva
    {65, 0.032f, 0.0f, 0.0381f, -0.327f, 0.0f}, // 3 Curva
    {81, 0.015f, 0.0f, 0.0475f, -0.278f, 0.0f}, // 4 Curva
    {65, -0.0043f, 0.0f, 0.0496f, -0.327f, 0.0f}, // 5 Curva
    {65, -0.021f, 0.0f, 0.0451f, -0.308f, 0.0f}, // 6 Curva
    {65, -0.0352f, 0.0f, 0.0352f, -0.288f, 0.0f}, // 7 Curva
    {49, -0.044f, 0.0f, 0.0229f, -0.357f, 0.0f}, // 8 Curva
    {65, -0.049f, 0.0f, 0.0086f, -0.288f, 0.0f}, // 9 Curva
    {65, -0.049f, 0.0f, -0.0086f, -0.305f, 0.0f}, // 10 Curva
    {75, -0.0437f, 0.0f, -0.0249f, -0.041f, 0.0f}, // 11 Curva
    {36, -0.0479f, 0.0f, -0.0138f, 0.412f, 0.0f}, // 12 Curva
    {707, -0.05f, 0.0f, 0.0f, 0.017f, -0.013f}, // 13 Curva
    {94, -0.047f, 0.0163f, 0.0062f, 0.133f, -0.101f}, // 14 Rampa
    {71, -0.0427f, 0.0162f, 0.02f, 0.229f, 0.0f}, // 15 Rampa
    {71, -0.0361f, 0.0162f, 0.0303f, 0.211f, 0.0f}, // 16 Rampa
    {71, -0.027f, 0.0162f, 0.0387f, 0.211f, 0.0f}, // 17 Rampa
    {71, -0.0161f, 0.0162f, 0.0443f, 0.211f, 0.0f}, // 18 Rampa
    {71, -0.0041f, 0.0162f, 0.047f, 0.194f, 0.0f}, // 19 Rampa
    {47, 0.0063f, 0.0163f, 0.0472f, 0.266f, 0.0f}, // 20 Rampa
    {71, 0.0162f, 0.0162f, 0.0443f, 0.194f, 0.0f}, // 21 Rampa
    {71, 0.0271f, 0.0162f, 0.0386f, 0.211f, 0.0f}, // 22 Rampa
    {71, 0.0362f, 0.0162f, 0.0303f, 0.211f, 0.0f}, // 23 Rampa
    {71, 0.0428f, 0.0162f, 0.0199f, 0.211f, 0.0f}, // 24 Rampa
    {71, 0.0465f, 0.0162f, 0.0081f, 0.194f, 0.0f}, // 25 Rampa
    {47, 0.0475f, 0.0163f, -0.0021f, 0.266f, 0.0f}, // 26 Rampa
    {71, 0.0455f, 0.0162f, -0.0122f, 0.194f, 0.0f}, // 27 Rampa
    {71, 0.0408f, 0.0162f, -0.0236f, 0.211f, 0.0f}, // 28 Rampa
    {71, 0.0333f, 0.0162f, -0.0334f, 0.194f, 0.0f}, // 29 Rampa
    {47, 0.0255f, 0.0163f, -0.0401f, 0.239f, 0.0f}, // 30 Rampa
    {47, 0.0182f, 0.0163f, -0.044f, 0.239f, 0.0f}, // 31 Rampa
    {71, 0.0081f, 0.0162f, -0.0465f, 0.158f, 0.148f}, // 32 Rampa
    {105, 0.0f, -0.0018f, -0.0501f, 0.047f, 0.266f}, // 33 Curva
    {36, 0.0f, -0.0302f, -0.0404f, 0.0f, 0.699f}, // 34 Rampa
    {316, 0.0f, -0.0395f, -0.0306f, 0.0f, 0.011f}, // 35 Rampa
    {42, 0.0f, -0.0347f, -0.0364f, 0.0f, -0.405f}, // 36 Rampa
    {56, 0.0f, -0.0156f, -0.0472f, -0.051f, -0.389f}, // 37 Rampa
    {108, 0.005f, 0.0f, -0.0498f, -0.084f, -0.084f}, // 38 Curva
    {72, 0.0157f, 0.0f, -0.0476f, -0.156f, 0.0f}, // 39 Curva
    {72, 0.0237f, 0.0f, -0.0442f, -0.139f, 0.0f}, // 40 Curva
    {72, 0.031f, 0.0f, -0.0394f, -0.156f, 0.0f}, // 41 Curva
    {108, 0.0387f, 0.0f, -0.0317f, -0.116f, 0.0f}, // 42 Curva
    {72, 0.0447f, 0.0f, -0.0226f, -0.156f, 0.0f}, // 43 Curva
    {72, 0.048f, 0.0f, -0.0145f, -0.139f, 0.0f}, // 44 Curva
    {72, 0.0498f, 0.0f, -0.0059f, -0.139f, 0.0f}, // 45 Curva
    {72, 0.05f, 0.0f, 0.0028f, -0.139f, 0.0f}, // 46 Curva
    {72, 0.0488f, 0.0f, 0.0115f, -0.156f, 0.0f}, // 47 Curva
    {108, 0.0451f, 0.0f, 0.0217f, -0.116f, 0.0f}, // 48 Curva
    {72, 0.0394f, 0.0f, 0.031f, -0.174f, 0.0f}, // 49 Curva
    {108, 0.0317f, 0.0f, 0.0387f, -0.116f, 0.0f}, // 50 Curva
    {72, 0.0226f, 0.0f, 0.0447f, -0.156f, 0.0f}, // 51 Curva
    {72, 0.0145f, 0.0f, 0.048f, -0.139f, 0.0f}, // 52 Curva
    {72, 0.0059f, 0.0f, 0.0498f, -0.139f, 0.0f}, // 53 Curva
    {72, -0.0028f, 0.0f, 0.05f, -0.156f, 0.0f}, // 54 Curva
    {108, -0.0136f, 0.0f, 0.0482f, -0.116f, 0.0f}, // 55 Curva
    {72, -0.0237f, 0.0f, 0.0442f, -0.156f, 0.0f}, // 56 Curva
    {72, -0.031f, 0.0f, 0.0394f, -0.139f, 0.0f}, // 57 Curva
    {72, -0.0374f, 0.0f, 0.0334f, -0.139f, 0.0f}, // 58 Curva
    {72, -0.0426f, 0.0f, 0.0264f, -0.139f, 0.0f}, // 59 Curva
    {72, -0.0465f, 0.0f, 0.0186f, -0.156f, 0.0f}, // 60 Curva
    {108, -0.0494f, 0.0f, 0.0081f, -0.099f, 0.001f}, // 61 Curva
    {500, -0.05f, -0.0001f, 0.0003f, 0.004f, 0.001f}, // 62 Recta
    {64, -0.0486f, -0.0008f, 0.0112f, 0.177f, 0.006f}, // 63 Curva
    {64, -0.0459f, -0.0008f, 0.0195f, 0.176f, 0.0f}, // 64 Curva
    {96, -0.0405f, -0.0008f, 0.0289f, 0.13f, 0.0f}, // 65 Curva
    {64, -0.0334f, -0.0008f, 0.037f, 0.195f, 0.0f}, // 66 Curva
    {96, -0.0245f, -0.0008f, 0.0433f, 0.143f, 0.0f}, // 67 Curva
    {96, -0.0125f, -0.0008f, 0.0482f, 0.156f, 0.0f}, // 68 Curva
    {96, 0.0004f, -0.0008f, 0.0498f, 0.143f, 0.0f}, // 69 Curva
    {64, 0.0112f, -0.0008f, 0.0486f, 0.195f, 0.0f}, // 70 Curva
    {96, 0.0214f, -0.0008f, 0.0449f, 0.143f, 0.0f}, // 71 Curva
    {96, 0.0323f, -0.0008f, 0.0378f, 0.143f, 0.0f}, // 72 Curva
    {64, 0.0398f, -0.0008f, 0.03f, 0.176f, 0.0f}, // 73 Curva
    {64, 0.0444f, -0.0008f, 0.0226f, 0.156f, 0.0f}, // 74 Curva
    {64, 0.0477f, -0.0008f, 0.0146f, 0.156f, 0.0f}, // 75 Curva
    {64, 0.0495f, -0.0008f, 0.0061f, 0.156f, 0.0f}, // 76 Curva
    {64, 0.0498f, -0.0008f, -0.0026f, 0.238f, -0.007f}, // 77 Curva
    {48, 0.0462f, 0.0f, -0.02f, 0.369f, -0.009f}, // 78 Curva
    {48, 0.0395f, 0.0f, -0.0313f, 0.312f, 0.0f}, // 79 Curva
    {48, 0.03f, 0.0f, -0.0404f, 0.312f, 0.0f}, // 80 Curva
    {48, 0.0185f, 0.0f, -0.0468f, 0.365f, 0.0f}, // 81 Curva
    {80, 0.0014f, 0.0f, -0.0501f, 0.274f, 0.0f}, // 82 Curva
    {134, -0.019f, 0.0f, -0.0463f, 0.029f, -0.017f}, // 83 Curva
    {83, -0.0053f, 0.0039f, -0.0495f, -0.085f, 0.011f}, // 84 Rampa
    {210, -0.0071f, -0.0015f, -0.0494f, -0.015f, 0.012f}, // 85 Rampa
    {231, 0.0001f, -0.0006f, -0.05f, -0.018f, -0.062f}, // 86 Curva
    {38, 0.0f, 0.0226f, -0.0448f, 0.001f, -0.009f}, // 87 Rampa
    {155, 0.0f, 0.0f, -0.0501f, 0.0f, 0.182f}, // 88 Recta
    {33, 0.0f, -0.0248f, -0.0438f, 0.0f, 0.197f}, // 89 Rampa
    {171, 0.0f, -0.0112f, -0.0487f, -0.044f, -0.087f}, // 90 Rampa
    {29, 0.0131f, 0.0f, -0.0488f, -0.733f, -0.223f}, // 91 Curva
    {25, 0.0331f, 0.0f, -0.0362f, -1.1f, 0.0f}, // 92 Curva
    {29, 0.0475f, 0.0f, -0.0173f, -0.818f, 0.084f}, // 93 Curva
    {406, 0.0498f, -0.0043f, 0.0f, -0.025f, 0.0f}, // 94 Rampa
};

// Variables y funciones de animacion del archivo principal.
float naveX, naveY, naveZ, giroNave, inclinacionNave;
float sateliteX, sateliteY, sateliteZ;
float panelPosZ, panelNegZ, antena;
int tramoNave = 0, pasoNave = 0, vueltasNave = 0;
int etapaSatelite = 0;
bool panelPosSube = true, panelNegSube = false;

void ReiniciarAnimacion()
{
    naveX = 23.76f; naveY = 3.878f; naveZ = -33.12f;
    giroNave = 89.839f; inclinacionNave = 2.443f;
    tramoNave = pasoNave = vueltasNave = 0;
    sateliteX = -18; sateliteY = 30; sateliteZ = -18;
    etapaSatelite = 0;
    panelPosZ = panelNegZ = antena = 0;
    panelPosSube = true; panelNegSube = false;
}

void ActualizarNave()
{
    const TramoNave& tramo = tramosNave[tramoNave];
    naveX += tramo.dx;
    naveY += tramo.dy;
    naveZ += tramo.dz;
    giroNave += tramo.giroY;
    inclinacionNave += tramo.giroX;
    pasoNave++;
    if (pasoNave >= tramo.pasos)
    {
        pasoNave = 0;
        tramoNave++;
        if (tramoNave >= sizeof(tramosNave) / sizeof(tramosNave[0]))
        {
            // Cierra la vuelta y elimina pequenos errores acumulados de float.
            tramoNave = 0;
            vueltasNave++;
            naveX = 23.76f; naveY = 3.878f; naveZ = -33.12f;
            giroNave = 89.839f; inclinacionNave = 2.443f;
        }
    }
}

void ActualizarSatelite()
{
    // Recorrido rectangular elevado: X, Z y pequenas subidas/bajadas.
    switch (etapaSatelite)
    {
    case 0:
        sateliteX += 0.025f; sateliteY += 0.00278f;
        if (sateliteX >= 18) { sateliteX = 18; sateliteY = 34; etapaSatelite = 1; }
        break;
    case 1:
        sateliteZ += 0.025f;
        if (sateliteZ >= 18) { sateliteZ = 18; etapaSatelite = 2; }
        break;
    case 2:
        sateliteX -= 0.025f; sateliteY -= 0.00278f;
        if (sateliteX <= -18) { sateliteX = -18; sateliteY = 30; etapaSatelite = 3; }
        break;
    case 3:
        sateliteZ -= 0.025f;
        if (sateliteZ <= -18) { sateliteZ = -18; etapaSatelite = 0; }
        break;
    }

    // Mismos ejes y limites que P05: paneles sobre Z y antena sobre Y.
    if (panelPosSube) panelPosZ += 0.417f;
    else panelPosZ -= 0.417f;
    if (panelPosZ >= 90) { panelPosZ = 90; panelPosSube = false; }
    if (panelPosZ <= -90) { panelPosZ = -90; panelPosSube = true; }

    if (panelNegSube) panelNegZ += 0.417f;
    else panelNegZ -= 0.417f;
    if (panelNegZ >= 90) { panelNegZ = 90; panelNegSube = false; }
    if (panelNegZ <= -90) { panelNegZ = -90; panelNegSube = true; }

    antena += 0.417f;
    if (antena >= 360) antena -= 360;
}


const float toRadians = 3.14159265f / 180;
const float escalaPista = 0.003f;
const float escalaNave = 0.28f;
const float pasoFijo = 1.0f / 120;
Window mainWindow(1366, 768);
Camera camera;
Shader shader;
// Plasma: reutiliza el shader de texturas, sin oscurecerlo con la luz.
Shader shaderPlasma;
MeshModel mallaPlasma;
Skybox skybox;
Model Pista, Nave, LandsatCuerpo, LandsatPanelPosZ, LandsatPanelNegZ, LandsatAntena;
Texture blanca;
Texture plasmaAzul;
Material opaco, metal;
DirectionalLight luzPrincipal;
PointLight lucesMotores[2];

// Los pivotes se conservan exactamente como en el P05 adjunto.
const glm::vec3 pivotePanelPosZ(0, 2.259f, 0.458f);
const glm::vec3 pivotePanelNegZ(0, 2.259f, -0.458f);
const glm::vec3 pivoteAntena(0, 2.901f, 0);
glm::mat4 matrizPista(1), matrizNave(1), matrizSatelite(1), matrizParte(1);
glm::mat4 projection(1), view(1);
glm::vec3 ojo(0), objetivo(0);
glm::vec4 motorIzq(0), motorDer(0);
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
GLuint uniformEye = 0, uniformColor = 0, uniformOffset = 0;
GLuint uniformSpecular = 0, uniformShininess = 0;
GLfloat now = 0, lastTime = 0, deltaTime = 0, acumulador = 0;

bool ComprobarRecursos()
{
    const char* archivos[] = {
        "Models/track.obj", "Models/track.mtl", "Textures/pista_carril.png",
        "Models/Nave_Azul.obj", "Models/Nave_Azul.mtl",
        "Models/Landsat/Cuerpo_Landsat.obj", "Models/Landsat/Panel_Solar_PosZ.obj",
        "Models/Landsat/Panel_Solar_NegZ.obj", "Models/Landsat/Antena.obj",
        "Models/Landsat/material.mtl", "Textures/plain.png",
        "Textures/nave_metal_oscuro.png", "Textures/nave_luces_azules.png",
        "Textures/plasma_azul.png", "shaders/shader_texture.vert",
        "shaders/shader_texture.frag",
        "shaders/shader_light.vert", "shaders/shader_light.frag"
    };
    bool completos = true;
    FILE* archivo = nullptr;
    for (const char* ruta : archivos)
    {
#ifdef _MSC_VER
        fopen_s(&archivo, ruta, "rb");
#else
        archivo = std::fopen(ruta, "rb");
#endif
        if (!archivo) { std::printf("Falta el recurso: %s\n", ruta); completos = false; }
        else std::fclose(archivo);
    }
    return completos;
}

// Dos planos cruzados permiten ver el plasma desde arriba y desde los lados.
// La boca comienza en Z = 0; el chorro sale hacia Z negativo del modelo.
void CrearPlasmaMotores()
{
    GLfloat vertices[] = {
        // X, Y, Z, U, V, normal X, normal Y, normal Z
        -0.625f, 0, 0,     0.28f, 0.97f, 0, 1, 0,
         0.625f, 0, 0,     0.72f, 0.97f, 0, 1, 0,
         0.625f, 0, -3.2f, 0.72f, 0.02f, 0, 1, 0,
        -0.625f, 0, -3.2f, 0.28f, 0.02f, 0, 1, 0,
        0, -0.625f, 0,     0.28f, 0.97f, 1, 0, 0,
        0,  0.625f, 0,     0.72f, 0.97f, 1, 0, 0,
        0,  0.625f, -3.2f, 0.72f, 0.02f, 1, 0, 0,
        0, -0.625f, -3.2f, 0.28f, 0.02f, 1, 0, 0
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7 };
    mallaPlasma.CreateMeshModel(vertices, indices, 64, 12);
}

bool CargarEscena()
{
    shader.CreateFromFiles("shaders/shader_light.vert", "shaders/shader_light.frag");
    if (!shader.EstaListo()) return false;
    shaderPlasma.CreateFromFiles("shaders/shader_texture.vert", "shaders/shader_texture.frag");
    if (!shaderPlasma.EstaListo()) return false;
    plasmaAzul = Texture("Textures/plasma_azul.png");
    if (!plasmaAzul.LoadTextureA()) return false;
    CrearPlasmaMotores();
    Pista.LoadModel("Models/track.obj");
    Nave.LoadModel("Models/Nave_Azul.obj");
    LandsatCuerpo.LoadModel("Models/Landsat/Cuerpo_Landsat.obj");
    LandsatPanelPosZ.LoadModel("Models/Landsat/Panel_Solar_PosZ.obj");
    LandsatPanelNegZ.LoadModel("Models/Landsat/Panel_Solar_NegZ.obj");
    LandsatAntena.LoadModel("Models/Landsat/Antena.obj");
    if (!Pista.EstaCargado() || !Nave.EstaCargado() || !LandsatCuerpo.EstaCargado() ||
        !LandsatPanelPosZ.EstaCargado() || !LandsatPanelNegZ.EstaCargado() ||
        !LandsatAntena.EstaCargado()) return false;
    blanca = Texture("Textures/plain.png");
    if (!blanca.LoadTextureA()) return false;
    opaco = Material(0.15f, 12);
    metal = Material(0.5f, 64);
    luzPrincipal = DirectionalLight(1, 1, 1, 0.35f, 0.6f, -0.4f, -1, -0.3f);
    lucesMotores[0] = PointLight(0, 0.45f, 1, 0, 2, 0, 0, 0, 1, 0.35f, 0.44f);
    lucesMotores[1] = PointLight(0, 0.45f, 1, 0, 2, 0, 0, 0, 1, 0.35f, 0.44f);
    std::vector<std::string> caras;
    caras.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_lf.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_up.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_dn.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    caras.push_back("Textures/Skybox/cupertin-lake_ft.tga");
    skybox = Skybox(caras);
    matrizPista = glm::scale(glm::mat4(1), glm::vec3(escalaPista));
    return true;
}

void PrepararMatrices()
{
    matrizNave = glm::translate(glm::mat4(1),
        glm::vec3(naveX, naveY, naveZ));
    matrizNave = glm::rotate(matrizNave, giroNave * toRadians, glm::vec3(0, 1, 0));
    matrizNave = glm::rotate(matrizNave, inclinacionNave * toRadians, glm::vec3(1, 0, 0));
    matrizNave = glm::scale(matrizNave, glm::vec3(escalaNave));
    // Centrar la nave completa antes de sus giros.
    matrizNave = glm::translate(matrizNave, glm::vec3(0, -0.195f, 3.548f));
    matrizSatelite = glm::translate(glm::mat4(1),
        glm::vec3(sateliteX, sateliteY, sateliteZ));
    matrizSatelite = glm::scale(matrizSatelite, glm::vec3(1.25f));
    motorIzq = matrizNave * glm::vec4(-1, 0.15f, -8.5f, 1);
    motorDer = matrizNave * glm::vec4(1, 0.15f, -8.5f, 1);
    lucesMotores[0].SetPos(glm::vec3(motorIzq));
    lucesMotores[1].SetPos(glm::vec3(motorDer));
}

void PrepararVista()
{
    // Estas vistas observan la animacion; no calculan el movimiento de los modelos.
    if (mainWindow.getVista() == 0)
    {
        ojo = glm::vec3(75, 70, 90);
        objetivo = glm::vec3(5.33f, 5, -6.96f);
    }
    else if (mainWindow.getVista() == 1)
    {
        objetivo = glm::vec3(naveX, naveY, naveZ);
        ojo = objetivo + glm::vec3(10, 7, 12);
    }
    else if (mainWindow.getVista() == 2)
    {
        objetivo = glm::vec3(sateliteX, sateliteY + 2, sateliteZ);
        ojo = objetivo + glm::vec3(10, 6, 10);
    }
    if (mainWindow.getVista() == 3)
    {
        ojo = camera.getCameraPosition();
        view = camera.calculateViewMatrix();
    }
    else view = glm::lookAt(ojo, objetivo, glm::vec3(0, 1, 0));
}

void DibujarEscena()
{
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizPista));
    glUniform3f(uniformColor, 1, 1, 1);
    opaco.UseMaterial(uniformSpecular, uniformShininess);
    Pista.RenderModel();

    glUniform3f(uniformColor, 1, 1, 1);
    metal.UseMaterial(uniformSpecular, uniformShininess);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizNave));
    Nave.RenderModel();

    blanca.UseTexture();
    glUniform3f(uniformColor, 0.72f, 0.74f, 0.8f);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizSatelite));
    LandsatCuerpo.RenderModel();

    // T(pivote) R T(-pivote): cada componente gira en su articulacion.
    matrizParte = glm::translate(matrizSatelite, pivotePanelPosZ);
    matrizParte = glm::rotate(matrizParte, panelPosZ * toRadians, glm::vec3(0, 0, 1));
    matrizParte = glm::translate(matrizParte, -pivotePanelPosZ);
    glUniform3f(uniformColor, 0.05f, 0.25f, 0.7f);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizParte));
    LandsatPanelPosZ.RenderModel();

    matrizParte = glm::translate(matrizSatelite, pivotePanelNegZ);
    matrizParte = glm::rotate(matrizParte, panelNegZ * toRadians, glm::vec3(0, 0, 1));
    matrizParte = glm::translate(matrizParte, -pivotePanelNegZ);
    glUniform3f(uniformColor, 0.05f, 0.55f, 0.9f);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizParte));
    LandsatPanelNegZ.RenderModel();

    matrizParte = glm::translate(matrizSatelite, pivoteAntena);
    matrizParte = glm::rotate(matrizParte, antena * toRadians, glm::vec3(0, 1, 0));
    matrizParte = glm::translate(matrizParte, -pivoteAntena);
    glUniform3f(uniformColor, 0.95f, 0.65f, 0.1f);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizParte));
    LandsatAntena.RenderModel();
}

// Se dibuja al final para mezclar el plasma sobre los objetos opacos.
void DibujarPlasmaMotores()
{
    shaderPlasma.UseShader();
    glUniformMatrix4fv(shaderPlasma.GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(shaderPlasma.GetViewLocation(), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(shaderPlasma.getColorLocation(), 1, 1, 1);
    plasmaAzul.UseTexture();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Mezcla aditiva para el brillo del plasma.
    glDepthMask(GL_FALSE); // Conserva la prueba de profundidad, sin tapar otros planos.

    // Centros de las salidas medidos en Nave_Azul.obj; quedan fuera de la tobera.
    matrizParte = glm::translate(matrizNave, glm::vec3(-1.03f, 0.18f, -8.52f));
    glUniformMatrix4fv(shaderPlasma.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(matrizParte));
    mallaPlasma.RenderMeshModel();
    matrizParte = glm::translate(matrizNave, glm::vec3(1.03f, 0.175f, -8.52f));
    glUniformMatrix4fv(shaderPlasma.GetModelLocation(), 1, GL_FALSE, glm::value_ptr(matrizParte));
    mallaPlasma.RenderMeshModel();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader.UseShader();
}

int main()
{
    if (mainWindow.Initialise() != 0) return 1;
    if (!ComprobarRecursos())
    {
        std::puts("Extrae todo el ZIP y usa la carpeta del proyecto como directorio de trabajo.");
        std::getchar();
        return 1;
    }
    ReiniciarAnimacion();
    std::printf("OpenGL: %s\n", glGetString(GL_VERSION));
    if (!CargarEscena())
    {
        std::puts("No se pudo completar la carga. Revisa los mensajes anteriores.");
        std::getchar();
        return 1;
    }
    std::puts("Escena cargada: pista, nave y cuatro piezas del Landsat.");
    camera = Camera(glm::vec3(85, 72, 98), glm::vec3(0, 1, 0), -128, -25, 25, 0.2f);
    projection = glm::perspective(45 * toRadians,
        mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 500.0f);
    uniformModel = shader.GetModelLocation();
    uniformProjection = shader.GetProjectionLocation();
    uniformView = shader.GetViewLocation();
    uniformEye = shader.GetEyePositionLocation();
    uniformColor = shader.getColorLocation();
    uniformOffset = shader.getOffsetLocation();
    uniformSpecular = shader.GetSpecularIntensityLocation();
    uniformShininess = shader.GetShininessLocation();
    lastTime = static_cast<GLfloat>(glfwGetTime());
    std::puts("P pausa | R reinicia | F1 nave | F2 satelite");
    std::puts("F5 general | F6 sigue nave | F7 sigue satelite | F8 camara WASD y mouse");

    while (!mainWindow.getShouldClose())
    {
        now = static_cast<GLfloat>(glfwGetTime());
        deltaTime = now - lastTime;
        lastTime = now;
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        glfwPollEvents();
        mainWindow.ActualizarControles(deltaTime, deltaTime, camera);
        if (mainWindow.getVista() == 3)
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        else { mainWindow.getXChange(); mainWindow.getYChange(); }
        if (mainWindow.ConsumirReinicio())
        {
            ReiniciarAnimacion();
            acumulador = 0;
        }
        if (!mainWindow.getPausa()) acumulador += deltaTime;
        while (acumulador >= pasoFijo)
        {
            if (mainWindow.getAnimarNave()) ActualizarNave();
            if (mainWindow.getAnimarSatelite()) ActualizarSatelite();
            acumulador -= pasoFijo;
        }
        PrepararMatrices();
        PrepararVista();
        mainWindow.ActualizarTitulo(tramoNave, vueltasNave);
        glClearColor(0.04f, 0.06f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.DrawSkybox(view, projection);
        shader.UseShader();
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(uniformEye, 1, glm::value_ptr(ojo));
        glUniform2f(uniformOffset, 0, 0);
        shader.SetDirectionalLight(&luzPrincipal);
        shader.SetPointLights(lucesMotores, 2);
        shader.SetSpotLights(nullptr, 0);
        DibujarEscena();
        DibujarPlasmaMotores();
        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}
