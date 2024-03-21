#ifndef _WALL_H_
#define _WALL_H_
#include "CMOGO.h"

class Wall : public CMOGO
{

public:
	Wall(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scaleF);
	~Wall();

	virtual void Tick(GameData* _GD) override;

};

#endif