# Motion-based Authentication

## Description
In this project , we developed an authentication system based on cell phones's movement.
The user can set a pattern for authentication and the sytems will save the pattern . Then , each time a user wants to access the mobile phone , he has to execute the pattern exactly.
The sytstem save the movement done by user , calculate the distance and rotation degree , determine the movement direction  and then compare the movement with saved pattern . ( The movement should be identical with pattern both in length for drifts and degree for rotation. )

The system is powered by useful UI which makes it easy to use . Each time a user tries to access the mobile phone , system  itself clear the history and analyze the new movement.


## Table of Contents
•  Features(#Features)

•  [Installation](#installation)

•  [Usage](#usage)

•  [Contributing](#contributing)

•  [License](#license)


## Features

## Installation
For developing the sytem and also working with sensors , we used Qt framwork .Qt is a cross-platform application framework that allows us to develop graphical user interfaces and other software components.

For Intallation the Qt on your system and preparing to use for this project , you can follow the steps below :

1. Install the Qt framework from the [Qt website](https://www.qt.io/download).
2. Install SDK and JDK : Ensure you have the compatible versions of SDK and JDK for your project. You can download them from their respective official websites.
3. Configure SDK and JDK in QT :
•  Open QT Creator and go to `Tools` > `Options`.

•  Under `Kits`, select the appropriate kit for your project.

•  Configure the paths to the SDK and JDK you installed earlier.
4. Clone or download the project repository from GitHub.
5. Open the Qt project file (`.pro`) in Qt Creator.
6. Configure the project settings and build the project.
7. Run the project.



## Usage
You can easily test and use the app . After downloading the application , you have to activate the developer options on your mobile phone and enable the usb debugging mode . 
Then you should connect you mobile phone with the usb cable to your system and then run the application . 
