#include "ShaderLoader.h" 
#include<iostream>
#include<fstream>	
#include<vector>

ShaderLoader::ShaderLoader(void) {}
ShaderLoader::~ShaderLoader(void) {}
std::unordered_map<GLuint, std::vector<ShaderPropertyInfo>> ShaderLoader::materialPropertiesByProgram;

GLuint ShaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{

	
	// Create the shaders from the filepath
	GLuint vertexShaderID = CreateShader(GL_VERTEX_SHADER, vertexShaderFilename);
	GLuint fragmentShaderID = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

	// Create the program handle, attach the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShaderID);
	glAttachShader(program, fragmentShaderID);
	glLinkProgram(program);



	// Check for link errors
	int link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		std::string programName = vertexShaderFilename + *fragmentShaderFilename;
		PrintErrorDetails(false, program, programName.c_str());
		
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		glDeleteProgram(program);

		return 0;
	}
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	
	ReflectMaterialProperties(program);
	return program;
}
GLuint ShaderLoader::CreateComputeProgram(const char* computeShaderFilename)
{
	GLuint computeShaderID = CreateShader(GL_COMPUTE_SHADER, computeShaderFilename);

	GLuint program = glCreateProgram();
	glAttachShader(program, computeShaderID);
	glLinkProgram(program);

	int link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		PrintErrorDetails(false, program, computeShaderFilename);
		return 0;
	}
	glDeleteShader(computeShaderID);

	return program;
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, const char* shaderName)
{
	// Read the shader files and save the source code as strings
	std::string shaderSourceCode = ReadShaderFile(shaderName);

	// Create the shader ID and create pointers for source code string and length
	GLuint shaderID = glCreateShader(shaderType);
	const char* shader_code_ptr = shaderSourceCode.c_str();
	const int shader_code_size = (int)shaderSourceCode.size();


	// Populate the Shader Object (ID) and compile
	glShaderSource(shaderID, 1, &shader_code_ptr, &shader_code_size);
	glCompileShader(shaderID);


	// Check for errors
	int compile_result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
	if (compile_result == GL_FALSE)
	{
		
		PrintErrorDetails(true, shaderID, shaderName);
		return 0;
	}
	return shaderID;
}

std::string ShaderLoader::ReadShaderFile(const char* filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}
const std::vector<ShaderPropertyInfo>& ShaderLoader::GetMaterialProperties(GLuint program)
{
    static std::vector<ShaderPropertyInfo> emptyProperties;

    std::unordered_map<GLuint, std::vector<ShaderPropertyInfo>>::iterator iterator =
        materialPropertiesByProgram.find(program);

    if (iterator == materialPropertiesByProgram.end())
    {
        return emptyProperties;
    }

    return iterator->second;
}

void ShaderLoader::ReflectMaterialProperties(GLuint program)
{
    std::vector<ShaderPropertyInfo> reflectedProperties;

    GLint uniformCount = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

    GLint maxUniformNameLength = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

    if (uniformCount <= 0 || maxUniformNameLength <= 0)
    {
        materialPropertiesByProgram[program] = reflectedProperties;
        return;
    }

    std::vector<char> uniformNameBuffer;
    uniformNameBuffer.resize(static_cast<size_t>(maxUniformNameLength));

    for (GLint uniformIndex = 0; uniformIndex < uniformCount; uniformIndex++)
    {
        GLsizei uniformNameLength = 0;
        GLint uniformSize = 0;
        GLenum uniformType = 0;

        glGetActiveUniform(
            program,
            uniformIndex,
            maxUniformNameLength,
            &uniformNameLength,
            &uniformSize,
            &uniformType,
            uniformNameBuffer.data()
        );

        std::string uniformName = uniformNameBuffer.data();

        if (uniformName.size() >= 3)
        {
            std::string suffix = uniformName.substr(uniformName.size() - 3);

            if (suffix == "[0]")
            {
                uniformName = uniformName.substr(0, uniformName.size() - 3);
            }
        }

        if (ShouldExposeUniformAsMaterialProperty(uniformName) == false)
        {
            continue;
        }

        ShaderPropertyType propertyType = ConvertOpenGLUniformType(uniformType, uniformName);

        if (propertyType == ShaderPropertyType::Unknown)
        {
            continue;
        }

        GLint location = glGetUniformLocation(program, uniformName.c_str());

        ShaderPropertyInfo propertyInfo;
        propertyInfo.name = uniformName;
        propertyInfo.type = propertyType;
        propertyInfo.location = location;
        propertyInfo.arraySize = uniformSize;

        reflectedProperties.push_back(propertyInfo);
    }

    materialPropertiesByProgram[program] = reflectedProperties;
}

bool ShaderLoader::ShouldExposeUniformAsMaterialProperty(const std::string& uniformName)
{
    if (uniformName.empty())
    {
        return false;
    }

    if (uniformName[0] != '_')
    {
        return false;
    }

    return true;
}

ShaderPropertyType ShaderLoader::ConvertOpenGLUniformType(
    GLenum uniformType,
    const std::string& uniformName
)
{
    switch (uniformType)
    {
    case GL_FLOAT:
        return ShaderPropertyType::Float;

    case GL_INT:
        return ShaderPropertyType::Int;

    case GL_BOOL:
        return ShaderPropertyType::Bool;

    case GL_FLOAT_VEC2:
        return ShaderPropertyType::Vec2;

    case GL_FLOAT_VEC3:
        return ShaderPropertyType::Vec3;

    case GL_FLOAT_VEC4:
    {
        if (uniformName.find("Color") != std::string::npos ||
            uniformName.find("Colour") != std::string::npos)
        {
            return ShaderPropertyType::Color;
        }

        return ShaderPropertyType::Vec4;
    }

    case GL_SAMPLER_2D:
        return ShaderPropertyType::Texture2D;

    default:
        return ShaderPropertyType::Unknown;
    }
}

void ShaderLoader::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader == true) ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength) : glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> log(infoLogLength);

	// Retrieve the log info and populate log variable
	(isShader == true) ? glGetShaderInfoLog(id, infoLogLength, NULL, &log[0]) : glGetProgramInfoLog(id, infoLogLength, NULL, &log[0]);
	std::cout << "Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
	std::cout << &log[0] << std::endl;
}