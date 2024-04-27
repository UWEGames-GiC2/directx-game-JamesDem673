//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <time.h>
#include <iostream>
#include <string>

    //Scarle Headers
#include "GameData.h"
#include "GameState.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"

#include "CMOGO.h"
#include <DirectXCollision.h>
#include "Collision.h"
#include <random>

    extern void ExitGame() noexcept;

    using namespace DirectX;

    using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(1920),
    m_outputHeight(1080),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND _window, int _width, int _height)
{
    m_window = _window;
    m_outputWidth = 1920;
    m_outputHeight = 1080;

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    //seed the random number generator
    srand((UINT)time(NULL));

    //set up keyboard and mouse system
    //documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(_window);
    m_mouse->SetMode(Mouse::MODE_RELATIVE);
    //Hide the mouse pointer
    ShowCursor(false);

    //create GameData struct and populate its pointers
    m_GD = new GameData;
    m_GD->m_GS = GS_MENU;

    //set up systems for 2D rendering
    m_DD2D = new DrawData2D();
    m_DD2D->m_Sprites.reset(new SpriteBatch(m_d3dContext.Get()));
    m_DD2D->m_Font.reset(new SpriteFont(m_d3dDevice.Get(), L"..\\Assets\\italic.spritefont"));
    m_states = new CommonStates(m_d3dDevice.Get());

    //set up DirectXTK Effects system
    m_fxFactory = new EffectFactory(m_d3dDevice.Get());
    //Tell the fxFactory to look to the correct build directory to pull stuff in from
    ((EffectFactory*)m_fxFactory)->SetDirectory(L"..\\Assets");
    //init render system for VBGOs
    VBGO::Init(m_d3dDevice.Get());

    //set audio system
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);

    //create a set of dummy things to show off the engine

    //find how big my window is to correctly calculate my aspect ratio
    float AR = (float)_width / (float)_height;


    //Create Grid for textured ground

    int floorGridX = mazeGridSize - 1;
    int floorGridZ = mazeGridSize - 1;
    float spacingX = 15.0f;
    float spacingZ = 15.0f;

    //Creates maze floor
    for (int x = 0; x <= floorGridX; x++) {
        for (int z = 0; z <= floorGridZ; z++) {
            Vector3 position(x * spacingX + 7.5, 2.5f, z * spacingZ + 7.5);
            Terrain* forLoopTiles = new Terrain("groundTile", m_d3dDevice.Get(), m_fxFactory, position, 0.0f, 0.0f, 0.0f, tileSize * Vector3::One);
            forLoopTiles->setTerrain(true);
            m_GameObjects.push_back(forLoopTiles);
            m_ColliderObjects.push_back(forLoopTiles);
        }
    }

    //Creates starting room floor
    for (int x = -3; x <= -1; x++) {
        for (int z = 0; z <= 2; z++)
        {
            Vector3 position(x * spacingX + 7.5, 2.5f, z * spacingZ + 7.5);
            Terrain* forLoopTiles = new Terrain("groundTile", m_d3dDevice.Get(), m_fxFactory, position, 0.0f, 0.0f, 0.0f, tileSize * Vector3::One);
            forLoopTiles->setTerrain(true);
            m_GameObjects.push_back(forLoopTiles);
            m_ColliderObjects.push_back(forLoopTiles);
        }
    }

    //Create maze roof
    int ceilingGridX = mazeGridSize - 1;
    int ceilingGridZ = mazeGridSize - 1;

    for (int x = 0; x <= ceilingGridX; x++) {
        for (int z = 0; z <= ceilingGridZ; z++) {
            Vector3 position(x * spacingX + 7.5, 17.5f, z * spacingZ + 7.5);
            Terrain* forLoopTiles = new Terrain("groundTile", m_d3dDevice.Get(), m_fxFactory, position, 0.0f, 0.0f, 0.0f, tileSize * Vector3::One);
            forLoopTiles->setTerrain(true);
            m_GameObjects.push_back(forLoopTiles);
            m_ColliderObjects.push_back(forLoopTiles);
        }
    }

    // Creates starting room roof
    for (int x = -3; x <= -1; x++) {
        for (int z = 0; z <= 2; z++)
        {
            Vector3 position(x * spacingX + 7.5, 17.5f, z * spacingZ + 7.5);
            Terrain* forLoopTiles = new Terrain("groundTile", m_d3dDevice.Get(), m_fxFactory, position, 0.0f, 0.0f, 0.0f, tileSize * Vector3::One);
            forLoopTiles->setTerrain(true);
            m_GameObjects.push_back(forLoopTiles);

            if (x != -2 && z != 2)
                m_ColliderObjects.push_back(forLoopTiles);
        }
    }

    int GrassSpacing = 45;
    //Creates grass plain
    for (int x = -10 ; x <= 10; x++) {
        for (int z = -10; z <= 10; z++) {
            Vector3 position(x * GrassSpacing + 7.5, 18.0f, z * GrassSpacing + 7.5);
            Terrain* forLoopTiles = new Terrain("grassTile", m_d3dDevice.Get(), m_fxFactory, position, 0.0f, 0.0f, 0.0f, tileSize * Vector3::One);
            forLoopTiles->setTerrain(true);
            m_GameObjects.push_back(forLoopTiles);

            if (x != -2 && z != 2)
                m_ColliderObjects.push_back(forLoopTiles);
        }
    }


    //creates passable tiles so the player can fall back into the starting room
    Terrain* passableRoof = new Terrain("groundTile", m_d3dDevice.Get(), m_fxFactory, Vector3(-22.5f, 17.5f, 22.5f), 0.0f, 0.0f, 0.0f, tileSize * Vector3::One);
    passableRoof->setTerrain(true);
    m_GameObjects.push_back(passableRoof);

    //Creates walls for starting room
    for (int i = 0; i < 3; i++)
    {
        Wall* startwall = new Wall("wallModel", m_d3dDevice.Get(), m_fxFactory, Vector3((- 15.0f * i) - 7.5f, 3.0f, -7.5f), 0.0f, 0.0f, 0.0f, Vector3(tileSize, tileSize, tileSize));
        startwall->setTerrain(true);
        m_GameObjects.push_back(startwall);
        m_ColliderObjects.push_back(startwall);
    }

    for (int i = 0; i < 3; i++)
    {
        Wall* startwall3 = new Wall("wallModel", m_d3dDevice.Get(), m_fxFactory, Vector3(-52.5f, 3.0f, 7.5f + (i * 15.0f)), 0.0f, 0.0f, 0.0f, Vector3(tileSize, tileSize, tileSize));
        startwall3->setTerrain(true);
        m_GameObjects.push_back(startwall3);
        m_ColliderObjects.push_back(startwall3);
    }

    for (int i = 0; i < 3; i++)
    {
        Wall* startwall = new Wall("wallModel", m_d3dDevice.Get(), m_fxFactory, Vector3((-15.0f * i) - 7.5f, 3.0f, 52.5f), 0.0f, 0.0f, 0.0f, Vector3(tileSize, tileSize, tileSize));
        startwall->setTerrain(true);
        m_GameObjects.push_back(startwall);
        m_ColliderObjects.push_back(startwall);
    }


    //add Player
    pPlayer = new Player("PlayerModel", m_d3dDevice.Get(), m_fxFactory);
    pPlayer->SetPos(Vector3(pPlayer->GetPos().x, 2.5, pPlayer->GetPos().z));
    m_GameObjects.push_back(pPlayer);
    m_PhysicsObjects.push_back(pPlayer);

    //add camera holder
    cHolder = new CameraHolder("ViewRadius", m_d3dDevice.Get(), m_fxFactory, Vector3(pPlayer->GetPos().x, 10.0f, pPlayer->GetPos().z), 0.0f, 0.0f, 0.0f, Vector3::One);
    m_GameObjects.push_back(cHolder);

    //add ViewRadius
    vRadius = new ViewRadius("ViewRadius", m_d3dDevice.Get(), m_fxFactory, Vector3(pPlayer->GetPos().x, pPlayer->GetPos().y, pPlayer->GetPos().z), 0.0f, 0.0f, 0.0f, Vector3::One * 25);
    m_GameObjects.push_back(vRadius);

    //add Monster
    npcMonster = new Monster("MonsterModel", m_d3dDevice.Get(), m_fxFactory, Vector3(157.5f, 3.0f, 142.5f), 0.0f, 0.0f, 0.0f, Vector3::One);
    m_GameObjects.push_back(npcMonster);
    m_PhysicsObjects.push_back(npcMonster);

    //add Exit
    // in game pos: Vector3(300.0f, 5.0f, 292.5f)         testing pos: (7.5f, 5.0f, 22.5f)
    exitGate = new Exit("ExitModel", m_d3dDevice.Get(), m_fxFactory, Vector3(300.0f, 5.0f, 292.5f), 0.0f, 0.0f, 0.0f, Vector3::One);
    m_GameObjects.push_back(exitGate);
    m_ColliderObjects.push_back(exitGate);

    //add house
    house = new Gazebo("Gazebo", m_d3dDevice.Get(), m_fxFactory, Vector3(-23 - 7.5f, 18.5f, 22.5f + 7.5f), 0.0f, 316.9f, 0.0f, Vector3::One * 10);
    m_GameObjects.push_back(house);

    CreateMazeFromArray();

    //create a base camera
    m_TPScam = new TPSCamera(0.25f * XM_PI, AR, 4.0f, 10000.0f, cHolder, Vector3::UnitY, Vector3(0.0f, 5.0f, 0.01f));
    m_GameObjects.push_back(m_TPScam);

    //create a main menu
    mainMenu = new ImageGO2D("mainMenu", m_d3dDevice.Get());
    mainMenu->SetPos(Vector2(m_outputWidth / 2, m_outputHeight / 2));
    mainMenu->SetScale(2.0f);
    m_GameObjects2D.push_back(mainMenu);
#
    //create a loss screen
    lossMenu = new ImageGO2D("lossScreen", m_d3dDevice.Get());
    lossMenu->SetPos(Vector2(m_outputWidth / 2, m_outputHeight / 2));
    lossMenu->SetScale(2.0f);
    m_GameObjects2D.push_back(lossMenu);

    //create a win screen
    winMenu = new ImageGO2D("winScreen", m_d3dDevice.Get());
    winMenu->SetPos(Vector2(m_outputWidth / 2, m_outputHeight / 2));
    winMenu->SetScale(2.0f);
    m_GameObjects2D.push_back(winMenu);

    //create a black screen
    blackScreen = new ImageGO2D("BlackScreen", m_d3dDevice.Get());
    blackScreen->SetPos(Vector2(m_outputWidth / 2, m_outputHeight / 2));
    blackScreen->SetScale(2.0f);
    m_GameObjects2D.push_back(blackScreen);

    //create a fuel meter
    fuelMeterShell = new ImageGO2D("FuelTrackerFrame", m_d3dDevice.Get());
    fuelMeterShell->SetPos(Vector2(m_outputWidth / 2, 4.5 * (m_outputHeight / 5)));
    m_GameObjects2D.push_back(fuelMeterShell);


    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i] = new ImageGO2D("FuelTrackerBar", m_d3dDevice.Get());
        fuelMeter[i]->SetPos(Vector2(490.0f + (50 * i), 4.5 * (m_outputHeight / 5)));
        m_GameObjects2D.push_back(fuelMeter[i]);
    }

    //add menu text
    startGameText = new TextGO2D("> Start Game");
    startGameText->SetPos(Vector2(100, 800));
    startGameText->SetScale(3.0f);
    startGameText->SetColour(Color((float*)&Colors::Gray));
    m_GameObjects2D.push_back(startGameText);

    //create DrawData struct and populate its pointers
    m_DD = new DrawData;
    m_DD->m_pd3dImmediateContext = nullptr;
    m_DD->m_states = m_states;
    m_DD->m_cam = m_cam;
    m_DD->m_light = m_light;

    //Init Audio
    levelOneMusic = new Loop(m_audioEngine.get(), "AmbientMusic");
    levelOneMusic->Play();
    levelOneMusic->SetVolume(0.1f);

    LimboMusic = new Loop(m_audioEngine.get(), "NightAmbienceSimple_02");
    LimboMusic->SetVolume(0.1f);

    levelTwoMusic = new Loop(m_audioEngine.get(), "LibetsDelay");
    levelTwoMusic->SetVolume(0.1f);

    WinMusic = new Loop(m_audioEngine.get(), "CrazyNoisyBizzareTown");
    WinMusic->SetVolume(0.1f);

    LossMusic = new Loop(m_audioEngine.get(), "WellMeetAgain");
    LossMusic->SetVolume(0.1f);

    FootStepOne = new TestSound(m_audioEngine.get(), "FootstepOne");
    FootStepOne->SetVolume(footstepVolume);
    m_Sounds.push_back(FootStepOne);

    FootStepTwo = new TestSound(m_audioEngine.get(), "FootstepTwo");
    FootStepTwo->SetVolume(footstepVolume);
    m_Sounds.push_back(FootStepTwo);

    DisplayMenu();
}

// Creates maze
void Game::CreateMazeFromArray()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distRow(0, ROW - 1);
    std::uniform_int_distribution<std::mt19937::result_type> distCol(0, COLUMN - 1);

    std::uniform_int_distribution<std::mt19937::result_type> centreMaze(1, COLUMN - 2);

    //starting cell
    grid[1][1] = 1;

    grid[1][0] = 1;
    grid[COLUMN - 2][ROW - 1] = 1;

    dfs(1, 1);

    for (int i = 0; i < 16; i++)
    {
        int ranx = centreMaze(rng);
        while (ranx % 2 != 0) {
            ranx = centreMaze(rng);
        }

        int ranz = centreMaze(rng);
        while (ranz % 2 != 0) {
            ranz = centreMaze(rng);
        }

        grid[ranz][ranx] = 1;
    }

    for (int x = 0; x < 21; x++)
    {
        for (int z = 0; z < 21; z++)
        {
            std::cout << grid[x][z] << " ";
        }
        std::cout << std::endl;
    }

    int gridX = 0;
    int gridZ = 0;

    int startX = 0;
    int startZ = 0;

    for (int x = 0; x < 21; x++)
    {
        for (int z = 0; z < 21; z++)
        {
            if (grid[z][x] == 0)
            {
                gridX = startX + (x * 15) + 7.5;
                gridZ = startZ + (z * 15) + 7.5;

                Wall* forloopWalls = new Wall("wallModel", m_d3dDevice.Get(), m_fxFactory, Vector3(gridX, 3.0f, gridZ), 0.0f, 0.0f, 0.0f, Vector3(tileSize, tileSize, tileSize));
                forloopWalls->setTerrain(true);
                m_GameObjects.push_back(forloopWalls);
                m_ColliderObjects.push_back(forloopWalls);
            }
        }
    }
}

//Maze generation
void Game::dfs(int r, int c)
{
    // 4 random directions
    int randDirs[4] = { 1, 2, 3, 4 };
    std::random_shuffle(std::begin(randDirs), std::end(randDirs));

    for (int i = 0; i < 4; i++)
    {
        switch (randDirs[i])
        {
        case 1: // Up
            //　Whether 2 cells up is out or not
            if (r - 2 <= 0)
                continue;
            if (grid[r - 2][c] != 1) {
                grid[r - 2][c] = 1;
                grid[r - 1][c] = 1;
                dfs(r - 2, c);
            }
            break;
        case 2: // Right
            // Whether 2 cells to the right is out or not
            if (c + 2 >= ROW - 1)
                continue;
            if (grid[r][c + 2] != 1) {
                grid[r][c + 2] = 1;
                grid[r][c + 1] = 1;
                dfs(r, c + 2);
            }
            break;
        case 3: // Down
            // Whether 2 cells down is out or not
            if (r + 2 >= COLUMN - 1)
                continue;
            if (grid[r + 2][c] != 1) {
                grid[r + 2][c] = 1;
                grid[r + 1][c] = 1;
                dfs(r + 2, c);
            }
            break;
        case 4: // Left
            // Whether 2 cells to the left is out or not
            if (c - 2 <= 0)
                continue;
            if (grid[r][c - 2] != 1) {
                grid[r][c - 2] = 1;
                grid[r][c - 1] = 1;
                dfs(r, c - 2);
            }
            break;
        }
    }
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& _timer)
{
    float elapsedTime = float(_timer.GetElapsedSeconds());
    m_GD->m_dt = elapsedTime;

    //this will update the audio engine but give us chance to do somehting else if that isn't working
    if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            // We lost the audio device!
        }
    }
    else
    {
        //update sounds playing
        for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
        {
            (*it)->Tick(m_GD);
        }
    }

    ReadInput();


    if (pPlayer->GetPos().x > 0 && pPlayer->GetPos().y)
    {
        monsterCanMove = true;
    }
    else
    {
        monsterCanMove = false;
    }

    //update all objects
    for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        (*it)->Tick(m_GD);
    }
    for (list<GameObject2D*>::iterator it = m_GameObjects2D.begin(); it != m_GameObjects2D.end(); it++)
    {
        (*it)->Tick(m_GD);
    }

    if (m_GD->m_KBS.P)
    {
        goUpFloor();
    }

    if (pPlayer->isRendered())
    {
        tempTrack += 1;

        if (tempTrack >= 50 && pPlayer->GetPos().y < 20)
        {               
            if (MeterCount + 3 <= 19 && m_GD->m_KBS.R)
            {
                blackScreen->SetRendered(true);

                MeterCount += 1;
                vRadius->increaseScale();
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 4.5 * (m_outputHeight / 5)));

                MeterCount += 1;
                vRadius->increaseScale();
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 4.5 * (m_outputHeight / 5)));

                MeterCount += 1;
                vRadius->increaseScale();
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 4.5 * (m_outputHeight / 5)));
            }
            else if (MeterCount + 2 <= 19 && m_GD->m_KBS.R)
            {     
                blackScreen->SetRendered(true);

                MeterCount += 1;
                vRadius->increaseScale();               
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 4.5 * (m_outputHeight / 5)));    

                MeterCount += 1;
                vRadius->increaseScale();
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 4.5 * (m_outputHeight / 5)));
            }
            else if (MeterCount + 1 <= 19 && m_GD->m_KBS.R)
            {
                blackScreen->SetRendered(true);

                MeterCount += 1;
                vRadius->increaseScale();
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 4.5 * (m_outputHeight / 5)));
            }
            else if (MeterCount - 1 >= -1)
            {
                blackScreen->SetRendered(false);

                vRadius->reduceScale();              
                fuelMeter[MeterCount]->SetPos(Vector2(fuelMeter[MeterCount]->GetPos().x, 9999));
                MeterCount -= 1;
            }

            if (!npcMonster->Intersects(*vRadius) && monsterCanMove)
            {
                npcMonster->searchFunction(m_GD, grid);

                float distanceX = npcMonster->GetPos().x - npcMonster->GetPos().y;
                float distanceY = npcMonster->GetPos().x - npcMonster->GetPos().y;

                float absoluteDistance = sqrt((distanceX * distanceX) + (distanceY * distanceY));
                
                footstepVolume = (400 - absoluteDistance) / 300;

                FootStepOne->SetVolume(footstepVolume);
                FootStepTwo->SetVolume(footstepVolume);

                if (lastStepPlayed == 0 || lastStepPlayed == 2)
                {
                    FootStepOne->PlaySound();
                    lastStepPlayed = 1;
                }
                else if (lastStepPlayed == 1)
                {
                    FootStepTwo->PlaySound();
                    lastStepPlayed = 2;
                }
            }

            tempTrack = 0;
        }

        if (std::round(pPlayer->GetPos().x / 15) == -2 && std::round(pPlayer->GetPos().z / 15) == 2 && pPlayer->GetPos().y > 3.2f)
        {
            pPlayer->SetPos(Vector3(- 23.0f, 3.06835f, 22.5f));
            goDownFloor();
        }
    }

    CheckCollision();

    //std::cout << "x: " << pPlayer->GetPos().x << " y: " << pPlayer->GetPos().y <<  " z: " << pPlayer->GetPos().z << std::endl;
    //std:cout << "xT: " << std::round(pPlayer->GetPos().x / 15) << "zT: " << std::round(pPlayer->GetPos().z / 15) << std::endl << std::endl;
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    //set immediate context of the graphics device
    m_DD->m_pd3dImmediateContext = m_d3dContext.Get();

    //set which camera to be used
    m_DD->m_cam = m_TPScam;

    //update the constant buffer for the rendering of VBGOs
    VBGO::UpdateConstantBuffer(m_DD);

    //Draw 3D Game Obejects
    for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        if ((*it)->isRendered())
        {
            (*it)->Draw(m_DD);
        }
    }

    // Draw sprite batch stuff 
    m_DD2D->m_Sprites->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());
    for (list<GameObject2D*>::iterator it = m_GameObjects2D.begin(); it != m_GameObjects2D.end(); it++)
    {
        if ((*it)->isRendered())
        {
            (*it)->Draw(m_DD2D);
        }
    }
    m_DD2D->m_Sprites->End();

    //drawing text screws up the Depth Stencil State, this puts it back again!
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int _width, int _height)
{
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& _width, int& _height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    _width = 960;
    _height = 540;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    //creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    //something missing on the machines in 2Q28
    //this should work!
#endif

    static const D3D_FEATURE_LEVEL featureLevels[] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        static_cast<UINT>(std::size(featureLevels)),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
    ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
        ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}

void Game::ReadInput()
{
    m_GD->m_KBS = m_keyboard->GetState();
    m_GD->m_KBS_tracker.Update(m_GD->m_KBS);
    //quit game on hiting escape
    if (m_GD->m_KBS.Escape)
    {
        ExitGame();
    }

    if (m_GD->m_KBS.Enter && m_GD->m_GS == GS_MENU)
    {
        m_GD->m_GS = GS_GAMEPLAY;
        DisplayGame();
    }

    m_GD->m_MS = m_mouse->GetState();


    //lock the cursor to the centre of the window
    RECT window;
    GetWindowRect(m_window, &window);
    SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);
}

void Game::CheckCollision()
{
    for (int i = 0; i < m_PhysicsObjects.size(); i++) for (int j = 0; j < m_ColliderObjects.size(); j++)
    {
        if (m_PhysicsObjects[i]->Intersects(*m_ColliderObjects[j])) //std::cout << "Collision Detected!" << std::endl;
        {
            XMFLOAT3 eject_vect = Collision::ejectionCMOGO(*m_PhysicsObjects[i], *m_ColliderObjects[j]);
            auto pos = m_PhysicsObjects[i]->GetPos();
            m_PhysicsObjects[i]->SetPos(pos - eject_vect);
        }
    }

    if (pPlayer->Intersects(*exitGate))
    {
        if(!secondRound)
        {
            goUpFloor();
        }
        else
        {
            m_GD->m_GS = GS_WIN;
            DisplayWin();
            //DisplayLoss();
        }
    }

    if (pPlayer->Intersects(*npcMonster) && blackScreen->isRendered())
    {
        m_GD->m_GS = GS_LOSE;
        DisplayLoss();
    }
}

void Game::DisplayMenu()
{
    //set menu active
    mainMenu->SetRendered(true);
    startGameText->SetRendered(true);


    //Set rest of assets inactive
    pPlayer->setRendered(false);
    npcMonster->setRendered(false);
    exitGate->setRendered(false);

    for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        if ((*it)->isTerrain())
        {
            (*it)->setRendered(false);
        }
    }

    winMenu->SetRendered(false);
    lossMenu->SetRendered(false);
    vRadius->setRendered(false);
    fuelMeterShell->SetRendered(false);
    blackScreen->SetRendered(false);

    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i]->SetRendered(false);
    }

    house->setRendered(false);
    
}

void Game::DisplayGame()
{
    //Set gameplay assets active
    pPlayer->setRendered(true);
    npcMonster->setRendered(true);
    exitGate->setRendered(true);
    vRadius->setRendered(true);
    fuelMeterShell->SetRendered(true);
    house->setRendered(true);
    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i]->SetRendered(true);
    }

    for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        if ((*it)->isTerrain())
        {
            (*it)->setRendered(true);
        }
    }

    //set rest of assets inactive
    mainMenu->SetRendered(false);
    startGameText->SetRendered(false);
    winMenu->SetRendered(false);
    lossMenu->SetRendered(false);
    blackScreen->SetRendered(false);

}

void Game::DisplayWin()
{
    //Set winscreen assets active
    winMenu->SetRendered(true);

    if (!WinScreenPlayOnce)
    {
        //Plays Music
        std::cout << "hello";
        levelTwoMusic->Play();
        WinMusic->Play();

        WinScreenPlayOnce = true;
    }

    //set rest of assets inactive
    pPlayer->setRendered(false);
    npcMonster->setRendered(false);
    exitGate->setRendered(false);

    for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        if ((*it)->isTerrain())
        {
            (*it)->setRendered(false);
        }
    }

    mainMenu->SetRendered(false);
    startGameText->SetRendered(false);
    lossMenu->SetRendered(false);
    vRadius->setRendered(false);
    fuelMeterShell->SetRendered(false);
    blackScreen->SetRendered(false);

    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i]->SetRendered(false);
    }

    house->setRendered(false);

    std::cout << "hello";
}

void Game::DisplayLoss()
{
    //Set winscreen assets active
    lossMenu->SetRendered(true);

    if (!LossScreenPlayOnce)
    {
        //Plays Music
        std::cout << "hello";

        if (secondRound)
        {
            levelTwoMusic->Play();
        }
        else
        {
            levelOneMusic->Play();
        }

        LossMusic->Play();

        LossScreenPlayOnce = true;
    }

    //set rest of assets inactive
    pPlayer->setRendered(false);
    npcMonster->setRendered(false);
    exitGate->setRendered(false);

    for (list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        if ((*it)->isTerrain())
        {
            (*it)->setRendered(false);
        }
    }

    mainMenu->SetRendered(false);
    startGameText->SetRendered(false);
    winMenu->SetRendered(false);
    vRadius->setRendered(false);
    fuelMeterShell->SetRendered(false);
    blackScreen->SetRendered(false);

    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i]->SetRendered(false);
    }

    house->setRendered(false);
}

void Game::goUpFloor()
{
    //In game: Vector3(-200, 20.6f, -100) Debugging: Vector3(-30, 20.6f, 50);
    pPlayer->SetPos(Vector3(-200, 20.6f, -100));
    vRadius->setRendered(false);
    fuelMeterShell->SetRendered(false);
    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i]->SetRendered(false);
    }

    npcMonster->setActive(false);

    LimboMusic->Play();
    levelOneMusic->Play();
}

void Game::goDownFloor()
{
    LimboMusic->Play();
    levelTwoMusic->Play();

    vRadius->setRendered(true);
    fuelMeterShell->SetRendered(true);
    for (int i = 0; i < 20; i++)
    {
        fuelMeter[i]->SetRendered(true);
    }

    npcMonster->setActive(true);
    secondRound = true;
    MeterCount = 19;
}