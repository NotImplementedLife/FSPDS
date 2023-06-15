#pragma once
#include "DSC"

class SimpleScene : public DSC::GenericScene256
{
public:
	void init() override;
	~SimpleScene() = default;
};