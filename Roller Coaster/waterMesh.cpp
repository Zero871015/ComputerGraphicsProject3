#include "waterMesh.h"

waterMesh::waterMesh(QVector3D location, int width, int length)
{
	this->location = location;
	this->width = width;
	this->length = length;
	Init();
}
void waterMesh::DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j<4; j++)
		for (int k = 0; k<4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
}
void waterMesh::SetProjectionMatrix(GLfloat * ProjectionMatrix)
{
	this->ProjectionMatrix = ProjectionMatrix;
}
void waterMesh::SetModelViewMatrix(GLfloat * ModelViewMatrix)
{
	this->ModelViewMatrix = ModelViewMatrix;
}
void waterMesh::SetTexture(QOpenGLTexture* skybox, QOpenGLTexture* normalmap1, QOpenGLTexture* normalmap2, QOpenGLTexture* heightmap)
{
	this->skybox = skybox;
	this->normalmap1 = normalmap1;
	this->normalmap2 = normalmap2;
	this->heightmap = heightmap;
}
void waterMesh::Paint(bool isNormalmap)
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
	vebo->bind();

	glEnable(GL_FRONT_AND_BACK);
	skybox->bind();
	this->shaderProgram->setUniformValue("skybox", 0);
	normalmap1->bind();
	this->shaderProgram->setUniformValue("normalmap1", 0);
	normalmap2->bind();
	this->shaderProgram->setUniformValue("normalmap2", 0);
	heightmap->bind();
	this->shaderProgram->setUniformValue("heightmap", 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	time += 0.01;
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	//pass whether to open Normalmap
	shaderProgram->setUniformValue("isNormalmap", GLfloat(isNormalmap));
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	uvbo.bind();
	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 2, NULL);
	uvbo.release();

	shaderProgram->setUniformValue("time", time);
	shaderProgram->setUniformValue("numWaves", count_wave);
	shaderProgram->setUniformValueArray("amplitude", this->waves.amplitude, MAX_WAVE, 1);
	shaderProgram->setUniformValueArray("wavelength", this->waves.waveLength, MAX_WAVE, 1);
	shaderProgram->setUniformValueArray("speed", this->waves.speed, MAX_WAVE, 1);
	shaderProgram->setUniformValueArray("direction", this->waves.direction, MAX_WAVE);
	QVector3D EyePos(MV[3][0] , MV[3][1] , MV[3][2]);
	shaderProgram->setUniformValue("EyePos", EyePos);
	
	//Draw triangles with 4 indices starting from the 0th index
	//glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	glDisable(GL_BLEND);

	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	vebo->release();
	shaderProgram->release();
}

void waterMesh::Init()
{
	InitShader("./Shader/water.vs", "./Shader/water.fs");
	InitVAO();
	InitVBO();
	InitEBO();
	InitWave();
	//InitTexture();
}
void waterMesh::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void waterMesh::InitVBO()
{
	//Push vertices
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < length; j++)
		{
			vertices << QVector3D(location[0] + i, location[1], location[2] + j);
		}
	}
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));


	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < length; j++)
		{
			uvs << QVector2D(i / width-1.0, j / length-1.0);
		}
	}
	// Create Buffer for uv
	uvbo.create();
	// Bind the buffer so that it is the current active buffer
	uvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	uvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	uvbo.allocate(uvs.constData(), uvs.size() * sizeof(QVector2D));
}

void waterMesh::InitEBO()
{
	//Push index
	for (int i = 0; i < width - 1; i++)
	{
		for (int j = 0; j < length - 1; j++)
		{
			//first tri
			indices << i + j * width
				<< i + j * width + 1
				<< i + 1 + (j + 1) * width;
			//secend tri
			indices << i + j * width
				<< i + (j + 1) * width
				<< i + 1 + (j + 1) * width;
		}
	}
	vebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	vebo->create();
	vebo->bind();
	vebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
	vebo->allocate(indices.constData(), indices.size() * sizeof(GLuint));
}

void waterMesh::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
{
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderProgram->link();
}

void waterMesh::InitWave()
{
	for (int i = 0; i < MAX_WAVE; i++)
	{
		this->waves.waveLength[i] = 0;
		this->waves.amplitude[i] = 0;
		this->waves.speed[i] = 0;
		this->waves.direction[i] = QVector2D(1, 0);
	}
	
	AddSineWave(25, 0.08, 30, QVector2D(1, 1));
	AddSineWave(50, 0.03, 15, QVector2D(1, 0));
	AddSineWave(30, 0.04, 20, QVector2D(0, 1));
	AddSineWave(20, 0.05, 50, QVector2D(1, -0.5));
	AddSineWave(60, 0.2, 10, QVector2D(-1.5, 0));
	
}

void waterMesh::AddSineWave(GLfloat waveLength, GLfloat amplitude, GLfloat speed, QVector2D direction)
{
	this->waves.waveLength[count_wave] = waveLength;
	this->waves.amplitude[count_wave] = amplitude;
	this->waves.speed[count_wave] = speed;
	this->waves.direction[count_wave] = direction;
	count_wave++;
	if (count_wave == MAX_WAVE)
	{
		count_wave = 0;
		qDebug() << "too many wave";
	}
}