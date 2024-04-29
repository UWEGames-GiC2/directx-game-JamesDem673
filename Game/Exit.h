#ifndef _EXIT_H_
#define _EXIT_H_
#include "CMOGO.h"

class Exit : public CMOGO
{

public:
	Exit(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale);
	~Exit();

	virtual void Tick(std::shared_ptr<GameData> _GD) override;
};

#endif
