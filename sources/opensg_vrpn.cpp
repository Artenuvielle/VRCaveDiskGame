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

#include <OSGCSM/OSGCAVESceneManager.h>
#include <OSGCSM/OSGCAVEConfig.h>
#include <OSGCSM/appctrl.h>

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

OSG_USING_NAMESPACE

OSGCSM::CAVEConfig cfg;
OSGCSM::CAVESceneManager *mgr = nullptr;
vrpn_Tracker_Remote* tracker =  nullptr;
vrpn_Button_Remote* button = nullptr;
vrpn_Analog_Remote* analog = nullptr;

void cleanup()
{
	delete mgr;
	delete tracker;
	delete button;
	delete analog;
}

void print_tracker();

TransformRecPtr transCore;
ComponentTransformRecPtr movableTransform;
Vec3f diskDirection(0.f,0.f,0.f);
Real32 startTime;

NodeTransitPtr buildScene()
{
        NodeRecPtr root = Node::create();
        root->setCore(Group::create());

        NodeRecPtr boundingBoxModel = SceneFileHandler::the()->read("models/bbox.3DS");

        ComponentTransformRecPtr boundingBoxModelCT = ComponentTransform::create();
        //boundingBoxModelCT->setTranslation(Vec3f(0,170,180));
        boundingBoxModelCT->setTranslation(Vec3f(0,135,-405));
        boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
        boundingBoxModelCT->setScale(Vec3f(270.f,270.f,270.f));

        NodeRecPtr boundingBoxModelTrans = makeNodeFor(boundingBoxModelCT);
        boundingBoxModelTrans->addChild(boundingBoxModel);

        root->addChild(boundingBoxModelTrans);


        NodeRecPtr diskModel = SceneFileHandler::the()->read("models/disk.3DS");
        movableTransform = ComponentTransform::create();
        //movableTransform->setTranslation(Vec3f(0,167,196));
        movableTransform->setTranslation(Vec3f(0,0,0));
        movableTransform->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
        movableTransform->setScale(Vec3f(10.f,10.f,10.f));

        NodeRecPtr diskModelTrans = makeNodeFor(movableTransform);
        diskModelTrans->addChild(diskModel);

        root->addChild(diskModelTrans);

        // you will see a donut at the floor, slightly skewed, depending on head_position
        return NodeTransitPtr(root);
}

template<typename T>
T scale_tracker2cm(const T& value)
{
	static const float scale = OSGCSM::convert_length(cfg.getUnits(), 1.f, OSGCSM::CAVEConfig::CAVEUnitCentimeters);
	return value * scale;
}

auto head_orientation = Quaternion(Vec3f(0.f, 1.f, 0.f), 3.141f);
auto head_position = Vec3f(0.f, 187.f, 100.f);	// a 1.7m Person 2m in front of the scene

bool out1 = true;
bool out2 = true;

void VRPN_CALLBACK callback_head_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	head_orientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
	head_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));

        if (out1) {
            out1 = false;
            std::cout << "Head position: " << head_position << " orientation: " << head_orientation << '\n';
        }
}

auto wand_orientation = Quaternion();
auto wand_position = Vec3f();
void VRPN_CALLBACK callback_wand_tracker(void* userData, const vrpn_TRACKERCB tracker)
{
	wand_orientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
	wand_position = Vec3f(scale_tracker2cm(Vec3d(tracker.pos)));
	
        /*Matrix m;
	m.setIdentity();
	m.setTranslate(wand_position);
	m.setRotate(wand_orientation);
        tPlayerDisk->setMatrix(m);*/
        movableTransform->setTranslation(wand_position);
        movableTransform->setRotation(wand_orientation);
        if (out2) {
            out2 = false;
            std::cout << "Wand position: " << wand_position << " orientation: " << wand_orientation << '\n';
        }
}

auto analog_values = Vec3f();
void VRPN_CALLBACK callback_analog(void* userData, const vrpn_ANALOGCB analog)
{
	if (analog.num_channel >= 2)
		analog_values = Vec3f(analog.channel[0], 0, -analog.channel[1]);
}

void VRPN_CALLBACK callback_button(void* userData, const vrpn_BUTTONCB button)
{
	if (button.button == 0 && button.state == 1)
		print_tracker();
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
                        movableTransform->setTranslation(movableTransform->getTranslation() + Vec3f(0,1,0));
                        break;
                case 's':
                        movableTransform->setTranslation(movableTransform->getTranslation() - Vec3f(0,1,0));
                        break;
                case 'a':
                        movableTransform->setTranslation(movableTransform->getTranslation() + Vec3f(0,0,1));
                        break;
                case 'd':
                        movableTransform->setTranslation(movableTransform->getTranslation() - Vec3f(0,0,1));
                        break;
                case 'x':
                        std::cout << "bounding box position: " << movableTransform->getTranslation() << '\n';
                        break;
                case ' ':
                        diskDirection = Vec3f(0.f,0.f,-1.f);
                        diskDirection.normalize();
                        startTime = glutGet(GLUT_ELAPSED_TIME);
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
		check_tracker();
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

		mgr = new OSGCSM::CAVESceneManager(&cfg);
		mgr->setWindow(mwin );
		mgr->setRoot(scene);
		mgr->showAll();
		mgr->getWindow()->init();
		mgr->turnWandOff();
	}
	catch(const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	glutMainLoop();
}
