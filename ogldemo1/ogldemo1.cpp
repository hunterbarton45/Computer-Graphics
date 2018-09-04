/**
*
* Author: Hunter Barton
*/


/**
* @file ctxoglsample.cpp
* A sample program demonstrating the use of the ctxogl classes.
*/

#include <exception>
#include <stdio.h>
#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <iostream>
#include "host.h"

using namespace ctxogl;

//start of making good graphics
//struct for shape
//Array of Structs
struct  Cube {
	GLfloat vertices[64] = {
		//front face
		-1.f, 1.f, 1.f, //z-value is closer to you.  //0
		1.f, 1.f, 0.f, //0 yellow
		-1.f, 1.f, //0 st coordinates

		1.f, 1.f, 1.f,   //1
		1.f, 0.f, 0.f, //1 red
		0.f, 1.f, //1 st

		1.f, 1.f, -1.f, //2
		1.f, 0.f, 0.f, //2 red
		0.f, 0.f, //2 st
		
		-1.f, 1.f, -1.f,  //3
		0.f, 0.f, 0.f, //3 black
		1.f, 1.f, //3 st
		
		-1.f, -1.f, 1.f,  //4
		0.f, 1.f, 1.f, //4 cyan
		1.f, -1.f, //4 st

		1.f, -1.f, 1.f,  //5
		0.f, 0.f, 1.f, //5 blue
		1.f, -1.f, //5 st
		
		1.f, -1.f, -1.f, //6
		1.f, 0.f, 1.f, //6 cyan
		0.f, -1.f, //6 st
		
		-1.f, -1.f, -1.f,   //7
		0.f, 1.f, 0.f, //7 green
		-1.f, 1.f, //7 st	
	};

	GLushort indeces[36] = { //this makes triangles from the vertex's 
		0,1,3,	1,2,3,	0,4,5,	5,1,0,	1,5,6,	1,6,2,	4,0,7,	0,3,7,	3,6,7,	3,2,6,	7,6,4,	4,6,5
	};
} shape;

const GLubyte coolTexture[] = {
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00,
};


static bool rotCam, toggle, toggle2, objRot; //camera rotate, 
static int frame, objectRotation; // define all vars
static float zoom, updown;
static int x = 0;
static float y = 0;
static bool transparency; //not used for this project anymore
static float numtransparency; //not used for this project anymore

//initialization of GL unsigned ints
GLuint indBufferID, vertBufferID, textBufferID, vertShaderID, basShadProgramID, textShadProgramID, colorTextShadProgramID, modelMatrixLoc, myTextureID, alphaLoc;

//vertex shader source code
//Why are all these strings? Find out.
static const GLchar* vertShaderSource = //start of writing shader
"#version 410\r\n\n" //does teh version # matter?
"in layout(location = 0) vec3 inPosition;\n" //taking first postion data
"in layout(location = 1) vec3 inColor;\n"
"in layout(location = 2) vec2 vertST;\n"
"uniform mat4 ModelViewProjectionMatrix;\n\n"
"out vec3 vertColor;\n"
"out vec2 ST;\n\n"
"void main() {\n" //main here ***********
"	gl_Position = ModelViewProjectionMatrix * vec4(inPosition, 1.0);\n" //the 1.0 is the w coordinate in homogeneous space.
"	vertColor = inColor;\n" "	ST = vertST;\n"
"}";

//start of fragment shader source code
static const GLchar* fragShadSource =
"#version 410\r\n\n"
"in vec3 vertColor; \n\n"
"out vec4 outColor;\n\n"
"void main() {\n"
"	outColor = vec4(vertColor, 1.0);\n"
"}";

//start of texture shader source code
static const GLchar* textShadSource =
"#version 410\r\n\n"
"in vec2 ST; \n\n"
"out vec4 outColor;\n\n"
"uniform sampler2D texSampler;"
"uniform float alpha;\n"
"void main() {\n"
"outColor = vec4(texture(texSampler, ST).rgb, mod(alpha, 0.5));\n"
"}";

//start of colored shader source code
static const GLchar* colorTextShadSource =
"#version 410\r\n\n"
"in vec3 vertColor; \n"
"in vec2 ST; \n\n"
"out vec4 outColor;\n\n"
"uniform sampler2D texSampler;\n"
"void main() {\n"
"	outColor = vec4(vertColor, 2.0) * texture(texSampler, ST);\n"
"}";


static void draw(unsigned frameCounter, float aspectRatio)
{
	
	if (rotCam)
		frame++;

	if (objRot) {
		objectRotation++;
	}

	glRotatef(objectRotation, 0, -1, 0);
	
	//view and projection matrix setup
	glm::mat4 projMatrix = glm::perspective(
		glm::radians(25.0f),    // Field of view in rad
		aspectRatio,            // Aspect ratio of drawing surface
		0.1f,                   // Z near clip plane
		300.0f);                 // Z far clip plane
	glm::mat4 viewMatrix = glm::lookAt(
		//ask about this shit
		glm::vec3(zoom*sin(frame / 50.0), updown, zoom*cos(frame / 50.0)), // this is the camera position in world coordinates
		glm::vec3(0, 0, 0), // this is the camera target, in world coordinates
		glm::vec3(0, 1, 0)  // this is the up vector in world coordinates
	);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 modelViewMatrix = projMatrix * viewMatrix * modelMatrix;

	glMatrixMode(GL_PROJECTION); glLoadMatrixf(&projMatrix[0][0]);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&modelViewMatrix[0][0]);

	// Enable standard alpha transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Clear the drawing surface
	glClearColor(.5f, .5f, .5f, .5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND); //these make alpha work
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	static bool oneTime = false;
	if (!oneTime) { //do once 

		alphaLoc = glGetUniformLocation(textShadProgramID, "offset");
		modelMatrixLoc = glGetUniformLocation(basShadProgramID, "ModelViewProjectionMatrix");
		glEnable(GL_DEPTH_TEST);

		rotCam = true; //all of these just as they sound like
		frame = 0;
		zoom = 15;
		updown = 5;
		toggle = false;
		objRot = false;

		// beginning VAO stuff
		glGenBuffers(1, &vertBufferID); //1 is the # of buffers and its addressed to my vertex Buffer ID
		glBindBuffer(GL_ARRAY_BUFFER, vertBufferID); //binds this as the current buffer.
		glBufferData(GL_ARRAY_BUFFER, sizeof(shape.vertices), shape.vertices, GL_STATIC_DRAW); //type is the first param, second param size of struct, third param starting point, 4th param is drawing method 

		glGenBuffers(1, &indBufferID); //1 is the # of buffers and its addressed to my indexed Buffer ID
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufferID); //binds this as the current buffer.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shape.indeces), shape.indeces, GL_STATIC_DRAW); //type is the first param, second param size of struct, third param starting point, 4th param is drawing method 

		//same as before
		glGenTextures(1, &myTextureID); glBindTexture(GL_TEXTURE_2D, myTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_BGR, GL_UNSIGNED_BYTE, coolTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glEnableVertexAttribArray(0); glEnableVertexAttribArray(1); glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
		// end  of VAO stuff

		
		//start of compiling and linker code
		//All of this is pretty much the same.
		//All this code deals with compiling.
		//compiling vertex shader
		vertShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertShaderID, 1, &vertShaderSource, nullptr);
		glCompileShader(vertShaderID);
		GLint compStatus = GL_FALSE;
		glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &compStatus);
		if (!compStatus) {
			//compiling shader failed code
			char information[4096]; //these are local variables
			glGetShaderInfoLog(vertShaderID, sizeof(information), NULL, information);
			fprintf(stderr, "Vertex shader compiliation failed:\n%s\n", information);
		}

		//regular fragment shader
		static GLuint basShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(basShaderID, 1, &fragShadSource, nullptr);
		glCompileShader(basShaderID);
		compStatus = GL_FALSE;
		glGetShaderiv(basShaderID, GL_COMPILE_STATUS, &compStatus);
		if (!compStatus) {
			//compiling shader failed code
			char information[4096];
			glGetShaderInfoLog(basShaderID, sizeof(information), NULL, information);
			fprintf(stderr, "regular shader compiliation failed:\n%s\n", information);
		}

		//texture fragment shader
		static GLuint textShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(textShaderID, 1, &textShadSource, nullptr);
		glCompileShader(textShaderID);
		compStatus = GL_FALSE;
		glGetShaderiv(textShaderID, GL_COMPILE_STATUS, &compStatus);
		if (!compStatus) {
			//compiling shader failed code
			char information[4096];
			glGetShaderInfoLog(textShaderID, sizeof(information), NULL, information);
			fprintf(stderr, "texture shader compiliation failed:\n%s\n", information);
		}

		
		//colored texture fragment shader
		static GLuint colorTextShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(colorTextShaderID, 1, &colorTextShadSource, nullptr);
		glCompileShader(colorTextShaderID);
		compStatus = GL_FALSE;
		glGetShaderiv(colorTextShaderID, GL_COMPILE_STATUS, &compStatus);
		if (!compStatus) {
			//compiling shader failed code
			char information[4096];
			glGetShaderInfoLog(colorTextShaderID, sizeof(information), NULL, information);
			fprintf(stderr, "coloredTexture shader compiliation failed:\n%s\n", information);
		}

		//Linking starts here
		basShadProgramID = glCreateProgram();
		glAttachShader(basShadProgramID, vertShaderID);
		glAttachShader(basShadProgramID, basShaderID);
		glLinkProgram(basShadProgramID);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(basShadProgramID, GL_LINK_STATUS, &linkStatus);
		if (!linkStatus) {
			//Shader linking failed code
			char information[4096];
			glGetProgramInfoLog(basShadProgramID, sizeof(information), NULL, information);
			fprintf(stderr, "Basic shader linking failed:\n%s\n", information);
		}

		textShadProgramID = glCreateProgram(); //need this for #2 to work
		glAttachShader(textShadProgramID, vertShaderID);
		glAttachShader(textShadProgramID, textShaderID);
		glLinkProgram(textShadProgramID);
		linkStatus = GL_FALSE;
		glGetProgramiv(textShadProgramID, GL_LINK_STATUS, &linkStatus);
		if (!linkStatus) {
			//Shader linking failed code
			char information[4096];
			glGetProgramInfoLog(textShadProgramID, sizeof(information), NULL, information);
			fprintf(stderr, "texture shader linking failed\n%s\n", information);
		}

		colorTextShadProgramID = glCreateProgram(); //need this for #3 to work
		glAttachShader(colorTextShadProgramID, vertShaderID);
		glAttachShader(colorTextShadProgramID, colorTextShaderID);
		glLinkProgram(colorTextShadProgramID);
		linkStatus = GL_FALSE;
		glGetProgramiv(colorTextShadProgramID, GL_LINK_STATUS, &linkStatus);
		if (!linkStatus) {
			//Shader linking failed code
			char information[4096];
			glGetProgramInfoLog(colorTextShadProgramID, sizeof(information), NULL, information);
			fprintf(stderr, "coloredTexture shader linking failed:\n%s\n", information);
		}

		glUseProgram(basShadProgramID); oneTime = true;
	}

	if (!toggle) {
		y = .9999;
	}

	alphaLoc = glGetUniformLocation(textShadProgramID, "alpha"); modelMatrixLoc = glGetUniformLocation(basShadProgramID, "ModelViewProjectionMatrix");

	if (alphaLoc != -1) {
		glUniform1f(alphaLoc, y);
		y += .01;
	}

	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelViewMatrix[0][0]);

	//magic happens here
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	//Code from 1st project
	/*
	glPushMatrix();
	//glTranslatef(3.0f, -1.0f, -4.0f);
	glRotatef(0, 0, 1, 1);
	glRotatef(objectRotation, 0, 1, 0);
	// White side - front
	glBegin(GL_QUADS); { //Cube
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);

	// Purple side - RIGHT
	glColor4f(1.0, 0.0, 1.0, 1.0);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);

	// Green side - LEFT
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);

	// Blue side - TOP
	glColor4f(0.0, 0.0, 1.0, 1.0);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, 0.5);

	// Red side - BOTTOM
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, -0.5);

	// black side - BACK
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	}
	glEnd();
	glPopMatrix();



	glPushMatrix();
	glTranslatef(-2.0f, 2.0f, 2.0f);
	glRotatef(90, 0, 1, 1);


	glBegin(GL_POLYGON); { //**********Pyramid***********
	// Black side - Bottom
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	// White side - BACK
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//right side
	glColor3f(0.5, 0.0, 0.5);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//Front Side
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//left side
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-3.5f, -1.0f, -2.0f);

	//Top Part of Octihedron
	glRotatef(objectRotation, 0, 1, 0);
	glBegin(GL_TRIANGLES); { //Top back triangle
	glColor3f(255.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glColor3f(0.0, 255.0, 0.0);
	glVertex3f(1.0, -1.0, -1.0);
	glColor3f(0.0, 0.0, 255.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//top right triangle
	glColor3f(0.0, 255.0, 0.0);
	glVertex3f(1.0, -1.0, -1.0);
	glColor3f(0.0, 0.0, 255.0);
	glVertex3f(1.0, -1.0, 1.0);
	glColor3f(255.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//Front Side top triangle
	glColor3f(0.0, 0.0, 255.0);
	glVertex3f(1.0, -1.0, 1.0);
	glColor3f(255.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glColor3f(0.0, 255.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//left side top triangle
	glColor3f(255.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glColor3f(0.0, 255.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glColor3f(0.0, 0.0, 255.0);
	glVertex3f(0.0, 1.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	//Bottom Half Octihedron
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(0.0, -2.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(0.0, -2.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(0.0, -2.0, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLES); {
	glColor3f(0.5, 1.0, 0.5);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(0.0, -2.0, 0.0);
	}
	glEnd();
	glPopMatrix();

	/*
	glPushMatrix();
	glRotatef(0, 0, 1, 1);

	//beginning of red tranparent
	glBegin(GL_QUADS); { //wall
	glColor4f(1.0, 0.0, 0.0, numtransparency);
	glVertex3f(4.0, -4.0, 4.0);
	glVertex3f(4.0, 4.0, 4.0);
	glVertex3f(-4.0, 4.0, 4.0);
	glVertex3f(-4.0, -4.0, 4.0);

	glVertex3f(4.0, -4.0, -4.0);
	glVertex3f(4.0, 4.0, -4.0);
	glVertex3f(4.0, 4.0, 4.0);
	glVertex3f(4.0, -4.0, 4.0);
	}
	glEnd();
	glPopMatrix();
	*/ 


}

/**
* This is the keyboard callback function.  It is called when the user presses a key while the
* drawing window has focus.
*/
static void keyboard(unsigned char key, int x, int y, bool shift, bool ctrl)
{
	if (key == 'r')
		rotCam = !rotCam;

	if (key == 'R')
		objRot = !objRot;

	if ((key == 'w') && (updown < 30))
		updown += .5;

	if ((key == 's') && (updown > -30))
		updown -= .5;

	if ((key == '-') && (zoom < 25))
		zoom += .5;

	if ((key == '=') && (zoom < 25))
		zoom -= .5;

	if (key == '1') {
		glUseProgram(basShadProgramID); toggle2 = toggle = false;
	}

	if (key == '2') {
		glUseProgram(colorTextShadProgramID); toggle2 = toggle = false;
	}

	if (key == '3') {
		glUseProgram(textShadProgramID); toggle2 = toggle = false;
	}
	
	if (key == '?') {
		toggle = !toggle; toggle2 = false;
	}

	if (key == '0') 
		abort();
	
	//printf("Key '%c' pressed at %d,%d (shift=%d, ctrl=%d)\n", key, x, y, shift, ctrl);
}

int main(int argc, char** argv)
{
	static const unsigned WINDOW_WIDTH = 1000;
	static const unsigned WINDOW_HEIGHT = 1000;

	try
	{
		Host host(WINDOW_WIDTH, WINDOW_HEIGHT, "CTX OGL Sample");

		host.start(draw, keyboard);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, "Exception caught: %s\n", e.what());
		return 1;
	}
	catch (...)
	{
		fprintf(stderr, "Unknown exception caught\n");
		return 1;
	}

	return 0;
}