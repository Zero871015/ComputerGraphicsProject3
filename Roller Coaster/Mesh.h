
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

class Mesh
{
public:
	QOpenGLShaderProgram * shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<GLuint> indices;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer* vebo;
	Wave waves;
	GLint count_wave = 0;
	GLfloat time = 0;
public:
	Mesh();
	void Init();
	void InitVAO();
	void InitVBO();
	void InitEBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void InitWave();
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);
	void End();

	void AddSineWave(GLfloat waveLength, GLfloat amplitude, GLfloat speed, QVector2D direction);
};