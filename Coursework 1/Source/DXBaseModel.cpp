
#include "stdafx.h"
#include <DXBaseModel.h>


DXBaseModel::~DXBaseModel() {

	if (vertexBuffer)
		vertexBuffer->Release();

	if (indexBuffer)
		indexBuffer->Release();

	if (inputLayout)
		inputLayout->Release();
}
