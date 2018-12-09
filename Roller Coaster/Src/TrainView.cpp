#include "TrainView.h"  
#include <time.h>
//#define DEBUG
TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
	
}  
TrainView::~TrainView()  
{}  
void TrainView::initializeGL()
{
	initializeOpenGLFunctions();
	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();

	phongtest = new Phong();
	phongtest->Init();

	skybox = new Skybox();
	skybox->Init();

	water = new Mesh();
	water->Init();

	//Initialize texture 
	initializeTexture();
}
void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/Tupi.bmp"));
#ifdef DEBUG
	const QImage up = QImage("./Textures/up.png").mirrored().convertToFormat(QImage::Format_RGBA8888);
	const QImage down = QImage("./Textures/down.png").mirrored().convertToFormat(QImage::Format_RGBA8888);
	const QImage front = QImage("./Textures/front.png").mirrored().convertToFormat(QImage::Format_RGBA8888);
	const QImage back = QImage("./Textures/back.png").mirrored().convertToFormat(QImage::Format_RGBA8888);
	const QImage right = QImage("./Textures/right.png").mirrored().convertToFormat(QImage::Format_RGBA8888);
	const QImage left = QImage("./Textures/left.png").mirrored().convertToFormat(QImage::Format_RGBA8888);
#else
	const QImage up = QImage("./Textures/skybox_top.png").convertToFormat(QImage::Format_RGBA8888);
	const QImage down = QImage("./Textures/skybox_bottom.png").convertToFormat(QImage::Format_RGBA8888);
	const QImage front = QImage("./Textures/skybox_front.png").convertToFormat(QImage::Format_RGBA8888);
	const QImage back = QImage("./Textures/skybox_back.png").convertToFormat(QImage::Format_RGBA8888);
	const QImage right = QImage("./Textures/skybox_right.png").convertToFormat(QImage::Format_RGBA8888);
	const QImage left = QImage("./Textures/skybox_left.png").convertToFormat(QImage::Format_RGBA8888);
#endif // DEBUG

	
	skycube = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
	skycube->create();
	glBindTexture(GL_TEXTURE_CUBE_MAP, skycube->textureId());


	skycube->setSize(up.width(), up.height(), up.depth());
	skycube->setFormat(QOpenGLTexture::RGBA8_UNorm);
	skycube->allocateStorage();
	skycube->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)left.constBits(), 0);
	skycube->setData(0, 0, QOpenGLTexture::CubeMapPositiveY,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)up.constBits(), 0);
	skycube->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)front.constBits(), 0);
	skycube->setData(0, 0, QOpenGLTexture::CubeMapNegativeX,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)right.constBits(), 0);
	skycube->setData(0, 0, QOpenGLTexture::CubeMapNegativeY,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)down.constBits(), 0);
	skycube->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)back.constBits(), 0);

	skycube->setWrapMode(QOpenGLTexture::ClampToEdge);
	skycube->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	skycube->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

	Textures.push_back(texture);
	Textures.push_back(skycube);
}
void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

void TrainView::paintGL()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0,0,width(),height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,0.3f,0);
	
	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	//Skybox, need to draw first!
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	skybox->Begin();
	Textures[1]->bind();
	skybox->shaderProgram->setUniformValue("skybox", 0);
	skybox->Paint(ProjectionMatrex, ModelViewMatrex);
	skybox->End();
	glEnable(GL_DEPTH_TEST);

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

	//Get modelview matrix
 	glGetFloatv(GL_MODELVIEW_MATRIX,ModelViewMatrex);
	//Get projection matrix
 	glGetFloatv(GL_PROJECTION_MATRIX,ProjectionMatrex);

	
	
	//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
 	//triangle->Paint(ProjectionMatrex,ModelViewMatrex);
    
	//we manage textures by Trainview class, so we modify square's render function
	/*square->Begin();
		//Active Texture
		glActiveTexture(GL_TEXTURE0);
		//Bind square's texture
		Textures[0]->bind();
		//pass texture to shader
		square->shaderProgram->setUniformValue("Texture",0);
		//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
		square->Paint(ProjectionMatrex,ModelViewMatrex);
	square->End();*/
	
	//phongtest->Paint(ProjectionMatrex, ModelViewMatrex);
	water->Begin();
	glEnable(GL_FRONT_AND_BACK);
	Textures[1]->bind();
	skybox->shaderProgram->setUniformValue("skybox", 0);
	water->Paint(ProjectionMatrex, ModelViewMatrex);
	water->End();

	glColor3f(1, 0, 0);
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0){
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}else if (this->camera == 1) 
	{
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	} 
	else if (this->camera == 2)
	{
		glMatrixMode(GL_PROJECTION);
		gluPerspective(120, 1, 1, 200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(this->trainPos.x, this->trainPos.y + 5,
			this->trainPos.z, this->trainPos.x+this->front_t.x,
			this->trainPos.y + this->front_t.y + 5,
			this->trainPos.z + this->front_t.z, this->up_t.x, this->up_t.y + 5, this->up_t.z);
		update();
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
		update();
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void drawCube(bool doingShadows,Pnt3f trainPos, Pnt3f front_t, Pnt3f cross_t, Pnt3f up_t)
{
	if (!doingShadows)glColor3ub(255, 255, 255);
	glBegin(GL_QUADS);
	glVertex3f(trainPos.x + front_t.x, trainPos.y + front_t.y, trainPos.z + front_t.z);
	glVertex3f(trainPos.x - cross_t.x, trainPos.y - cross_t.y, trainPos.z - cross_t.z);
	glVertex3f(trainPos.x - front_t.x, trainPos.y - front_t.y, trainPos.z - front_t.z);
	glVertex3f(trainPos.x + cross_t.x, trainPos.y + cross_t.y, trainPos.z + cross_t.z);
	glEnd(); 
	if (!doingShadows)glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(trainPos.x + front_t.x, trainPos.y + front_t.y, trainPos.z + front_t.z);
	glVertex3f(trainPos.x - cross_t.x, trainPos.y - cross_t.y, trainPos.z - cross_t.z);
	glVertex3f(trainPos.x - cross_t.x + up_t.x, trainPos.y - cross_t.y + up_t.y, trainPos.z - cross_t.z + up_t.z);
	glVertex3f(trainPos.x + front_t.x + up_t.x, trainPos.y + front_t.y + up_t.y, trainPos.z + front_t.z + up_t.z);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(trainPos.x + front_t.x, trainPos.y + front_t.y, trainPos.z + front_t.z);
	glVertex3f(trainPos.x + cross_t.x, trainPos.y + cross_t.y, trainPos.z + cross_t.z);
	glVertex3f(trainPos.x + cross_t.x + up_t.x, trainPos.y + cross_t.y + up_t.y, trainPos.z + cross_t.z + up_t.z);
	glVertex3f(trainPos.x + front_t.x + up_t.x, trainPos.y + front_t.y + up_t.y, trainPos.z + front_t.z + up_t.z);
	glEnd();
	if (!doingShadows)glColor3ub(255, 255, 255);
	glBegin(GL_QUADS);
	glVertex3f(trainPos.x - front_t.x, trainPos.y - front_t.y, trainPos.z - front_t.z);
	glVertex3f(trainPos.x + cross_t.x, trainPos.y + cross_t.y, trainPos.z + cross_t.z);
	glVertex3f(trainPos.x + cross_t.x + up_t.x, trainPos.y + cross_t.y + up_t.y, trainPos.z + cross_t.z + up_t.z);
	glVertex3f(trainPos.x - front_t.x + up_t.x, trainPos.y - front_t.y + up_t.y, trainPos.z - front_t.z + up_t.z);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(trainPos.x - front_t.x, trainPos.y - front_t.y, trainPos.z - front_t.z);
	glVertex3f(trainPos.x - cross_t.x, trainPos.y - cross_t.y, trainPos.z - cross_t.z);
	glVertex3f(trainPos.x - cross_t.x + up_t.x, trainPos.y - cross_t.y + up_t.y, trainPos.z - cross_t.z + up_t.z);
	glVertex3f(trainPos.x - front_t.x + up_t.x, trainPos.y - front_t.y + up_t.y, trainPos.z - front_t.z + up_t.z);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(trainPos.x + front_t.x + up_t.x, trainPos.y + front_t.y + up_t.y, trainPos.z + front_t.z + up_t.z);
	glVertex3f(trainPos.x - cross_t.x + up_t.x, trainPos.y - cross_t.y + up_t.y, trainPos.z - cross_t.z + up_t.z);
	glVertex3f(trainPos.x - front_t.x + up_t.x, trainPos.y - front_t.y + up_t.y, trainPos.z - front_t.z + up_t.z);
	glVertex3f(trainPos.x + cross_t.x + up_t.x, trainPos.y + cross_t.y + up_t.y, trainPos.z + cross_t.z + up_t.z);
	glEnd();
}

int M_Cardinal[4][4] = { { -1,2,-1,0 },{ 3,-5,0,2 },{ -3,4,1,0 },{ 1,-1,0,0 } };
int M_B_Spline[4][4] = { { -1,3,-3,1 },{ 3,-6,0,4 },{ -3,3,3,1 },{ 1,0,0,0 } };
float train_dis = 0;
static unsigned long lastRedraw = 0;
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != 2) {
		for(size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	spline_t type_spline = (spline_t)curve;
	float dis_t = 0;
	float dis = 0;
	bool isFindTrain = false;
	for (int i = 0; i < (int)m_pTrack->points.size(); ++i)
	{
		//pos
		Pnt3f cp_pos_p[4];
		Pnt3f c[4];
		for (int j = 0; j < 4; j++)
		{
			cp_pos_p[j] = m_pTrack->points[(i + j) % (int)m_pTrack->points.size()].pos;
		}

		//orient
		Pnt3f cp_orient_p[4];
		for (int j = 0; j < 4; j++)
		{
			cp_orient_p[j] = m_pTrack->points[(i + j) % (int)m_pTrack->points.size()].orient;
		}
		
		Pnt3f qt, orient_t;
		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		if (type_spline == spline_Linear)
		{
			qt = (1 - t)*cp_pos_p[0] + t * cp_pos_p[1];
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				if (type_spline == spline_CardinalCubic)
				{
					c[j] = cp_pos_p[0] * M_Cardinal[0][j] * 0.5;
					for (int k = 1; k < 4; k++)
					{
						c[j] = c[j] + cp_pos_p[k] * M_Cardinal[k][j] * 0.5;
					}
				}
				else if (type_spline == spline_CubicB_spline)
				{
					c[j] = cp_pos_p[0] * M_B_Spline[0][j] * 0.166666f;
					for (int k = 1; k < 4; k++)
					{
						c[j] = c[j] + cp_pos_p[k] * M_B_Spline[k][j] * 0.166666f;
					}
				}
			}
			qt = c[0] * powf(t, 3);
			for (int j = 1; j < 4; j++)
			{
				qt = qt + c[j] * powf(t, 3 - j);
			}
		}
		
		for (int j = 0; j < DIVIDE_LINE; j++)
		{
			Pnt3f qt0 = qt;
			if (type_spline == spline_Linear)
			{
				orient_t = (1 - t)*cp_orient_p[0] + t * cp_orient_p[1];
			}
			else
			{
				for (int j = 0; j < 4; j++)
				{
					if (type_spline == spline_CardinalCubic)
					{
						c[j] = cp_orient_p[0] * M_Cardinal[0][j] * 0.5;
						for (int k = 1; k < 4; k++)
						{
							c[j] = c[j] + cp_orient_p[k] * M_Cardinal[k][j] * 0.5;
						}
					}
					else if (type_spline == spline_CubicB_spline)
					{
						c[j] = cp_orient_p[0] * M_B_Spline[0][j] * 0.166666f;
						for (int k = 1; k < 4; k++)
						{
							c[j] = c[j] + cp_orient_p[k] * M_B_Spline[k][j] * 0.166666f;
						}
					}
				}
				orient_t = c[0] * powf(t, 3);
				for (int j = 1; j < 4; j++)
				{
					orient_t = orient_t + c[j] * powf(t, 3 - j);
				}
			}
			t += percent;
			if (type_spline == spline_Linear)
			{
				qt = (1 - t)*cp_pos_p[0] + t * cp_pos_p[1];
			}
			else
			{
				for (int j = 0; j < 4; j++)
				{
					if (type_spline == spline_CardinalCubic)
					{
						c[j] = cp_pos_p[0] * M_Cardinal[0][j] * 0.5;
						for (int k = 1; k < 4; k++)
						{
							c[j] = c[j] + cp_pos_p[k] * M_Cardinal[k][j] * 0.5;
						}
					}
					else if (type_spline == spline_CubicB_spline)
					{
						c[j] = cp_pos_p[0] * M_B_Spline[0][j] * 0.166666f;
						for (int k = 1; k < 4; k++)
						{
							c[j] = c[j] + cp_pos_p[k] * M_B_Spline[k][j] * 0.166666f;
						}
					}
				}
				qt = c[0] * powf(t, 3);
				for (int j = 1; j < 4; j++)
				{
					qt = qt + c[j] * powf(t, 3 - j);
				}
			}
			Pnt3f qt1 = qt;

			//compute sum of dis
			dis += sqrtf(powf(qt1.x - qt0.x, 2) + powf(qt1.y - qt0.y, 2) + powf(qt1.z - qt0.z, 2));
			dis_t += sqrtf(powf(qt1.x - qt0.x, 2) + powf(qt1.y - qt0.y, 2) + powf(qt1.z - qt0.z, 2));
			
			//Find train pos
			if (!isFindTrain&&dis_t > train_dis)
			{
				trainPos = qt0;
				front_t = (qt0 * -1) + qt1;
				cross_t = orient_t;
				isFindTrain = true;
			}
			
			//cross
			orient_t.normalize();
			Pnt3f cross_t = (qt0 + (qt1*-1))*orient_t;
			cross_t.normalize();
			cross_t = cross_t * 5;
			//Draw wood
			if (dis >= 6)
			{
				Pnt3f qtf = (qt1 + qt0 * -1);
				qtf.normalize();
				qtf = qtf * 2;
				qtf = qt0 + qtf;
				if (!doingShadows)glColor3ub(255, 255, 255);
				glBegin(GL_QUADS);
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
				glVertex3f(qtf.x - cross_t.x, qtf.y - cross_t.y, qtf.z - cross_t.z);
				glVertex3f(qtf.x + cross_t.x, qtf.y + cross_t.y, qtf.z + cross_t.z);
				glEnd();
				dis -= 6;
			}
			cross_t = cross_t * 0.5;

			//Draw Lines
			glLineWidth(4);
			glBegin(GL_LINES);
			if (!doingShadows)glColor3ub(32, 32, 64);
			glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
			glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
			glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
			glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
			glEnd();
			glLineWidth(1);
		}
		
	}

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################

	//Find where is the train.
	if (this->isrun)
	{
		if (clock() - lastRedraw > CLOCKS_PER_SEC / 60) {
			lastRedraw = clock();
			train_dis += 1.5;
			if (train_dis > dis_t)
				train_dis = 0;
		}
	}
	
	front_t.normalize();
	front_t = front_t * 5;
	cross_t = cross_t * front_t;
	cross_t.normalize();
	cross_t = cross_t * 5;
	up_t = cross_t * front_t;
	up_t.normalize();
	up_t = up_t * -10;
	if(this->camera!=2)
		drawCube(doingShadows ,trainPos, front_t, cross_t, up_t);

#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}
