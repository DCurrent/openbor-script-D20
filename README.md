# DC D20

Generate random values in OpenBOR Script.

Load this library into your module projects to enable controllable random number generation.

## Dependencies

None.

## Installation

1. Download and unpack the [latest release](../../releases).
1. Place the `dc_d20` folder into your `data/scripts` folder.
1. Add the following include to any script where you want to use this library:

```c
#include "data/scripts/dc_d20/main.c"
```

Any DC library that lists DC D20 as a dependency may already include it.

## Optional: Add Player Input Entropy

Highly recommended: Add `keyall.c` to your `data/scripts` folder if it does not exist already. Include the library as above, then add the following code inside `main()`:

```c
#include "data/scripts/dc_d20/main.c"

void main()
{
    int player_index = getlocalvar("player");
    int key_press = getplayerproperty(player_index, "keys");
    int player_entropy = (player_index + 1) << 16;

    /*
     * Mix key state and player identity into the entropy seed.
     * This helps player input influence future random values.
     */
    dc_d20_update_entropy_seed(key_press ^ player_entropy);
}
```

This improves randomness by introducing human entropy into random generation.

## Use Cases

Generate a random integer between lower and upper boundary.

```c
int lower_bound = 0;
int upper_bound = 10;

int i = dc_d20_generate_random_int(lower_bound, upper_bound);
```

Update the entropy seed. It is recommended to run this in the `keyall` event to add human entropy into random generation.

```c
int additional_entropy = 1;

int updated_seed = dc_d20_update_entropy_seed(additional_entropy);
```

Get the current entropy seed without modifying it.

```c
int entropy_seed = dc_d20_get_entropy_seed();
```