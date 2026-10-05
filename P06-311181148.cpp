/*
Práctica 6: Texturizado
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

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz

std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture holocronTexture;

Model Kitt_M;
Model Llanta_M;
Model Dado_M;
Model Holocron_M;
Model Avion_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Angulos compartidos por ambos holocrones, en grados.
GLfloat giroHolocronX = 0.0f;
GLfloat giroHolocronY = 0.0f;
GLfloat giroHolocronZ = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";





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



	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	// Atlas de GIMP: 2048 x 1536; cada cara ocupa 512 x 512.
	// Texture::LoadTextureA invierte verticalmente la imagen.
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 10, 9, 10, 8, 11,
		12, 13, 14, 14, 15, 12,
		16, 18, 17, 18, 16, 19,
		20, 21, 22, 22, 23, 20,
	};
	GLfloat cubo_vertices[] = {
		// x, y, z, u, v, nx, ny, nz
		// Frente: New republic
		-0.50000000f, -0.50000000f, 0.50000000f, 0.50000000f, 0.33333333f, 0.00000000f, 0.00000000f, 1.00000000f,
		0.50000000f, -0.50000000f, 0.50000000f, 0.75000000f, 0.33333333f, 0.00000000f, 0.00000000f, 1.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.75000000f, 0.66666667f, 0.00000000f, 0.00000000f, 1.00000000f,
		-0.50000000f, 0.50000000f, 0.50000000f, 0.50000000f, 0.66666667f, 0.00000000f, 0.00000000f, 1.00000000f,
		// Derecha: Rebel alliance
		0.50000000f, -0.50000000f, 0.50000000f, 0.75000000f, 0.33333333f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 1.00000000f, 0.33333333f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 1.00000000f, 0.66666667f, 1.00000000f, 0.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.75000000f, 0.66666667f, 1.00000000f, 0.00000000f, 0.00000000f,
		// Atras: Black sun
		-0.50000000f, -0.50000000f, -0.50000000f, 0.25000000f, 0.33333333f, 0.00000000f, 0.00000000f, -1.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.00000000f, 0.33333333f, 0.00000000f, 0.00000000f, -1.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.00000000f, 0.66666667f, 0.00000000f, 0.00000000f, -1.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.25000000f, 0.66666667f, 0.00000000f, 0.00000000f, -1.00000000f,
		// Izquierda: Mandalorian crest
		-0.50000000f, -0.50000000f, -0.50000000f, 0.25000000f, 0.33333333f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, -0.50000000f, 0.50000000f, 0.50000000f, 0.33333333f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, 0.50000000f, 0.50000000f, 0.66666667f, -1.00000000f, 0.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.25000000f, 0.66666667f, -1.00000000f, 0.00000000f, 0.00000000f,
		// Inferior: Galactic empire
		-0.50000000f, -0.50000000f, 0.50000000f, 0.50000000f, 0.00000000f, 0.00000000f, -1.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, 0.50000000f, 0.75000000f, 0.00000000f, 0.00000000f, -1.00000000f, 0.00000000f,
		0.50000000f, -0.50000000f, -0.50000000f, 0.75000000f, 0.33333333f, 0.00000000f, -1.00000000f, 0.00000000f,
		-0.50000000f, -0.50000000f, -0.50000000f, 0.50000000f, 0.33333333f, 0.00000000f, -1.00000000f, 0.00000000f,
		// Superior: Lord revan
		-0.50000000f, 0.50000000f, 0.50000000f, 0.50000000f, 0.66666667f, 0.00000000f, 1.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, 0.50000000f, 0.75000000f, 0.66666667f, 0.00000000f, 1.00000000f, 0.00000000f,
		0.50000000f, 0.50000000f, -0.50000000f, 0.75000000f, 1.00000000f, 0.00000000f, 1.00000000f, 0.00000000f,
		-0.50000000f, 0.50000000f, -0.50000000f, 0.50000000f, 1.00000000f, 0.00000000f, 1.00000000f, 0.00000000f,
	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);
}


void CrearHolocron()
{
	// Misma geometria: 28 triangulos. Cada triangulo tiene sus propios vertices.
	// Asi una esquina puede usar diferentes UV y normales en cada cara.
	// Atlas holocron_logos2.png. LoadTextureA invierte la imagen verticalmente.
	unsigned int holocron_indices[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35,
		36, 37, 38,
		39, 40, 41,
		42, 43, 44,
		45, 46, 47,
		48, 49, 50,
		51, 52, 53,
		54, 55, 56,
		57, 58, 59,
		60, 61, 62,
		63, 64, 65,
		66, 67, 68,
		69, 70, 71,
		72, 73, 74,
		75, 76, 77,
		78, 79, 80,
		81, 82, 83,
	};
	GLfloat holocron_vertices[] = {
		// x, y, z, u, v, nx, ny, nz
		// Esquina inferior izquierda atras (triangulo 1)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.24886772f, 0.42288384f, -0.52904187f, -0.58890214f, -0.61099015f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.24561066f, 0.29506838f, -0.52904187f, -0.58890214f, -0.61099015f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.32065952f, 0.29363674f, -0.52904187f, -0.58890214f, -0.61099015f,
		// Izquierda: Republica (triangulo 2)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.16096842f, 0.89546653f, -0.99846324f, 0.04023156f, -0.03811263f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.34296795f, 0.71201269f, -0.99846324f, 0.04023156f, -0.03811263f,
		-2.45137200f, 2.42175900f, 0.07753800f, 0.34605376f, 0.89509500f, -0.99846324f, 0.04023156f, -0.03811263f,
		// Esquina superior derecha atras: Dark Lord (triangulo 3)
		2.52120600f, 2.44824100f, 0.06338800f, 0.38624786f, 0.42395457f, 0.56607770f, 0.59935832f, -0.56597318f,
		2.53494400f, -0.06114600f, -2.58028000f, 0.46922082f, 0.28409778f, 0.56607770f, 0.59935832f, -0.56597318f,
		0.04964900f, 2.44012800f, -2.41721700f, 0.54247152f, 0.42350241f, 0.56607770f, 0.59935832f, -0.56597318f,
		// Esquina superior izquierda atras: Senado (triangulo 4)
		0.04964900f, 2.44012800f, -2.41721700f, 0.39399967f, 0.64443833f, -0.57367971f, 0.58743881f, -0.57079527f,
		-2.45398000f, -0.06338800f, -2.47745100f, 0.46864619f, 0.51042747f, -0.57367971f, 0.58743881f, -0.57079527f,
		-2.45137200f, 2.42175900f, 0.07753800f, 0.54702648f, 0.64345505f, -0.57367971f, 0.58743881f, -0.57079527f,
		// Frente: Phoenix (triangulo 5)
		-0.14069600f, -2.60193400f, 2.55856100f, 0.78525585f, 0.66268011f, -0.02235395f, 0.00355940f, 0.99974378f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.58618304f, 0.88141437f, -0.02235395f, 0.00355940f, 0.99974378f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.57762279f, 0.66268011f, -0.02235395f, 0.00355940f, 0.99974378f,
		// Esquina superior izquierda frente: Rebelde (triangulo 6)
		-2.45137200f, 2.42175900f, 0.07753800f, 0.60623395f, 0.65388480f, -0.56493997f, 0.58866509f, 0.57820086f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.66930845f, 0.52487707f, -0.56493997f, 0.58866509f, 0.57820086f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.74935490f, 0.65277760f, -0.56493997f, 0.58866509f, 0.57820086f,
		// Esquina superior derecha frente: Primera Orden (triangulo 7)
		0.05115200f, 2.39978100f, 2.54504300f, 0.77007439f, 0.62502149f, 0.57317160f, 0.57708605f, 0.58176112f,
		2.59300300f, -0.08522200f, 2.50575500f, 0.86101010f, 0.47449847f, 0.57317160f, 0.57708605f, 0.58176112f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.94327533f, 0.62795684f, 0.57317160f, 0.57708605f, 0.58176112f,
		// Esquina inferior izquierda frente (triangulo 8)
		-2.64678600f, -0.13904700f, 2.49375700f, 0.30873604f, 0.40394022f, -0.59336121f, -0.58934361f, 0.54826689f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.30918695f, 0.29824413f, -0.59336121f, -0.58934361f, 0.54826689f,
		-0.14069600f, -2.60193400f, 2.55856100f, 0.36982859f, 0.29716209f, -0.59336121f, -0.58934361f, 0.54826689f,
		// Esquina inferior derecha frente: Raziel (triangulo 9)
		2.59300300f, -0.08522200f, 2.50575500f, 0.67514280f, 0.44420556f, 0.59328886f, -0.56795205f, 0.57047244f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.67284152f, 0.32890769f, 0.59328886f, -0.56795205f, 0.57047244f,
		2.63220100f, -2.55389200f, 0.00722600f, 0.74217496f, 0.32672269f, 0.59328886f, -0.56795205f, 0.57047244f,
		// Esquina inferior derecha atras: Cold Order (triangulo 10)
		2.53494400f, -0.06114600f, -2.58028000f, 0.87936347f, 0.46200386f, 0.53772387f, -0.57673654f, -0.61500244f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.87892669f, 0.32094679f, 0.53772387f, -0.57673654f, -0.61500244f,
		0.08143200f, -2.51320400f, -2.42600500f, 0.95763959f, 0.32107564f, 0.53772387f, -0.57673654f, -0.61500244f,
		// Atras: Antigua Republica (triangulo 11)
		0.08143200f, -2.51320400f, -2.42600500f, 0.36448326f, 0.68322800f, -0.06410820f, 0.00135916f, -0.99794203f,
		0.04964900f, 2.44012800f, -2.41721700f, 0.57379474f, 0.89695119f, -0.06410820f, 0.00135916f, -0.99794203f,
		2.53494400f, -0.06114600f, -2.58028000f, 0.36076073f, 0.89278751f, -0.06410820f, 0.00135916f, -0.99794203f,
		// Base: logo inferior (triangulo 12)
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, 0.01066270f, -0.99993918f, -0.00281980f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.23977939f, 0.09015456f, 0.01066270f, -0.99993918f, -0.00281980f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, 0.01066270f, -0.99993918f, -0.00281980f,
		// Derecha: Mandalorian Clan (triangulo 13)
		2.63220100f, -2.55389200f, 0.00722600f, 0.98488673f, 0.69418826f, 0.99973246f, 0.02225438f, -0.00630419f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.77165400f, 0.90518855f, 0.99973246f, 0.02225438f, -0.00630419f,
		2.59300300f, -0.08522200f, 2.50575500f, 0.77429810f, 0.69418826f, 0.99973246f, 0.02225438f, -0.00630419f,
		// Superior: Boba Fett (triangulo 14)
		0.04964900f, 2.44012800f, -2.41721700f, 0.15564658f, 0.69749336f, -0.01144543f, 0.99990142f, 0.00813344f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.38384146f, 0.46621343f, -0.01144543f, 0.99990142f, 0.00813344f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.38490407f, 0.69547793f, -0.01144543f, 0.99990142f, 0.00813344f,
		// Superior: Boba Fett (triangulo 15)
		0.04964900f, 2.44012800f, -2.41721700f, 0.15564658f, 0.69749336f, 0.00076554f, 0.99996666f, 0.00813027f,
		-2.45137200f, 2.42175900f, 0.07753800f, 0.15373877f, 0.46621343f, 0.00076554f, 0.99996666f, 0.00813027f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.38384146f, 0.46621343f, 0.00076554f, 0.99996666f, 0.00813027f,
		// Atras: Antigua Republica (triangulo 16)
		0.08143200f, -2.51320400f, -2.42600500f, 0.36448326f, 0.68322800f, 0.02213706f, 0.00191576f, -0.99975311f,
		-2.45398000f, -0.06338800f, -2.47745100f, 0.57732230f, 0.68322800f, 0.02213706f, 0.00191576f, -0.99975311f,
		0.04964900f, 2.44012800f, -2.41721700f, 0.57379474f, 0.89695119f, 0.02213706f, 0.00191576f, -0.99975311f,
		// Derecha: Mandalorian Clan (triangulo 17)
		2.63220100f, -2.55389200f, 0.00722600f, 0.98488673f, 0.69418826f, 0.99962143f, 0.02236112f, -0.01603072f,
		2.53494400f, -0.06114600f, -2.58028000f, 0.99013562f, 0.90960490f, 0.99962143f, 0.02236112f, -0.01603072f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.77165400f, 0.90518855f, 0.99962143f, 0.02236112f, -0.01603072f,
		// Frente: Phoenix (triangulo 18)
		-0.14069600f, -2.60193400f, 2.55856100f, 0.78525585f, 0.66268011f, 0.01744185f, 0.00203325f, 0.99984581f,
		2.59300300f, -0.08522200f, 2.50575500f, 0.79621632f, 0.88196512f, 0.01744185f, 0.00203325f, 0.99984581f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.58618304f, 0.88141437f, 0.01744185f, 0.00203325f, 0.99984581f,
		// Izquierda: Republica (triangulo 19)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.16096842f, 0.89546653f, -0.99915310f, -0.01325560f, -0.03895343f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.16174745f, 0.71201269f, -0.99915310f, -0.01325560f, -0.03895343f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.34296795f, 0.71201269f, -0.99915310f, -0.01325560f, -0.03895343f,
		// Esquina inferior izquierda atras (triangulo 20)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.24886772f, 0.42288384f, -0.55796169f, -0.58971810f, -0.58387611f,
		0.08143200f, -2.51320400f, -2.42600500f, 0.17182594f, 0.29703232f, -0.55796169f, -0.58971810f, -0.58387611f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.24561066f, 0.29506838f, -0.55796169f, -0.58971810f, -0.58387611f,
		// Esquina inferior izquierda frente (triangulo 21)
		-2.64678600f, -0.13904700f, 2.49375700f, 0.30873604f, 0.40394022f, -0.59524609f, -0.58935323f, 0.54620955f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.24764450f, 0.29815077f, -0.59524609f, -0.58935323f, 0.54620955f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.30918695f, 0.29824413f, -0.59524609f, -0.58935323f, 0.54620955f,
		// Esquina inferior derecha frente: Raziel (triangulo 22)
		2.59300300f, -0.08522200f, 2.50575500f, 0.67514280f, 0.44420556f, 0.53374744f, -0.56659454f, 0.62776134f,
		-0.14069600f, -2.60193400f, 2.55856100f, 0.59705397f, 0.32429472f, 0.53374744f, -0.56659454f, 0.62776134f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.67284152f, 0.32890769f, 0.53374744f, -0.56659454f, 0.62776134f,
		// Esquina inferior derecha atras: Cold Order (triangulo 23)
		2.53494400f, -0.06114600f, -2.58028000f, 0.87936347f, 0.46200386f, 0.57823863f, -0.57659642f, -0.57721457f,
		2.63220100f, -2.55389200f, 0.00722600f, 0.79396312f, 0.31864052f, 0.57823863f, -0.57659642f, -0.57721457f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.87892669f, 0.32094679f, 0.57823863f, -0.57659642f, -0.57721457f,
		// Base: logo inferior (triangulo 24)
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, 0.02023575f, -0.99963300f, -0.01801078f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.34250140f, 0.12453748f, 0.02023575f, -0.99963300f, -0.01801078f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.30742869f, 0.15614844f, 0.02023575f, -0.99963300f, -0.01801078f,
		// Base: logo inferior (triangulo 25)
		-1.18505500f, -2.53049900f, -1.19826100f, 0.30742869f, 0.15614844f, -0.00552381f, -0.99978906f, -0.01978196f,
		0.08143200f, -2.51320400f, -2.42600500f, 0.27408958f, 0.18846727f, -0.00552381f, -0.99978906f, -0.01978196f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, -0.00552381f, -0.99978906f, -0.01978196f,
		// Base: logo inferior (triangulo 26)
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, -0.00620712f, -0.99997747f, -0.00255518f,
		2.63220100f, -2.55389200f, 0.00722600f, 0.20695100f, 0.12440466f, -0.00620712f, -0.99997747f, -0.00255518f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.23977939f, 0.09015456f, -0.00620712f, -0.99997747f, -0.00255518f,
		// Base: logo inferior (triangulo 27)
		1.38502800f, -2.54947600f, 1.30867700f, 0.23977939f, 0.09015456f, 0.01076212f, -0.99952688f, -0.02881320f,
		-0.14069600f, -2.60193400f, 2.55856100f, 0.27994608f, 0.05723932f, 0.01076212f, -0.99952688f, -0.02881320f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, 0.01076212f, -0.99952688f, -0.02881320f,
		// Base: logo inferior (triangulo 28)
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, -0.00551307f, -0.99979468f, -0.01949871f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.30742869f, 0.15614844f, -0.00551307f, -0.99979468f, -0.01949871f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, -0.00551307f, -0.99979468f, -0.01949871f,
	};

	MeshModel* holocron = new MeshModel();
	// 84 vertices x 8 componentes = 672 floats; 84 indices.
	holocron->CreateMeshModel(holocron_vertices, holocron_indices,
		sizeof(holocron_vertices) / sizeof(holocron_vertices[0]),
		sizeof(holocron_indices) / sizeof(holocron_indices[0]));
	meshListModel.push_back(holocron);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(-1.0f, 7.0f, 23.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -7.0f, 0.3f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/logos_starwars.png");
	dadoTexture.LoadTextureA();
	holocronTexture = Texture("Textures/holocron_logos2.png");
	holocronTexture.LoadTextureA();

	Dado_M = Model();
	Dado_M.LoadModel("Models/CuboStarWars.obj");

	Holocron_M = Model();
	Holocron_M.LoadModel("Models/holocron_simple.obj");

	Avion_M = Model();
	Avion_M.LoadModel("Models/avion.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		// Tiempo real entre cuadros para girar a 60 grados por segundo.
		GLfloat pasoGiroHolocron = 60.0f * (now - lastTime);
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();

		// Mantener E, R o T para girar en X, Y o Z.
		// Shift junto con la tecla invierte el sentido.
		bool* teclasHolocron = mainWindow.getsKeys();
		GLfloat sentidoGiro = 1.0f - 2.0f * teclasHolocron[GLFW_KEY_LEFT_SHIFT];
		giroHolocronX += pasoGiroHolocron * sentidoGiro * teclasHolocron[GLFW_KEY_E];
		giroHolocronY += pasoGiroHolocron * sentidoGiro * teclasHolocron[GLFW_KEY_R];
		giroHolocronZ += pasoGiroHolocron * sentidoGiro * teclasHolocron[GLFW_KEY_T];
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshListModel[2]->RenderMeshModel();




		//Dado de Opengl
		//Ejercicio 1: Texturizar su dado con la imagen ya optimizada por ustedes con logos de star wars
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();

		//Ejercicio 2:Importar el cubo texturizado en el programa de modelado con 
		//la imagen ya optimizada por ustedes

		//Cubo texturizado y exportado desde Blender
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado_M.RenderModel();




		/*Reporte de práctica :

		Ejercicio 1: Crear o modificar el holocron y texturizarlo por medio de código
		Ejercicio 2: Importar el modelo del holocron texturizardo en el programa de modelado
		Ejercicio 3: Importar un modelo de avión con con la textura de la cara del personaje de la imagen del previo:
		Vidrio fonrtal: OJOS
		Frente del avión: Nariz y Sonrisa
		Alas: Logos del universo del personaje

		*/


		//Holocrones
		color = glm::vec3(1.0f, 1.0f, 1.0f);//blanco para conservar los colores de los logos
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.5f, 4.5f, 0.0f));
		// Girar sobre el origen del modelo antes de escalarlo.
		model = glm::rotate(model, giroHolocronX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, giroHolocronY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, giroHolocronZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocronTexture.UseTexture();
		meshListModel[5]->RenderMeshModel();


		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.5f, 2.5f, 0.0f));
		// El holocron importado comparte los controles E, R y T.
		model = glm::rotate(model, giroHolocronX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, giroHolocronY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, giroHolocronZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Holocron_M.RenderModel();





		// Avion de Goku a la derecha, separado de cubos y holocrones.
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.0f, 3.5f, -2.0f));
		model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.55f, 0.55f, 0.55f));
		// Centrar el modelo: su origen original esta cerca de la nariz.
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		Avion_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/