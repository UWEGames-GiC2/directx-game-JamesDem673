#ifndef _GAZEBO_H_
#define _GAZEBO_H_
#include "CMOGO.h"

class Gazebo : public CMOGO
{

public:
	Gazebo(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scaleF);
	~Gazebo();

	virtual void Tick(GameData* _GD) override;

};

#endif