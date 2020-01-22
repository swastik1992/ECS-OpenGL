# Breakout-ECS-OpenGL
Simple demo of classic breakout game using OpenGL and entity component system.

-----------
This is a work in progress repository to create a simple breakout game using ECS. OpenGL has been used for all the rendering and all the libraries for graphics, text, sound is already added. 
Note: Libraries are only added for 32bit so the target should be set to 32bit (X86) in visual studio.  

### ESC
Entity component system is one of the programming design patterns largely used in game development or any high performance needed systems. ECS is slightly different from generic concepts of object oriented programming and to design an application using it require a little different approach. 
As the name suggests, ECS consist of entities which are usually some unique ids, components which is your actual data and systems which are simply functionalities eg. rendering, collision systems etc.  

### Implementation:

### Entity
In order to make ECS as friendly as possible, we would have to architect our system in a way so the user doesn’t feel that the implementation is too different from a typical OOP systems. For example, an entity in ECS is just a unique number to efficiently keep track of every object but to add a component/data to the created entity, we would have to somehow map the entity id to the component object. 
To make this simple, we can add all the utility functions like adding/removing a component, giving entity a tag or group, finding if the entity has a particular component etc. in the Entity class itself. 

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/blob/master/Images/entity.png)

We would also need a manager to hold an array of entities as well as all the components that have been added for each entity. 
EntityManager would have many utility functions to create/destroy/get entity, add/remove/get component from/to an entity, functionalities for grouping and tagging entities etc. 

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/blob/master/Images/entitymanager.PNG)

Entity class would also hold a pointer to EntityManager class so it can implement all the utility functions using entity manager’s utility functions.

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/blob/master/Images/entitymanager-pointer.png)

### Component
Components are the actual data that any entity will use. It could be just a single data or collection of variables that would be used together by a system. Since we want to keep our same type data together in a continuous memory ie. every object of “X” component would be stored together in the memory using a fixed size pool. It also means that we can assign unique ids to these component type and use it as an index to keep all the same type of components.

Since compiler would create a new type of struct for each template type, getId function would return an incremental unique number for each component type.

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/component.PNG)

Also, by default we have a max number of component types and I also have a bitset of the same size. 
### System
Systems are the functional implementation and are the last piece of ECS. System would usually have a set of components that they require and if an entity has all those components, system will add that entity into a list and will run the implementation for those entities. 
Any basic system would require an array of entity that it is interested in, functions to add or remove any entity from it and most importantly a function to set all the required component for the system. 

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/system.PNG)

We would also need a system manager to keep a list of all the systems and a way to add or remove systems from the world. 

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/system-manager.PNG)

### Event
The last thing we need in this entire ecs system for now is a way for different systems to communicate with each other, send one off messages or bind to a particular event and receive callbacks.
I have decided to go with a publish/Subscribe (known as Pub|Sub) event manager. 

The way it works is that we would create an Event, which will contain any event related variables like CollisionEvent would have collision directions, normals etc.

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/event.PNG)

 Next step is to subscribe to this event in any system by creating a function in that system that takes the event as a parameter. 
 
![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/event-subscribe.PNG)

This function would be called any time the event gets published.

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/damage-event.PNG)

Last step is to publish the event wherever it is necessary. We would create a new object of the event, fill up with the required data and publish it. 

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/event-publish.PNG)
 
### Step for creating an entity
* Create all the required components if it doesn’t already exist. Eg: TransformComponent, CollisionComponent, RenderingComponent etc.
* Create required System that uses those components.  Eg: RenderingSystem, CollisionSystem etc. 
* Create an Entity using createEntity function of entity manager. 
* Add all the required components into the new entity with their initial values.
* Add the system if it hasn’t already added. Each system only needs to be added once to the world.
* You can also turn on or off any system to debug or test functionalities. 

![Screen Capture](https://github.com/swastik1992/Breakout-ECS-OpenGL/master/Images/step-demo.PNG)


Game Demo
Gif of game.
IMAGE
