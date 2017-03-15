#ifndef _Input_H_
#define _Input_H_

#include <map>
#include <string>

#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGVector.h>
#include <OpenSG/OSGQuaternion.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

#include "Common.h"

OSG_USING_NAMESPACE
	
class InputHandler {
public:
	virtual void handleHeadTracking(Vec3f position, Quaternion orientation) = 0;
	virtual void handleMainHandTracking(Vec3f position, Quaternion orientation) = 0;
	virtual void handleOffHandTracking(Vec3f position, Quaternion orientation) = 0;
	virtual void handleButtonUpdate(int buttonId, bool isPressed) = 0;
};


class Input;

typedef Input* (*InputCreation)(OSGCSM::CAVEConfig &cfg);

template<typename T> Input* createInput(OSGCSM::CAVEConfig &cfg) {
    return new T(cfg); 
}

class Input {
public:
	template<typename T> static void registerInput(std::string inputName) {
		InputCreation creatorFunction = &createInput<T>;
		__registeredInputs.insert(std::make_pair(inputName, creatorFunction));
	}

	static Input* create(std::string inputName, OSGCSM::CAVEConfig &cfg){
		if (__registeredInputs.find(inputName) == __registeredInputs.end()) {
			return nullptr;
		}
		return __registeredInputs[inputName](cfg);
	}

	void setInputHandler(InputHandler* handler);

	Vec3f getHeadPosition();
	Vec3f getMainHandPosition();
	Vec3f getOffHandPosition();
	Quaternion getHeadOrientation();
	Quaternion getMainHandOrientation();
	Quaternion getOffHandOrientation();

	void updateInputs();
#ifdef _simulate_
	void startSimulation();
#endif
	void printTracker();
protected:
	Input(OSGCSM::CAVEConfig &cfg);
	virtual void checkInputs() = 0;
	void setHeadData(Vec3f pos, Quaternion rot);
	void setMainHandData(Vec3f pos, Quaternion rot);
	void setOffHandData(Vec3f pos, Quaternion rot);
	void notifyButtonState(int buttonId, bool isPressed);
	OSGCSM::CAVEConfig getConfig();
private:
	static std::map<std::string, InputCreation> __registeredInputs;
	OSGCSM::CAVEConfig _cfg;
	Vec3f _headPosition;
	Vec3f _mainHandPosition;
	Vec3f _offHandPosition;
	Quaternion _headOrientation;
	Quaternion _mainHandOrientation;
	Quaternion _offHandOrientation;
	InputHandler* _inputHandler;
#ifdef _simulate_
	Real32 _simStartTime;
#endif
};

#endif