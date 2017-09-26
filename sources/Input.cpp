#include "Input.h"

#include "Common.h"

#ifdef _simulate_
#include "Simulation.h"
#endif

template<typename T>
T scale_tracker2cm(const T& value, OSGCSM::CAVEConfig &cfg)
{
	static const float scale = OSGCSM::convert_length(cfg.getUnits(), 1.f, OSGCSM::CAVEConfig::CAVEUnitCentimeters);
	return value * scale;
}

Input::Input(OSGCSM::CAVEConfig &cfg) : _cfg(cfg) {
	_headOrientation = Quaternion(-0.2f, 0.f, 0.f, 1.f);
	_headPosition = Vec3f(0.f, 170.f, 130.f);
	_mainHandOrientation = Quaternion();
	_mainHandPosition = Vec3f(0,135,0);
	_offHandOrientation = Quaternion();
	_offHandPosition = Vec3f(0,135,0);
#ifdef _simulate_
	_simStartTime = 0;
	initSimulation();
#endif
}

void Input::setInputHandler(InputHandler* handler) {
	_inputHandler = handler;
}

Vec3f Input::getHeadPosition() {
	return _headPosition;
}

Vec3f Input::getMainHandPosition() {
	return _mainHandPosition;
}

Vec3f Input::getOffHandPosition() {
	return _offHandPosition;
}

Quaternion Input::getHeadOrientation() {
	return _headOrientation;
}

Quaternion Input::getMainHandOrientation() {
	return _mainHandOrientation;
}

Quaternion Input::getOffHandOrientation() {
	return _offHandOrientation;
}

void Input::updateInputs() {	
#ifdef _simulate_
	if (_simStartTime > 0) {
		int time = glutGet(GLUT_ELAPSED_TIME);
		SimStep t = getSimulationStep(time - _simStartTime + 14500);
		setHeadData(t.head_position, t.head_orientation);
		setMainHandData(t.wand_position, t.wand_orientation);
		InputStep i = getInputStep(time - _simStartTime + 14500);
		notifyButtonState(0, i.buttonPushed);
	}
#endif
	checkInputs();
}

#ifdef _simulate_
void Input::startSimulation() {
	initSimulation();
	_simStartTime = glutGet(GLUT_ELAPSED_TIME);
}
#endif

void Input::printTracker() {
	std::cout << "Head position: " << _headPosition << " orientation: " << _headOrientation << std::endl;
	std::cout << "Wand position: " << _mainHandPosition << " orientation: " << _mainHandOrientation << std::endl;
	std::cout << "Shield position: " << _offHandPosition << " orientation: " << _offHandOrientation << std::endl;
}

void Input::setHeadData(Vec3f pos, Quaternion rot) {
	_headPosition = pos;
	_headOrientation = rot;
	_inputHandler->handleHeadTracking(pos, rot);
}

void Input::setMainHandData(Vec3f pos, Quaternion rot) {
	_mainHandPosition = pos;
	_mainHandOrientation = rot;
	_inputHandler->handleMainHandTracking(pos, rot);
}

void Input::setOffHandData(Vec3f pos, Quaternion rot) {
	_offHandPosition = pos;
	_offHandOrientation = rot;
	_inputHandler->handleOffHandTracking(pos, rot);
}

void Input::notifyButtonState(int buttonId, bool isPressed) {
	_inputHandler->handleButtonUpdate(buttonId, isPressed);
}

OSGCSM::CAVEConfig Input::getConfig() {
	return _cfg;
}



void VRPN_CALLBACK callbackHeadTracker(void* userData, const vrpn_TRACKERCB tracker);
void VRPN_CALLBACK callbackMainHandTracker(void* userData, const vrpn_TRACKERCB tracker);
void VRPN_CALLBACK callbackOffHandTracker(void* userData, const vrpn_TRACKERCB tracker);
void VRPN_CALLBACK callbackButton(void* userData, const vrpn_BUTTONCB button);

class VRPNInput : public Input {
public:
	VRPNInput(OSGCSM::CAVEConfig &cfg) : Input(cfg) {
		_tracker = nullptr;
		_button = nullptr;
		try	{
			const char* const vrpn_name = "DTrack@localhost";
			_tracker = new vrpn_Tracker_Remote(vrpn_name);
			_tracker->shutup = true;
			_tracker->register_change_handler(this, callbackHeadTracker, cfg.getSensorIDHead());
			_tracker->register_change_handler(this, callbackMainHandTracker, cfg.getSensorIDController());
			_tracker->register_change_handler(this, callbackOffHandTracker, 3);
			_button = new vrpn_Button_Remote(vrpn_name);
			_button->shutup = true;
			_button->register_change_handler(this, callbackButton);
		} catch(const std::exception& e) {
			std::cout << "ERROR: " << e.what() << '\n';
			return;
		}
	}

	~VRPNInput() {
		delete _tracker;
		delete _button;
	}

	void updateHeadTracker(const vrpn_TRACKERCB tracker) {
		Quaternion headOrientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]);
		Vec3f headPosition = Vec3f(scale_tracker2cm(Vec3d(tracker.pos), getConfig()));

		if(headPosition.z() > WALL_Z_MAX) {
			headOrientation = Quaternion(-0.2f, 0.f, 0.f, 1.f);
			headPosition = Vec3f(0.f, 170.f, 130.f);
		}
		setHeadData(headPosition, headOrientation);
	}

	void updateMainHandTracker(const vrpn_TRACKERCB tracker) {
		Quaternion mainHandOrientation = Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]) * Quaternion(Vec3f(0,1,0), osgDegree2Rad(180));
		Vec3f mainHandPosition = Vec3f(scale_tracker2cm(Vec3d(tracker.pos), getConfig()));
		setMainHandData(mainHandPosition, mainHandOrientation);
	}

	void updateOffHandTracker(const vrpn_TRACKERCB tracker) {
		Quaternion offHandOrientation = Quaternion(Vec3f(1,0,0), -osgDegree2Rad(30)) * Quaternion(tracker.quat[0], tracker.quat[1], tracker.quat[2], tracker.quat[3]) * Quaternion(Vec3f(0,1,0), osgDegree2Rad(180));
		Vec3f shield_up;
		offHandOrientation.multVec(Vec3f(0,1,0), shield_up);
		Vec3f offHandPosition = Vec3f(scale_tracker2cm(Vec3d(tracker.pos), getConfig())) - shield_up * 5;
		setOffHandData(offHandPosition, offHandOrientation);
	}

	void updateButton(const vrpn_BUTTONCB button) {
		notifyButtonState(button.button, button.state == 1);
	}

	void checkInputs() {
		_tracker->mainloop();
		_button->mainloop();
	}
private:
	vrpn_Tracker_Remote* _tracker;
	vrpn_Button_Remote* _button;
};

void VRPN_CALLBACK callbackHeadTracker(void* userData, const vrpn_TRACKERCB tracker) {
	reinterpret_cast<VRPNInput*>(userData)->updateHeadTracker(tracker);
}

void VRPN_CALLBACK callbackMainHandTracker(void* userData, const vrpn_TRACKERCB tracker) {
	reinterpret_cast<VRPNInput*>(userData)->updateMainHandTracker(tracker);
}

void VRPN_CALLBACK callbackOffHandTracker(void* userData, const vrpn_TRACKERCB tracker) {
	reinterpret_cast<VRPNInput*>(userData)->updateOffHandTracker(tracker);
}

void VRPN_CALLBACK callbackButton(void* userData, const vrpn_BUTTONCB button) {
	reinterpret_cast<VRPNInput*>(userData)->updateButton(button);
}


std::map<std::string, InputCreation> Input::__registeredInputs;

template <typename T>
class Registrar {
public:
	Registrar(std::string name) {
		Input::registerInput<T>(name);
	}
};

Registrar<VRPNInput> vrpn("VRPN");


//std::map<std::string, InputCreation> inputPossibilities;
//inputPossibilities.emplace("VRPN", getInputCreation<VRPNInput>);
