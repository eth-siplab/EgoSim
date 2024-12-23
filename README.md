# EgoSim: An Egocentric Multi-view Simulator and Real Dataset for Body-worn Cameras during Motion and Activity (NeurIPS 2024)
[Dominik Hollidt](https://dominikvincent.github.io/), [Paul Streli](https://paulstreli.com/), [Jiaxi Jiang](https://jiaxi-jiang.com/), [Yasaman Haghighi](https://ch.linkedin.com/in/yasaman-haghighi), [Changlin Qian](https://ch.linkedin.com/in/changlin-qian-57901616bF), [Xintong Liu](https://ch.linkedin.com/in/xintong-liu-2b022b135), [Christian Holz](https://www.christianholz.net)<br/>
[Sensing, Interaction & Perception Lab](https://siplab.org), Department of Computer Science, ETH Zürich, Switzerland <br/>

<p align="center">
<img src="figures/teaser.gif" width="1200">
</p>

___________

This repository contains the code of EgoSim, a novel simulator of body-worn cameras that generates realistic egocentric renderings from multiple perspectives across a wearer's body.
EgoSim supports real-world motion capture data for rendering realistic images.
EgoSim is an extension of the [AirSim simulator](https://github.com/Microsoft/AirSim) to simulate avatars. It supports all the features of AirSim in Unreal Engine.

## Install
First, build the project using build.cmd / build.sh. Then, build the UE project. [more detailed instruction](https://github.com/Microsoft/AirSim)

## How to Use It
Start by obtaining a skeletal mesh and an [animation blueprint](https://docs.unrealengine.com/4.27/en-US/AnimatingObjects/SkeletalMeshAnimation/AnimBlueprints/). You can begin with [Maximo characters](https://www.mixamo.com/#/?page=1&type=Character) or any other Mocap dataset available in FBX format. Alternatively you can use any SMPL/SMPL-H/SMPL-X representation and convert it to FBX with this [blender addon](https://github.com/Meshcapade/SMPL_blender_addon). When importing the character you might have to tick the checkbox saying import Animation in Unreal when importing the character and adjust the scaling. The import should yield three 4 imported entities, a skeletal mesh, a animation, a physics asset and a skeleton. 

Next, add sockets to the joints where you intend to mount the camera. EgoSim supports up to 6 sockets per avatar, but it can be easily extended by adjusting the EgoSim Plugin code. Finally, configure the `settings.json` file to suit your specific use case. Inside `settings.json`, ensure that the simulation mode is set to `SIPCharacter`. EgoSim will automatically associate the animation (created automatically from the import) named as the skeletal mesh (created automatically from the import) with the character
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
      "CharacterBp": "path_to_skeletal_mesh", // the path is a relative path to the project root, e.g. the one you obtain when you right click the asset and "Copy Reference", but remove the prefix of the path, e.g. <del>SkeletalMesh'</del>/Game/SMPLX/CNRS/288/CNRS_288.CNRS_288'^
      "Material": "Material'/Game/chars/Materials/charMaterial.charMaterial'", // a material for the skeletal mesh, useful for apperance randomization
      "AnimationBp": "path_to_animation_blueprint", e.g. "path/AnimBlueprint.AnimBlueprint_C",
      "X": -2.0,
      "Y": -2.0,
      "Z": 0,
      "Yaw":90

      "Socket1": "socket_name",
      "Spring1": false,
      "LinearAcc_std1":1.0,
      "AngulerVel_std1":1.0,
      "Cam1RotX":90.0,
      "Cam1RotY":90.0

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


## Adjusting EgoSim
Adapting EgoSim for your own recording purposes can be done twofold. On the one hand you can use the existing recording capabilities provided via the AirSim.json file or you can use the existing AirSim Api that EgoSim fully supports.

On the other hand, you can adjust the code to your needs. For that the relevant starting points are the `AirSim\Source\Vehicles\SIPCharacter\SIPCharacterPawn.cpp` that determines the behaviour of the skeletal avatars, and the `AirSim\Source\Vehicles\SIPCharacter\SIPCharacterPawnSimApi.cpp` that determines the logging to the logging `.csv` file. 