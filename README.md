# About

**Actor Flow Graph** is the editor plugin for Unreal Engine 5.
It provides a Node Editor for simple visual scripting of event chains in game levels (for example, entering a trigger opens a door), similar to Flow Graph in Leadwerks or CryEngine.

Blueprints can be used in a similar way, but Actor Flow Graph requires fewer nodes, should be significantly more performant (as it is implemented purely in C++), and is more intuitive (at least for me):

* **Every Node is based on an Actor with its Components**. This means you can see direct connections between your actors in the Actor Flow Graph editor, rather than between wrappers that only reference actors via variables and use them in obscure ways. Selecting an actor in the level selects the corresponding node in the graph editor and vice versa.
* **Input pins are actor functions or functions of actor components**. They are added automatically if a function has metadata, or can be added manually via the function picker.
* **Outputs are fired automatically if they have the same name as the activated input function, or manually via a subsystem function**. Output pins can be added via class metadata or manually.
* **Each output→input connection may have a variables map for input function parameters**. Keys are added automatically from function signatures if there is a class inherited from `UFlowVariableBase` for the parameter type. You can add new such classes in your project code and register a new `FFlowVariableFactoryDelegate` function for automatic creation on connection creation.

**C++**, **Blueprint**, and **AngelScript** actors and components are supported.
The plugin is being developed for UE **5.7**. A few changes may be required to support earlier 5.x versions.

---

# How to Install the Plugin

Download / clone / fork ActorFlowGraph.
Put it into the `YourProject/Plugins` folder (create it if it doesn’t exist).

The folder structure should look like:
`AdventureGame/Plugins/ActorFlowGraph`,
and inside `ActorFlowGraph` you should have `Source`, `Resources`, `ActorFlowGraph.uplugin`, etc.

Build your project. After that, a **Flow** category with **Actor Flow Graph** should appear in the asset creation menu.
If not, open the Plugins window, find **Actor Flow Graph**, and enable it if it’s not already enabled.

---

# Usage

## Setup

Actor Flow Graph requires only minimal setup:

1. In the Content Drawer, create an **Actor Flow Graph** asset (in the **Flow** category):

   <img width="327" height="377" alt="UnrealEditor_zbzdVJwg3G" src="https://github.com/user-attachments/assets/e6f5be40-9f42-45e8-a6fc-115872cc0ca5"/>

   When you save it, an **Actor Flow Graph Runtime** asset will be created automatically with the name `YourAssetName_Runtime`.

2. Add an **ActorFlowGraphManagerActor** to your game level/map.

3. Add your `YourAssetName_Runtime` asset to the **Graph Asset** array variable (Flow category) of this actor:

   <img width="332" height="284" alt="UnrealEditor_innuJ5S8Tw" src="https://github.com/user-attachments/assets/ab7d0022-b1fd-436a-8288-fb22470b8d14"/>

4. Now you can work with the `YourAssetName` asset in the Actor Flow Graph Editor (double-click the asset to open it).
   The runtime version is updated automatically on every asset save.
   Once your graph becomes too large, or if you want to split node groups logically without connecting them, simply create another Graph Asset and add it to the **ActorFlowGraphManagerActor** as well.

---

## Actor Flow Graph Editor

On the left, by default, you have a Palette with logical components and an **Add Comment** option (works the same as the Comment node in other node editors).

When you add a logical component to the graph, a simple `AActor` with this component is created. Such components already have predefined input and output pins.

On the right, you have the **Details** panel.
If you click on a node, you will see its Actor and Component variables.
If you select a connection between pins, you can see its variables map in Details. If you don’t fill it with values, the parameters will be empty during the function call.

To **add a new Node** based on your actor, simply **drag and drop the actor from the Level Outliner** into the graph.

---

### Blueprints

The video below shows an example of using a pure Blueprint actor as a node in Actor Flow Graph:

* How to add an Input Pin (Node Context Menu -> Add Input Pin -> Choose Actor or Component -> Pick Event/Function - functions can be quickly found via the search bar at the top)
* Two ways to create Output Pins:
  1. Via the **Actor Flow Graph Subsystem** `Emit` function
  2. By entering the same name as the input pin in the Pick Function widget

[![Video Title](https://img.youtube.com/vi/eFoVpxoyIIM/0.jpg)](https://www.youtube.com/watch?v=eFoVpxoyIIM)

---

### C++

You can find the required function in the same way, or add `"FlowInput"` metadata to a `UFUNCTION()` to make the node include it automatically:

<img width="517" height="91" alt="devenv_HYRhVUjSY7" src="https://github.com/user-attachments/assets/8fe8e9dc-8c9d-40c5-aeb1-893535dfcd49"/>

The **Reconstruct Flow Pins** option in the Node Context Menu will also add it if the node was created before the metadata was added.

To add output pins in a similar way, add this metadata to the `UCLASS` of the Actor or Component:
**FlowOutputs = "Output1, Output2"**

<img width="948" height="62" alt="изображение" src="https://github.com/user-attachments/assets/2fc65b06-999b-4083-a5f5-f5bebd668069" />

Output names can match input pins for automatic firing when the input is triggered, or you can fire them manually using a macro:

**`FLOW_EMIT("OutputName");`**

Required include:
`#include "ActorFlowUtils.h"`

---

### AngelScript

Same as C++, except there are no macros for manual output firing. The code is still minimal:

**`UActorFlowSubsystem::Get().Emit(this, n"OutputName");`**

---

## Logical Flow Components

There are ready-to-use components with predefined pins available in the palette on the left.
When you add a logical component to the graph, a simple `AActor` with this component is created.

Current list:

* **Input Counter** – fires the output once `CurrentCount == MaxCount`
* **Logical AND** – has a `TArray<bool> InputsMap` variable and fires when all values in the array are `true`. In Connection Variables, set the required index to modify a specific value
* **Sequence** – on input, fires outputs `0, 1, 2, ...` depending on `MaxOutputCount`. Add additional outputs after `1` manually via the Function Picker
* **Timer** – uses `TimerManager` and fires output when the timer expires. Can start immediately or be triggered manually. Timer rate is a variable
* **FlowTriggerActorsTakenByTagOrClass** – useful when you want to trigger all actors with the same tag and/or class without adding them all to the graph. Class and Tag are variables. The `CallActorsFunctionByName` input accepts `FName InFuncName` and `TMap<FName, UFlowVariableBase*> InVariablesMap`, which must be set via Connection Variables

You can add your own components by using the **FlowComponent** metadata on a `UCLASS`.

---

## Connection Variables

The variable types shown in Connection properties are limited to classes inheriting from **UFlowVariableBase**.
You can extend this list by adding new classes in your project code.

### `TArray<int32>` example:

```cpp
#include "FlowVariableBase.h"
// Generated include

UCLASS(EditInlineNew)
class UFlowVar_IntArray : public UFlowVariableBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    TArray<int32> Value;

    virtual void PushToArgs(void* Dest) const override
    {
        TArray<int32>& OutArray =
            *static_cast<TArray<int32>*>(Dest);
        OutArray = Value;
    }

    virtual void PullFromArgs(const void* Src) override
    {
        const TArray<int32>& InArray =
            *static_cast<const TArray<int32>*>(Src);
        Value = InArray;
    }
};
```

And in your project module:

```cpp
#include "TestProject.h"
#include "Modules/ModuleManager.h"
#include "FlowVariableFactoryRegistry.h"

class FTestGameModule : public IModuleInterface
{
public:
    FDelegateHandle FlowVariableFactoryHandle;

    virtual void StartupModule() override
    {
        FlowVariableFactoryHandle =
            FFlowVariableFactoryRegistry::RegisterFactory(
                FFlowVariableFactoryDelegate::CreateStatic(&FTestGameModule::MakeCustomFlowVar));
    }

    virtual void ShutdownModule() override
    {
        FFlowVariableFactoryRegistry::UnregisterFactory(FlowVariableFactoryHandle);
    }

    static UFlowVariableBase* MakeCustomFlowVar(UObject* Outer, FProperty* Property)
    {
        if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
        {
            if (CastField<FIntProperty>(ArrayProperty->Inner) != nullptr)
            {
                return NewObject<UFlowVar_IntArray>(Outer);
            }
        }

        return nullptr;
    }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FTestGameModule, TestProject, "TestProject");
```

`MakeCustomFlowVar` may contain multiple checks and return different `UFlowVariableBase`-derived classes.

---

# Debugging

Since everything is implemented in pure C++ and runtime nodes are not used directly, you can debug the plugin’s classes or your own actors and components as usual.

I may later find a way to display triggered connections in the graph during PIE (similar to Blueprints), or at least add some logging.
