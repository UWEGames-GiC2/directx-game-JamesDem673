#ifndef _CAMERAHOLDER_H_
#define _CAMERAHOLDER_H
#include "CMOGO.h"

class CameraHolder : public CMOGO
{

public:
	CameraHolder(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scaleF);
	~CameraHolder();

	virtual void Tick(std::shared_ptr<GameData> _GD) override;
};

#endif