////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_FogShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename[128];
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	// Set the file name of the model.
    strcpy_s(modelFilename, "assets/models/cube.txt");

    // Set the file name of the texture.
    strcpy_s(textureFilename, "assets/textures/stone01.tga");

    // Create and initialize the model object.
    m_Model = new ModelClass;

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
    if(!result)
    {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

	// Create and initialize the texture shader object.
	m_FogShader = new FogShaderClass{};

	result = m_FogShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the render to texture object.
	

	return true;
}


void ApplicationClass::Shutdown()
{


	// Release the texture shader object.
	if(m_FogShader)
	{
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = 0;
	}

	// Release the model object.
    if(m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	static float rotation = 0.0f;
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if(Input->IsEscapePressed())
	{
		return false;
	}

	// Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.25f;
    if(rotation < 0.0f)
    {
        rotation += 360.0f;
    }

	

	// Render the final graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}



bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float fogColor, fogStart, fogEnd;
	bool result;

	fogColor = 0.5f;

	fogStart = 0.f;
	fogEnd = 10.f;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	// Set the position of the camera for viewing the display planes with the render textures on them.

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Setup matrices - Top display plane.
    worldMatrix = XMMatrixRotationY(rotation);



	// Render the display plane using the texture shader and the render texture resource.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_FogShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), fogStart, fogEnd);
	if(!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}