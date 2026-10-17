/*
Practica 7: Iluminacion 1
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
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminacion
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
// Anadir Texturas del dado y del holocron


Model Blackhawk_M;
// Rover blanco: cargar solo geometria y normales, sin leer texturas del MTL.
// Se mantiene Model.cpp original para los otros modelos.
class PiezaRover
{
public:
	void LoadModel(const std::string& archivo)
	{
		Assimp::Importer importer;
		const aiScene* escena = importer.ReadFile(archivo,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals);
		if (!escena || !escena->mRootNode) {
			printf("No se pudo cargar la pieza: %s\n", archivo.c_str());
			return;
		}
		CargarNodo(escena->mRootNode, escena);
	}
	void RenderModel()
	{
		// Aplicar blanco justo antes de dibujar, sin reemplazos por materiales.
		plainTexture.UseTexture();
		for (size_t i = 0; i < mallas.size(); i++)
			mallas[i]->RenderMeshModel();
	}
private:
	std::vector<MeshModel*> mallas;
	void CargarNodo(aiNode* nodo, const aiScene* escena)
	{
		for (unsigned int i = 0; i < nodo->mNumMeshes; i++) {
			aiMesh* malla = escena->mMeshes[nodo->mMeshes[i]];
			std::vector<GLfloat> vertices;
			std::vector<unsigned int> indices;
			for (unsigned int j = 0; j < malla->mNumVertices; j++) {
				// Normales negativas para coincidir con el shader original de la practica.
				vertices.insert(vertices.end(), {
				 malla->mVertices[j].x, malla->mVertices[j].y, malla->mVertices[j].z,
				 0.0f, 0.0f,
				 -malla->mNormals[j].x, -malla->mNormals[j].y, -malla->mNormals[j].z });
			}
			for (unsigned int j = 0; j < malla->mNumFaces; j++)
				for (unsigned int k = 0; k < malla->mFaces[j].mNumIndices; k++)
					indices.push_back(malla->mFaces[j].mIndices[k]);
			if (!vertices.empty() && !indices.empty()) {
				MeshModel* nueva = new MeshModel();
				nueva->CreateMeshModel(vertices.data(), indices.data(),
					static_cast<unsigned int>(vertices.size()), static_cast<unsigned int>(indices.size()));
				mallas.push_back(nueva);
			}
		}
		for (unsigned int i = 0; i < nodo->mNumChildren; i++)
			CargarNodo(nodo->mChildren[i], escena);
	}
};

PiezaRover cuerpoRover, brazoInferior, brazoCentral, pinzaRover;
PiezaRover ruedaDI, ruedaDD, ruedaCI, ruedaCD, ruedaTI, ruedaTD;
GLfloat giroRuedas = 0, giroBrazo1 = 0, giroBrazo2 = 0, giroPinza = 0;
Texture dadoTexture, holocronTexture;
//Sustituir por sus propios Rovers Con jerarquia
//Anadir modelo del avion.
Model Avion_M;
// NAVE: modelo nuevo y textura del plasma.
Model Nave_M;
Texture plasmaTexture;
MeshModel plasmaMesh;
GLfloat avanceRover = 0, avanceAvion = 0, alturaAvion = 0;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//funcion de calculo de normales por promedio de vertices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{

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
		-0.5f, -0.5f, 0.5f, 0.5f, 0.3333f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.5f, 0.75f, 0.3333f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.5f, 0.75f, 0.6667f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.5f, 0.5f, 0.6667f, 0.0f, 0.0f, -1.0f,
		// Derecha: Rebel alliance
		0.5f, -0.5f, 0.5f, 0.75f, 0.3333f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.3333f, -1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.6667f, -1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.75f, 0.6667f, -1.0f, 0.0f, 0.0f,
		// Atras: Black sun
		-0.5f, -0.5f, -0.5f, 0.25f, 0.3333f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.3333f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.6667f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.25f, 0.6667f, 0.0f, 0.0f, 1.0f,
		// Izquierda: Mandalorian crest
		-0.5f, -0.5f, -0.5f, 0.25f, 0.3333f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.5f, 0.3333f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.5f, 0.6667f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.25f, 0.6667f, 1.0f, 0.0f, 0.0f,
		// Inferior: Galactic empire
		-0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.75f, 0.3333f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.5f, 0.3333f, 0.0f, 1.0f, 0.0f,
		// Superior: Lord revan
		-0.5f, 0.5f, 0.5f, 0.5f, 0.6667f, 0.0f, -1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.75f, 0.6667f, 0.0f, -1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.75f, 1.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f,
	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);
}


void CrearHolocron()
{
	// Misma geometria: 28 triangulos. Cada triangulo tiene sus propios vertices.

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
		-2.45398000f, -0.06338800f, -2.47745100f, 0.24886772f, 0.42288384f, 0.52904187f, 0.58890214f, 0.61099015f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.24561066f, 0.29506838f, 0.52904187f, 0.58890214f, 0.61099015f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.32065952f, 0.29363674f, 0.52904187f, 0.58890214f, 0.61099015f,
		// Izquierda: Republica (triangulo 2)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.16096842f, 0.89546653f, 0.99846324f, -0.04023156f, 0.03811263f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.34296795f, 0.71201269f, 0.99846324f, -0.04023156f, 0.03811263f,
		-2.45137200f, 2.42175900f, 0.07753800f, 0.34605376f, 0.89509500f, 0.99846324f, -0.04023156f, 0.03811263f,
		// Esquina superior derecha atras: Dark Lord (triangulo 3)
		2.52120600f, 2.44824100f, 0.06338800f, 0.38624786f, 0.42395457f, -0.56607770f, -0.59935832f, 0.56597318f,
		2.53494400f, -0.06114600f, -2.58028000f, 0.46922082f, 0.28409778f, -0.56607770f, -0.59935832f, 0.56597318f,
		0.04964900f, 2.44012800f, -2.41721700f, 0.54247152f, 0.42350241f, -0.56607770f, -0.59935832f, 0.56597318f,
		// Esquina superior izquierda atras: Senado (triangulo 4)
		0.04964900f, 2.44012800f, -2.41721700f, 0.39399967f, 0.64443833f, 0.57367971f, -0.58743881f, 0.57079527f,
		-2.45398000f, -0.06338800f, -2.47745100f, 0.46864619f, 0.51042747f, 0.57367971f, -0.58743881f, 0.57079527f,
		-2.45137200f, 2.42175900f, 0.07753800f, 0.54702648f, 0.64345505f, 0.57367971f, -0.58743881f, 0.57079527f,
		// Frente: Phoenix (triangulo 5)
		-0.14069600f, -2.60193400f, 2.55856100f, 0.78525585f, 0.66268011f, 0.02235395f, -0.00355940f, -0.99974378f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.58618304f, 0.88141437f, 0.02235395f, -0.00355940f, -0.99974378f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.57762279f, 0.66268011f, 0.02235395f, -0.00355940f, -0.99974378f,
		// Esquina superior izquierda frente: Rebelde (triangulo 6)
		-2.45137200f, 2.42175900f, 0.07753800f, 0.60623395f, 0.65388480f, 0.56493997f, -0.58866509f, -0.57820086f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.66930845f, 0.52487707f, 0.56493997f, -0.58866509f, -0.57820086f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.74935490f, 0.65277760f, 0.56493997f, -0.58866509f, -0.57820086f,
		// Esquina superior derecha frente: Primera Orden (triangulo 7)
		0.05115200f, 2.39978100f, 2.54504300f, 0.77007439f, 0.62502149f, -0.57317160f, -0.57708605f, -0.58176112f,
		2.59300300f, -0.08522200f, 2.50575500f, 0.86101010f, 0.47449847f, -0.57317160f, -0.57708605f, -0.58176112f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.94327533f, 0.62795684f, -0.57317160f, -0.57708605f, -0.58176112f,
		// Esquina inferior izquierda frente (triangulo 8)
		-2.64678600f, -0.13904700f, 2.49375700f, 0.30873604f, 0.40394022f, 0.59336121f, 0.58934361f, -0.54826689f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.30918695f, 0.29824413f, 0.59336121f, 0.58934361f, -0.54826689f,
		-0.14069600f, -2.60193400f, 2.55856100f, 0.36982859f, 0.29716209f, 0.59336121f, 0.58934361f, -0.54826689f,
		// Esquina inferior derecha frente: Raziel (triangulo 9)
		2.59300300f, -0.08522200f, 2.50575500f, 0.67514280f, 0.44420556f, -0.59328886f, 0.56795205f, -0.57047244f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.67284152f, 0.32890769f, -0.59328886f, 0.56795205f, -0.57047244f,
		2.63220100f, -2.55389200f, 0.00722600f, 0.74217496f, 0.32672269f, -0.59328886f, 0.56795205f, -0.57047244f,
		// Esquina inferior derecha atras: Cold Order (triangulo 10)
		2.53494400f, -0.06114600f, -2.58028000f, 0.87936347f, 0.46200386f, -0.53772387f, 0.57673654f, 0.61500244f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.87892669f, 0.32094679f, -0.53772387f, 0.57673654f, 0.61500244f,
		0.08143200f, -2.51320400f, -2.42600500f, 0.95763959f, 0.32107564f, -0.53772387f, 0.57673654f, 0.61500244f,
		// Atras: Antigua Republica (triangulo 11)
		0.08143200f, -2.51320400f, -2.42600500f, 0.36448326f, 0.68322800f, 0.06410820f, -0.00135916f, 0.99794203f,
		0.04964900f, 2.44012800f, -2.41721700f, 0.57379474f, 0.89695119f, 0.06410820f, -0.00135916f, 0.99794203f,
		2.53494400f, -0.06114600f, -2.58028000f, 0.36076073f, 0.89278751f, 0.06410820f, -0.00135916f, 0.99794203f,
		// Base: logo inferior (triangulo 12)
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, -0.01066270f, 0.99993918f, 0.00281980f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.23977939f, 0.09015456f, -0.01066270f, 0.99993918f, 0.00281980f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, -0.01066270f, 0.99993918f, 0.00281980f,
		// Derecha: Mandalorian Clan (triangulo 13)
		2.63220100f, -2.55389200f, 0.00722600f, 0.98488673f, 0.69418826f, -0.99973246f, -0.02225438f, 0.00630419f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.77165400f, 0.90518855f, -0.99973246f, -0.02225438f, 0.00630419f,
		2.59300300f, -0.08522200f, 2.50575500f, 0.77429810f, 0.69418826f, -0.99973246f, -0.02225438f, 0.00630419f,
		// Superior: Boba Fett (triangulo 14)
		0.04964900f, 2.44012800f, -2.41721700f, 0.15564658f, 0.69749336f, 0.01144543f, -0.99990142f, -0.00813344f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.38384146f, 0.46621343f, 0.01144543f, -0.99990142f, -0.00813344f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.38490407f, 0.69547793f, 0.01144543f, -0.99990142f, -0.00813344f,
		// Superior: Boba Fett (triangulo 15)
		0.04964900f, 2.44012800f, -2.41721700f, 0.15564658f, 0.69749336f, -0.00076554f, -0.99996666f, -0.00813027f,
		-2.45137200f, 2.42175900f, 0.07753800f, 0.15373877f, 0.46621343f, -0.00076554f, -0.99996666f, -0.00813027f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.38384146f, 0.46621343f, -0.00076554f, -0.99996666f, -0.00813027f,
		// Atras: Antigua Republica (triangulo 16)
		0.08143200f, -2.51320400f, -2.42600500f, 0.36448326f, 0.68322800f, -0.02213706f, -0.00191576f, 0.99975311f,
		-2.45398000f, -0.06338800f, -2.47745100f, 0.57732230f, 0.68322800f, -0.02213706f, -0.00191576f, 0.99975311f,
		0.04964900f, 2.44012800f, -2.41721700f, 0.57379474f, 0.89695119f, -0.02213706f, -0.00191576f, 0.99975311f,
		// Derecha: Mandalorian Clan (triangulo 17)
		2.63220100f, -2.55389200f, 0.00722600f, 0.98488673f, 0.69418826f, -0.99962143f, -0.02236112f, 0.01603072f,
		2.53494400f, -0.06114600f, -2.58028000f, 0.99013562f, 0.90960490f, -0.99962143f, -0.02236112f, 0.01603072f,
		2.52120600f, 2.44824100f, 0.06338800f, 0.77165400f, 0.90518855f, -0.99962143f, -0.02236112f, 0.01603072f,
		// Frente: Phoenix (triangulo 18)
		-0.14069600f, -2.60193400f, 2.55856100f, 0.78525585f, 0.66268011f, -0.01744185f, -0.00203325f, -0.99984581f,
		2.59300300f, -0.08522200f, 2.50575500f, 0.79621632f, 0.88196512f, -0.01744185f, -0.00203325f, -0.99984581f,
		0.05115200f, 2.39978100f, 2.54504300f, 0.58618304f, 0.88141437f, -0.01744185f, -0.00203325f, -0.99984581f,
		// Izquierda: Republica (triangulo 19)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.16096842f, 0.89546653f, 0.99915310f, 0.01325560f, 0.03895343f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.16174745f, 0.71201269f, 0.99915310f, 0.01325560f, 0.03895343f,
		-2.64678600f, -0.13904700f, 2.49375700f, 0.34296795f, 0.71201269f, 0.99915310f, 0.01325560f, 0.03895343f,
		// Esquina inferior izquierda atras (triangulo 20)
		-2.45398000f, -0.06338800f, -2.47745100f, 0.24886772f, 0.42288384f, 0.55796169f, 0.58971810f, 0.58387611f,
		0.08143200f, -2.51320400f, -2.42600500f, 0.17182594f, 0.29703232f, 0.55796169f, 0.58971810f, 0.58387611f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.24561066f, 0.29506838f, 0.55796169f, 0.58971810f, 0.58387611f,
		// Esquina inferior izquierda frente (triangulo 21)
		-2.64678600f, -0.13904700f, 2.49375700f, 0.30873604f, 0.40394022f, 0.59524609f, 0.58935323f, -0.54620955f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.24764450f, 0.29815077f, 0.59524609f, 0.58935323f, -0.54620955f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.30918695f, 0.29824413f, 0.59524609f, 0.58935323f, -0.54620955f,
		// Esquina inferior derecha frente: Raziel (triangulo 22)
		2.59300300f, -0.08522200f, 2.50575500f, 0.67514280f, 0.44420556f, -0.53374744f, 0.56659454f, -0.62776134f,
		-0.14069600f, -2.60193400f, 2.55856100f, 0.59705397f, 0.32429472f, -0.53374744f, 0.56659454f, -0.62776134f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.67284152f, 0.32890769f, -0.53374744f, 0.56659454f, -0.62776134f,
		// Esquina inferior derecha atras: Cold Order (triangulo 23)
		2.53494400f, -0.06114600f, -2.58028000f, 0.87936347f, 0.46200386f, -0.57823863f, 0.57659642f, 0.57721457f,
		2.63220100f, -2.55389200f, 0.00722600f, 0.79396312f, 0.31864052f, -0.57823863f, 0.57659642f, 0.57721457f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.87892669f, 0.32094679f, -0.57823863f, 0.57659642f, 0.57721457f,
		// Base: logo inferior (triangulo 24)
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, -0.02023575f, 0.99963300f, 0.01801078f,
		-2.51727400f, -2.57909500f, 0.00211500f, 0.34250140f, 0.12453748f, -0.02023575f, 0.99963300f, 0.01801078f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.30742869f, 0.15614844f, -0.02023575f, 0.99963300f, 0.01801078f,
		// Base: logo inferior (triangulo 25)
		-1.18505500f, -2.53049900f, -1.19826100f, 0.30742869f, 0.15614844f, 0.00552381f, 0.99978906f, 0.01978196f,
		0.08143200f, -2.51320400f, -2.42600500f, 0.27408958f, 0.18846727f, 0.00552381f, 0.99978906f, 0.01978196f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, 0.00552381f, 0.99978906f, 0.01978196f,
		// Base: logo inferior (triangulo 26)
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, 0.00620712f, 0.99997747f, 0.00255518f,
		2.63220100f, -2.55389200f, 0.00722600f, 0.20695100f, 0.12440466f, 0.00620712f, 0.99997747f, 0.00255518f,
		1.38502800f, -2.54947600f, 1.30867700f, 0.23977939f, 0.09015456f, 0.00620712f, 0.99997747f, 0.00255518f,
		// Base: logo inferior (triangulo 27)
		1.38502800f, -2.54947600f, 1.30867700f, 0.23977939f, 0.09015456f, -0.01076212f, 0.99952688f, 0.02881320f,
		-0.14069600f, -2.60193400f, 2.55856100f, 0.27994608f, 0.05723932f, -0.01076212f, 0.99952688f, 0.02881320f,
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, -0.01076212f, 0.99952688f, 0.02881320f,
		// Base: logo inferior (triangulo 28)
		-1.32899500f, -2.57838600f, 1.29783800f, 0.31122270f, 0.09043267f, 0.00551307f, 0.99979468f, 0.01949871f,
		-1.18505500f, -2.53049900f, -1.19826100f, 0.30742869f, 0.15614844f, 0.00551307f, 0.99979468f, 0.01949871f,
		1.34418200f, -2.54257100f, -1.29438700f, 0.24085383f, 0.15866948f, 0.00551307f, 0.99979468f, 0.01949871f,
	};

	MeshModel* holocron = new MeshModel();
	// 84 vertices x 8 componentes = 672 floats; 84 indices.
	holocron->CreateMeshModel(holocron_vertices, holocron_indices,
		sizeof(holocron_vertices) / sizeof(holocron_vertices[0]),
		sizeof(holocron_indices) / sizeof(holocron_indices[0]));
	meshListModel.push_back(holocron);
}

// Las piezas OBJ ya estan centradas en sus puntos de giro.
void DibujarPieza(PiezaRover& pieza, const glm::mat4& matriz, GLuint uniformModel)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matriz));
	pieza.RenderModel();
}
// Matriz padre compartida por el cuerpo, las piezas y el faro del rover.
glm::mat4 MatrizRover()
{
	glm::mat4 raiz(1.0f);
	raiz = glm::translate(raiz, glm::vec3(3.0f + avanceRover, -1.0f, 6.0f));
	raiz = glm::scale(raiz, glm::vec3(0.4f));
	return raiz;
}
// Se conservan la ubicacion, orientacion y escala del avion anterior.
glm::mat4 MatrizAvion()
{
	glm::mat4 raiz(1.0f);
	raiz = glm::translate(raiz, glm::vec3(6.0f, 3.5f + alturaAvion, -2.0f));
	raiz = glm::rotate(raiz, -20.0f * toRadians, glm::vec3(0, 1, 0));
	// Avanza sobre su eje local Z: sigue la direccion de su nariz.
	raiz = glm::translate(raiz, glm::vec3(0, 0, avanceAvion));
	raiz = glm::scale(raiz, glm::vec3(0.55f));
	raiz = glm::translate(raiz, glm::vec3(0, 0, 4.65f));
	return raiz;
}
// ACTIVIDAD: matriz compartida por la pinza y su luz.
glm::mat4 MatrizPinza(const glm::mat4& raiz)
{
	glm::mat4 padre1 = glm::translate(raiz, glm::vec3(1.701f, 5.982f, -1.100f));
	padre1 = glm::rotate(padre1, giroBrazo1 * toRadians, glm::vec3(0, 0, 1));
	glm::mat4 padre2 = glm::translate(padre1, glm::vec3(2.535f, 2.556f, 0));
	padre2 = glm::rotate(padre2, giroBrazo2 * toRadians, glm::vec3(0, 0, 1));
	glm::mat4 padre3 = glm::translate(padre2, glm::vec3(-2.648f, 2.952f, 0.186f));
	return glm::rotate(padre3, giroPinza * toRadians, glm::vec3(0, 0, 1));
}

void DibujarRover(GLuint uniformModel, const glm::mat4& raiz)
{
	DibujarPieza(cuerpoRover, raiz, uniformModel);
	glm::mat4 pieza(1.0f);
	pieza = glm::translate(raiz, glm::vec3(2.100000f, 4.208000f, 3.054000f));
	pieza = glm::rotate(pieza, giroRuedas * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(ruedaDI, pieza, uniformModel);
	pieza = glm::translate(raiz, glm::vec3(2.100000f, 4.208000f, -3.066000f));
	pieza = glm::rotate(pieza, giroRuedas * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(ruedaDD, pieza, uniformModel);
	pieza = glm::translate(raiz, glm::vec3(-3.035000f, 3.284000f, 3.009000f));
	pieza = glm::rotate(pieza, giroRuedas * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(ruedaCI, pieza, uniformModel);
	pieza = glm::translate(raiz, glm::vec3(-3.037000f, 3.284000f, -3.009000f));
	pieza = glm::rotate(pieza, giroRuedas * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(ruedaCD, pieza, uniformModel);
	pieza = glm::translate(raiz, glm::vec3(-3.035000f, 3.284000f, 3.009000f));
	pieza = glm::rotate(pieza, giroRuedas * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(ruedaTI, pieza, uniformModel);
	pieza = glm::translate(raiz, glm::vec3(-3.037000f, 3.284000f, -3.009000f));
	pieza = glm::rotate(pieza, giroRuedas * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(ruedaTD, pieza, uniformModel);
	glm::mat4 padre1 = glm::translate(raiz, glm::vec3(1.701000f, 5.982000f, -1.100000f));
	padre1 = glm::rotate(padre1, giroBrazo1 * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(brazoInferior, padre1, uniformModel);
	glm::mat4 padre2 = glm::translate(padre1, glm::vec3(2.535000f, 2.556000f, 0.000000f));
	padre2 = glm::rotate(padre2, giroBrazo2 * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(brazoCentral, padre2, uniformModel);
	glm::mat4 padre3 = glm::translate(padre2, glm::vec3(-2.648000f, 2.952000f, 0.186000f));
	padre3 = glm::rotate(padre3, giroPinza * toRadians, glm::vec3(0, 0, 1));
	DibujarPieza(pinzaRover, MatrizPinza(raiz), uniformModel);
}

// NAVE: flota sobre el piso. Modelo y luces comparten esta matriz.
glm::mat4 MatrizNave()
{
	glm::mat4 matriz = glm::translate(glm::mat4(1), glm::vec3(-4, 3, -4));
	return glm::scale(matriz, glm::vec3(0.7f));
}
void CrearPlasma()
{
	// Plano vertical: la imagen empieza en Z=0 y sale hacia Z negativo.
	GLfloat vertices[] = {
		-0.6f, 0, 0, 0, 1, 0, 1, 0,
		 0.6f, 0, 0, 1, 1, 0, 1, 0,
		 0.6f, 0,-3, 1, 0, 0, 1, 0,
		-0.6f, 0,-3, 0, 0, 0, 1, 0
	};
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	plasmaMesh.CreateMeshModel(vertices, indices, 32, 6);
}
void DibujarPlasma(GLuint uniformModel, const glm::mat4& matriz)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matriz));
	plasmaMesh.RenderMeshModel();
	// Segundo plano cruzado para que el plasma se vea desde varios angulos.
	glm::mat4 cruzado = glm::rotate(matriz, 90 * toRadians, glm::vec3(0, 0, 1));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(cruzado));
	plasmaMesh.RenderMeshModel();
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();
	CrearPlasma();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	// Si faltan los atlas originales, usar blanco permite comprobar las normales.
	dadoTexture = Texture("Textures/logos_starwars.png");
	if (!dadoTexture.LoadTextureA()) { dadoTexture = Texture("Textures/plain.png"); dadoTexture.LoadTextureA(); }
	holocronTexture = Texture("Textures/holocron_logos2.png");
	if (!holocronTexture.LoadTextureA()) { holocronTexture = Texture("Textures/plain.png"); holocronTexture.LoadTextureA(); }
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	cuerpoRover.LoadModel("Models/Cuerpo_Rover.obj");
	brazoInferior.LoadModel("Models/Brazo_01_Inferior.obj");
	brazoCentral.LoadModel("Models/Brazo_02_Central.obj");
	pinzaRover.LoadModel("Models/Brazo_03_Pinza.obj");
	ruedaDI.LoadModel("Models/Rueda_Delantera_Izquierda.obj");
	ruedaDD.LoadModel("Models/Rueda_Delantera_Derecha.obj");
	ruedaCI.LoadModel("Models/Rueda_Central_Izquierda.obj");
	ruedaCD.LoadModel("Models/Rueda_Central_Derecha.obj");
	ruedaTI.LoadModel("Models/Rueda_Trasera_Izquierda.obj");
	ruedaTD.LoadModel("Models/Rueda_Trasera_Derecha.obj");

	//Anadir modelo del avion
	Avion_M = Model();
	Avion_M.LoadModel("Models/avion.obj");
	Nave_M.LoadModel("Models/Nave_Azul.obj");
	plasmaTexture = Texture("Textures/plasma_azul.png");
	plasmaTexture.LoadTextureA();

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, solo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.6f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaracion de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// ACTIVIDAD: dos luces puntuales adicionales; MAX_POINT_LIGHTS sigue en 3.
	pointLightCount = 3;
	bool luzHolocronEncendida = true, luzPinzaEncendida = true;
	bool antesC = false, antesH = false, antesP = false;
	int indiceColor = 0;
	const glm::vec3 coloresFaro[6] = {
		glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec3(1, 0, 1)
	};

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.05f, 0.01f, // Atenuacion con la distancia.
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, -1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.05f, 0.01f, // Atenuacion con la distancia.
		15.0f);
	spotLightCount++;

	// Tres faros de la practica. Las posiciones se actualizan con sus padres.
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 3.0f, 0, 0, 0, 1, -0.15f, 0,
		1.0f, 0.05f, 0.01f, 25.0f); // Rover: adelante.
	spotLightCount++;
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 3.0f, 0, 0, 0, 0, -1, 0,
		1.0f, 0.05f, 0.01f, 25.0f); // Avion: abajo.
	spotLightCount++;
	spotLights[4] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 3.0f, 0, 0, 0, 0, -0.15f, 1,
		1.0f, 0.05f, 0.01f, 25.0f); // Avion: adelante.
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);



	////Loop mientras no se cierra la ventana
	// NAVE: estados independientes; una accion por pulsacion.
	bool lucesNave = true, motor1 = true, motor2 = true;
	bool antesL = false, antes1 = false, antes2 = false;
	glm::mat4 raizNave = MatrizNave(), plasma1(1), plasma2(1);
	glm::vec3 posLuzNave(0), posFaroNave(0), dirFaroNave(0);
	glm::vec3 posMotor1(0), posMotor2(0);
	DirectionalLight luzTextura(1, 1, 1, 1, 0, 0, -1, 0);
	pointLightCount = 6;
	spotLightCount = 6;

	// Variables del ciclo: se declaran una vez y se actualizan en cada cuadro.
	GLfloat now = 0, paso = 0;
	float sentido = 1;
	bool* teclas = nullptr;
	glm::vec3 lowerLight(0), colorFaro(0), punta(0), color(1);
	glm::mat4 raizRover(1), raizAvion(1), model(1);

	while (!mainWindow.getShouldClose())
	{
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		glfwPollEvents();
		teclas = mainWindow.getsKeys();
		// ACTIVIDAD: una accion por pulsacion, sin repetir mientras se sostiene.
		if (teclas[GLFW_KEY_C] && !antesC) indiceColor = (indiceColor + 1) % 6;
		if (teclas[GLFW_KEY_H] && !antesH) luzHolocronEncendida = !luzHolocronEncendida;
		if (teclas[GLFW_KEY_P] && !antesP) luzPinzaEncendida = !luzPinzaEncendida;
		antesC = teclas[GLFW_KEY_C];
		antesH = teclas[GLFW_KEY_H];
		antesP = teclas[GLFW_KEY_P];
		// NAVE: L controla las luces generales; 1 y 2 controlan los motores.
		if (teclas[GLFW_KEY_L] && !antesL) lucesNave = !lucesNave;
		if (teclas[GLFW_KEY_1] && !antes1) motor1 = !motor1;
		if (teclas[GLFW_KEY_2] && !antes2) motor2 = !motor2;
		antesL = teclas[GLFW_KEY_L];
		antes1 = teclas[GLFW_KEY_1];
		antes2 = teclas[GLFW_KEY_2];
		sentido = teclas[GLFW_KEY_LEFT_SHIFT] ? -1.0f : 1.0f;
		// J/K trasladan el rover; no giran los OBJ de ruedas con soportes.
		giroBrazo1 += 35.0f * (now - lastTime) * sentido * teclas[GLFW_KEY_U];
		giroBrazo2 += 35.0f * (now - lastTime) * sentido * teclas[GLFW_KEY_I];
		giroPinza += 35.0f * (now - lastTime) * sentido * teclas[GLFW_KEY_O];
		// Tiempo real para que la velocidad de los modelos no dependa de los FPS.
		paso = now - lastTime;
		avanceRover += 2.0f * paso * (teclas[GLFW_KEY_J] - teclas[GLFW_KEY_K]);
		avanceAvion += 2.0f * paso * (teclas[GLFW_KEY_M] - teclas[GLFW_KEY_N]);
		alturaAvion += 2.0f * paso * (teclas[GLFW_KEY_V] - teclas[GLFW_KEY_B]);
		lastTime = now;

		//Recibir eventos del usuario

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
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//informacion en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la camara de tipo flash
		//sirve para que en tiempo de ejecucion (dentro del while) se cambien propiedades de la luz
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		raizRover = MatrizRover();
		raizAvion = MatrizAvion();
		// w=1 transforma una posicion; w=0 transforma una direccion sin trasladarla.
		// Se normaliza la direccion para eliminar el efecto de la escala del padre.
		spotLights[2].SetFlash(
			glm::vec3(raizRover * glm::vec4(3.7f, 6.0f, 0, 1)),
			glm::normalize(glm::vec3(raizRover * glm::vec4(1, -0.15f, 0, 0))));
		// ACTIVIDAD: color del faro inferior, manteniendo sus parametros.
		colorFaro = coloresFaro[indiceColor];
		spotLights[3] = SpotLight(colorFaro.r, colorFaro.g, colorFaro.b,
			0, 3, 0, 0, 0, 0, -1, 0, 1, 0.05f, 0.01f, 25);
		spotLights[3].SetFlash(
			glm::vec3(raizAvion * glm::vec4(0, -1.6f, -4.65f, 1)),
			glm::normalize(glm::vec3(raizAvion * glm::vec4(0, -1, 0, 0))));
		spotLights[4].SetFlash(
			glm::vec3(raizAvion * glm::vec4(0, 0, 1.1f, 1)),
			glm::normalize(glm::vec3(raizAvion * glm::vec4(0, -0.15f, 1, 0))));

		// ACTIVIDAD: posicion del Holocron y extremo local de la pinza.
		// Color negro al apagar: elimina tambien el reflejo especular.
		pointLights[1] = PointLight(luzHolocronEncendida ? 0.3f : 0.0f,
			luzHolocronEncendida ? 0.7f : 0.0f, luzHolocronEncendida ? 1.0f : 0.0f,
			// Componente ambiental: azul en todas las caras, con atenuacion.
			luzHolocronEncendida ? 1.5f : 0.0f,
			luzHolocronEncendida ? 4.0f : 0.0f,
			-7, 7, 6, 1, 0.35f, 0.44f);
		// Vertice real del extremo superior de Brazo_03_Pinza.obj.
		// Misma matriz que dibuja la pinza: sigue U, I, O y J/K.
		punta = glm::vec3(MatrizPinza(raizRover) *
			glm::vec4(0.795737f, 0.954305f, -0.024624f, 1));
		pointLights[2] = PointLight(0, luzPinzaEncendida ? 1.0f : 0.0f, 0,
			0, luzPinzaEncendida ? 3.0f : 0.0f,
			punta.x, punta.y, punta.z, 1, 0.35f, 0.44f);

		// NAVE: luces generales y dos luces azules en las salidas de los motores.
		posLuzNave = glm::vec3(raizNave * glm::vec4(0, 1.8f, -2, 1));
		posFaroNave = glm::vec3(raizNave * glm::vec4(0, -0.8f, 2, 1));
		dirFaroNave = glm::normalize(glm::vec3(raizNave * glm::vec4(0, -0.2f, 1, 0)));
		posMotor1 = glm::vec3(raizNave * glm::vec4(-1.03f, 0.18f, -8.5f, 1));
		posMotor2 = glm::vec3(raizNave * glm::vec4(1.03f, 0.18f, -8.5f, 1));
		pointLights[3] = PointLight(0, lucesNave ? 0.5f : 0, lucesNave ? 1.0f : 0,
			0, lucesNave ? 2.0f : 0, posLuzNave.x, posLuzNave.y, posLuzNave.z, 1, 0.2f, 0.1f);
		spotLights[5] = SpotLight(lucesNave ? 1.0f : 0, lucesNave ? 1.0f : 0, lucesNave ? 1.0f : 0,
			0, lucesNave ? 3.0f : 0, posFaroNave.x, posFaroNave.y, posFaroNave.z,
			dirFaroNave.x, dirFaroNave.y, dirFaroNave.z, 1, 0.05f, 0.01f, 25);
		pointLights[4] = PointLight(0, motor1 ? 0.5f : 0, motor1 ? 1.0f : 0,
			0, motor1 ? 3.0f : 0, posMotor1.x, posMotor1.y, posMotor1.z, 1, 0.35f, 0.44f);
		pointLights[5] = PointLight(0, motor2 ? 0.5f : 0, motor2 ? 1.0f : 0,
			0, motor2 ? 3.0f : 0, posMotor2.x, posMotor2.y, posMotor2.z, 1, 0.35f, 0.44f);
		plasma1 = glm::translate(raizNave, glm::vec3(-1.03f, 0.18f, -8.5f));
		plasma2 = glm::translate(raizNave, glm::vec3(1.03f, 0.18f, -8.5f));

		//informacion al shader de fuentes de iluminacion
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		model = glm::mat4(1.0f);

		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();

		//BLACKHAWK
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		plainTexture.UseTexture();
		DibujarRover(uniformModel, raizRover);

		//Dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();

		//Holocron
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, 7.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocronTexture.UseTexture();
		meshListModel[5]->RenderMeshModel();


		// El avion y sus dos faros comparten la misma matriz padre.
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(raizAvion));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		Avion_M.RenderModel();

		// NAVE: color blanco para respetar los colores de las imagenes.
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(raizNave));
		glUniform3f(uniformColor, 1, 1, 1);
		Nave_M.RenderModel();



		//Agave que sucede si lo renderizan antes del helicoptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);

		// NAVE: plasma con alpha y color propio, sin alterar el shader.
		shaderList[0].SetDirectionalLight(&luzTextura);
		shaderList[0].SetPointLights(pointLights, 0);
		shaderList[0].SetSpotLights(spotLights, 0);
		glUniform3f(uniformColor, 1, 1, 1);
		plasmaTexture.UseTexture();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		if (motor1) DibujarPlasma(uniformModel, plasma1);
		if (motor2) DibujarPlasma(uniformModel, plasma2);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
