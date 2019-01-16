#include "Square.h"

Square::Square()
{
}
void Square::DimensionTransformation(GLfloat source[],GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for(int j=0;j<4;j++)
		for(int k=0;k<4;k++)
		{
			target[j][k] = source[i];
			i++;
		}
}
void Square::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
}
void Square::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,int shaderType)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix,P);
	DimensionTransformation(ModelViewMatrix,MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix",P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix",MV);
	static float t = 0;
	t += 0.03;
	shaderProgram->setUniformValue("shaderType", float(shaderType));
	shaderProgram->setUniformValue("time", t);
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0,GL_FLOAT,0,2,NULL);
	//unbind buffer
	vvbo.release();

	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Enable Attribute 1
	shaderProgram->enableAttributeArray(1);

	shaderProgram->setAttributeArray(1,GL_FLOAT,0,2,NULL);
	uvbo.release();

	//Draw triangles with 4 indices starting from the 0th index

	glDrawArrays(GL_TRIANGLE_FAN,0,vertices.size());
}
void Square::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
}

void Square::Init(int w,int h)
{
	this->w = w;
	this->h = h;
	initializeOpenGLFunctions();
	InitShader("./Shader/Square.vs","./Shader/Square.fs");
	InitVAO();
	InitVBO();
	//InitTexture();
}
void Square::InitVAO()
{
	 // Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Square::InitVBO()
{
	//Set each vertex's position
	vertices << QVector2D(-1, 1)
		<< QVector2D(-1, -1)
		<< QVector2D(1, -1)
		<< QVector2D(1, 1);
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(),vertices.size() * sizeof(QVector3D));

	//Set each vertex's uv
	uvs<<QVector2D(0.0f,1.0f)
		<<QVector2D(0.0f,0.0f)
		<<QVector2D(1.0f,0.0f)
		<<QVector2D(1.0f,1.0f);
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(),uvs.size() * sizeof(QVector2D));

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer. 
	FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The texture we're going to render to 
	glGenTextures(1, &renderedTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture 
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	// Give an empty image to OpenGL ( the last "0" ) 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering. Needed ! 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Depth need to fixed//
	
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	

	// Set "renderedTexture" as our colour attachement #0 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	// Set the list of draw buffers. 
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}
void Square::InitShader(QString vertexShaderPath,QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if(vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if(vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug()<<vertexShaderFile.filePath()<<" can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if(fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if(fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug()<<fragmentShaderFile.filePath()<<" can't be found";
	shaderProgram->link();
}