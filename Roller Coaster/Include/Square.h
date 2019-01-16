
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


class Square : protected QOpenGLFunctions_4_3_Core
{
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector2D> vertices;
	QVector<QVector2D> uvs;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;
	GLuint FramebufferName;
	GLuint renderedTexture;
	int w, h;
public:
	Square();
	void Init(int,int);
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath,QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[],GLfloat target[][4]);
	void Begin();
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix,int shaderType);
	void End();

};