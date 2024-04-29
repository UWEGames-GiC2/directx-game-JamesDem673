//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include <list>
#include "CommonStates.h"
#include "../DirectXTK/Inc/Effects.h" //this clashes with a SDK file so must explitily state it
#include "Keyboard.h"
#include "Mouse.h"
#include "Audio.h"
#include "CMOGO.h"

#define ROW 21
#define COLUMN 21
#define WALL 0
#define EMPTY 1

using std::list;

// Forward declarations
struct GameData;
struct DrawData;
struct DrawData2D;
class GameObject;
class GameObject2D;
class Camera;
class TPSCamera;
class Light;
class Sound;
class Player;
class Monster;
class Exit; 
class Terrain;
class ImageGO2D;
class TextGO2D;
class Wall;
class ViewRadius;
class CameraHolder;
class Gazebo;
class Loop;
class TestSound;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND _window, int _width, int _height);
    void DisplayMenu();
    void DisplayGame();
    void DisplayWin();
    void DisplayLoss();
    void DisplayLore();
    void CreateMazeFromArray();
    void DisplayJumpsScare();

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int _width, int _height);

    // Properties
    void GetDefaultSize( int& _width, int& _height ) const noexcept;

private:

    void Update(DX::StepTimer const& _timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();
    void dfs(int r, int c);

    void goUpFloor();
    void goDownFloor();

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

    //Scarle Added stuff
    std::shared_ptr<GameData> m_GD = NULL;			//Data to be shared to all Game Objects as they are ticked
    std::shared_ptr<DrawData> m_DD = NULL;			//Data to be shared to all 3D Game Objects as they are drawn
    std::shared_ptr<DrawData2D> m_DD2D = NULL;	    //Data to be passed by game to all 2D Game Objects via Draw 

    //Basic 3D renderers
    std::shared_ptr<Camera> m_cam = NULL; //principle camera
    std::shared_ptr<TPSCamera> m_TPScam = NULL;//TPS cam
    std::shared_ptr<Light> m_light = NULL; //base light

    //maze 
    int grid[ROW][COLUMN]
        = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},};

    //GameObjects
    std::shared_ptr<Player> pPlayer = NULL;
    std::shared_ptr<Monster> npcMonster = NULL;
    std::shared_ptr<Exit> exitGate = NULL;
    int mazeGridSize = 21;
    float tileSize = 0.15f;
    std::shared_ptr<ViewRadius> vRadius = NULL;
    std::shared_ptr<CameraHolder> cHolder = NULL;
    std::shared_ptr<Gazebo> house = NULL;
    
    std::shared_ptr<Terrain> passableRoof = NULL;

    //2D Game Objects
    std::shared_ptr<ImageGO2D> mainMenu = NULL;
    std::shared_ptr<ImageGO2D> lossText = NULL;
    std::shared_ptr<ImageGO2D> winMenu = NULL;
    std::shared_ptr<ImageGO2D> blackScreen = NULL;
    std::shared_ptr<ImageGO2D> jumpScare = NULL;
    std::shared_ptr<TextGO2D> startGameText = NULL;
    std::shared_ptr<TextGO2D> loreScreen = NULL;

    std::shared_ptr<ImageGO2D> fuelMeterShell = NULL;
    std::shared_ptr<ImageGO2D> fuelMeter[20] = { NULL,NULL,NULL,NULL,NULL,
                                NULL,NULL,NULL,NULL,NULL,
                                NULL,NULL,NULL,NULL,NULL,
                                NULL,NULL,NULL,NULL,NULL};
    int MeterCount = 19;


    //Audio
    std::shared_ptr<Loop> levelOneMusic = NULL;
    std::shared_ptr<Loop> levelTwoMusic = NULL;
    std::shared_ptr<Loop> LimboMusic = NULL;
    std::shared_ptr<Loop> WinMusic = NULL;
    std::shared_ptr<Loop> LossMusic = NULL;
    std::shared_ptr<TestSound> JumpScareMusic = NULL;
    std::shared_ptr<TestSound> FootStepOne = NULL;
    std::shared_ptr<TestSound> FootStepTwo = NULL;
    float footstepVolume = 0.5;
    int lastStepPlayed = 0;

    bool WinScreenPlayOnce = false;
    bool LossScreenPlayOnce = false;
    bool JumpscarePlayOnce = false;
    int jumpscareTimer = 0;

    //Pathfinding
    int currentTileX = 0;
    int currentTileZ = 1;
    int tempTrack = 0;

    bool monsterCanMove = false;
    bool noLight = false;
    bool secondRound = false;
    Vector3 prevPosition = Vector3(0, 0, 0);

    //required for the CMO model rendering system
    std::shared_ptr<DirectX::CommonStates> m_states = NULL;
    DirectX::IEffectFactory* m_fxFactory = NULL;

    //basic keyboard and mouse input system
    void ReadInput(); //Get current Mouse and Keyboard states
    std::shared_ptr<DirectX::Keyboard> m_keyboard;
    std::shared_ptr<DirectX::Mouse> m_mouse;

    std::vector<std::shared_ptr<GameObject>> m_GameObjects; //data structure to hold pointers to the 3D Game Objects
    std::vector<std::shared_ptr<GameObject2D>> m_GameObjects2D; //data structure to hold pointers to the 2D Game Objects 

    //list<CMOGO*> m_CMOGameObjects; //data structure to hold pointers to all 3D CMO Game Objects
    //list<CMOGO*> m_PhysicsObjects

    std::vector<std::shared_ptr<CMOGO>> m_ColliderObjects;
    std::vector<std::shared_ptr<CMOGO>> m_PhysicsObjects;

    void CheckCollision();
                                         
    //sound stuff
	//This uses a simple system, but a better pipeline can be used using Wave Banks
	//See here: https://github.com/Microsoft/DirectXTK/wiki/Creating-and-playing-sounds Using wave banks Section
    std::shared_ptr<DirectX::AudioEngine> m_audioEngine;
    std::vector<Sound*>m_Sounds;
};
