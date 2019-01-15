
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL/QtOpenGL> 
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QFileInfo>
#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>
#include <QString>

#define MAX_WAVE 8
struct Wave
{
	GLfloat waveLength[MAX_WAVE];
	GLfloat amplitude[MAX_WAVE];
	GLfloat speed[MAX_WAVE];
	QVector2D direction[MAX_WAVE];
};

class waterMesh
{
public:
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector2D> uvs;
	QVector<GLuint> indices;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	QOpenGLBuffer* vebo;
	Wave waves;
	GLint count_wave = 0;
	GLfloat time = 0;
	QVector3D location;
	int width, length;
	GLfloat* ProjectionMatrix;
	GLfloat* ModelViewMatrix;
	QOpenGLTexture* skycube;
	QOpenGLTexture* normalmap1;
	QOpenGLTexture* normalmap2;
	QOpenGLTexture* heightmap;

public:
	waterMesh(QVector3D location, int width, int length);
	void Init();
	void InitVAO();
	void InitVBO();
	void InitEBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void InitWave();
	void InitTexture();
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void SetProjectionMatrix(GLfloat* ProjectionMatrix);
	void SetModelViewMatrix(GLfloat* ModelViewMatrix);
	void SetTexture(QOpenGLTexture* skybox, QOpenGLTexture* normalmap1, QOpenGLTexture* normalmap2, QOpenGLTexture* heightmap);
	void Paint(bool isNormalmap = 0);

	void AddSineWave(GLfloat waveLength, GLfloat amplitude, GLfloat speed, QVector2D direction);
};