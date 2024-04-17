#ifndef _VIEWRADIUS_H_
#define _VIEWRADIUS_H_
#include "CMOGO.h"

class ViewRadius : public CMOGO
{

public:
	ViewRadius(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scaleF);
	~ViewRadius();

	virtual void Tick(GameData* _GD) override;

};
#endif