# DC D20
Generate random values in OpenBOR script.

Load this library into your module projects to enable controllable random number generation. 

## Dependencies

None

## Installation

1. Download and unzip the [latest release](../../releases).
1. Place the *dc_d20* folder into your *data/scripts* folder.
1. Add ```#include data/scripts/dc_d20/main.c``` into any other script you would like to add this library’s functionality to. Note any of the "DC" library series listing this as a dependency will already have this step completed.
1. (Optional, highly reccomended) - Add *keyall.c* to *data/scripts* folder if it does not exist already. Include the library as above, then add the following code into *main()*:

```c
int player_index = getlocalvar("player");
int key_press = getplayerproperty(player_index, "keys");

/* 
* Update entropy seed with key press and player 
* index to add human based entropy to random number 
* generation.
*/
dc_d20_update_entropy_seed(key_press ^ player_index);
```

This will improve the randomness by introducing human etrophy to the random generation.

## Use Cases

Generate a random integer between lower and upper boundry. 
```c

int lower_bound = 0;
int upper_bound = 10;

int i = dc_d20_generate_random_int(lower_bound, upper_bound);
```

Update the entrophy seed. It is reccomended to run this in the keyall event to add human entrphy into random generation.
```c
int additional_entropy = 1;

int updated_seed = dc_d20_update_entropy_seed(additional_entropy);
```

Gets current entrophy seed without modifying.
```c
int entrophy_seed = dc_d20_update_entropy_seed();
```

