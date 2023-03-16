Scene: Untitled
Entities:
  - Entity: 1089345276139500720
    TagComponent:
      Tag: space Ship
    TransformComponent:
      Translation: [0, 5, 3]
      Rotation: [0, 0, 0, 1]
      Scale: [0.400000006, 0.400000006, 0.400000006]
    MeshComponent:
      ModelPath: assets/models/space-ship.obj
      ModelName: spaceship
      ModelShaderType: 1
  - Entity: 1550940702046470574
    TagComponent:
      Tag: Box
    TransformComponent:
      Translation: [0, 5, 0]
      Rotation: [-0.152405322, 0.976206481, -0.152405322, 0.0237935111]
      Scale: [1, 1, 1]
    MeshComponent:
      ModelPath: assets/models/Box.obj
      ModelName: box
      ModelShaderType: 0
    RigidBodyComponent:
      RigidBodyType: 1
    ColliderComponent:
      ColliderType: 1
      HalfExtent: [0.5, 0.5, 0.5]
    MusicComponent:
      MusicPath: assets/musics/liz.mp3
  - Entity: 10248461570597896056
    TagComponent:
      Tag: Plane
    TransformComponent:
      Translation: [0, 0, 0]
      Rotation: [0, 0, 0, 1]
      Scale: [1, 1, 1]
    MeshComponent:
      ModelPath: assets/models/Plane.obj
      ModelName: plane
      ModelShaderType: 1
    RigidBodyComponent:
      RigidBodyType: 0
    ColliderComponent:
      ColliderType: 1
      HalfExtent: [10, 1, 10]