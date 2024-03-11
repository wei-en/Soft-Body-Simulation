#include "graphics.h"

#include "glad/glad.h"
#include "GL/freeglut.h"

namespace gfx
{
	// initialize color to none
	Color Graphics::s_currentColor = Color::none;

	void Graphics::startFrame(const std::array<float, 4>& clearColor)
	{
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		glClearDepth(1.0);
		glClearStencil(0);
		glClearAccum(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	}

	void Graphics::finishFrame()
	{
		glutSwapBuffers();
		glutPostRedisplay();
	}

	void Graphics::setColor(const Color& color)
	{
		glDisable(GL_TEXTURE_2D);

		s_currentColor = color;
		glColor4f(s_currentColor.r, s_currentColor.g, s_currentColor.b, s_currentColor.a);
	}

	void Graphics::setTexture(const Texture& texture)
	{
		Graphics::setColor(Color::white);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture.getID());
	}

	void Graphics::setTextureToNull()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Graphics::renderPoint(const std::array<float, 3>& position, const float& size)
	{
		glEnable(GL_POINT_SMOOTH);
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glPointSize(size);
		glBegin(GL_POINTS);
		glVertex3fv(position.data());
		glEnd();
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}

	void Graphics::renderLineSegment(const std::array<float, 3>& start, const std::array<float, 3>& end)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glVertex3fv(start.data());
		glVertex3fv(end.data());
		glEnd();
		glEnable(GL_LIGHTING);
	}

	void Graphics::renderPlane(const std::array<float, 3>& min_position, const std::array<float, 3>& max_position, const float uvScalar)
	{
		glBegin(GL_QUADS);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3d(max_position[0], max_position[1], max_position[2]);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(uvScalar, 0.0f);
		glVertex3d(max_position[0], max_position[1], min_position[2]);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(uvScalar, uvScalar);
		glVertex3d(min_position[0], min_position[1], min_position[2]);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, uvScalar);
		glVertex3f(min_position[0], min_position[1], max_position[2]);

		glEnd();
	}

	void Graphics::renderSphere(const std::array<float, 3>& position, const float& radius)
	{
		glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glutSolidSphere(radius, 50, 50);
		glPopMatrix();
	}

	void Graphics::renderWireSphere(const std::array<float, 3>& position, const float & radius)
	{
		glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glutWireSphere(radius, 50, 50);
		glPopMatrix();
	}

	void Graphics::renderCube(const std::array<float, 3>& position, const float& size)
	{
		glPushMatrix();

		glScalef(size, size, size);

		glBegin(GL_QUADS);

		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);

		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		glEnd();

		glPopMatrix();
	}

	void Graphics::renderCylinder(const std::array<float, 3>& start, const std::array<float, 3>& end, const float& radius)
	{
		//the same quadric can be re-used for drawing many cylinders
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		int subdivisions = 18;

		std::array<float, 3> cylinder_vector{ end[0] - start[0], end[1] - start[1] ,end[2] - start[2] };
		float vx = cylinder_vector[0];
		float vy = cylinder_vector[1];
		float vz = cylinder_vector[2];
		float ax, rx, ry, rz;
		float cyl_len = cylinder_vector[0] * cylinder_vector[0] + cylinder_vector[1] * cylinder_vector[1] + cylinder_vector[2] * cylinder_vector[2];
		cyl_len = sqrt(cyl_len);

		glPushMatrix();

		glTranslatef(start[0], start[1], start[2]);
		if (fabs(vz) < 0.0001) {
			glRotatef(90, 0, 1, 0);
			ax = 57.2957795f * -atan(vy / vx);
			if (vx < 0)
				ax = ax + 180;
			rx = 1;
			ry = 0;
			rz = 0;
		}
		else {
			ax = 57.2957795f * acos(vz / cyl_len);
			if (vz < 0.0)
				ax = -ax;
			rx = -vy * vz;
			ry = vx * vz;
			rz = 0;
		}
		glRotatef(ax, rx, ry, rz);
		gluQuadricOrientation(quadric, GLU_OUTSIDE);
		gluCylinder(quadric, radius, radius, cyl_len, subdivisions, 1);

		glPopMatrix();

		gluDeleteQuadric(quadric);
	}
}