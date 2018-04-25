#ifndef SHADERSYSTEM_H
#define SHADERSYSTEM_H

#include "Shader.h"
/** Collection class for shader system management, like handles for uniforms etc */
class ShaderSystem {
	Shader shaderLoader;
public:
	/** Initiates the shader system with default values, and pass over neccessary assets handles */
	void initiate();

	/** Enabeling of shaders, and shader variable settings here */
};

#endif