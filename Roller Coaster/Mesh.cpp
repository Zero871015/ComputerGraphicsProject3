#include "Mesh.h"

Mesh::Mesh()
{
}
void Mesh::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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
void Mesh::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
	vebo->bind();
}
void Mesh::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix)
{
	time += 0.01;
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

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
}
void Mesh::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	vebo->release();
	shaderProgram->release();
}

void Mesh::Init()
{
	InitShader("./Shader/water.vs", "./Shader/water.fs");
	InitVAO();
	InitVBO();
	InitEBO();
	InitWave();
	//InitTexture();
}
void Mesh::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Mesh::InitVBO()
{
	//Push vertices
	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 200; j++)
		{
			vertices << QVector3D(i-100, 3, j);
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

}

void Mesh::InitEBO()
{
	//Push index
	for (int i = 0; i < 199; i++)
	{
		for (int j = 0; j < 199; j++)
		{
			//first tri
			indices << i + j * 200
				<< i + j * 200 + 1
				<< i + 1 + (j + 1) * 200;
			//secend tri
			indices << i + j * 200
				<< i + (j + 1) * 200
				<< i + 1 + (j + 1) * 200;
		}
	}
	vebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	vebo->create();
	vebo->bind();
	vebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
	vebo->allocate(indices.constData(), indices.size() * sizeof(GLuint));
}

void Mesh::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void Mesh::InitWave()
{
	for (int i = 0; i < MAX_WAVE; i++)
	{
		this->waves.waveLength[i] = 0;
		this->waves.amplitude[i] = 0;
		this->waves.speed[i] = 0;
		this->waves.direction[i] = QVector2D(1, 0);
	}
	AddSineWave(10, 2, 3, QVector2D(1, 1));
	AddSineWave(9, 1.5, 15, QVector2D(1, 0));
	AddSineWave(3, 1, 20, QVector2D(0, 1));
	AddSineWave(20, 2.5, 5, QVector2D(1, 0.5));
	AddSineWave(6, 2, 10, QVector2D(1.5, 0));
}

void Mesh::AddSineWave(GLfloat waveLength, GLfloat amplitude, GLfloat speed, QVector2D direction)
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