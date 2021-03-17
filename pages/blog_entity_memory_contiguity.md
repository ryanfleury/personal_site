title: "Entity Memory Contiguity: A Story About Simplicity"
desc:  "A dive into two ways of organizing entity storage in games, and the various performance and simplicity trade-offs they incur."
date:  (2020/3/3)
index: {"blog"}
parent: "blog"
header_image: "data/melodist_sleep.png"

"""
Traditional games tend to have a lot of stuff. When we design games, we like to be detail-oriented—not only because we care about our games and want to realize our ideas fully, but also because it seems to improve the player experience. Games have always been about bringing worlds to life, and in my estimation, the most interesting worlds to explore probably aren't empty.

So, what do we do? It can sometimes depend on the team's organizational structure, but in general, the concept of an @i "entity" arises time and time again. So, what is an entity?

Well, you can think of an entity as just a unit of this so-called @i "stuff". It is a generic term—it doesn't necessarily have a direct equivalent in the game's code. It is just a way to refer to some "unit of stuff".

These entities usually aren't static—they are normally mutating and interacting with each other as time progresses. Part of the reason why games are interesting at all, in fact, is @i "because" of these mutations and complicated interactions. Mutation and interaction is what makes a world feel @i "real".

What does this mean for our (the people programming the game's) job? Ultimately, it breaks down into a set of data transformations. Each frame we have some input entity state, and some output entity state. If we are trying to be smart about the way we do that, we'll consider the kinds of transformations that need to happen, and we'll architect a system around that.

A short aside: this doesn't necessarily mean a specific system for all cases. The concept of an "entity component system" is very popular nowadays, but in my experience this architecture is not a "one size fits all" solution. It lends itself well to some ideas, and worse to others. This is also why, in my estimation, it is not a good idea for a game engine that supports many possible games to enforce this kind of design structure on gameplay code, because it constrains design ideas. But I digress...

What I'm getting at is that almost all entities need @i "state", which means they need @i "memory" to store that state. So, what do we do? Well, we can start by allocating some memory:
"""

@code
"""
Entity *entities = Allocate(MAX_ENTITIES * sizeof(*entities));
"""

"""
If we wanted to do more "component-wise" allocation and data transformations, we might do something like this instead:
"""

@code
"""
PositionComponent *positions = Allocate(MAX_ENTITIES * sizeof(*positions));
RadiusComponent *radii = Allocate(MAX_ENTITIES * sizeof(*radii));
HealthComponent *health = Allocate(MAX_ENTITIES * sizeof(*health));
"""

"""
In that case, we could pick and choose which streams of entity data we want to feed into different transformation pipelines, which may or may not work better for our scenario.

That all being said, for the remainder of this blog post, I'm going to stick with the simpler "struct per entity" model I showed first, because it is simpler to write about, and the concepts I will speak about trivially map to the component-based allocation structure. In fact, I encountered this subject while working on The Melodist, which does use a component-based allocation structure for entities, but the specific implementation details of that system (and how I feel that it was a mistake now) are more appropriate for another blog post.

Alright, wonderful—we've got memory for our entities. I can already hear some complaints, though... “What is @code "MAX_ENTITIES"? Why are you doing one contiguous allocation and then calling it good? How does this work for small maps without being wasteful, but scale to large maps?”

This might bridge into philosophical differences, but I have a couple of responses to these questions. In my projects, I define a harsh limit like this because the engine is trying to solve the hard problems in making the game run—that is, making the @i "largest, most intensive" places in the world run smoothly. @code "MAX_ENTITIES" should be large enough to support these most intensive places.

A user's machine should never be able to support the memory for one map or game world region, and not be able to support the memory for another one, and to ensure reliability as the user traverses the world, we should guarantee the availability of all memory required for any possible region upfront (so, within this philosophy, the @code "Allocate" call I used in earlier examples would probably be sub-allocating from an even larger block).

One might respond "but the game should be able to run on lower-end machines". I am completely on board with this goal, but if some regions of the world can load into memory just fine, and others cannot, then the game @i "doesn't" run on lower-end machines, only @i "part" of the game does. Supplying @i "part" of the game to a theoretical device with absurdly low memory usage capabilities is not worth the reduction of reliability and simplicity offered by the alternative.

Additionally, it is very unlikely on modern machines that entity memory storage will be the bottleneck on the number of entities that our engine can process. Memory is relatively cheap—it is far more likely that we will be living under some de facto harsh upper-bound of entities either way. Stated formally, there is some limit @i"n" of entities such that our game's physics is too slow, there is some limit @i"m" of entities such that our game's rendering is too slow, and there is some limit @i"o" of entities such that our game uses too much memory. Based on the characteristics of modern machines on which games are played, and based on the characteristics of modern games and their requirements, it is much more likely that @i"n" and @i"m" are smaller than @i"o", than the other way around.

So, we'll continue forward with the understanding that we have some fix-sized buffer for entity storage. How do we organize entities inside of this buffer, assuming we are not using all of the slots?

When first considering this problem in The Melodist, I started with the understanding that entity memory should be contiguous—that is, for each entity (besides the last active entity in the set), there is an active entity immediately following in memory, such that an update routine can simply loop from the 0th element to the nth element (where @i"n" is the number of entities we consider to "exist"), and work on each entity:
"""

@code
"""
for(u32 i = 0; i < entity_count; ++i)
{
  Entity *entity = entities + i;
  // Work on entity
}
"""

"""
And then, if we ever want to remove an entity, assuming we don't care about order (which we don't seem to need to), all we need to do is swap the last entity into the slot of the entity being removed, and then we're done:
"""

@code
"""
entity_count -= 1;
entities[entity_being_removed] = entities[entity_count];
"""

"""
Now, you might be saying... "Alright, hang on. You just broke something". And you're right. In this system, if we ever have to refer to an entity persistently, we can't assume where it is in memory. If entities could not be removed, we could just keep the index of the entity around and persistently get the correct entity for some region... but not anymore. Now, if we assume any entity can be removed, we have no real way of knowing that some index is the actual entity we mean to refer to, because the @i"actual" entity we were hoping to refer to has either been removed, or deleted. In either case, we'd like to know about it—right now, however, we don't have any way to.

So... what do we do?

The approach that I took looked like this: I introduced some form of ID that I can guarantee will never change. Then, I stored a centralized table that maps these IDs to indices, and then any time I'd like to persistently keep track of an entity, I could just hold on to its ID, and then resolve that ID down to which index that entity exists at later, when I actually have to get or modify the associated entity's information.

Enter: Asynchronously Loaded Assets.

Suppose now that you're in a situation where you don't know which assets you need before something (like a map) loads. In The Melodist, this normally would not be a problem, because the game knows upfront (when loading a map) which entities are necessary, and therefore the assets that it must load. So, when a map loads, we could just wait until all of the required assets are loaded, either by dispatching jobs to load them on worker threads, or just doing it on the main thread and waiting. The Melodist uses the first option, though, because generally the main thread should not stall at all for whatever reason, to prevent audio and game hiccups (the music, synthesis, and audio mixing all happen on the main thread).

...But unfortunately, it isn't that simple. Imagine, now, that you introduce the ability to add entities to the world dynamically (like in a map editor or in a special map event). Our system must now also handle @i"partial entity initialization"—in other words, all of our code must be robust to unloaded assets that an entity depends on.

In the case of a map editor, because it's a developer tool, we might get away with just blocking the editor from working while assets are being loaded (but this isn't ideal—I personally don't like compromising with tool quality, even though sometimes it is a necessity).

In the other case though, we really don't want to block anything while an entity's desired assets are being loaded in, because this can negatively impact the player's experience.

So, okay, great. We're stuck with preparing our systems to be robust to partial initialization. We have a few choices. First, we could just go through every loop everywhere that does processing on our entities and modify them to check if an entity is read to be updated. Sometimes this doesn't matter, but in other cases, it does. Secondly, we could replace the implementation of our function that dynamically adds entities to the game state—let's call it @code "AddEntity"—by adding newly created entities to a queue. Every tick, we can check the entities in this queue to see if their assets are ready, and then move them to the actually active entity memory.

This is the approach I took in The Melodist. I decided to place the "partially-initialized entity queue" inside of the active entity memory, but stored @i"after" the last active entity. This made the most sense to me, because there can only be a maximum number of entities, and the system should never have more entities in the partial-initialization queue than can be supported by the actual active entity buffer. So, we need to maintain this partial-initialization queue.

So, let's recap. The problem I presented was keeping entity memory contiguous, and stably referring to entities, while still allowing entity removal. Here is a list of things we've introduced so far in order to maintain these properties:

"""

@list
{
    "Entity index-to-ID table",
    "Entity ID-to-index table",
    "Entity partial-initialization queue",
}

"""

This is a lot of state to keep track of, and there's a lot of code that goes into making sure it is correctly maintained and used. This bothered me, and cost me a lot of debugging time in The Melodist. It also added a lot of state that had to be correct inside of a map file, and the more state that is required that must remain correct, the more chances there are for corruption, and the more difficult the system is to debug. Ideally, something like an index-to-ID and ID-to-index table could be produced from scratch given a set of entities. There was probably a better solution here than what I ended up doing, so I think my choices made a lot of this worse, but nonetheless it is a problem that requires solving.

Now, I'll pose a question that I wrestled with when brainstorming about simpler solutions... Why do we care about entity memory being contiguous? What are the benefits that this option provides? What case does it optimize for?

It shouldn't be too unclear—contiguous memory lets a loop over entities early-out:

"""

@code
"""
// Only performs entity_count iterations...
// entity_count could be much less than MAX_ENTITIES.
for(u32 i = 0; i < entity_count; ++i)
{
  // Do something with entities[i]
}
"""

"""
This seems obviously better for cases where @code "entity_count" is much less than @code "MAX_ENTITIES". When we don't care about entity memory staying contiguous (we allow holes in the entity storage, that we just skip over), our loop changes:
"""

@code
"""
for(u32 i = 0; i < MAX_ENTITIES; ++i)
{
  if(entity_flags[i] & EntityFlag_Active)
  {
    // Do something with entities[i]...
    // If and only if it's marked as active.
  }
}
"""

"""
So, what've we done here? We'll get to the performance analysis in a bit, but we should notice that our loop has gotten more complex. Luckily, this kind of loop can be fairly easily rewritten to be just as easy as the other case.
"""

@code
"""
b32 IncrementEntity(Entity **entity_ptr)
{
  Entity *entity = *entity_ptr;
  u32 start_index = 0;
  if(entity != 0)
  {
    start_index = (entity - entities) + 1;
  }
  entity = 0;
  for(u32 idx = start_index; idx < MAX_ENTITIES; idx += 1)
  {
    if(entity_flags[idx] & EntityFlag_Active)
    {
      entity = entities + idx;
      break;
    }
  }
  *entity_ptr = entity;
  return !!entity;
}
"""

"""
This function allows us to rewrite our loop as follows:
"""

@code
"""
for(Entity *entity = 0; IncrementEntity(&entity);)
{
  // Use entity
}
"""

"""
Alright, great. So, we've definitely introduced some complexity by having the more abstract loop (and therefore more code under-the-hood happening when we loop), but generally our code is just as simple as before, and arguably more fault-tolerant.

What else has happened? Well, now we know that if we have a permanent entity that is guaranteed to not ever be removed from the world—say, a player—we can take a pointer to that entity and be completely sure that it will always remain stable. This is almost certainly a simplicity win over the other case, where we would've needed to keep something like an @code "EntityID" around (that could then be mapped to a proper entity index when we'd like to use the associated entity).

We can't take pointers to non-permanent entities, however, because that entity could theoretically be removed and replaced by another entity, and then our pointer, while not being @i"invalid", will not be what we expect it to be. In these cases, we still require something extra—some form of handle—for entities. One solution that can be used to prevent the case I mentioned is to have a handle that keeps track of both an index and a generation. The generation count can be incremented whenever a substantial change occurs—like entity removal—and if a handle refers to a stale generation, it will fail to resolve. This code would look something like:
"""

@code
"""
EntityHandle handle = HandleFromEntity(entity);
// Later...
Entity *entity = EntityFromHandle(handle);
if(entity)
{
  // Entity pointer was properly resolved.
}
else
{
  // Entity handle was invalid or out-of-date, so we don't
  // have a valid entity anymore.
}
"""

"""
This is actually a little bit of a robustness win over the contiguous case—if an entity ID were to be @i"recycled", for example, there is the possibility that an entity ID could be @i"valid", but point at an entity that is @i"completely different". In the non-contiguous case, our tracking of the @i"generation" can be robust to potentially 2^64 generations. This means we can change each entity slot 18446744073709551616 times before recycling a generation. If someone ever finds a case where this becomes a problem, please let me know.

What about asynchronous assets, when our entity memory is non-contiguous? Well, that gets simplified as well—if we want "partially initialized entities", we just don't mark those entities as active until they're ready. The iteration over entity memory is, by default, robust to partial-initialization.

And... that's it. By keeping our constraint of being robust to deletion, but removing our constraint of keeping entity memory contiguous, we've simplified our implementation requirements (and therefore the amount of code we need to get right in order to have a properly robust system). We only need to keep track of entity storage, a flags integer for each entity, and a generation count for each entity.

Depending on our exact problem circumstances, we might want to merge entity flags and generation count with entity storage (in which case we keep track of nothing other than entity storage). For example, if we expect to do random access on our entity storage a lot, and if the entity storage is relatively non-sparse, we wouldn't want the dereferences to the entity flags, the entity generation counts, @i"and" the entity storage, since random access implies the potential for cold reads of memory, and we would be needlessly pulling in three different cache lines for a single entity access.

But that brings me to another point of comparison: Performance.

Let's go over a few notes we can reason about first, and then I will introduce measurements to confirm our reasoning.

First of all, let's return to the observation that the contiguous memory case optimizes for early-outing when @code "entity_count" is much less than @code "MAX_ENTITIES". Depending on your problem, this may or may not be worthwhile, but it is my estimation that intensive games that require tightly-optimized entity hot loops are optimizing for the most intensive scenes—that is, when @code "entity_count" is @i"closer" to @code "MAX_ENTITIES". Now, let's consider a measurement of our entity storage sparseness. In the contiguous storage case, sparseness is always 0. In the non-contiguous storage case, let's define it as being the number of all entity storage slots that are marked as @i"inactive" with @i"active" entities following them in memory, divided by the maximum number of entities.

Within this case, as the number of active entities gets closer to the maximum number of entities, sparseness of our non-contiguous storage gets closer and closer to 0. When we have the maximum number of entities, for example, sparseness in both cases will be 0. Trivially, in this case, we are wasting the same number of cache lines in either case, meaning we lose very little from allowing non-contiguous storage. We @i"do" still need to check whether each entity is active, but that is negligible cost compared to both memory access, and if memory access is not the bottleneck, it is negligible compared to whatever work we're doing that @i"is" our bottleneck.

So, it can be said that in the cases where the game is optimizing for the upper-bound case (which I estimate is what is most often necessary), the non-contiguous storage loses nothing. However, in cases where budgets can change—for example, if any less time spent processing entities should be reallocated to something else that can be offered @i"more" time, we are hurt in the non-contiguous storage case. That being said, there are certainly strategies that can be taken to reduce time spent in the sparse case, like caching an iteration list that will help avoid some of the cache misses implicit in iterating an entire empty entity batch, and then only updating that list when the entity batch changes.

Another important observation is that, within the non-contiguous storage case, we can never take @i"longer" than when we have full entity storage, even if the sparseness of our entity storage is maximally bad (equal to 1). In other words, spending time for entity memory access for @code "MAX_ENTITIES" entities, @i"and" entity work for some @code "n" that is less than or equal to @code "MAX_ENTITIES" entities, will always be strictly larger than entity memory access for @code "MAX_ENTITIES" entities, assuming entity work is nonzero (true in all nontrivial cases). This means that, assuming a fixed budget for entity-update, if we support the upper-bound of entities with our budget on target platforms, then we can support any number that is lesser than the upper-bound.

I wrote a small program to measure the performance of different storage techniques. The measurements generally verify the reasoning we just walked through. In the test, I am considering 4096 entities. All listed figures were taken on a Skylake Intel chip running at 3.6 GHz.

Each entity is 128 bytes wide (two cache lines). I wrote two bogus procedures that "update" entities. The first one looks like this:
"""

@code
"""
int DoEntityWork_Piggy(Entity *entity)
{
  int sum = 0;
  for(int i = 0; i < sizeof(entity->data); ++i)
  {
    sum += entity->data[i];
  }
  entity->data[0] = 1;
  entity->data[66] = 2;
  entity->data[119] = 3;
  return sum;
}
"""

"""
This procedure takes ~560 cycles on my machine, so it's just meant to serve as some pretty substantial work that needs to occur for each entity. This makes the update routine the bottleneck, instead of memory access. For the case where @i"memory access" is bottlenecking our routine, and our actual update work is much slimmer, I've prepared this alternate procedure:
"""

@code
"""
int DoEntityWork_Slim(Entity *entity)
{
  return entity->data[0] + 25;
}
"""

"""
With these two procedures in mind, let's continue.

First, let's see how much work goes into iterating a completely empty set of entities. We can imagine that this is going to be really quick with contiguous entity memory, because all the program needs to do is compare against the number of entities we are considering, and skip the rest. Obviously, this remains true:
"""

@code
"""
Empty Case (Contiguous, Piggy):
  1024 iterations, 27.488281 cycles (average)
Empty Case (Sparse, Piggy):
  1024 iterations, 18786.892578 cycles (average)
Empty Case (Sparse with Iteration List, Piggy):
  1024 iterations, 27.524414 cycles (average)
Empty Case (Contiguous, Weight Watchers):
  1024 iterations, 27.464844 cycles (average)
Empty Case (Sparse, Weight Watchers):
  1024 iterations, 22555.677734 cycles (average)
Empty Case (Sparse with Iteration List, Weight Watchers):
  1024 iterations, 25.426758 cycles (average)
"""

"""
Obviously, in this case, the weight of the update routine doesn't change anything significantly, because aren't calling it (there are no active entities). We can see here that we waste ~20,000 cycles with sparse entity memory. Let's say this is on a 2 GHz CPU, this comes out to about a 10 microsecond waste, just to give us an idea of the scale of waste we're dealing with.

We can also observe that the fast iteration list gives us a good win here, because it allows us to skip over empty slots, much like contiguous storage does (but instead of the entity memory being contiguous, the list memory is contiguous).

How does the update over an empty entity batch compare with a @i"full" batch of entities? In this case, as I discussed earlier, sparseness for both contiguous and non-contiguous entity storage is zero, so we should see fairly small differences between contiguous and non-contiguous entity storage.
"""

@code
"""
Full Case (Contiguous, Piggy):
  1024 iterations, 2164290.577148 cycles (average)
Full Case (Sparse, Piggy):
  1024 iterations, 2172001.588867 cycles (average)
Full Case (Sparse with Iteration List, Piggy):
  1024 iterations, 2189763.810547 cycles (average)
Full Case (Contiguous, Weight Watchers):
  1024 iterations, 26667.936523 cycles (average)
Full Case (Sparse, Weight Watchers):
  1024 iterations, 32269.322266 cycles (average)
Full Case (Sparse with Iteration List, Weight Watchers):
  1024 iterations, 31268.362305 cycles (average)
"""

"""
In this case we can pretty clearly see that there aren't substantial differences between contiguous and sparse storage, more-or-less because the sparse storage is has a sparseness of zero in the full case. We still see some cycle increases (that I found were relatively reliably reproduced). There seem to be ~5,000-10,000 cycles lost with sparse storage with both a piggy and super light-weight update function.

I expect that, when we do not have an iteration list, this disparity is due to the extra work associated with checking whether an entity is active or not. Each test is updating 4,096 entities, and we could expect that this check will require a single cycle of latency (a single @code "AND" instruction), so that works out to be roughly what we might expect.

In the case where we @i"do" have an iteration list, I expect the disparity to come from extra work associated with reading from the iteration list indices, and then adding the indices to the base address of the entity storage.

So, we are wasting a bit of time with sparse storage in this case. On the hypothetical 2 GHz processor I mentioned earlier, this is a 2.5 microsecond waste. An important note, however, is that in the piggy entity update case (which I posit is more like the common-case, because it implies actually interesting work happening for each entity), this is a measly 0.25% of our total time spent updating. This confirms our earlier suspicion that any time spent dealing with checking whether an entity is active or not in the sparse case is negligible.

This covers the two extremes—when we've got no entities, or when we've got as many as we claim to handle—but I wanted to test a case in the middle range as well, so I ran a test that collected numbers for the half-full entity batch worst-case. The worst-case for the sparse storage, in my estimation, is every other entity slot being inactive, so that our useful-data-per-cache is effectively cut in half. Here are the numbers:
"""

@code
"""
Half-Full, Worst Case (Contiguous, Piggy):
  1024 iterations, 1114799.429688 cycles (average)
Half-Full, Worst Case (Sparse, Piggy):
  1024 iterations, 1099689.594727 cycles (average)
Half-Full, Worst Case (Sparse with Iteration List, Piggy):
  1024 iterations, 1108070.508789 cycles (average)
Half-Full, Worst Case (Contiguous, Weight Watchers):
  1024 iterations, 13159.281250 cycles (average)
Half-Full, Worst Case (Sparse, Weight Watchers):
  1024 iterations, 29817.289062 cycles (average)
Half-Full, Worst Case (Sparse with Iteration List, Weight Watchers):
  1024 iterations, 16059.382812 cycles (average)
"""

"""
In this case, the weight of our update routine makes a pretty big difference. When we are not bottlenecked by memory access, and instead by the update routine, we notice a very small difference between contiguous and sparse storage. However, when we are bottlenecked by memory access, we take a hit when we don't use an iteration list by roughly 2x. This is relatively expected, and I will again argue that this is not the common or interesting case.
"""

@subtitle "Closing Thoughts"

"""
So, what was learned? What was the point of any of this?

First things first, enforcing contiguous entity storage requires a lot of code complexity, especially when we introduce the requirement of robustness towards partial-initialization. Sparse, non-contiguous entity storage helps us on this front—it allows pointer and index stability for permanent entities, and trivial robustness for partial-initialization.

What are the tradeoffs with sparse entity memory? We found that they are relatively non-existent when we have a @i"lot" of entities—close to the entities we consider as our game's upper-bound. However, they become more prevalent in cases where we have much fewer entities than our game's upper-bound. In these cases, sparseness of entity storage might cost us in the order of 10-20 microseconds per entity-batch iteration on a common consumer CPU. However, it appears that caching an entity iteration list, that keeps a contiguous list of active entity indices in the sparse entity storage, can help speed us back up significantly in these cases.

So, what's the conclusion we draw from all of this? Well, hopefully I've made it clear that it, to some degree, depends on your problem, but really only in extreme cases. If your game is being optimized for the maximally-intensive case, and you can afford a 10 microsecond (on a common consumer CPU) penalty, then the simplicity wins of sparse entity storage are almost certainly worth paying a bit of runtime cost for.

This is all assuming that the entity tick will be the bottleneck of your game as well—for me, it has become increasingly clear that sparse entity storage fits The Melodist very well. Entity updates are not the bottleneck in the game at this point, and this small change will dramatically simplify existing systems.

I would suggest the following as a TL;DR for independent developers of smaller-scale games, like myself: I am almost 100% certain that your game is not bottlenecked by iterating entities, and it seems that sparse entity storage can make your systems significantly more robust, less error-prone, require less code and state to work correctly, and be more easily verified to work correctly.

The best way to solve pointer invalidation is to not invalidate pointers, and sparse entity storage is a very useful tool in doing that; while it does have a (relatively inexpensive) price, it helps us solve an entire set of problems (pointer invalidation) in a much simpler way. When it comes to performance, games in this space can, @i "as a whole", probably afford substantially more than a 10 microsecond penalty on an entity update tick.

To wrap everything up, the things I've learned from all of this, and by doing what I'd call the @i "wrong" thing in The Melodist first, are: keep it simple whenever possible, avoid problems by not causing them, and understand your constraints.

Thanks for reading!
"""