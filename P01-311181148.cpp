#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <stdlib.h>
#include <time.h>

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;
// Colores de fondo en formato RGB
// Variables para guardar el color actual del fondo
float colorRojo = 0.0f;
float colorVerde = 0.0f;
float colorAzul = 0.0f;

// Cada color se muestra durante 2 segundos.
const double SEGUNDOS_POR_COLOR = 2.0;
// Se inicializa en -2 para generar un color desde el inicio
double tiempoUltimoCambio = -2.0;


//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,0.0f,0.0f,1.0f);	 			\n\
}";



void CrearFiguras()
{
	

	const GLfloat vertices[] = {

		
		// LETRA I esta formada por 3 barras cada una de 2 triangulos
		

		// Barra superior: triángulo 1
		-0.95f,  0.75f, 0.0f,
		-0.47f,  0.75f, 0.0f,
		-0.47f,  0.58f, 0.0f,

		// Barra superior: triángulo 2
		-0.95f,  0.75f, 0.0f,
		-0.47f,  0.58f, 0.0f,
		-0.95f,  0.58f, 0.0f,

		// Barra vertical: triángulo 1
		-0.76f,  0.58f, 0.0f,
		-0.66f,  0.58f, 0.0f,
		-0.66f, -0.58f, 0.0f,

		// Barra vertical: triángulo 2
		-0.76f,  0.58f, 0.0f,
		-0.66f, -0.58f, 0.0f,
		-0.76f, -0.58f, 0.0f,

		// Barra inferior: triángulo 1
		-0.95f, -0.58f, 0.0f,
		-0.47f, -0.58f, 0.0f,
		-0.47f, -0.75f, 0.0f,

		// Barra inferior: triángulo 2
		-0.95f, -0.58f, 0.0f,
		-0.47f, -0.75f, 0.0f,
		-0.95f, -0.75f, 0.0f,


		
		// LETRA B esta formada por 6 barras de 2 triangulos cada una
		

		// Barra vertical izquierda: triángulo 1
		-0.32f,  0.75f, 0.0f,
		-0.22f,  0.75f, 0.0f,
		-0.22f, -0.75f, 0.0f,

		// Barra vertical izquierda: triángulo 2
		-0.32f,  0.75f, 0.0f,
		-0.22f, -0.75f, 0.0f,
		-0.32f, -0.75f, 0.0f,

		// Barra superior: triángulo 1
		-0.22f,  0.75f, 0.0f,
		 0.10f,  0.75f, 0.0f,
		 0.10f,  0.58f, 0.0f,

		 // Barra superior: triángulo 2
		 -0.22f,  0.75f, 0.0f,
		  0.10f,  0.58f, 0.0f,
		 -0.22f,  0.58f, 0.0f,

		 // Barra central: triángulo 1
		 -0.22f,  0.08f, 0.0f,
		  0.10f,  0.08f, 0.0f,
		  0.10f, -0.08f, 0.0f,

		  // Barra central: triángulo 2
		  -0.22f,  0.08f, 0.0f,
		   0.10f, -0.08f, 0.0f,
		  -0.22f, -0.08f, 0.0f,

		  // Barra inferior: triángulo 1
		  -0.22f, -0.58f, 0.0f,
		   0.10f, -0.58f, 0.0f,
		   0.10f, -0.75f, 0.0f,

		   // Barra inferior: triángulo 2
		   -0.22f, -0.58f, 0.0f,
			0.10f, -0.75f, 0.0f,
		   -0.22f, -0.75f, 0.0f,

		   // Barra derecha superior: triángulo 1
			0.10f,  0.58f, 0.0f,
			0.20f,  0.58f, 0.0f,
			0.20f,  0.08f, 0.0f,

			// Barra derecha superior: triángulo 2
			 0.10f,  0.58f, 0.0f,
			 0.20f,  0.08f, 0.0f,
			 0.10f,  0.08f, 0.0f,

			 // Barra derecha inferior: triángulo 1
			  0.10f, -0.08f, 0.0f,
			  0.20f, -0.08f, 0.0f,
			  0.20f, -0.58f, 0.0f,

			  // Barra derecha inferior: triángulo 2
			   0.10f, -0.08f, 0.0f,
			   0.20f, -0.58f, 0.0f,
			   0.10f, -0.58f, 0.0f,


			   
			   // LETRA A formada por 2 diagonales de 2 triangulos  y una barra de 2 triangulos
			  

			   // Barra diagonal izquierda: triángulo 1
				0.34f, -0.75f, 0.0f,
				0.47f, -0.75f, 0.0f,
				0.69f,  0.75f, 0.0f,

				// Barra diagonal izquierda: triángulo 2
				 0.34f, -0.75f, 0.0f,
				 0.69f,  0.75f, 0.0f,
				 0.60f,  0.75f, 0.0f,

				 // Barra diagonal derecha: triángulo 1
				  0.82f, -0.75f, 0.0f,
				  0.95f, -0.75f, 0.0f,
				  0.69f,  0.75f, 0.0f,

				  // Barra diagonal derecha: triángulo 2
				   0.82f, -0.75f, 0.0f,
				   0.69f,  0.75f, 0.0f,
				   0.60f,  0.75f, 0.0f,

				   // Barra central: triángulo 1
					0.46f,  0.06f, 0.0f,
					0.84f,  0.06f, 0.0f,
					0.84f, -0.10f, 0.0f,

					// Barra central: triángulo 2
					 0.46f,  0.06f, 0.0f,
					 0.84f, -0.10f, 0.0f,
					 0.46f, -0.10f, 0.0f
	};
	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gráfica
	//verificaciones y prevención de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}



}

void EstablecerColorDeFondo()
{
	// Obtener el tiempo transcurrido
	double tiempoActual = glfwGetTime();

	// Comprobar si ya pasaron dos segundos
	if (tiempoActual - tiempoUltimoCambio >= SEGUNDOS_POR_COLOR)
	{
		// Generar las tres componentes aleatorias
		colorRojo =
			static_cast<float>(rand()) / RAND_MAX;

		colorVerde =
			static_cast<float>(rand()) / RAND_MAX;

		colorAzul =
			static_cast<float>(rand()) / RAND_MAX;

		// Guardar el momento en que cambió el color
		tiempoUltimoCambio = tiempoActual;
	}

	// Entregar el color generado a OpenGL
	glClearColor(
		colorRojo,
		colorVerde,
		colorAzul,
		1.0f
	);
}

int main()
{
	
	/*
		Iniciar el generador de números aleatorios.

		time(NULL) permite que los colores sean diferentes
		cada vez que se abre el programa.
	*/
	srand(
		static_cast<unsigned int>(
			time(NULL)
			)
	);
	
	
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	//Llamada a las funciones creadas antes del main
	CrearFiguras();
	CompileShaders();


	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		// Establecer color de fondo
		EstablecerColorDeFondo();
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		// Se modifica el numero de vertices que se van a dibujar por un total de 72
		glDrawArrays(GL_TRIANGLES, 0, 72);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA LÍNEA DESPUÉS DE glfwSwapBuffers(mainWindow); 
	}


	return 0;
}