# Realities

## Setup

Our API mostly sits on top of existing Unreal gameplay systems. Follow the following steps to get going with a fresh project:

---
### 1. Plugins

For our gamesystems to work you must add the plugin "RealitiesCore" to your project. "RealitiesCore" depends on "RealitiesUtility" and "RealitiesUGC".
"RealitiesAnimation" and "RealitiesProcedural" are optional plugins that contain several useful features for content creation.

Copypasta for .uproject file:
```
"Plugins": 
[
    {
        "Name": "RealitiesCore",
        "Enabled": true
    },
    {
        "Name": "RealitiesAnimation",
        "Enabled": true
    },
    {
        "Name": "RealitiesProcedural",
        "Enabled": true
    },
]
```

---
### 2. Gameplay systems

The project GameInstance needs to inherit from *BP_GameInstance*, or one of the following C++ classes:
```
TGOR_ModGameInstance: Enables usage of content classes
TGOR_GameInstance: Enables saving and loading profiles, as well as several gameplay features (timestamps, versioning, data systems)
TGOR_DimensionGameInstance: Enables replication optimisation based on dimensions
```

The project GameMode needs to inherit from *BP_GameMode*, or one of the following C++ classes:
```
TGOR_GameMode: Makes connecting players load the same content mods as the player
TGOR_DimensionGameMode: Spawns custom default pawn for connecting clients using TGOR spawn system
```

The project WorldSettings needs to inherit from *BP_WorldSettings*, or one of the following C++ classes:
```
TGOR_WorldSettings: Adds settings for profiles and disables BeginPlay before the current dimension is loaded
TGOR_DimensionWorldSettings: Defines what dimension the connected world belongs to
```

Optional: In case you don't want to inherit from the provided blueprint gameplay classes, add any of the following classes to your GameMode:
```
TGOR_GameState: Progress mod and content setups
TGOR_DimensionGameState: Server-side Dimension management

TGOR_DimensionController: Handles which dimension is loaded for owning player
TGOR_OnlineController: Adds online features like user accounts/chat/unlocks/avatar management
TGOR_PlayerController: Adds several quality of life blueprint accessors

TGOR_HUD: Adds menu system
```

---
### 3. Mods and setups

A lot of game concepts (dimensions, creatures, items, actions, animations...) are organised in content classes *TGOR_Content* which are organised in mods *TGOR_Mod*. Mods are how servers make sure all clients have the same content available and enabled. They also provide a list of setups *TGOR_Setup* that allow customising the game's startup sequence.
There are two types of mods:

#### Core Mod:

```
Every mod that has "CoreOnly" set to true is a Core Mod (inherit from TGOR_CoreMod for an easy template). There can only be one Core Mod per project and it automatically loads all content in the project's content folder as well as all plugins with the category "TGOR_Core".

You can define which Core Mod is loaded in the world settings, which is how you can easily test dimensions from the editor:
Create a debug Core Mod with setups that e.g. automatically spawns the avatar (inherit from *TGOR_TestMod* for an easy template).
```

#### Content Mod:
```
Content Mods are plugins that have the category "TGOR_Mod" and are loaded in the same configuration as the server. Place the Content Mod asset in its plugin and all content classes in the same plugin are assigned to it. Content classes in a plugin without a Content Mod asset are ignored.
```

---
### 4. Default pawns

Our API does not currently support true SpectatorPawns, instead all connecting players should spawn as a TGOR_Spectator. TGOR_Spectator pawns act as spectators but exist on both server and client. This is needed for interactions with the dimension and action system. Make sure "StartPlayersAsSpectators" is disabled in your GameMode and your DefaultPawn is set to a base of TGOR_Spectator.

This spectator is also meant to spawn/possess the player's avatar. You can automatically spawn an avatar (especially useful for testing) by adding a custom SpawnSetup to your debug mod.


