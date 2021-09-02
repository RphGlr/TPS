# TPS
***
The project is a TPS.

## Table of Contents
1. [General Info](#general-info)
2. [Features](#features)
4. [Can do](#can-do)
5. [How to use](#how-to-use)

## General Info
***

### How to play

* Game rules

> you spawn in a desert map at night, you have to eliminate 25 enemies. but pay attention not to get detected by them cause they'll kill you fast.
> you can kill enemies from behind if you're close enough.
> if you're too close from an enemy and he sees you, he will instantly kill you.

##  Features
***

> special features :

* In this project i choosed to handle collision differently compared to the FPS Project, i used a textures to keep differents informations needed for the game to work :

* "CollisionTexture" which keeps the informations of the terrain to be used for collision and the updating of the "fovTexture" mentioned bellow.
* "fovTexture" which is used to keep the information of the enemies visions on the map.
* "trailTexture" used to make the terrainShader and get the trailEffect when the player move onto the sand.

> general features : 

* Enemies will have the following behavior : 
    * Patrol between two points until they see the player in their field of view (FOV).
    * Chase the player when they see him until the player get out of the FOV or get behind a wall.
    * Get back to their patrol path when they lose sight of the player.

* Shaders : 
    * EnemyShader : vert, frag, geo
    * PlayerShader : vert, frag
    * TerrainShader : vert, frag
    * SkyBoxShader : vert, frag

## Can do
***

* **Player Inputs**

 -   w, a, s, d to move around.

 -   The space bar is used to kill enemies when you're close enough behing them


## How to use
***
- You can run the project by pressing f5 on visual studio 2019 or by clicking on the .exe .

- all model, texture and shaders are load one time.

