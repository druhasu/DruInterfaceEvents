# Dru Interface Events
[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

This plugin allows you to expose [Dynamic Multicast delegates](https://dev.epicgames.com/documentation/en-us/unreal-engine/dynamic-delegates-in-unreal-engine) through [Unreal C++ interfaces](https://dev.epicgames.com/documentation/en-us/unreal-engine/interfaces-in-unreal-engine) and use them in Blueprints

## Supported versions

I test this plugin to work with the latest version of Unreal Engine and two versions before it. Supported versions are also listed in the description of every [release](https://github.com/druhasu/DruInterfaceEvents/releases)

## Installation

1. Download [latest release](https://github.com/druhasu/DruInterfaceEvents/releases)
2. Extract archive contents into your project `/Plugins/` folder
3. Generate project files and build

## Getting started

To expose an event from C++ interface to Blueprints using this plugin, you should do several things:

1. Declare [Unreal C++ interface](https://dev.epicgames.com/documentation/en-us/unreal-engine/interfaces-in-unreal-engine) like this (or use an existing one):
   ```cpp
   UINTERFACE()
   class UMyCoolInterface : public UInterface { GENERATED_BODY() };

   class IMyCoolInterface
   {
   	GENERATED_BODY()

   public:
   	/* Some code here */
   };
   ```

2. Declare [Dynamic Multicast delegate](https://dev.epicgames.com/documentation/en-us/unreal-engine/dynamic-delegates-in-unreal-engine) like this (or use an existing one):
   ```cpp
   // Declare delegate signature with any amount of parameters. All variants are supported
   DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyCoolInterfaceDelegate)
   ```

3. Declare a method with following signature in your interface:
   ```cpp
   public:
   	// Method must be public, pure virtual and return delegate object by reference.
   	// Also, any comment you add here will be displayed in the Blueprint editor.
   	virtual FMyCoolInterfaceDelegate& OnSomethingCoolHappened() = 0;
   ```

4. Add following line to the `Build.cs` file of the module that contains the interface:
   ```c#
   PrivateDependencyModuleNames.Add("DruInterfaceEvents");
   ```
   Without this line your code will still compile, but nothing will show in the Blueprint editor.

5. Build your project. You should see these additional nodes under `Class / My Cool Interface` category:
   * Bind Event to OnSomethingCoolHappened
   * Unbind Event from OnSomethingCoolHappened
   * Unbind All Events from OnSomethingCoolHappened

   You may need to disable "Context sensitive" or drag a wire from the pin of `MyCoolInterface` instance.

## Contributing

If you have any issues, please submit them [here](https://github.com/druhasu/DruInterfaceEvents/issues/new)

I don't plan to accepts PRs right now.
