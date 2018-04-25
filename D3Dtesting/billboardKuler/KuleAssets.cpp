#include "DrawSystem.h"
#include "CDXInclude.h"
#include <random>

void KuleAssets::cleanup() {
	safeRelease(cpuPos);
	safeRelease(cpuVel);
	safeRelease(visualPos);
	safeRelease(visualVelocity);
	safeRelease(difuseColor);
	safeRelease(shaderConstants);
	safeRelease(kuleVertexMove);
	safeRelease(kuleVertexStatic);
	safeRelease(kuleVertexCopy);
	safeRelease(kuleGeometryMove);
	safeRelease(kuleGeometryCopy);
	safeRelease(kuleHull);
	safeRelease(kuleDomain);
	safeRelease(kulePixel);
	safeRelease(kuleCopyLayout);
	safeRelease(kuleMoveLayout);
	safeRelease(kuleStaticLayout);
	initialized = false;
	cleanHardware = true;
}

void KuleAssets::sampleGenerate(int seed) {
	static const int pScale = 10000;
	static const int vScale = 1000;
	static const int rMin = 100;
	static const int rDelta = 200;
	std::mt19937 gen(seed);
	if (data.positions) delete data.positions;
	if (data.velocities) delete data.velocities;
	if (data.colors) delete data.colors;
	data.positions = new INT32[numKule * 3];
	data.velocities = new INT32[numKule * 3];
	data.colors = new unsigned char[numKule * 4];
	for (int i = 0; i < numKule * 3; i++) {
		data.positions[i] = (gen() % pScale - pScale / 2);
		data.velocities[i] = (gen() % vScale - vScale / 2);
		data.colors[i + i/3] = gen() % 256;
	}
	for (int i = 0; i < numKule; i++) {
		data.radii[i] = (float) (gen() % rDelta + rMin);
		data.colors[i * 4 - 1] = 255;
	}
}

bool KuleAssets::draw(ID3D11DeviceContext *context, ID3D11RenderTargetView *target, ID3D11DepthStencilView* zBuffer, Camera& camera) {
	context->ClearState();
	if (cleanHardware) {
		context->VSSetShader(kuleVertexCopy, NULL, 0);
		ID3D11Buffer* buffers[2] = { cpuPos, cpuVel };
		UINT strides[] = { 12, 12 };
		UINT offsets[] = { 0, 0 };
		context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		context->IASetInputLayout(kuleCopyLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context->GSSetShader(kuleGeometryCopy, NULL, 0);
		buffers[0] = visualPosTarget;
		buffers[1] = visualVelocityTarget;
		context->SOSetTargets(2, buffers, offsets);
		context->Draw(numKule, 0);
	}
	else {
		context->VSSetShader(kuleVertexMove, NULL, 0);
		ID3D11Buffer* buffers[4] = { cpuPos, cpuVel, visualPos, visualVelocity };
		UINT strides[] = { 12, 12, 12, 12 };
		UINT offsets[] = { 0, 0, 0, 0 };
		context->IASetVertexBuffers(0, 4, buffers, strides, offsets);
		context->IASetInputLayout(kuleMoveLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context->GSSetShader(kuleGeometryMove, NULL, 0);
		buffers[0] = visualPosTarget;
		buffers[1] = visualVelocityTarget;
		context->SOSetTargets(2, buffers, offsets);
		context->Draw(numKule, 0);
	}
	ID3D11Buffer *tmp;
	tmp = visualPos;
	visualPos = visualPosTarget;
	visualPosTarget = tmp;
	tmp = visualVelocity;
	visualVelocity = visualVelocityTarget;
	visualVelocityTarget = tmp;

	context->VSSetShader(kuleVertexStatic, NULL, 0);
	ID3D11Buffer* buffers[3] = { visualPos, radius, difuseColor};
	UINT strides[] = { 12, 4, 4 };
	UINT offsets[] = { 0, 0, 0 };
	context->IASetVertexBuffers(0, 3, buffers, strides, offsets);
	context->IASetInputLayout(kuleStaticLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	context->HSSetShader(kuleHull, NULL, 0);
	context->DSSetShader(kuleDomain, NULL, 0);
	context->PSSetShader(kulePixel, NULL, 0);
	ID3D11RenderTargetView * targets[] = { target };
	context->OMSetRenderTargets(1, targets, NULL );


	return true;
}

bool KuleAssets::intializeAssets(ID3D11Device *dev) {
	if (initialized) return false;
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.ByteWidth = numKule * 3 * sizeof(INT32);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA init1 = { data.positions, 0, 0 };
	D3D11_SUBRESOURCE_DATA *pInit = &init1;
	if (data.positions == NULL) pInit = NULL;
	HRESULT res = dev->CreateBuffer(&bufferDesc, pInit, &cpuPos);
	if (FAILED(res)) {
		cleanup();
		return false;
	}

	D3D11_SUBRESOURCE_DATA init2 = { data.velocities, 0, 0 };
	pInit = &init2;
	if (data.velocities == NULL) pInit = NULL;
	res = dev->CreateBuffer(&bufferDesc, pInit, &cpuVel);
	if (FAILED(res)) {
		cleanup();
		return false;
	}

	bufferDesc.ByteWidth = numKule * 3 * sizeof(float);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	res = dev->CreateBuffer(&bufferDesc, NULL, &visualPos);
	if (FAILED(res)) {
		cleanup();
		return false;
	}
	res = dev->CreateBuffer(&bufferDesc, NULL, &visualPosTarget);
	if (FAILED(res)) {
		cleanup();
		return false;
	}

	res = dev->CreateBuffer(&bufferDesc, NULL, &visualVelocity);
	if (FAILED(res)) {
		cleanup();
		return false;
	}
	res = dev->CreateBuffer(&bufferDesc, NULL, &visualVelocityTarget);
	if (FAILED(res)) {
		cleanup();
		return false;
	}

	bufferDesc.ByteWidth = numKule * 4 * sizeof(unsigned char);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA init3 = { data.colors, 0, 0 };
	pInit = &init3;
	if (data.colors == NULL) pInit = NULL;
	res = dev->CreateBuffer(&bufferDesc, pInit, &difuseColor);
	if (FAILED(res)) {
		cleanup();
		return false;
	}

	bufferDesc.ByteWidth = numKule * sizeof(float);
	D3D11_SUBRESOURCE_DATA init4 = { data.radii, 0, 0 };
	pInit = &init4;
	if (data.radii == NULL) pInit = NULL;
	res = dev->CreateBuffer(&bufferDesc, pInit, &radius);
	if (FAILED(res)) {
		cleanup();
		return false;
	}

	static const int maxpath = 400;
	char currentDirectory[maxpath];
	int size = GetModuleFileNameA(NULL, currentDirectory, maxpath);
	CDXInclude* includer = new CDXInclude(currentDirectory);
	UINT compileOptions = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_OPTIMIZATION_LEVEL1;

	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	char shader[] = "#include \"kuleVertexMove.hlsl\"";
	res = D3DCompile(shader, sizeof(shader), "kuleVertexMove.hlsl", NULL, includer, "main", "vs_5_0", compileOptions, 0, &shaderBlob, &errorBlob);
	if (FAILED(res)) {
		if (errorBlob) {
			char* text = (char*)errorBlob->GetBufferPointer();
			MessageBox(NULL, text, "Compile error!", MB_OK);
		}
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	res = dev->CreateVertexShader((DWORD*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &kuleVertexMove);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[4];
	memset(inputDesc, 0, sizeof(inputDesc));
	inputDesc[0].SemanticName = "POSITION";
	inputDesc[1].SemanticName = "TEXCOORD";
	inputDesc[2].SemanticName = "POSITION";
	inputDesc[2].SemanticIndex = 1;
	inputDesc[3].SemanticName = "TEXCOORD";
	inputDesc[3].SemanticIndex = 1;
	inputDesc[0].Format = DXGI_FORMAT_R32G32B32_UINT;
	inputDesc[1].Format = DXGI_FORMAT_R32G32B32_UINT;
	inputDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot = 0;
	inputDesc[1].InputSlot = 1;
	inputDesc[2].InputSlot = 2;
	inputDesc[3].InputSlot = 3;
	// setInputSlotClass to D3D11_INPUT_PER_INSTANCE_DATA for dx10
	res = dev->CreateInputLayout(inputDesc, 4, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &kuleMoveLayout);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}

	D3D11_SO_DECLARATION_ENTRY streamOutputDecleration[2];
	streamOutputDecleration[0].Stream = 0;
	streamOutputDecleration[0].SemanticName = "POSITION";
	streamOutputDecleration[0].SemanticIndex = 0;
	streamOutputDecleration[0].StartComponent = 0;
	streamOutputDecleration[0].ComponentCount = 3;
	streamOutputDecleration[0].OutputSlot = 0;
	streamOutputDecleration[1] = streamOutputDecleration[0];
	streamOutputDecleration[1].SemanticName = "NORMAL";
	streamOutputDecleration[1].SemanticIndex = 0;
	streamOutputDecleration[1].Stream = 0;
	streamOutputDecleration[1].OutputSlot = 1;
	//UINT bufferStrides[] = {3, 3};
	res = dev->CreateGeometryShaderWithStreamOutput((void*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), streamOutputDecleration, 2, NULL, 0, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &kuleGeometryMove);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	//safeRelease(shaderBlob);
	//safeRelease(errorBlob);

	char shader2[] = "#include \"kuleVertexCopy.hlsl\"";
	res = D3DCompile(shader2, sizeof(shader2), "kuleVertexCopy.hlsl", NULL, includer, "main", "vs_5_0", compileOptions, 0, &shaderBlob, &errorBlob);
	if (FAILED(res)) {
		if (errorBlob) {
			char* text = (char*)errorBlob->GetBufferPointer();
			MessageBox(NULL, text, "Compile error!", MB_OK);
		}
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	res = dev->CreateVertexShader((DWORD*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &kuleVertexCopy);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}

	res = dev->CreateInputLayout(inputDesc, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &kuleMoveLayout);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}

	res = dev->CreateGeometryShaderWithStreamOutput((void*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), streamOutputDecleration, 2, NULL, 0, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &kuleGeometryCopy);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}


	//safeRelease(shaderBlob);
	//safeRelease(errorBlob);

	char shader1[] = "#include \"kuleVertexStatic.hlsl\"";
	res = D3DCompile(shader1, sizeof(shader1), "kuleVertexStatic.hlsl", NULL, includer, "main", "vs_5_0", compileOptions, 0, &shaderBlob, &errorBlob);
	if (FAILED(res)) {
		if (errorBlob) {
			char* text = (char*)errorBlob->GetBufferPointer();
			MessageBox(NULL, text, "Compile error!", MB_OK);
		}
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	res = dev->CreateVertexShader((DWORD*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &kuleVertexStatic);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}

	inputDesc[2].SemanticName = "COLOR";
	inputDesc[2].SemanticIndex = 0;
	inputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[1].Format = DXGI_FORMAT_R32_FLOAT;
	inputDesc[2].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	res = dev->CreateInputLayout(inputDesc, 3, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &kuleStaticLayout);

	//safeRelease(shaderBlob);
	//safeRelease(errorBlob);



	char shader3[] = "#include \"kuleHull.hlsl\"";
	res = D3DCompile(shader3, sizeof(shader3), "kuleHull.hlsl", NULL, includer, "main", "hs_5_0", compileOptions, 0, &shaderBlob, &errorBlob);
	if (FAILED(res)) {
		if (errorBlob) {
			char* text = (char*)errorBlob->GetBufferPointer();
			MessageBox(NULL, text, "Compile error!", MB_OK);
		}
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	res = dev->CreateHullShader((DWORD*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &kuleHull);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	//safeRelease(shaderBlob);
	//safeRelease(errorBlob);

	char shader4[] = "#include \"kuleDomain.hlsl\"";
	res = D3DCompile(shader4, sizeof(shader4), "kuleDomain.hlsl", NULL, includer, "main", "ds_5_0", compileOptions, 0, &shaderBlob, &errorBlob);
	if (FAILED(res)) {
		if (errorBlob) {
			char* text = (char*)errorBlob->GetBufferPointer();
			MessageBox(NULL, text, "Compile error!", MB_OK);
		}
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	res = dev->CreateDomainShader((DWORD*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &kuleDomain);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	//safeRelease(shaderBlob);
	//safeRelease(errorBlob);

	char shader5[] = "#include \"kulePixel.hlsl\"";
	res = D3DCompile(shader5, sizeof(shader5), "kulePixel.hlsl", NULL, includer, "main", "ps_5_0", compileOptions, 0, &shaderBlob, &errorBlob);
	if (FAILED(res)) {
		if (errorBlob) {
			char* text = (char*)errorBlob->GetBufferPointer();
			MessageBox(NULL, text, "Compile error!", MB_OK);
		}
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	res = dev->CreatePixelShader((DWORD*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &kulePixel);
	if (FAILED(res)) {
		safeRelease(shaderBlob);
		safeRelease(errorBlob);
		cleanup();
		return false;
	}
	safeRelease(shaderBlob);
	safeRelease(errorBlob);

	

	initialized = true;
	return true;
}