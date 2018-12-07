#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  
#include <QGLWidget> 
#include <QtGui>  
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "Phong.h"
#include "skybox.h"
#include "Mesh.h"

#include "Model.h"
#include "point3d.h"

class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);

	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

	void initializeGL();
	void initializeTexture();


public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene

	int camera;
	int curve;
	int track;
	bool isrun;
	float t_time;
	unsigned int DIVIDE_LINE = 500;
	Pnt3f trainPos;
	Pnt3f front_t;
	Pnt3f cross_t;
	Pnt3f up_t;
	typedef enum {
		spline_Linear = 0,
		spline_CardinalCubic = 1,
		spline_CubicB_spline = 2
	}spline_t;

	Phong *phongtest;
	Triangle* triangle;
	Square* square;
	Skybox* skybox;
	Mesh* water;
	QOpenGLTexture* skycube;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;

};  
#endif // TRAINVIEW_H  