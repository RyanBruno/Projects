### Sync Procedure

Our first prodcedure works with three sets.
This maps great to filesystems so we do not need to do extra work on every file change.
The first set is the filesystem itself at the time of sync.
The second is the local knowledge usually stored in a file within the directory.
The third is the remote set that will be merged with the local one.

-- Step 1

Create the first set:
 Every item in the set will corrispond to a file on the filesystem.
 The set will extract the file path, date modified.

-- Step 2

Merge first set with second set:
 Every item not in first set but in second set will be tombstoned.
 Every item in both sets but with differing date modifies will be tombstoned then re-entered.

-- Step 3

Merge third set to yield actions:
 Tombstones in remote set and in local set yeild a delete.
 Items in remote set and in local set with differing modified date will yeild a fetch.

Iteration 2

ForEach File w/ stat:
 if...
    Tombstone in remote set
        - Delete
        - Tombstone in DB
    Mod Date or node id different in remote set
        - Conflict
 then...
    Delete from remote set

ForEach in remote set
 if...
    Tombstone
        - Add to local db
    Not Tombstone
        - Fetch
        - Add to local db

Local DB:
file path   date modified   node\_id    tombstone?
4096        8               7           1

