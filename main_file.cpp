/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

///Tutorial files
#include <obj.h>
#include <vbo_indexer.h>
#include <texture_loader.h>
#include <shader.h>
#include <controls.h>
using namespace glm;


///Object class, loading obj and texture files and making them ready for use, good for moving as well as stationary objects
class Object{
    public:
    float position_x=0;
    float position_y=0;
    float position_z=0;


    std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

    GLuint vertexbuffer; ///Vertex buffer identifier
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;

    GLuint texture;
    GLuint textureID;


    Object(GLuint programID, const char * obj_name, const char * texture_name)
    {

        bool res = loadOBJ(obj_name, vertices, uvs, normals); ///loads .obj files

        indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
        // Load it into a VBO

        //GLuint vertexbuffer;
        glGenBuffers(1, &vertexbuffer); ///Generate one buffer and place its index in vertexbuffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW); ///Pass vertices to OpenGL

        //GLuint uvbuffer;
        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

        //GLuint normalbuffer;
        glGenBuffers(1, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

        // Generate a buffer for the indices as well
        //GLuint elementbuffer;
        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

        // Load the texture
        texture = loadDDS(texture_name);

        // Get a handle for our "myTextureSampler" uniform
        textureID  = glGetUniformLocation(programID, "myTextureSampler");

    }


    ~Object(){// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteTextures(1, &texture);
	}

    void Draw()
    {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            /// Set our "myTextureSampler" sampler to use Texture Unit 0
            glUniform1i(textureID, 0);

           // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(
                0,                  // attribute 0, ¿eby pasowaæ do uk³adu w shaderze (layout(location = 0) in vec3 vertexPosition_modelspace;)
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glVertexAttribPointer(
                1,                                // attribute (layout(location = 1) in vec2 vertexUV; //aTexCoords?)
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glVertexAttribPointer(
                2,                                // attribute (layout(location = 2) in vec3 vertexNormal_modelspace;)
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

            // Draw the triangles!
            glDrawElements(
                GL_TRIANGLES,      // mode
                indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
            );


    }



};

void InitOpenGL()
{
	glfwWindowHint(GLFW_SAMPLES, 8); /// 8x antialiasing (wyg³adzanie krawêdzi)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); ///We use OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); ///We use new OpenGL (core profile for shaders)
}


///Globals
int gScrWidth = 1920;
int gScrHeight = 1080;
GLFWwindow* window;

int main(void){

    ///Initialization
    if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}
    InitOpenGL();

    ///Window initialization

    window = glfwCreateWindow( gScrWidth, gScrHeight, "Aquarium Project", glfwGetPrimaryMonitor(), NULL);
    if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}

    glfwMakeContextCurrent(window);

    ///GLEW Initialization
    glewExperimental = true; /// Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, gScrWidth/2, gScrHeight/2);


	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	/// Create and compile our GLSL program from the shaders
	GLuint shaderID = LoadShaders("vshader.glsl", "fshader.glsl");
	GLuint lightShaderID = LoadShaders("light_vshader.glsl", "light_fshader.glsl");

	GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
	GLuint viewMatrixID = glGetUniformLocation(shaderID, "V");
	GLuint modelMatrixID = glGetUniformLocation(shaderID, "M");

    GLuint MatrixID2 = glGetUniformLocation(lightShaderID, "MVP");
	GLuint viewMatrixID2 = glGetUniformLocation(lightShaderID, "V");
	GLuint modelMatrixID2 = glGetUniformLocation(lightShaderID, "M");


	///Loading objects
	Object fish(shaderID, "fish.obj", "fish_texture.dds");

	//Object skull(shaderID, "skull.obj", "skull.dds");

	Object fish2(shaderID, "shark.obj", "shark.dds");

	Object waterbox(shaderID, "Waterbox.obj","background.dds");

	Object grass(shaderID, "Grass_01.obj", "algae.dds");

	Object LightSource(lightShaderID, "Waterbox.obj", "background.dds");

	glUseProgram(shaderID);

	GLuint firstLightID = glGetUniformLocation(shaderID, "LightPosition_worldspace");

    GLuint secondLightID = glGetUniformLocation(shaderID, "Light2Position_worldspace");

    double lastTime = glfwGetTime();
	int nbFrames = 0;



	float position_x=0;
	float position_y=0;
	float position_z=0;
    float angle = 0;
    int i = 0;

    do{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		static double lastTime = glfwGetTime();
        currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        position_x = (cos(deltaTime)*7);
		position_z = (sin(deltaTime)*7);
		angle = fmod(deltaTime,6.29f);


        computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

        glUseProgram(shaderID);

        ///Light1
		glm::vec3 lightPos = glm::vec3(-15,-15,-15);
		glUniform3f(firstLightID, lightPos.x, lightPos.y, lightPos.z);
		///Light2
		glm::vec3 light2Pos = glm::vec3(15,15,15);
		glUniform3f(secondLightID, light2Pos.x, light2Pos.y, light2Pos.z);

		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


		///First fish
        glm::mat4 fishMatrix1 = glm::mat4(1.0);

		///Transformations
        fishMatrix1 = glm::translate(fishMatrix1, vec3(position_x,-1.5*position_y,position_z+3));
        fishMatrix1 = glm::rotate(fishMatrix1,3.14f,vec3(0,0,1));
        fishMatrix1 = glm::scale(fishMatrix1, vec3(1.2, 1.2, 1.2));
		glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * fishMatrix1;

		// Wysy³amy nasze transformacje do aktualnie podpiêtego programu cieniuj¹cego
		// do zmiennej uniform "MVP"
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &fishMatrix1[0][0]);

		fish.Draw();

        ///Second fish
        glm::mat4 fishMatrix2 = glm::mat4(1.0);
        fishMatrix2 = glm::translate(fishMatrix2, vec3(11,4+position_x,11));

		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * fishMatrix2;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &fishMatrix2[0][0]);
        fish.Draw();


        ///Third fish
        glm::mat4 fishMatrix3 = glm::mat4(1.0);
        fishMatrix3 = glm::translate(fishMatrix3, vec3(6+position_x, -3+position_y, 3+position_z));
        fishMatrix3 = glm::rotate(fishMatrix3, angle, vec3(1,1,1));
        MVP2 = ProjectionMatrix * ViewMatrix * fishMatrix3;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &fishMatrix3[0][0]);
        fish.Draw();
		///trawa
		glm::mat4 grassMatrix = glm::mat4(1.0);
		grassMatrix = glm::translate(grassMatrix, vec3(0,-19.5,10));
        grassMatrix = glm::rotate(grassMatrix, 1.0f, vec3(-1,0,0));
        grassMatrix = glm::scale(grassMatrix, vec3(5.0, 5.0, 5.0));
		glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * grassMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &grassMatrix[0][0]);

		grass.Draw();

        ///Shark
        glm::mat4 sharkMatrix = glm::mat4(1.0);
		sharkMatrix = glm::translate(sharkMatrix, vec3(-4,-3,0));
        sharkMatrix = glm::rotate(sharkMatrix, angle, vec3(1, 1, 0));
		glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * sharkMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &sharkMatrix[0][0]);

		fish2.Draw();

		///tło
		glm::mat4 ModelMatrix4 = glm::mat4(1.0);
        ModelMatrix4 = glm::scale(ModelMatrix4, vec3(20, 20, 20));
        glm::mat4 MVP5 = ProjectionMatrix * ViewMatrix * ModelMatrix4;


        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP5[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &ModelMatrix4[0][0]);

		waterbox.Draw();
        ///Œwiec¹ca lampka
		///TUTAJ ZACZYNA DZIA£AÆ DRUGI PROGRAM CIENIUJ¥CY
		glUseProgram(lightShaderID);
		glUniformMatrix4fv(viewMatrixID2, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"


        glm::mat4 ModelMatrix6 = glm::mat4(1.0);
		ModelMatrix6 = glm::translate(ModelMatrix6, vec3(15,15,15));
		ModelMatrix6 = glm::rotate(ModelMatrix6,11.0f,vec3(1,0,0));
		ModelMatrix6 = glm::scale(ModelMatrix6,vec3(0.5,0.5,0.5));
		glm::mat4 MVP6 = ProjectionMatrix * ViewMatrix * ModelMatrix6;

		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP6[0][0]);
		glUniformMatrix4fv(modelMatrixID2, 1, GL_FALSE, &ModelMatrix6[0][0]);

		LightSource.Draw();
		///Koniec Lampki1


        ///Œwiec¹ca lampka2

        glm::mat4 ModelMatrix7 = glm::mat4(1.0);
		ModelMatrix7 = glm::translate(ModelMatrix7, vec3(-15,-15,-15));
		ModelMatrix7 = glm::rotate(ModelMatrix7,11.0f,vec3(1,0,0));//}
		ModelMatrix7 = glm::scale(ModelMatrix7,vec3(0.5,0.5,0.5));
		glm::mat4 MVP7 = ProjectionMatrix * ViewMatrix * ModelMatrix7;

		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP7[0][0]);
		glUniformMatrix4fv(modelMatrixID2, 1, GL_FALSE, &ModelMatrix7[0][0]);

		LightSource.Draw();



		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

    }while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


}
