#pragma once
class Effect
{
	ID3D11RasterizerState					*RasterizerState = nullptr;
	ID3D11DepthStencilState					*DepthStencilState = nullptr;
	ID3D11BlendState						*BlendState = nullptr;
	ID3D11VertexShader						*VertexShader = nullptr;
	ID3D11PixelShader						*PixelShader = nullptr;
	ID3D11GeometryShader					*GeometryShader = nullptr;
	ID3D11InputLayout						*VSInputLayout = nullptr;

	//Apply blendState
	FLOAT			blendFactor[4];
	UINT			sampleMask;

	uint32_t LoadShader(const char *filename, char **bytecode);
public:
	Effect(ID3D11Device *device, ID3D11VertexShader	*_VertexShader, ID3D11PixelShader *_PixelShader, ID3D11InputLayout *_VSInputLayout);
	Effect(ID3D11Device *device, ID3D11VertexShader	*_VertexShader, ID3D11PixelShader *_PixelShader, ID3D11GeometryShader *_GeometryShader, ID3D11InputLayout *_VSInputLayout);

	Effect(ID3D11Device *device, const char *vertexShaderPath, const char *pixelShaderPath, const D3D11_INPUT_ELEMENT_DESC vertexDesc[], UINT numVertexElements);
	Effect(ID3D11Device *device, const char *vertexShaderPath, const char *pixelShaderPath, const char *geometryShaderPath, const D3D11_INPUT_ELEMENT_DESC vertexDesc[], UINT numVertexElements);

	ID3D11InputLayout *getVSInputLayout(){ return VSInputLayout; };
	ID3D11VertexShader *getVertexShader(){ return VertexShader; };
	ID3D11PixelShader *getPixelShader(){ return PixelShader; };
	ID3D11GeometryShader *getGeometryShader(){ return GeometryShader; };
	ID3D11RasterizerState	* getRasterizerState(){ return RasterizerState; };
	ID3D11DepthStencilState	* getDepthStencilState(){ return DepthStencilState; };
	ID3D11BlendState	* getBlendState(){ return BlendState; };

	void setPixelShader(ID3D11PixelShader	*_PixelShader){ PixelShader = _PixelShader; };
	void setGeometryShader(ID3D11GeometryShader	*_GeometryShader){ GeometryShader = _GeometryShader; };
	void setVertexShader(ID3D11VertexShader	*_VertexShader){ VertexShader = _VertexShader; };
	void setVSInputLayout(ID3D11InputLayout	*_VSInputLayout){ VSInputLayout = _VSInputLayout; };
	void setRasterizerState(ID3D11RasterizerState	*_RasterizerState){ RasterizerState = _RasterizerState; };
	void setDepthStencilState(ID3D11DepthStencilState	*_DepthStencilState){ DepthStencilState = _DepthStencilState; };
	void setBlendState(ID3D11BlendState	*_BlendState){ BlendState = _BlendState; };
	void initDefaultStates(ID3D11Device *device);
	void bindPipeline(ID3D11DeviceContext *context);

	uint32_t Effect::CreateVertexShader(ID3D11Device *device, const char *filename, char **VSBytecode, ID3D11VertexShader **vertexShader);
	HRESULT Effect::CreatePixelShader(ID3D11Device *device, const char *filename, char **PSBytecode, ID3D11PixelShader **pixelShader);
	HRESULT Effect::CreateGeometryShader(ID3D11Device *device, const char *filename, char **GSBytecode, ID3D11GeometryShader **geometryShader);
	~Effect();
};

