#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <iostream>
#include <ios>

#include "enet/enet.h"
#include "Network.h"

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGMultiDisplayWindow.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGThreadManager.h>

#include <OpenSG/OSGGradientBackground.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

#include <OSGCSM/OSGCAVESceneManager.h>
#include <OSGCSM/OSGCAVEConfig.h>
#include <OSGCSM/appctrl.h>

#include "Common.h"
#include "BuildScene.h"
#include "Input.h"
#include "Game.h"
#include "Animations.h"

#include "LightTrail.h"

OSG_USING_NAMESPACE

OSGCSM::CAVEConfig cfg;
OSGCSM::CAVESceneManager *mgr = nullptr;

const char* useInput = "VRPN";
Input* input;
Client* networkClient;
GameManager* gameManager;

ThreadRefPtr networkingThread;

/*void handleGameStateBroadcast(GameInformation* information) {
	if (information->isRunning) {
		std::cout << "game started by server" << std::endl;
		startGame();
	} else {
		// ensure game ending
	}
}

void handlePlayerIdentification(PlayerInformation* information) {
	std::cout << "got id: " << information->playerId << std::endl;
	playerId = information->playerId;
}

void handlePlayerPositionBroadcast(PlayerPosition* information) {
	if (information->playerId != playerId) {
		enemy->setHeadPosition(Vec3f(information->headPosX, information->headPosY, information->headPosZ));
		enemy->setHeadRotation(Quaternion(information->headRotX, information->headRotY, information->headRotZ, information->headRotW));
		enemy->setDiskArmPosition(Vec3f(information->rightPosX, information->rightPosY, information->rightPosZ));
		enemy->setDiskArmRotation(Quaternion(information->rightRotX, information->rightRotY, information->rightRotZ, information->rightRotW));
		enemy->setShieldArmPosition(Vec3f(information->leftPosX, information->leftPosY, information->leftPosZ));
		enemy->setShieldArmRotation(Quaternion(information->leftRotX, information->leftRotY, information->leftRotZ, information->leftRotW));
	}
}

void handlePlayerLoseLifeBroadcast(PlayerCounterInformation* information) {
	if (information->playerId == playerId) {
		user->getLifeCounter()->setLifeCount(information->counter);
	} else {
		user->getLifeCounter()->setLifeCount(information->counter);
	}
}

void handlePlayerShieldChargeBroadcast(PlayerCounterInformation* information) {
	if (information->playerId == playerId) {
		user->getShield()->setCharges(information->counter);
	} else {
		user->getShield()->setCharges(information->counter);
	}
}

void handleDiskStatusBroadcast(DiskStatusInformation* information) {
	// TODO: sync local calculation
}

void handleDiskThrowBroadcast(DiskThrowInformation* information) {
	// TODO: implement extern disk throw
}

void handleDiskPositionBroadcast(DiskPosition* information) {
	// TODO: sync local calculation
}

void handlePacket(ENetEvent event) {
	SToCPacketType* header = reinterpret_cast<SToCPacketType*>(event.packet->data);
	void* actualData = reinterpret_cast<void*>(header + 1);
	switch (*header) {
	case STOC_PACKET_TYPE_GAME_STATE_BROADCAST:
		handleGameStateBroadcast(reinterpret_cast<GameInformation*>(actualData));
		break;
	case STOC_PACKET_TYPE_PLAYER_IDENTIFICATION:
		handlePlayerIdentification(reinterpret_cast<PlayerInformation*>(actualData));
		break;
	case STOC_PACKET_TYPE_PLAYER_POSITION_BROADCAST:
		handlePlayerPositionBroadcast(reinterpret_cast<PlayerPosition*>(actualData));
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_LIFE_BROADCAST:
		handlePlayerLoseLifeBroadcast(reinterpret_cast<PlayerCounterInformation*>(actualData));
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_SHIELD_CHARGE_BROADCAST:
		handlePlayerShieldChargeBroadcast(reinterpret_cast<PlayerCounterInformation*>(actualData));
		break;
	case STOC_PACKET_TYPE_DISK_STATUS_BROADCAST:
		handleDiskStatusBroadcast(reinterpret_cast<DiskStatusInformation*>(actualData));
		break;
	case STOC_PACKET_TYPE_DISK_THROW_BROADCAST:
		handleDiskThrowBroadcast(reinterpret_cast<DiskThrowInformation*>(actualData));
		break;
	case STOC_PACKET_TYPE_DISK_POSITION_BROADCAST:
		handleDiskPositionBroadcast(reinterpret_cast<DiskPosition*>(actualData));
		break;
	}
}*/

void cleanup()
{
	LightTrail::deleteAllTrailsInstantly();
	networkClient->disconnect();
	delete networkClient;
	delete input;
	delete gameManager;
	delete mgr;
}

bool showFPS = false;
int fpsCount = 0;
Real32 lastFPSUpdate;

void keyboard(unsigned char k, int x, int y)
{
	Real32 ed;
	ENetPacket *packet;
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
			input->printTracker();
			break;
		case 'f':
			showFPS = !showFPS;
			break;
		case 'g':
			gameManager->requestGameStart();
			break;
		case 'd':
			if (networkClient->isConnected()) {
				networkClient->disconnect();
			}
			break;
		case 'c':
			if (networkClient->connect("127.0.0.1", 13244)) {
				networkingThread->runFunction(networkLoopOnClient, 1, networkClient);
			}
			break;
#ifdef _simulate_
		case 'x':
			input->startSimulation();
			break;
#endif
		default:
			std::cout << "Key '" << k << "' ignored\n";
	}
}

void setupGLUT(int *argc, char *argv[])
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB  |GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("The Grid");
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
		input->updateInputs();
		Vec3f forward(0,0,1);
		input->getHeadOrientation().multVec(forward, forward);
		headLight->setDirection(forward);
		LightTrail::updateTrails(input->getHeadPosition());

		if (showFPS) {
			if (time - lastFPSUpdate > 1000) {
				std::cout << "FPS: " << fpsCount << '\n';
				fpsCount = 0;
				lastFPSUpdate = time;
			}
			fpsCount++;
		}

		gameManager->handleGameTick();
		
		updateAnimations();
		
		const auto speed = 1.f;
		mgr->setUserTransform(input->getHeadPosition(), input->getHeadOrientation());
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
	
	if (enet_initialize () != 0)
    {
        std::cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

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

		MultiDisplayWindowRefPtr mwin = createAppWindow(cfg, cfg.getBroadcastaddress());

		if (!scene) 
			scene = buildScene();
		commitChanges();

		input = Input::create(useInput, cfg);
		if (input == nullptr) {
			std::cout << "ERROR: VRPN input class not found\n";
			return EXIT_FAILURE;
		}
		networkClient = new Client();
		gameManager = new GameManager(networkClient, input);
		input->setInputHandler(gameManager);
		networkClient->setPacketHandler(gameManager);
		networkingThread = dynamic_pointer_cast<Thread>(ThreadManager::the()->getThread("Networking", TRUE));

		mgr = new OSGCSM::CAVESceneManager(&cfg);
		mgr->setWindow(mwin );
		mgr->setRoot(scene);
		mgr->showAll();
		mgr->getWindow()->init();
		mgr->turnWandOff();
		
		mgr->setHeadlight(false);
	}
	catch(const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	glutMainLoop();
}
