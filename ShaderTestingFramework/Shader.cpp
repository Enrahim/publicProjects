#include "Shader.h"

Shader::Shader()
{
	//glewInit();//#define i glew.h
	//if (glewIsSupported("GL_VERSION_2_0"))
	//	printf("Ready for OpenGL 2.0\n");
	//else {
	//	printf("OpenGL 2.0 not supported\n");
	//	exit(1);
	//}
}

Shader::~Shader()
{
	glDetachShader(program, vertex);
	glDeleteShader(vertex);

	glDetachShader(program, fragment);
	glDeleteShader(fragment);

	glDeleteProgram(program);
}

int Shader::printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
   }
    return retCode;
}

void Shader::printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = new char[infologLength];
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        delete infoLog;
   }
}

void Shader::printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

//	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *) malloc(infologLength);
//        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
        free(infoLog);
   }
}

GLuint Shader::setShader(char *sp)
{
	char *vert = new char[strlen(sp) + 6];
	strcpy(vert, sp);
	strcat(vert, ".vert");
	char *frag = new char[strlen(sp) + 6];
	strcpy(frag, sp);
	strcat(frag, ".frag");

	char *vs = NULL, *fs = NULL;
//#1 : opprette shader container. Retur : handle
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
//Kan ha mange shadere, bare en main for hver type. 

//Lese kildekode :
	vs = textFileRead(vert);
	fs = textFileRead(frag);

	delete vert;
	delete frag;

	const char *vv = vs;
	const char *ff = fs;
//#2 : forberede kompilering
	glShaderSource(vertex, 1, &vv, NULL);
	glShaderSource(fragment, 1, &ff, NULL);

	delete vs;//definert med new i textFileRead(...)
	delete fs;//definert med new i textFileRead(...)

//#3 : kompilering
	glCompileShader(vertex);
	glCompileShader(fragment);

	printShaderInfoLog(vertex);
	printShaderInfoLog(fragment);

//Nå skal shaderprogrammet klargjøres for eksekvering
	program = glCreateProgram();//#1 (kan lage mange programmer som det kan byttes mellom)
	glAttachShader(program, vertex);//#2
	glAttachShader(program, fragment);//#3
	glLinkProgram(program);//#4
	printProgramInfoLog(program);
	//glUseProgram(program);//#5
	return program;
}

char *Shader::textFileRead(char *fn)
{
	std::fstream fp(fn, std::ios::in | std::ios::out | std::ios::binary);
	char *content = 0;
	int count = 0;
	if (fp.is_open()) {
		fp.seekg(0, std::ios::end);
		count = fp.tellg();
		content = new char[count + 1];
		fp.seekg(0, std::ios::beg);
		fp.read(content, count);
		content[count] = '\0';
		fp.close();
	}
	return content;
}

