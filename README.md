# 42.lem-ipc
The goal of this project is to make two (or more) processes communicate and interact. This will be a mini game where 2 teams will faces each others in a battle on a 2D board

## Game rules
  - 2 teams (the number of player in each teams isn't limited)
  - For a team to be victorious, they will have to be the last team on the board.
  - When a player dies, they disappear form the board
  - For a player to be killed, they must be touched by at least 2 players from the same team, that is, one adjacent tile to the tile the target stands on (diagonal works).
  - When a player understands they’re surrounded by at least 2 players from another and same team, he must leave the board and end their execution.
  - One board tile will only take one player at a time.

## Technical constraints:
  - Each client is a process and there must be only one executable
  - When a player wuits, they will make sure they're the last player on board. If so, they will have to clean all the IPCs created by the first player
  - The board must be stocked in a Shared Memory Segment (SHM)
  - Each player can check the content of the board, but they will have to respect the constraints tied to the shared resources and the competitive access (semaphores)
  - A player can only communicate with other player through Message Queue (MSGQ)
  - On the map, player can only see if a tile is empty or occupied by another player. Is so, the player's team number will be displayed. You cannot differentiate players from the same team
