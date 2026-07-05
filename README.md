# Trigger Command System

A small Unreal Engine plugin that implements trigger volumes using the **Command design pattern**.

## The pattern, mapped to UE

| Command pattern role | Class |
|---|---|
| Command interface | `UTriggerCommand` (abstract UObject, `Execute` / `CanExecute`) |
| Concrete Commands | `UCommand_PrintMessage`, `UCommand_PlaySound`, `UCommand_ToggleActorVisibility`, `UCommand_SetActorEnabled`, `UCommand_ApplyDamage`, `UCommand_OpenDoor`, `UCommand_Delay` (decorator), `UCommand_Composite` (runs several commands) |
| Invoker | `ACommandTrigger` (fires `OnBeginOverlapCommands` / `OnEndOverlapCommands`) |
| Receiver | Whatever actor/subsystem each concrete command acts on (a door actor, `UGameplayStatics`, etc.) |

Commands are `UObject`s marked `EditInlineNew` + `Instanced`, stored in `TArray<UTriggerCommand*>`
properties. That's the idiomatic UE way to get a polymorphic "pick a class from a dropdown, then
edit its exposed properties inline" experience in the Details panel — no custom editor code needed.

## Installation

1. Copy the `TriggerCommandSystem` folder into your project's `Plugins/` directory
   (create that folder if it doesn't exist yet).
2. Regenerate project files / open the project — Unreal will compile the plugin module.
3. Enable it in **Edit > Plugins** if it isn't already (it's set to load by default).

## Usage

1. Drag a **Command Trigger** into the level (or make a Blueprint subclass of it if you
   want a custom mesh/scale as a base).
2. In the Details panel, expand **Trigger > Commands**.
3. In `On Begin Overlap Commands`, click **+**, then pick a command from the class dropdown
   (e.g. `Open Door`). Fill in its properties (target actor, rotation, duration, etc.).
4. Repeat for `On End Overlap Commands` if you want something to happen when the overlap ends.
5. Optionally set `Actor Class Filter` / `Required Actor Tag` (e.g. restrict to Pawns) and
   `Trigger Once` under **Trigger > Filter / Settings**.

Combine commands:
- Use **Composite** to run several commands from one array entry.
- Use **Delay** to wrap any other command and run it after N seconds
  (e.g. Delay → PlaySound, so a door creaks a moment after it starts opening).

## Adding your own predefined commands

1. Add a new header/cpp under `Commands/`, subclassing `UTriggerCommand`.
2. Override `Execute_Implementation` (and `CanExecute_Implementation` if you need a guard).
3. Expose whatever properties designers should configure with `UPROPERTY(EditAnywhere, ...)`.
4. Give it a friendly name with `UCLASS(meta = (DisplayName = "My Command"))`.

That's it — it will automatically show up in the command-picker dropdown on every
`ACommandTrigger` in the project. You can also subclass any `UCommand_*` in Blueprint
if you'd rather implement `Execute` visually.

## Notes

- `UCommand_OpenDoor` uses a simple timer-based lerp for rotation. For production-quality motion,
  consider driving it with a Timeline component or an animation on the door actor instead — the
  command's job is just to kick that off.
- All "target actor" references use `TSoftObjectPtr<AActor>` so they don't force-load actors
  and can safely point at actors placed elsewhere in the level.