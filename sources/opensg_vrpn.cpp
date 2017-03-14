#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <iostream>
#include <fstream>
#include <ios>

#include "enet/enet.h"
#include "NetworkPackets.h"

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

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

#include "Common.h"
#include "BuildScene.h"
#include "Player.h"
#include "Animations.h"
#include "ArtificialIntelligence.h"

#include "LightTrail.h"

#ifdef _simulate_
#include "Simulation.h"
#endif

OSG_USING_NAMESPACE

OSGCSM::CAVEConfig cfg;
OSGCSM::CAVESceneManager *mgr = nullptr;
vrpn_Tracker_Remote* tracker =  nullptr;
vrpn_Button_Remote* button = nullptr;
vrpn_Analog_Remote* analog = nullptr;

Player *user, *enemy;
AI *ai;
ENetHost *client;
ENetPeer *peer = nullptr;
int playerId = -2;
ThreadRefPtr networkingThread;

#ifdef _logFrames_
std::ofstream logFile;
#endif

void startGame() {
	if (!gameRunning) {
		if (gameResultAnimationId >= 0) {
			endAnimation(gameResultAnimationId);
			gameResultAnimationId = -1;
		}
		user->getLifeCounter()->setLifeCount(lifeCounterMaxLife);
		user->getShield()->refillCharges();
		enemy->getLifeCounter()->setLifeCount(lifeCounterMaxLife);
		enemy->getShield()->refillCharges();
		ai->resetState();
		std::cout << "Game starting, have fun :)" << '\n';
		gameRunning = true;
	} else {
		std::cout << "Game allready running, wait for it to finish..." << '\n';
	}
}

void sendPacket(CToSPacketType header, void* data, int size, bool reliable = false) {
	void* packetData = malloc(size + sizeof(CToSPacketType));
	memcpy(packetData, &header, sizeof(CToSPacketType));
	memcpy((reinterpret_cast<unsigned char *>(packetData) + sizeof(CToSPacketType)), data, size);
	ENetPacket* packet = enet_packet_create(packetData, size + sizeof(CToSPacketType), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	enet_peer_send(peer, 1, packet);
}

void sendPlayerPosition() {
	PlayerPosition pp;
	pp.playerId = playerId;
	pp.factionId = userFaction;
	user->getHeadPosition().getSeparateValues(pp.headPosX, pp.headPosY, pp.headPosZ);
	user->getHeadRotation().getValueAsQuat(pp.headRotX, pp.headRotY, pp.headRotZ, pp.headRotW);
	user->getDiskArmPosition().getSeparateValues(pp.rightPosX, pp.rightPosY, pp.rightPosZ);
	user->getDiskArmRotation().getValueAsQuat(pp.rightRotX, pp.rightRotY, pp.rightRotZ, pp.rightRotW);
	user->getShieldArmPosition().getSeparateValues(pp.leftPosX, pp.leftPosY, pp.leftPosZ);
	user->getShieldArmRotation().getValueAsQuat(pp.leftRotX, pp.leftRotY, pp.leftRotZ, pp.leftRotW);
	sendPacket(CTOS_PACKET_TYPE_PLAYER_POSITION_INFORMATION, &pp, sizeof(PlayerPosition));
}

void handleGameStateBroadcast(GameInformation* information) {
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
}

void networkLoop(void* args) {
	ENetEvent event;
	if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		std::cout << "Connection to server succeeded\n";
	} else {
		enet_peer_reset (peer);
		peer = nullptr;
		std::cout << "Connection to server failed\n";
		return;
	}

	while (true) {
		while (enet_host_service (client, &event, 0) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_RECEIVE:
				handlePacket(event);
				enet_packet_destroy (event.packet);
				break;
       
			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << "Server closed connection\n";
				return;
			}
		}
	}
}

void disconnect() {
	if (peer != nullptr) {
		if (networkingThread->exists()) {
			networkingThread->kill();
		}
		ENetEvent event;
		enet_peer_disconnect(peer, 0);
		/* Allow up to 3 seconds for the disconnect to succeed
		 * and drop any packets received packets.
		 */
		while (enet_host_service (client, & event, 3000) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy (event.packet);
			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				std::cout << "disconnect acknowledged by server\n";
				break;
			}
		}
		enet_peer_reset (peer);
		peer = nullptr;
		playerId = -2;
		std::cout << "Disconnection succeeded\n";
	}
}

bool connect(const char *hostName, enet_uint16 port) {
	if (peer != nullptr) {
		disconnect();
	}
	ENetAddress address;
	ENetEvent event;

	/* Connect to some.server.net:1234. */
	enet_address_set_host (&address, hostName);
	address.port = port;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect (client, &address, 2, 0);    
	if (peer == NULL)
	{
	   std::cerr << "No available peers for initiating an ENet connection.\n";
	   peer = nullptr;
	   return false;
	}
	networkingThread->runFunction(networkLoop, 1, NULL);
	return true;
}

void cleanup()
{
#ifdef _logFrames_
	logFile.close();
#endif
	LightTrail::deleteAllTrailsInstantly();
	delete ai;
	delete user, enemy;
	delete mgr;
	delete tracker;
	delete button;
	delete analog;
	
	enet_host_destroy(client);
	
	disconnect();
}

void print_tracker();

template<typename T>
T scale_tracker2cm(const T& value)
{
	static const float scale = OSGCSM::convert_length(cfg.getUnits(), 1.f, OSGCSM::CAVEConfig::CAVEUnitCentimeters);
	return value * scale;
}

auto head_orientation = Quaternion(-0.2f, 0.f, 0.f, 1.f);//Quaternion(Vec3f(1.f, 0.f, 0.f), Pi);
auto head_position = Vec3f(0.f, 170.f, 130.f);	// a 1.7m Person 2m in front of the scene

void VRPN_CALLBACK callback_head_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	head_orientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
	head_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));

	if(head_position.z() > WALL_Z_MAX) {
		head_orientation = Quaternion(-0.2f, 0.f, 0.f, 1.f);
		head_position = Vec3f(0.f, 170.f, 130.f);
	}
}

auto wand_orientation = Quaternion();
auto wand_position = Vec3f(0,135,0);
auto shield_orientation = Quaternion();
auto shield_position = Vec3f(0,135,0);
void VRPN_CALLBACK callback_wand_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	wand_orientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]) * Quaternion(Vec3f(0,1,0), osgDegree2Rad(180));
	wand_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));
}

void VRPN_CALLBACK callback_shield_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	shield_orientation = Quaternion(Vec3f(1,0,0), -osgDegree2Rad(30)) * Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]) * Quaternion(Vec3f(0,1,0), osgDegree2Rad(180));
	Vec3f shield_up;
	shield_orientation.multVec(Vec3f(0,1,0), shield_up);
	shield_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos))) - shield_up * 5;
}

auto analog_values = Vec3f();
void VRPN_CALLBACK callback_analog(void* userData, const vrpn_ANALOGCB analog)
{
	if (analog.num_channel >= 2)
		analog_values = Vec3f(analog.channel[0], 0, -analog.channel[1]);
}

#ifdef _logFrames_
bool isButtonPushed = false;
#endif

void VRPN_CALLBACK callback_button(void* userData, const vrpn_BUTTONCB button)
{
	if (button.button == 0) {
		if (button.state == 1) {
#ifdef _logFrames_
			if (!isButtonPushed) {
				logFile << "		inputSteps.push(InputStep(" << glutGet(GLUT_ELAPSED_TIME) << ", true));";
				isButtonPushed = true;
			}
#endif
			if(user->getDisk()->getState() == DISK_STATE_READY) {
				user->getDisk()->startDraw(wand_position);
			}
		} else {
#ifdef _logFrames_
			if (isButtonPushed) {
				logFile << "		inputSteps.push(InputStep(" << glutGet(GLUT_ELAPSED_TIME) << ", false));";
				isButtonPushed = false;
			}
#endif
			if(user->getDisk()->getState() == DISK_STATE_DRAWN) {
				user->getDisk()->endDraw(wand_position);
			}
		}
	} else if (!gameRunning && button.button == 1) {
		if (button.state == 1) {
			startGame();
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
			print_tracker();
			break;
		case 'f':
			showFPS = !showFPS;
			break;
		case 'g':
			if (playerId < 0) {
				startGame();
			} else {
				sendPacket(CTOS_PACKET_TYPE_START_GAME_REQUEST, new GameInformation(), sizeof(GameInformation), true);
			}
			break;
		case 'd':
			disconnect();
			break;
		case 'c':
			connect("127.0.0.1", 13244);
			break;
		case 'b':
			/* Create a reliable packet of size 7 containing "packet\0" */
			packet = enet_packet_create ("packet", 
										 strlen ("packet") + 1, 
										 ENET_PACKET_FLAG_RELIABLE);
			/* Send the packet to the peer over channel id 0. */
			enet_peer_send (peer, 0, packet);
			enet_host_flush(client);
			break;
#ifdef _simulate_
		case 'x':
			initSimulation();
			simStartTime = glutGet(GLUT_ELAPSED_TIME);
			break;
		case ' ':
			user->getDisk()->startDraw(Vec3f(-1,135,1));
			user->getDisk()->setPosition(Vec3f(0,135,0));
			user->getDisk()->endDraw(Vec3f(0,135,0));
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
		Vec3f forward(0,0,1);
		head_orientation.multVec(forward, forward);
		headLight->setDirection(forward);
		LightTrail::updateTrails(head_position);

		if (showFPS) {
			if (time - lastFPSUpdate > 1000) {
				std::cout << "FPS: " << fpsCount << '\n';
				fpsCount = 0;
				lastFPSUpdate = time;
			}
			fpsCount++;
		}

		user->setHeadRotation(head_orientation);
		user->setHeadPosition(head_position);
		user->setDiskArmRotation(wand_orientation);
		user->setDiskArmPosition(wand_position);
		user->setShieldArmRotation(shield_orientation);
		user->setShieldArmPosition(shield_position);
		user->update();

		if (playerId < 0) {
			ai->update();
		}
		enemy->update();

		if (playerId >= 0) {
			sendPlayerPosition();
		}

		updateAnimations();
		
		check_tracker();

#ifdef _logFrames_
		logFile << "		simSteps.push(SimStep(" << time << ", Vec3f(" << head_position << "), Quaternion(" << head_orientation.x() << ", " << head_orientation.y() << ", " << head_orientation.z() << ", " << head_orientation.w();
		logFile << "), Vec3f(" << wand_position << "), Quaternion(" << wand_orientation.x() << ", " << wand_orientation.y() << ", " << wand_orientation.z() << ", " << wand_orientation.w();
		logFile << ", Vec3f(" << shield_position << "), Quaternion(" << shield_orientation.x() << ", " << shield_orientation.y() << ", " << shield_orientation.z() << ", " << shield_orientation.w() << ")));\n";
#endif

#ifdef _simulate_
		if (simStartTime > 0) {
			SimStep t = getSimulationStep(time - simStartTime + 14500);
			wand_position = t.wand_position;
			wand_orientation = t.wand_orientation;
			head_position = t.head_position;
			head_orientation = t.head_orientation;
			InputStep i = getInputStep(time - simStartTime + 14500);
			if (i.buttonPushed) {
				if(user->getDisk()->getState() == DISK_STATE_READY) {
					user->getDisk()->startDraw(wand_position);
				}
			} else {
				if(user->getDisk()->getState() == DISK_STATE_DRAWN) {
					user->getDisk()->endDraw(wand_position);
				}
			}
		}
#endif
		
		const auto speed = 1.f;
		mgr->setUserTransform(head_position, head_orientation);
		//mgr->setTranslation(mgr->getTranslation() + speed * analog_values);
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

		InitTracker(cfg);

		MultiDisplayWindowRefPtr mwin = createAppWindow(cfg, cfg.getBroadcastaddress());

		if (!scene) 
			scene = buildScene();
		commitChanges();
		
#ifdef _logFrames_
		logFile.open ("example.txt");
#endif
#ifdef _simulate_
		initSimulation();
#endif
		user = new Player(userFaction, false);
		enemy = new Player(enemyFaction, true);
		user->setEnemy(enemy);
		enemy->setEnemy(user);
		ai = new AI(enemy);
		
		client = enet_host_create (NULL       /* create a client host */,
									1         /* only allow 1 outgoing connection */,
									2         /* allow up 2 channels to be used, 0 and 1 */,
									57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
									14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
		if (client == NULL)
		{
			std::cerr << "An error occurred while trying to create an ENet client host.\n";
			exit (EXIT_FAILURE);
		}

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
