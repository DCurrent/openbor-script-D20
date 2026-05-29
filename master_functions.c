/*
 * DC D20 Library
 * Copyright © Damon V. Caskey. All rights reserved.
 *
 * Royalty-free use is permitted for OpenBOR modules and related projects.
 * Original author credit, copyright notices, and license notices must remain
 * intact in the source code. Projects using this library must credit
 * Damon V. Caskey for the DC D20 library in a visible module credit,
 * readme, documentation, website, or equivalent location.
 *
 * This software is provided "as is", without warranty of any kind.
 * Damon V. Caskey assumes no liability for damages, losses, or other claims
 * arising from use, misuse, modification, distribution, or inability to use
 * this software.
 */

#include "data/scripts/dc_d20/master_config.h"

/* 
* Caskey, Damon V.
*
* Fetch current global seed as a sane integer.
*/
int dc_d20_get_entropy_seed(){
    int seed = getglobalvar(DC_D20_VARKEY_ENTROPY_SEED);

    /* Default to zero if missing or invalid. */
    if (typeof(seed) != openborconstant("VT_INTEGER")) {
        seed = 0;
    }
    return seed;
}

/*
* Caskey, Damon V.
*
* Updates the entropy seed with an additive delta.
* Mask enforces 31-bit positive integer.
*/
int dc_d20_update_entropy_seed(int additional_entropy){

    /* Guards. */
    if (typeof(additional_entropy) != openborconstant("VT_INTEGER")){
        shutdown(1, "\n\n ERROR: dc_d20_update_entropy_seed(" + additional_entropy + ") - Missing or invalid parameter.\n");
    }

    /* Get current seed or initialize if missing. */
    int entropy_seed = dc_d20_get_entropy_seed();

    /*
     * Add new entropy to the current seed and
     * constrain to 31 bits.
     */
    entropy_seed = (entropy_seed + additional_entropy) & DC_D20_INTEGER_MASK_31BIT;

    /* Persist updated seed. */
    setglobalvar(DC_D20_VARKEY_ENTROPY_SEED, entropy_seed);

    return entropy_seed;
}

/*
* Caskey, Damon V.
*
* Generates a pseudo-random integer in [lower_bound, upper_bound].
* Mixes engine elapsed time and evolving global entropy to perturb rand().
*/
int dc_d20_generate_random_int(int lower_bound, int upper_bound){
    
    /* Guards. */
    if (typeof(lower_bound) != openborconstant("VT_INTEGER")
        || typeof(upper_bound) != openborconstant("VT_INTEGER")){
    
        shutdown(1, "\n\n ERROR: dc_d20_generate_random_int(" + lower_bound + ", " + upper_bound + ") - Missing or invalid parameter.\n");
    }

    /* Defensive range fix (swap if inverted). */
    if (upper_bound < lower_bound){
        int temp = lower_bound;
        lower_bound = upper_bound;
        upper_bound = temp;
    }

    /* Fast exit if degenerate range. */
    int mod = (upper_bound - lower_bound) + 1;
    if (mod <= 0){
        return lower_bound;
    }

    /* 
    * Get elapsed time to use as a time-based entropy. 
    * The +1 is because elapsed_time can be 0 outside of
    * gameplay or on very first frame.
    */
    int time_seed = 1 + openborvariant("elapsed_time");

    /* 
    * Nudge the global seed with current frame timing.
    * Using a small additive delta keeps growth controlled.
    */
    int entropy_seed = dc_d20_update_entropy_seed(time_seed);

    /*
    * Mix sources:
    *  - rand(): engine PRNG
    *  - time_seed: frame timing, offset so it is never zero
    *  - entropy_seed: persistent cross-call noisy state
    * XOR blends without overflow concerns, mask ensures positive 31-bit.
    */
    int raw = (rand() ^ time_seed ^ entropy_seed) & DC_D20_INTEGER_MASK_31BIT;

    /*
    * Evolve the seed slightly from the new randomness. 
    * Delta uses shifted raw to pull higher-order bits, 
    * plus current time.
    */
    int delta = ((raw >> 5) + time_seed) & DC_D20_INTEGER_MASK_31BIT;
    dc_d20_update_entropy_seed(delta);

    /*
    * Map to range via modulo and return. Has a slight
    * modulo bias, but it's simple and fast.
    */ 
    return lower_bound + (raw % mod);

    /* -----------------------------------------------------------------------
    * Rejection sampling to reduce modulo bias (slower).
    *
    * int limit = (DC_D20_INTEGER_MASK_31BIT / mod) * mod;    // largest multiple of mod
    * do {
    *   raw = (rand() ^ time_seed ^ dc_d20_get_entropy_seed()) & DC_D20_INTEGER_MASK_31BIT;
    * } while (raw >= limit);
    * return lower_bound + (raw % mod);
    */
}


