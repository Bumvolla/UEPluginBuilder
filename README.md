# UE_PluginBuilder

This is a simple C++ application that provides a GUI for building Unreal Engine plugins to greater engine versions taking advantage of the Unreal Automation Tool (UAT).

## Features

- Support for Unreal Engine 5.*.
- Batch plugin building
- Print the UAT log to a console.
- Creates log files for each plugin version in the packaging folder

## Usage
1. Download the latest version of the app on [Releases page](https://github.com/Bumvolla/UE_PluginBuilder/releases/latest) and open the .exe file.
2. Use the buttons to open the windows file explorer and provide the required folder/file.
3. Click the Build Plugin button.

**Select UE Path** -> Select engine installation path. 

**Select .uplugin file** -> Select the .uplugin file of the plugin you want to update

**Select package folder** -> Select an empty folder where the folders for new plugin builds will be created.

**Select UE versions** -> Select the versions you want to build the plugin for

## Contributing

Contributions to this project are welcome. If you find any issues or have suggestions for improvements, feel free to create an issue or submit a pull request.

## Disclaimer
This tool is in development, expect new features in the early future!

This tool was made using QT Community edition.
