#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Camera;

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	// Los eventos de teclado y los movimientos se resuelven en Window.
	void ActualizarControles(GLfloat paso, GLfloat deltaCamara, Camera& camara);
	bool getLuzHolocron() const { return luzHolocron; }
	bool getLuzPinza() const { return luzPinza; }
	bool getLucesNave() const { return lucesNave; }
	bool getMotor1() const { return motor1; }
	bool getMotor2() const { return motor2; }
	int getColorFaro() const { return colorFaro; }
	GLfloat getGiroBrazo1() const { return brazo1; }
	GLfloat getGiroBrazo2() const { return brazo2; }
	GLfloat getGiroPinza() const { return pinza; }
	GLfloat getAvanceRover() const { return rover; }
	GLfloat getAvanceAvion() const { return avion; }
	GLfloat getAlturaAvion() const { return altura; }
	~Window();
private:
	bool luzHolocron = true, luzPinza = true, lucesNave = true;
	bool motor1 = true, motor2 = true;
	int colorFaro = 0;
	GLfloat brazo1 = 0, brazo2 = 0, pinza = 0;
	GLfloat rover = 0, avion = 0, altura = 0;
	GLFWwindow* mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange = 0;
	GLfloat yChange = 0;
	GLfloat muevex;
	bool mouseFirstMoved = true;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

