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

# Global Functions

## `number` GetClusterForOrigin(`Vector` origin)
### Description
Finds and returns the ID of the cluster containing the specified location.
### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `origin` | `Vector` | The position whose cluster we want the ID of. |
### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `cluster` | `number` | The ID of the cluster if it was found; `-1` if it was not. |

---

## `number` GetClusterCount()
> [!TIP]
> `math.ceil(0.125 * GetClusterCount())` returns the appropriate length in bytes for a PVS buffer in the current map. There's really no use for this information in the Lua environment, though, as this module aims to phase out any direct interaction with said byte buffers. Nonetheless, I'm including it here.
### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `count` | `number` | The total number of visbility clusters within the current map. |

---

## [`PVS`](#pvs) GetPVSForCluster(`number` cluster)
### Description
Retrieves the Potentially Visible Set (PVS) data associated with the specified visibility cluster in the current map.
### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `cluster` | `number` | A number representing the ID of a visibility cluster in the map. |
### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `pvs` | [`PVS`](#pvs) | A userdata object for interacting with the PVS data associated with the provided cluster ID. |

---

## `table` GetAllClusterBounds()
### Description
Retrieves the bounding boxes for all visibility clusters in the map. Each bounding box is represented by its minimum and maximum corner coordinates.
> [!WARNING]
> In Lua, tables are indexed beginning at `1`, not `0`. Consequently, the bounding box for cluster ID `exampleClusterID` will be located at index `exampleClusterID + 1` in the table returned by this function.

> [!NOTE]
> Visibility clusters in the Source engine are convex shapes, which means they are not all axis-aligned boxes as the existence of this function may suggest. Consequently, the returned bounding boxes can and will overlap each other when clusters assume more complicated forms.

> [!TIP]
> If you are attempting to determine which cluster a particular point in the world belongs to, a better alternative would be to use [GetClusterForOrigin](#number-getclusterfororiginvector-origin), which is not only more efficient but also accounts for irregular cluster shapes as aforementioned.
### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `boundsList` | `table` | A table of bounding boxes. Each bounding box is a table with the following structure:<br /><ul><li>`Vector` `mins`<ul><li>The minimum corner of the bounding box.</li></ul></li><li>`Vector` `maxs`<ul><li>The maximum corner of the bounding box.</li></ul></li> |

---

## `number` GetArea(`Vector` origin)
### Description
Finds and returns the ID of the area containing the specified location.
> [!NOTE]
> Areas are completely separate collections of leaves, either set in stone by brushwork or created dynamically by closed areaportals (the latter of which PVS alone cannot account for).
### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `origin` | `Vector` | The position whose area we want the ID of. |
### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `area` | `number` | The ID of the area if it was found; `-1` if it was not. |

---

## `boolean` CheckAreasConnected(`number` area1, `number` area2)
### Description
Determines whether `area1` flows into `area2` and vice versa. Result depends on areaportal state.
> [!NOTE]
> Areaportals [will close of their own accord depending on the position of the player](https://developer.valvesoftware.com/wiki/CAreaPortal#UpdateVisibility.28.29). Area testing is therefore not always a good idea!
### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `origin` | `Vector` | The position whose area we want the ID of. |
### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `result` | `number` | `true` if `area1` flows into `area2` and vice versa (when the function was called). |

# PVS
## Description
A `userdata` type representing a Potentially Visible Set (PVS) in the context of map visibility computations. Comes packaged with several methods for working with its associated visibility data.

## Metamethods
### General
| Metamethod | Description | Syntax |
| :---: | :--- | :--- |
| __index | When passed number values, acts as an alias of `PVS:ContainsCluster`, returning a `boolean` | `pvs[147]` |
| __len | Returns the number of clusters visible in the PVS in question | `#pvs` |
| __tostring | Returns a string in the format of `"[PVS] X/Y clusters visible"` | `tostring(pvs)` |

### Arithmetic
`PVS` objects come complete with several arithmetic operator overloads for performing a number of basic bitwise operations on the byte buffers of said objects.
| Metamethod | Description | Bitwise | Syntax |
| :---: | :--- | :---: | :---: |
| __add | Returns a `PVS` with all the clusters from both of the two `PVS` objects provided | `A \| B` | `A + B` |
| __sub | Returns a copy of `A` with all clusters shared with `B` omitted | `A & ~B` | `A - B` |
| __mul | Returns a `PVS` containing only clusters `A` and `B` have in common | `A & B` | `A * B` |
| __div | Returns a `PVS` containing every cluster in the map *except* those `A` and `B` have in common | `~(A & B)` | `A / B` |
| __unm | Returns an *inversion* of the `PVS` provided, containing any clusters the former didn't and not containing any it did | `~A` | `-A` |

## Methods
### `boolean` PVS:CheckOrigin(`Vector` origin)
#### Description
Determines whether a given point exists within the PVS.
#### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `origin` | `Vector` | The position in the map to evaluate. |
#### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `result` | `boolean` | `true` if the position is within the PVS. |

---

### `boolean` PVS:CheckBox(`Vector` mins, `Vector` maxs)
#### Description
Determines whether the given box is within the PVS at any point.
#### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `mins` | `Vector` | The minimum corner of the box to evaluate. |
| `maxs` | `Vector` | The maximum corner of the box to evaluate. |
#### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `result` | `boolean` | `true` if the given box is within the PVS at any point. |

---

### `boolean` PVS:ContainsCluster(`number` cluster)
#### Description
Determines whether a particular cluster exists within the PVS.
#### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `cluster` | `number` | The ID of the cluster to search for. |
#### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `result` | `boolean` | `true` if the given cluster exists within the PVS. |

---

### `number` PVS:GetTotalClusters()
> [!NOTE]
> In practice, this function should *always* return the same result as [GetClusterCount](#number-getclustercount).
>
> The PVS objects in this module record the total number of clusters in the map when they are created on an individual basis. Realistically, this should never affect anything, but was implemented in this manner "just in case."
#### Returns
| Name | Type | Description |
| :---: | :---: | :--- |
| `total` | `number` | The total number of clusters accounted for by the PVS object's internal byte buffer (visibility notwithstanding). |

---

### `void` PVS:DebugOverlay(`Color` color, `number` duration)
#### Description
Shows colored boxes for a specified duration indicating the bounding boxes of each cluster visible with the PVS in question.
> [!IMPORTANT]
> In order to see anything, you must:
> * Set `developer` to `2` in the game console
> * Be the host of a listen server or singleplayer game

> [!TIP]
> Use transparent colors. A color alpha of `255` will produce completely opaque boxes, which are hardly conducive to an effective visual display. Try starting out with `40`.
#### Parameters
| Name | Type | Description |
| :---: | :---: | :--- |
| `color` | `Color` | The color the boxes should be drawn with. |
| `duration` | `number` | The length of time (in seconds) that the overlay should persist before it disappears. |
