/// GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; ///Extern jest po to aby dostaæ siê do zmiennej window z maina

/// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.h"
#include <stdio.h>
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

///Scroll do zmiany FoV
double yOffset = 0;

///Pozycja pocz¹tkowa kamery: na +Z
glm::vec3 position = glm::vec3( 0, 10, -10 );
///K¹t poziomy:
float horizontalAngle = 0.0f;
///Pocz¹tkowy k¹t pionowy:
float verticalAngle = -0.9f;
///Pocz¹tkowy Field of View
float initialFoV = 45.0f;//45.0f;

float speed = 3.0f; // 3 jednostki / sekunda
float mouseSpeed = 0.005f;

//Scroll - ma zmieniaæ Field of View
//Deklaracja funkcji, implementacja na dole.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

///Oblicz macierz MVP na podstawie inputu z klawiatury i myszy (co ka¿d¹ klatkê)
///Funkcja poni¿ej odczytuje input z myszy i klawiatury i oblicza macierz projekcji (Projection) i widoku (View)
void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Policz ró¿nicê czasu pomiêdzy obecn¹ i ostatni¹ klatk¹
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Zdob¹dŸ pozycjê myszy
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int w,h; ///Szerokoœæ i wysokoœæ okna
	glfwGetWindowSize(window,&w,&h); ///Odczytaj aktualn¹ szerokoœæ i wysokoœæ okna
	// Zresetuj pozycjê myszy dla nastêpnej klatki
	glfwSetCursorPos(window, w/2, h/2);

	// Oblicz now¹ orientacjê / nowe k¹ty widzenia
	horizontalAngle += mouseSpeed * float(w/2 - xpos ); //w/2 - xpos jak daleko jest mysz od centrum okna, im wiêksza wartoœæ tym bardziej chcemy siê okrêciæ.
	verticalAngle   += mouseSpeed * float(h/2 - ypos ); //MouseSpeed po to aby zwolniæ/przyspieszyæ obracanie.

	// Kierunek : Wspó³rzêdne sferyczne do wspó³rzêdnych Kartezjañskich (konwersja) //Wektor który reprezentuje w przestrzeni œwiata (world space) kierunek w którym patrzymy
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Prawy wektor - je¿eli idziemy na prawo od kamery, to zawsze poziomo
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	// Wektor w górê (prostopad³y do prawego i kierunku)
	glm::vec3 up = glm::cross( right, direction );

	///Pozycja
	// Porusz siê do przodu
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Porusz siê do ty³u
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
	///Delta czasu s³u¿y temu, ¿eby na ka¿dym komputerze wszystko trwa³o tyle samo, a nie, ¿e na lepszym coœ dzieje siê szybciej.

    glfwSetScrollCallback(window, scroll_callback); /// Callback dla kó³ka myszy, na potrzeby zmiany FoV

    if(yOffset > 7.5f)
    {
        yOffset = 7.5f;
    }
    if(yOffset < -22.5f)
    {
        yOffset = -22.5f;
    }
	float FoV = initialFoV - 2 * yOffset;


	// Macierz projekcji (Projection matrix) :  Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Macierz widoku (Camera matrix)
	ViewMatrix       = glm::lookAt(
								position,           // Kamera jest w tej pozycji
								position+direction, // i patrzy tu : w tej samej pozycji, plus "direction"
								up                  // G³owa jest do góry (ustaw na 0,-1,0 to look upside-down)
						   );

	// Dla nastêpnej klatki, "last time" bêdzie "now"
	lastTime = currentTime;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
        yOffset += yoffset;
}

