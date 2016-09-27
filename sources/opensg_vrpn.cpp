#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <iostream>
#include <ios>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGMultiDisplayWindow.h>
#include <OpenSG/OSGSceneFileHandler.h>

#include <OpenSG/OSGGradientBackground.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

#include <OSGCSM/OSGCAVESceneManager.h>
#include <OSGCSM/OSGCAVEConfig.h>
#include <OSGCSM/appctrl.h>

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

#include "Common.h"
#include "BuildScene.h"
#include "Disk.h"
#include "Player.h"
#include "Animations.h"
#include "Simulation.h"

OSG_USING_NAMESPACE

OSGCSM::CAVEConfig cfg;
OSGCSM::CAVESceneManager *mgr = nullptr;
vrpn_Tracker_Remote* tracker =  nullptr;
vrpn_Button_Remote* button = nullptr;
vrpn_Analog_Remote* analog = nullptr;

Disk *playerDisk;
Player *user, *enemy;

void cleanup()
{
	delete playerDisk;
	delete user, enemy;
	delete mgr;
	delete tracker;
	delete button;
	delete analog;
}

void print_tracker();

template<typename T>
T scale_tracker2cm(const T& value)
{
	static const float scale = OSGCSM::convert_length(cfg.getUnits(), 1.f, OSGCSM::CAVEConfig::CAVEUnitCentimeters);
	return value * scale;
}

auto head_orientation = Quaternion(Vec3f(0.f, 1.f, 0.f), 3.141f);
auto head_position = Vec3f(0.f, 170.f, 130.f);	// a 1.7m Person 2m in front of the scene

void VRPN_CALLBACK callback_head_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	head_orientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
	head_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));
}

auto wand_orientation = Quaternion();
auto wand_position = Vec3f(0,135,0);
auto shield_orientation = Quaternion();
auto shield_position = Vec3f(0,135,0);
void VRPN_CALLBACK callback_wand_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	wand_orientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
	wand_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));
}

void VRPN_CALLBACK callback_shield_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	shield_orientation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(30)) * Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
	shield_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));
}

auto analog_values = Vec3f();
void VRPN_CALLBACK callback_analog(void* userData, const vrpn_ANALOGCB analog)
{
	if (analog.num_channel >= 2)
		analog_values = Vec3f(analog.channel[0], 0, -analog.channel[1]);
}

void VRPN_CALLBACK callback_button(void* userData, const vrpn_BUTTONCB button)
{
	if (button.button == 0) {
		if (button.state == 1) {
			if(playerDisk->getState() == DISK_STATE_READY) {
				playerDisk->startDraw(wand_position);
			}
		} else {
			if(playerDisk->getState() == DISK_STATE_DRAWN) {
				playerDisk->endDraw(wand_position);
			}
		}
	}
}

void InitTracker(OSGCSM::CAVEConfig &cfg)
{
	try
	{
		const char* const vrpn_name = "DTrack@localhost";
		tracker = new vrpn_Tracker_Remote(vrpn_name);
		tracker->shutup = true;
		tracker->register_change_handler(NULL, callback_head_tracker, cfg.getSensorIDHead());
		tracker->register_change_handler(NULL, callback_wand_tracker, cfg.getSensorIDController());
		tracker->register_change_handler(NULL, callback_shield_tracker, 3);
		button = new vrpn_Button_Remote(vrpn_name);
		button->shutup = true;
		button->register_change_handler(nullptr, callback_button);
		analog = new vrpn_Analog_Remote(vrpn_name);
		analog->shutup = true;
		analog->register_change_handler(NULL, callback_analog);
	}
	catch(const std::exception& e) 
	{
		std::cout << "ERROR: " << e.what() << '\n';
		return;
	}
}

void check_tracker()
{
	tracker->mainloop();
	button->mainloop();
	analog->mainloop();
}

void print_tracker()
{
	std::cout << "Head position: " << head_position << " orientation: " << head_orientation << '\n';
	std::cout << "Wand position: " << wand_position << " orientation: " << wand_orientation << '\n';
	std::cout << "Analog: " << analog_values << '\n';
}

Real32 simStartTime = 0;

void keyboard(unsigned char k, int x, int y)
{
	Real32 ed;
	switch(k)
	{
		case 'q':
		case 27: 
			cleanup();
			exit(EXIT_SUCCESS);
			break;
		case 'e':
			ed = mgr->getEyeSeparation() * .9f;
			std::cout << "Eye distance: " << ed << '\n';
			mgr->setEyeSeparation(ed);
			break;
		case 'E':
			ed = mgr->getEyeSeparation() * 1.1f;
			std::cout << "Eye distance: " << ed << '\n';
			mgr->setEyeSeparation(ed);
			break;
		case 'h':
			cfg.setFollowHead(!cfg.getFollowHead());
			std::cout << "following head: " << std::boolalpha << cfg.getFollowHead() << '\n';
			break;
		case 'i':
			print_tracker();
			break;
		case 'w':
			//movableTransform->setTranslation(movableTransform->getTranslation() + Vec3f(0,1,0));
			//xangle--;
			//std::cout << xangle << '\n';
			//boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(xangle)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
			break;
		case 's':
			//movableTransform->setTranslation(movableTransform->getTranslation() - Vec3f(0,1,0));
			//xangle++;
			//std::cout << xangle << '\n';
			//boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(xangle)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
			break;
		case 'a':
			//movableTransform->setTranslation(movableTransform->getTranslation() + Vec3f(0,0,1));
			break;
		case 'd':
			//movableTransform->setTranslation(movableTransform->getTranslation() - Vec3f(0,0,1));
			break;
		case 'x':
			initSimulation();
			simStartTime = glutGet(GLUT_ELAPSED_TIME);
			break;
		case ' ':
			playerDisk->startDraw(Vec3f(-1,135,1));
			playerDisk->setPosition(Vec3f(0,135,0));
			playerDisk->endDraw(Vec3f(0,135,0));
			break;
		default:
			std::cout << "Key '" << k << "' ignored\n";
	}
}

void setupGLUT(int *argc, char *argv[])
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB  |GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("OpenSG CSMDemo with VRPN API");
	glutDisplayFunc([]()
	{
		// black navigation window
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
	});
	glutReshapeFunc([](int w, int h)
	{
		mgr->resize(w, h);
		glutPostRedisplay();
	});
	glutKeyboardFunc(keyboard);
	glutIdleFunc([]()
	{
		// get the time since the application started
		int time = glutGet(GLUT_ELAPSED_TIME);

		playerDisk->setPosition(shield_position);
		playerDisk->setRotation(shield_orientation);
		playerDisk->setTargetOwnerPosition(wand_position);
		playerDisk->setTargetEnemyPosition(enemyPoint->getTranslation());
		playerDisk->updatePosition();
		
		user->update();

		enemy->setHeadDirection(head_orientation);
		enemy->setHeadPosition(head_position - Vec3f(0,135,0) + Vec3f(0,135,-810));
		enemy->setRightArmDirection(wand_orientation);
		enemy->setRightArmPosition((wand_position - Vec3f(0,135,0)) * (-1) + Vec3f(0,135,-810));
		enemy->update();

		updateAnimations();
		
		check_tracker();

		/**/
		if (simStartTime > 0) {
			SimStep t = getSimulationStep(time - simStartTime + 14500);
			wand_position = t.wand_position;
			wand_orientation = t.wand_orientation;
			head_position = t.head_position;
			head_orientation = t.head_orientation;
			InputStep i = getInputStep(time - simStartTime + 14500);
			if (i.buttonPushed) {
				if(playerDisk->getState() == DISK_STATE_READY) {
					playerDisk->startDraw(wand_position);
				}
			} else {
				if(playerDisk->getState() == DISK_STATE_DRAWN) {
					playerDisk->endDraw(wand_position);
				}
			}
		}
		/*
		*/
		
		const auto speed = 1.f;
		mgr->setUserTransform(head_position, head_orientation);
		mgr->setTranslation(mgr->getTranslation() + speed * analog_values);
		commitChanges();
		mgr->redraw();
		// the changelist should be cleared - else things could be copied multiple times
		OSG::Thread::getCurrentChangeList()->clear();
	});
}

int main(int argc, char **argv)
{
#if WIN32
	OSG::preloadSharedObject("OSGFileIO");
	OSG::preloadSharedObject("OSGImageFileIO");
#endif
	try
	{
		bool cfgIsSet = false;
		NodeRefPtr scene = nullptr;

		// ChangeList needs to be set for OpenSG 1.4
		ChangeList::setReadWriteDefault();
		osgInit(argc,argv);

		// evaluate intial params
		for(int a=1 ; a<argc ; ++a)
		{
			if( argv[a][0] == '-' )
			{
				if ( strcmp(argv[a],"-f") == 0 ) 
				{
					char* cfgFile = argv[a][2] ? &argv[a][2] : &argv[++a][0];
					if (!cfg.loadFile(cfgFile)) 
					{
						std::cout << "ERROR: could not load config file '" << cfgFile << "'\n";
						return EXIT_FAILURE;
					}
					cfgIsSet = true;
				}
			} else {
				std::cout << "Loading scene file '" << argv[a] << "'\n";
				scene = SceneFileHandler::the()->read(argv[a], NULL);
			}
		}

		// load the CAVE setup config file if it was not loaded already:
		if (!cfgIsSet) 
		{
			const char* const default_config_filename = "config/mono.csm";
			if (!cfg.loadFile(default_config_filename)) 
			{
				std::cout << "ERROR: could not load default config file '" << default_config_filename << "'\n";
				return EXIT_FAILURE;
			}
		}

		cfg.printConfig();

		// start servers for video rendering
		if ( startServers(cfg) < 0 ) 
		{
			std::cout << "ERROR: Failed to start servers\n";
			return EXIT_FAILURE;
		}

		setupGLUT(&argc, argv);

		InitTracker(cfg);

		MultiDisplayWindowRefPtr mwin = createAppWindow(cfg, cfg.getBroadcastaddress());

		if (!scene) 
			scene = buildScene();
		commitChanges();

		initSimulation();
		playerDisk = new Disk(mainUserFaction);
		user = new Player(mainUserFaction, false);
		enemy = new Player(enemyFaction, true);

		mgr = new OSGCSM::CAVESceneManager(&cfg);
		mgr->setWindow(mwin );
		mgr->setRoot(scene);
		mgr->showAll();
		mgr->getWindow()->init();
		mgr->turnWandOff();
		
		// alternatively use a gradient background
		GradientBackgroundRecPtr bkg = GradientBackground::create();
		bkg->addLine(Color3f(0.7f, 0.7f, 0.8f), 0);
		bkg->addLine(Color3f(0.0f, 0.1f, 0.3f), 1);

		mgr->getWindow()->getPort(0)->setBackground(bkg);
	}
	catch(const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	glutMainLoop();
}
