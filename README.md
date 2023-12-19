<div align="center">
  <img src="https://github.com/piqey/gmsv_visinfo/actions/workflows/build.yml/badge.svg">
</div>

# Overview
This module aims to enrich _Garry's Mod_'s Lua API by offering an intuitive Lua interface for several [visibility-related engine functions](https://developer.valvesoftware.com/wiki/PVS) not currently implemented in the game's native API. While some functions mirror their C++ counterparts, most have been restructured to align seamlessly with Lua scripting paradigms. Most notably, the adaptation of PVS byte buffers into user-friendly Lua userdata objects, complete with methods for easy access and manipulation of relevant data.

With the help of this module, server-side Lua scripts are capable of accessing information regarding **Clusters**, **PVS** and **Areas**.

## Example
```lua
-- Load the visinfo module
require("visinfo")

-- Store PVS userdata of cluster 123
local pvs = GetPVSForCluster(123)

print("Is cluster 164 visible from cluster 123?", pvs[164])
-- > Is cluster 164 visible from cluster 123? [true/false]

print("Is Vector(123, 456, 789) within the PVS of cluster 123?", pvs:CheckOrigin(Vector(123, 456, 789)))
-- > Is Vector(123, 456, 789) within the PVS of cluster 123? [true/false]

print("How many clusters are visible in cluster 123's PVS?", #pvs)
-- > How many clusters are visible in this PVS? [number of visible clusters]
```
