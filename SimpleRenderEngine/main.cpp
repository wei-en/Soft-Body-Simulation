/*

Computer animation assignment 1 : particle system for softbody (jelly) simulation

First step : Look for TODO comments to find the methods that you need to implement.

*/

#include <ctime>

#include "glad/glad.h"

#include "imgui.h"
#include "imgui_impl_freeglut.h"
#include "imgui_impl_opengl2.h"
#include "GL/freeglut.h"

#include "gfx.h"
#include "output/exporter.h"

#include "simulation.h"

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#pragma warning (disable: 4996)
#endif

/*

These global variables are here just for testing, these should be deleted in release projects.

Why? Because global variables are evil and should be eliminated on sight. >:(

Update : using anonymous namespace : not making global variables, but convenient to access in local file.

*/


namespace
{
	// TODO
	// TO DO : Please change this c string to your student ID
	const char* studentID = "987654321";

	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	float g_fLightPos[4][4] = { 3.0f, 10.0f, -3.0f, 1.0f,
								-3.0f, 10.0f,  3.0f, 1.0f,
								3.0f, 10.0f,  3.0f, 1.0f,
								-3.0f, 10.0f, -3.0f, 1.0f };

	int g_iScreenWidth = 1024, g_iScreenHeight = 768;

	gfx::Camera mainCamera = gfx::Camera();

	// for graphics
	gfx::Texture planeTexture;
	// this one should also affect how simulation handles collision
	std::shared_ptr<simulation::Terrain> terrain = simulation::TerrainFactory::CreateTerrain(simulation::TerrainType::Plane);
	std::shared_ptr<simulation::Integrator> integrator =
		simulation::IntegratorFactory::CreateIntegrator(simulation::IntegratorType::ExplicitEuler);
	// for recording
	bool isRecording = false;
	// for ui
	bool isUsingCameraPanel = false;
	bool isUsingRecordingPanel = false;
	bool isUsingSimulationPanel = false;
	// for camera
	float cameraRotationAngle = 60.0f;
	float cameraRotationRadius = 17.0f;
	float cameraYOffset = 0.2f;
	// for simulation
	simulation::MassSpringSystem particleSystem;
	int simulationPerFrame = 5;
}

void render_ui()
{
	// Main Panel
	{
		ImGui::Begin("Main Control Panel");

		if (particleSystem.CheckStable() == false)
		{
			ImGui::Text("System Unstable : \nPlease reset the parameters!");
		}
		else
		{
			//clear_color = ImVec4(0.0f, 0.3f, 0.5f, 0.0f);
			clear_color = ImVec4(0.2f, 0.4f, 0.3f, 0.0f);
		}

		if (ImGui::Button("Camera Panel"))
		{
			isUsingCameraPanel = !isUsingCameraPanel;
		}
		if (ImGui::Button("Recording Panel"))
		{
			isUsingRecordingPanel = !isUsingRecordingPanel;
		}
		if (ImGui::Button("Simulation Panel"))
		{
			isUsingSimulationPanel = !isUsingSimulationPanel;
		}

		// Terrain setup
		ImGui::NewLine();
		ImGui::Text("Select terrain type here :");
		if (ImGui::Button("Plane"))
		{
			terrain = simulation::TerrainFactory::CreateTerrain(simulation::TerrainType::Plane);
			// also, pass this info to particle system
			particleSystem.SetTerrain(terrain);
		}
		ImGui::SameLine();
		if (ImGui::Button("Sphere"))
		{
			terrain = simulation::TerrainFactory::CreateTerrain(simulation::TerrainType::Sphere);
			particleSystem.SetTerrain(terrain);
		}
		if (ImGui::Button("Bowl"))
		{
			terrain = simulation::TerrainFactory::CreateTerrain(simulation::TerrainType::Bowl);
			particleSystem.SetTerrain(terrain);
		}
		ImGui::SameLine();
		if (ImGui::Button("TiltedPlane"))
		{
			terrain = simulation::TerrainFactory::CreateTerrain(simulation::TerrainType::TiltedPlane);
			particleSystem.SetTerrain(terrain);
		}

		ImGui::End();
	}
	// Camera control
	if (isUsingCameraPanel)
	{
		// Create imgui window
		ImGui::Begin("Camera Control Panel");

		ImGui::Text("Use this panel to control the camera");

		ImGui::Text("Camera position : (%f, %f, %f)", mainCamera.position[0], mainCamera.position[1], mainCamera.position[2]);
		ImGui::Text("Camera lookat : (%f, %f, %f)", mainCamera.lookAt[0], mainCamera.lookAt[1], mainCamera.lookAt[2]);

		ImGui::SliderFloat("Camera rotation angle", &cameraRotationAngle, 0.0f, 360.0f);
		ImGui::SliderFloat("Camera rotation radius", &cameraRotationRadius, 2.0f, 30.0f);
		ImGui::SliderFloat("Camera Y Offset", &cameraYOffset, -2.0f, 5.0f);

		//ImGui::ColorEdit3("Clear color", (float*)&clear_color);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	// Recording Control
	if (isUsingRecordingPanel)
	{
		ImGui::Begin("Recording Control Panel");

		ImGui::Text("Currently %srecording...", isRecording ? "" : "not ");

		std::string recordingText = (isRecording ? "Stop Recording" : "Start Recording");
		if (ImGui::Button(recordingText.c_str()))
		{
			// start recording
			if (isRecording == false)
			{
				isRecording = true;
			}
			else // stop recording and output video
			{
				isRecording = false;
				output::Exporter::Reset();
			}
		}

		ImGui::End();
	}
	// Simulation Control Panel
	if (isUsingSimulationPanel)
	{
		ImGui::Begin("Simulation Control Panel");

		std::string simulateText = particleSystem.isSimulating ? "Stop" : "Start";
		if (ImGui::Button(simulateText.c_str()))
		{
			particleSystem.Reset();
			particleSystem.isSimulating = !particleSystem.isSimulating;
		}

		// Render checkbox can be edited while simulating
		ImGui::Checkbox("Draw Cube", &particleSystem.isDrawingCube);
		ImGui::Checkbox("Draw Struct", &particleSystem.isDrawingStruct);
		ImGui::Checkbox("Draw Shear", &particleSystem.isDrawingShear);
		ImGui::Checkbox("Draw Bending", &particleSystem.isDrawingBending);

		// Only allow simulation parameter editing when system is not simulating.
		if (!particleSystem.isSimulating)
		{
			ImGui::Text("Integration Method = %d", (int)particleSystem.integratorType);
			//ImGui::SameLine();
			if (ImGui::Button("EXPLICIT_EULER"))
			{
				integrator = simulation::IntegratorFactory::CreateIntegrator(simulation::IntegratorType::ExplicitEuler);
				particleSystem.SetIntegrator(integrator);
			}
			ImGui::SameLine();
			if (ImGui::Button("RUNGE_KUTTA"))
			{
				integrator = simulation::IntegratorFactory::CreateIntegrator(simulation::IntegratorType::RungeKuttaFourth);
				particleSystem.SetIntegrator(integrator);
			}
			// ADDED for imp_euler
			//ImGui::SameLine();
			if (ImGui::Button("IMPLICIT_EULER"))
			{
				integrator = simulation::IntegratorFactory::CreateIntegrator(simulation::IntegratorType::ImplicitEuler);
				particleSystem.SetIntegrator(integrator);
			}
			ImGui::SameLine();
			if (ImGui::Button("MIDPOINT_EULER"))
			{
				integrator = simulation::IntegratorFactory::CreateIntegrator(simulation::IntegratorType::MidpointEuler);
				particleSystem.SetIntegrator(integrator);
			}
			//

			ImGui::InputInt("Simulation Per Frame", &simulationPerFrame);
			simulationPerFrame = std::max(simulationPerFrame, 1);

			ImGui::InputDouble("Delta Time", &particleSystem.deltaTime, 0.0005, 0.0001, "%.4f");

			ImGui::InputDouble("Spring Coef", &particleSystem.springCoefStruct, 10.0, 1.0);
			particleSystem.springCoefStruct = std::max(particleSystem.springCoefStruct, 0.0);
			particleSystem.SetSpringCoef(particleSystem.springCoefStruct, simulation::Spring::SpringType::STRUCT);
			particleSystem.SetSpringCoef(particleSystem.springCoefStruct, simulation::Spring::SpringType::SHEAR);
			particleSystem.SetSpringCoef(particleSystem.springCoefStruct, simulation::Spring::SpringType::BENDING);

			ImGui::InputDouble("Damper Coef", &particleSystem.damperCoefStruct, 10.0, 1.0);
			particleSystem.damperCoefStruct = std::max(particleSystem.damperCoefStruct, 0.0);
			particleSystem.SetDamperCoef(particleSystem.damperCoefStruct, simulation::Spring::SpringType::STRUCT);
			particleSystem.SetDamperCoef(particleSystem.damperCoefStruct, simulation::Spring::SpringType::SHEAR);
			particleSystem.SetDamperCoef(particleSystem.damperCoefStruct, simulation::Spring::SpringType::BENDING);

			ImGui::InputDouble("Y Offset", &particleSystem.position(1), 0.3, 0.05);
			ImGui::InputDouble("Rotation", &particleSystem.rotation, 0.3, 0.05);
		}
		else
		{
			ImGui::Text("Stop simulation to edit paramters...");
		}

		if (ImGui::Button("Reset"))
		{
			particleSystem.Reset();
		}

		ImGui::End();
	}
}

void setupLight()
{
	static float g_fLightKa[4][4] = { 0.2f, 0.2f, 0.2f, 1.0f,
								0.0f, 0.0f, 0.0f, 1.0f,
								0.0f, 0.0f, 0.0f, 1.0f,
								0.0f, 0.0f, 0.0f, 1.0f };

	static float g_fLightKd[4][4] = { 1.0f, 1.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 1.0f, 1.0f };

	static float g_fLightKs[4][4] = { 1.0f, 1.0f, 1.0f, 1.0f,
								0.0f, 0.0f, 0.0f, 1.0f,
								1.0f, 1.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 1.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, g_fLightPos[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, g_fLightKa[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_fLightKd[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, g_fLightKs[0]);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, g_fLightPos[1]);
	glLightfv(GL_LIGHT1, GL_AMBIENT, g_fLightKa[1]);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, g_fLightKd[1]);
	glLightfv(GL_LIGHT1, GL_SPECULAR, g_fLightKs[1]);
}

void setupLightShadow()
{
	static float g_fShadowLightKa[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	static float g_fShadowLightKd[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static float g_fShadowLightKs[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, g_fLightPos[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, g_fShadowLightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_fShadowLightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, g_fShadowLightKs);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

void renderCubeShadow()
{
	glPushAttrib(GL_ENABLE_BIT);

	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// write depth data in
	glDepthMask(GL_TRUE);
	// render the actual floor first, for depth data and for correct color blending
	glPushAttrib(GL_CURRENT_BIT);
	gfx::Graphics::setTexture(planeTexture);
	terrain->Draw();
	gfx::Graphics::setTextureToNull();
	glPopAttrib();
	//
	Eigen::Vector3d LightPos = Eigen::Vector3d((double)g_fLightPos[0][0], (double)g_fLightPos[0][1], (double)g_fLightPos[0][2]);

	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	//pass2
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);
	particleSystem.DrawShadow(LightPos);
	//pass3
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);
	particleSystem.DrawShadow(LightPos);
	//
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	gfx::Graphics::setColor(gfx::Color(0.1f, 0.1f, 0.1f, 0.8f));
	terrain->Draw();

	glDisable(GL_STENCIL_TEST);

	glEnable(GL_LIGHTING);

	glPopAttrib();
}

void display()
{
	/*
	Tip : Render scene first, then render UI. So that the interface will block(blend with) scene properly.
	*/
	gfx::Graphics::startFrame({ clear_color.x,clear_color.y,clear_color.z,clear_color.w });

	// Render scene
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)g_iScreenWidth / (double)g_iScreenHeight, 0.001, 100.0);

	mainCamera.position[0] = mainCamera.lookAt[0] + cameraRotationRadius * std::cos(cameraRotationAngle / 180.0f * 3.14f);
	mainCamera.position[1] = (mainCamera.lookAt[1] + cameraYOffset) * cameraRotationRadius;
	mainCamera.position[2] = mainCamera.lookAt[2] + cameraRotationRadius * std::sin(cameraRotationAngle / 180.0f * 3.14f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(mainCamera.position[0], mainCamera.position[1], mainCamera.position[2],
		mainCamera.lookAt[0], mainCamera.lookAt[1], mainCamera.lookAt[2],
		mainCamera.up[0], mainCamera.up[1], mainCamera.up[2]);

	// Simulation and stability checking
	for (int i = 0; i < simulationPerFrame; i++)
	{
		particleSystem.SimulationOneTimeStep();
	}

	if (particleSystem.CheckStable() == false)
	{
		clear_color = ImVec4(1, 0, 0, 1);
		particleSystem.isSimulating = false;
	}
	//
	renderCubeShadow();

	gfx::Graphics::setColor(gfx::Color::white);
	particleSystem.Draw();

	// texture setup and drawing plane
	gfx::Graphics::setTexture(planeTexture);
	terrain->Draw();
	//gfx::graphics::renderPlane({ -30.0f, -1.0f, -30.0f }, { 30.0f, -1.0f, 30.0f }, 20.0f);
	gfx::Graphics::setTextureToNull();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplFreeGLUT_NewFrame();

	render_ui();

	// Rendering
	ImGui::Render();
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	if (isRecording)
	{
		output::Exporter::OutputScreenShot(g_iScreenWidth, g_iScreenHeight);
		if (output::Exporter::GetPictureCounter() >= 1000)
		{
			isRecording = false;
			output::Exporter::Reset();
		}
	}

	gfx::Graphics::finishFrame();
}

void reshape(int iScreenWidth, int iScreenHeight)
{
	// required for ImGui : copied from ImGui_ImplFreeGLUT_InstallFuncs
	ImGui_ImplFreeGLUT_ReshapeFunc(iScreenWidth, iScreenHeight);

	g_iScreenWidth = iScreenWidth;
	g_iScreenHeight = iScreenHeight;

	glViewport(0, 0, g_iScreenWidth, g_iScreenHeight);
}

void initialize()
{
	// OpenGL initialization
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setupLight();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);

	// camera setup
	mainCamera.position = { 0, 8, 0 };
	mainCamera.lookAt = { 0, 0, 0 };
	mainCamera.up = { 0, 1, 0 };

	// texture setup ground
	planeTexture.loadTextureFile("assets/texture/sea.bmp");

	// terrain setup
	//terrain = std::make_shared<simulation::PlaneTerrain>();
	particleSystem.SetTerrain(terrain);
	particleSystem.SetIntegrator(integrator);
}

int main(int argc, char** argv)
{
	srand(static_cast<unsigned>(time(NULL)));

	// Create GLUT window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_ACCUM);
	glutInitWindowSize(g_iScreenWidth, g_iScreenHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CA Render template!");

	if (!gladLoadGL()) {
		printf("Something went wrong!\n");
		exit(-1);
	}
	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	// Setup Dear ImGui context
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplFreeGLUT_Init();
	// this will register all functions except display, so register your custom funcs after this line
	ImGui_ImplFreeGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	initialize();

	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplFreeGLUT_Shutdown();
	ImGui::DestroyContext();

	return 0;
}