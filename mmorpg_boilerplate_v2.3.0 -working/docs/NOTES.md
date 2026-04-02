This build is based on the user's working "upgraded overworld build" and has been
restructured into a cleaner folder layout.

Main gameplay code lives under:
- client/include/core
- client/include/world
- client/src/core
- client/src/world

The optional server and shared protocol are included for future multiplayer work,
but the client remains the main working game target.
