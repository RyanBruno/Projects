
## Abstracted Specification for an OR-Set Imp

Terminology

Network: Consists of a series of nodes.

Node: A machine connected to the network containing one or more OR-Sets and actively participating in the OrSet Synchronization.

OrSet Synchronization: A process consisting of two operations (given below) one that relays changes to its Set to other nodes and one that merges operations received over the network to its own set.

Set: The set used to store information is an OR-Set.

OR-Set: OR-Sets have two modification operations that allow for it's conflict free operations and synchronization. Add() inserts data into the OR-Set at a uniquely generated id. Remove() which replaces the data in that node with a tombstone.

Merge Operation: A merge operation occurs when a node receives synchronization information from another node. This can be a node's entire encoded Set or a part of that set (for performance). This may be done one-by-one. For each in the remote set, (1) if the item is not in the local set, add it, (2) if a tombstone is not a tombstone in the local set delete it.

Relaying Changes: In order to be an active member of the network a node must occasionally\* send it's entire or part of its Set to another node to trigger a Merge Operation.
