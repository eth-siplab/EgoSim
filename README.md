# EgoSim : A Multimodal Egocentric Simulator for Body-Worn Cameras
EgoSim is fork of the [AirSim simulator](https://github.com/Microsoft/AirSim) extended to simulate avatars. It supports all the features of AirSim in Unreal Engine.

## Install
First, build the project using build.cmd / build.sh. Then, build the UE project. [more detailed instruction](https://github.com/Microsoft/AirSim)

## How to Use It
Start by obtaining a skeletal mesh and an animation blueprint. You can begin with [Maximo characters](https://www.mixamo.com/#/?page=1&type=Character) or any other Mocap dataset available in FBX format. Next, add sockets to the joints where you intend to mount the camera. EgoSim supports up to 6 sockets per avatar. Finally, configure the `settings.json` file to suit your specific use case. Inside `settings.json`, ensure that the simulation mode is set to `SIPCharacter`.
```
"SimMode": "SIPCharacter"
```
You have the flexibility to add multiple avatars to the `"Vehicles"` list.

#### Avatar Staring Location
`"X"`, `"Y"`, `"Z"` and `"Yaw"` will set the location of each avatar w.r.t. the player start.

#### Camera Location
To set the camera rotation and offset with respect to the socket, modify the values of `"CamRotX"`, `"CamRotY"`, `"CamRotZ"`, `"CamX"`, `"CamY"`, and `"CamZ"`.

#### Spring Simulation
To attach a `"Spring"` to each socket, set the spring variable of the desired socket to `true`. 

#### IMU Simulation
The Simulator provides automated calculations for the Linear Acceleration and Angular Velocity of each socket. You can adjust the variance of the noise added to these measurements using the parameters `"LinearAcc_std"` and `"AngularVel_std"`.


An example configuration is provided below to illustrate the setup for camera rotation and offset and IMU relative to socket1:
```
 "Vehicles": {

    "AvatarName": {
      "VehicleType": "SIPCharacter",
      "CharacterBp": "path_to_skeletal_mesh",
      "AnimationBp": "path_to_animation_blueprint",
      "X": -2.0,
      "Y": -2.0,
      "Z": 0,
      "Yaw":90

      "Socket1": "socket_name",
      "Spring1": false,
      "LinearAcc_std1":1.0,
      "AngulerVel_std1":1.0,
      "Cam1RotX":90.0,
      "Cam1RotY":90.0,

    }
  },
```
#### Camera and Noise Configuration
You can set the camera parameters as well as noise parameters in the `"CameraDefaults"` section of the `setting.json`. An example is provided below and you can find more details [here](https://microsoft.github.io/AirSim/settings/).

```
"CameraDefaults": {
      "CaptureSettings": [
      {
      "ImageType": 0,
      "Width": 1200,
      "Height": 680,
      "FOV_Degrees": 90,
      "MotionBlurAmount":1.0
      }
      ],
     "NoiseSettings": [
      {
        "Enabled": false,
        "ImageType": 0,
      }
      ]
}
```

Description of different `"ImageType"`:

```
  RGB = 0, 
  DepthPlanar = 1, 
  DepthPerspective = 2,
  DepthVisualization = 3, 
  DisparityNormalized = 4,
  Segmentation = 5,
  SurfaceNormals = 6,
  Infrared = 7,
  OpticalFlow = 8,
  OpticalFlowVisualization = 9
```
## Recording
EgoSim will automatically save the recordings of the cameras listed in the `"Recording"` section of the `setting.json`. Below is an example for recording the RGB images captured by the camera attached to socket1:

```
"Recording": {
        "RecordOnMove": false,
        "RecordInterval": 0.03, //recording with 30 FPS
        "Enabled": true,
        "Cameras": [
                  {
                    "CameraName": "socket1",
                    "ImageType": 0,
                    "VehicleName": "AvatarName",
                    "PixelsAsFloat": false,
                    "Compress": true
                    }
                   
          ]
        
        }
```
More details on recording settings could be find [here](https://microsoft.github.io/AirSim/settings/).

EgoSim automatically records the global position of attached cameras. To record more information such as the position of specific joints you should modify the `SIPCharacterPawnSimApi.cpp` file.
