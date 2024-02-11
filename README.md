# This project is no longer maintained because VRC has resolved the issue.

# device16

A SteamVR driver that makes lighthouse controller's tracked device index <16

## Why do I need this?

Due to [problem with the unity steamvr plugin](https://github.com/ValveSoftware/steamvr_unity_plugin/blob/b96c786b4a17089a988fafd1541ab953f2e2e651/Assets/SteamVR/Scripts/SteamVR_TrackedObject.cs#L14), there is a problem that in games such as VRChat, if the tracked device index is higher than 15, it is not tracked properly.
This index includes HMD and basestations and disconnected devices. sometimes it can be a problem.
In my case, the problem occurred because I am using a quest2 HMD, 2 base stations, 2 quest2 controllers, 2 index controllers, and 8 trackers.

## Disclaimer

This driver uses a very hacky method. There may be problems with other versions of SteamVR or environments, etc.
