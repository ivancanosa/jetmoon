#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <assert.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>

#include "GLUtils.hpp"

GLuint generateMonocolorTexture(glm::vec4 color){
	auto[_, textureId] = glGenerateTexture(1, 1);
	GLubyte texData[] = { (GLubyte)color[0], (GLubyte)color[1], (GLubyte)color[2], (GLubyte)color[3] };
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

std::tuple<GLuint,GLuint> glGenerateTexture(int width, int height, bool genDepht){
	assert(width > 0 && "The generated framebuffer width is equal or below 0");
	assert(height > 0 && "The generated framebuffer height is equal or below 0");
	GLint actualFrameBuffer{};
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &actualFrameBuffer);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  
	//
	// generate texture
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	if(genDepht){
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
		glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8 , width, height);  
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			assert(false && "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Error");
	glBindFramebuffer(GL_FRAMEBUFFER, actualFrameBuffer);  
	return {framebuffer, texColorBuffer};
}

std::tuple<GLuint,GLuint,GLuint> glGenerateTexture2(int width, int height, bool genDepht){
	assert(width > 0 && "The generated framebuffer width is equal or below 0");
	assert(height > 0 && "The generated framebuffer height is equal or below 0");
	GLint actualFrameBuffer{};
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &actualFrameBuffer);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  
	//
	// generate texture
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int texColorBuffer2;
	glGenTextures(1, &texColorBuffer2);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_INTEGER, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texColorBuffer2, 0);

	if(genDepht){
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
		glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8 , width, height);  
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			assert(false && "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	}
	glEnable (GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Error");
	glBindFramebuffer(GL_FRAMEBUFFER, actualFrameBuffer);  
	return {framebuffer, texColorBuffer, texColorBuffer2};
}


//This returns VBO, VAO EBO
std::tuple<GLuint, GLuint, GLuint> glGenerateSprite(float xo, float yo){
	GLuint VAO, VBO, EBO;
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f+xo,  0.5f+yo, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f+xo, -0.5f+yo, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f+xo, -0.5f+yo, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f+xo,  0.5f+yo, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {  
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return {VAO, VBO, EBO};
}

GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path, const char* geometry_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryShader = 0;
	if(geometry_file_path != nullptr){
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	}

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	// Read the Geometry Shader code from the file
	std::string GeometryShaderCode;
	if(geometryShader != 0){
		std::ifstream GeometryShaderStream(geometry_file_path, std::ios::in);
		if(GeometryShaderStream.is_open()){
			std::stringstream sstr;
			sstr << GeometryShaderStream.rdbuf();
			GeometryShaderCode = sstr.str();
			GeometryShaderStream.close();
		}
	}


	GLint Result = GL_FALSE;
	int InfoLogLength;
	

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Compile Geometry Shader
	if(geometryShader != 0){
		printf("Compiling shader : %s\n", geometry_file_path);
		char const * GeometrySourcePointer = GeometryShaderCode.c_str();
		glShaderSource(geometryShader, 1, &GeometrySourcePointer , NULL);
		glCompileShader(geometryShader);
	}

	// Check Geometry Shader
	if(geometryShader != 0){
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> GeometryShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(geometryShader, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
			printf("%s\n", &GeometryShaderErrorMessage[0]);
		}
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	if(geometryShader != 0){
		glAttachShader(ProgramID, geometryShader);
	}
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
std::tuple<GLuint, GLuint, GLuint> glGenerateUnityVertex(){
	GLuint VAO, VBO, EBO;
	float vertices[] = {
		// positions         
		 1.f,  1.f, 0.0f,		1.f, 1.f,
		 1.f,  -1.f, 0.0f,		1.f, 0.f,
		 -1.f,  -1.f, 0.0f,		0.f, 0.f,
		 -1.f,  1.f, 0.0f,		0.f, 1.f
	};
	unsigned int indices[] = {  
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	return {VAO, VBO, EBO};
}

GLuint loadComputeShader(const char * computeShaderPath){
	GLuint shaderId = glCreateShader(GL_COMPUTE_SHADER);

	// Read the Compute Shader code from the file
	std::string computerShaderCode;
	std::ifstream shaderStream(computeShaderPath, std::ios::in);
	if(shaderStream.is_open()){
		std::stringstream sstr;
		sstr << shaderStream.rdbuf();
		computerShaderCode = sstr.str();
		shaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", computeShaderPath);
		getchar();
		return 0;
	}

	char const * computerShaderPointer = computerShaderCode.c_str();
	glShaderSource(shaderId, 1, &computerShaderPointer, NULL);
	glCompileShader(shaderId);

	// check compilation errors
	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> shaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
	}


	GLuint computerShaderProgram = glCreateProgram();
	glAttachShader(computerShaderProgram, shaderId);
	glLinkProgram(computerShaderProgram);

	//Check link errors
	glGetProgramiv(computerShaderProgram, GL_LINK_STATUS, &Result);
	glGetProgramiv(computerShaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(computerShaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

//	glDetachShader(computerShaderProgram, shaderId);
//	glDeleteShader(shaderId);

	return computerShaderProgram;
}
